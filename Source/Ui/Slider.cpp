/*
  ==============================================================================

    Slider.cpp
    Created: 13 Apr 2025 3:53:23pm
    Author:  Myles Wang

  ==============================================================================
*/

#include "Slider.h"

Slider::Slider(juce::RangedAudioParameter& param, juce::UndoManager* um)
    : audioParam (param)
    , paramAttachment (audioParam, [&] (float v) { updateValue (v); }, um)
{
    setWantsKeyboardFocus (true);
    setRepaintsOnMouseActivity (true);
    setColour (foregroundArcColorId, UseColors::red);
    setColour (backgroundArcColorId, UseColors::yellow);
    setColour (needleColorId, UseColors::green);
    setColour (borderColorId, UseColors::beige);

    setLabelColor (UseColors::yellow);
    label.setText (audioParam.getName (8), juce::NotificationType::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setInterceptsMouseClicks (false, false);

    setTextBoxColor (UseColors::yellow);
    textBox.onTextChange = [&]
    {
        const auto newNormValue = audioParam.getValueForText (textBox.getText());
        const auto newDenormValue = audioParam.convertFrom0to1 (newNormValue);
        paramAttachment.setValueAsCompleteGesture (newDenormValue);
        textBox.setText (audioParam.getCurrentValueAsText(), juce::NotificationType::dontSendNotification);
    };

    addAndMakeVisible (label);
    addAndMakeVisible (textBox);

    paramAttachment.sendInitialUpdate();
}

void Slider::resized()
{
    auto bounds = getLocalBounds().toFloat();
    
    borderPath.clear();
    createBorder(bounds);

    const auto subAreaHeight = bounds.getHeight() / 4.0f;
    label.setBounds (bounds.removeFromTop (subAreaHeight).toNearestInt());
    textBox.setBounds (bounds.removeFromBottom (subAreaHeight).toNearestInt());

    label.setFont (juce::FontOptions { static_cast<float> (label.getHeight()) * 0.7f });
    textBox.setFont (juce::FontOptions { static_cast<float> (textBox.getHeight()) * 0.7f });

    mainArea = bounds.expanded (1.0f).withY (bounds.getY() + 1);
}

void Slider::paint (juce::Graphics& g)
{
    drawSlider (g);

    if (hasKeyboardFocus (true))
        g.strokePath (borderPath, juce::PathStrokeType { borderThickness });
}

void Slider::mouseDown (const juce::MouseEvent& e)
{
    paramAttachment.beginGesture();

    e.source.enableUnboundedMouseMovement (true);

    mousePosWhenLastDragged = e.position;
}

void Slider::mouseDrag (const juce::MouseEvent& e)
{
    auto diffY = (mousePosWhenLastDragged.y - e.position.y) * sensitivity;

    if (e.mods.isShiftDown())
        diffY *= 0.1f;

    value = juce::jlimit (0.0f, 1.0f, value + diffY);
    const auto newDenormValue = audioParam.convertFrom0to1 (value);
    paramAttachment.setValueAsPartOfGesture (newDenormValue);
    textBox.setText (audioParam.getCurrentValueAsText(), juce::NotificationType::dontSendNotification);

    mousePosWhenLastDragged = e.position;
}

void Slider::mouseUp (const juce::MouseEvent& e)
{
    paramAttachment.endGesture();

    e.source.enableUnboundedMouseMovement (false);

    juce::Desktop::getInstance().getMainMouseSource().setScreenPosition (e.source.getLastMouseDownPosition());
}

void Slider::mouseDoubleClick (const juce::MouseEvent& e)
{
    juce::ignoreUnused (e);

    const auto defaultValue = audioParam.getDefaultValue();
    value = defaultValue;

    const auto newDenormValue = audioParam.convertFrom0to1 (defaultValue);
    paramAttachment.setValueAsCompleteGesture (newDenormValue);

    textBox.setText (audioParam.getCurrentValueAsText(), juce::NotificationType::dontSendNotification);
}

bool Slider::keyPressed (const juce::KeyPress& k)
{
    if (('0' <= k.getKeyCode() && k.getKeyCode() <= '9') || k.getKeyCode() == '-' || k.getKeyCode() == '.')
    {
        textBox.valueShownWithEditor = juce::String::charToString (k.getTextCharacter());
        textBox.showEditor();

        return true;
    }

    if (k.getKeyCode() == juce::KeyPress::upKey)
    {
        const auto newValue = getValue() + (k.getModifiers().isShiftDown() ? fineInterval : interval);
        paramAttachment.setValueAsCompleteGesture (newValue);

        return true;
    }

    if (k.getKeyCode() == juce::KeyPress::downKey)
    {
        const auto newValue = getValue() - (k.getModifiers().isShiftDown() ? fineInterval : interval);
        paramAttachment.setValueAsCompleteGesture (newValue);

        return true;
    }

    return false;
}

void Slider::focusGained (FocusChangeType cause)
{
    juce::ignoreUnused (cause);
    repaint();
}

void Slider::focusLost (FocusChangeType cause)
{
    juce::ignoreUnused (cause);
    repaint();
}

float Slider::getValue() const { return audioParam.convertFrom0to1 (value); }

void Slider::setTextBoxColor (juce::Colour newColour)
{
    textBox.setColour (juce::Label::textColourId, newColour);
    textBox.setColour (juce::Label::textWhenEditingColourId, newColour);
}

void Slider::setLabelColor (juce::Colour newColour) { label.setColour (juce::Label::textColourId, newColour); }

void Slider::setLabelText (const juce::String& newLabelText)
{
    label.setText (newLabelText, juce::NotificationType::dontSendNotification);
}

void Slider::updateValue (float newValue)
{
    value = audioParam.convertTo0to1 (newValue);
    textBox.setText (audioParam.getCurrentValueAsText(), juce::NotificationType::dontSendNotification);
    repaint();
}

void Slider::drawSlider (juce::Graphics& g)
{
    const auto radius = juce::jmin (mainArea.getWidth(), mainArea.getHeight()) * 0.5f;
    const auto toAngle = startAngle + value * (endAngle - startAngle);
    const auto lineWidth = radius * 0.1f;
    const auto arcRadius = radius - lineWidth;
    const auto centre = mainArea.getCentre();
    auto space = 0.2f;

    if (toAngle + space >= endAngle - space)
    {
        const auto restAngle = endAngle - toAngle;
        space *= restAngle / (space * 2.0f);
    }

    juce::Path backgroundArc;
    backgroundArc.addCentredArc (centre.x,
                                 centre.y,
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 juce::jlimit (startAngle, endAngle, toAngle + space),
                                 endAngle,
                                 true);
    g.setColour (findColour (backgroundArcColorId));
    g.strokePath (backgroundArc, juce::PathStrokeType { lineWidth });

    juce::Path valueArc;
    valueArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f, startAngle, toAngle, true);
    g.setColour (findColour (foregroundArcColorId));
    g.strokePath (valueArc, juce::PathStrokeType { lineWidth });

    juce::Path needle;
    const auto needleWidth = lineWidth * 1.5f;
    const auto needleLen = radius + lineWidth * 0.3f;
    needle.addRoundedRectangle (centre.x - needleWidth * 0.5f,
                                centre.y + needleWidth * 0.5f - needleLen,
                                needleWidth,
                                needleLen,
                                needleWidth * 0.5f);
    g.setColour (findColour (needleColorId));
    g.fillPath (needle, juce::AffineTransform::rotation (toAngle, centre.x, centre.y));
}

void Slider::createBorder (const juce::Rectangle<float>& bounds)
{
    const auto area = bounds.reduced (borderThickness * 0.5f);
    auto radian = 0.0f;

    for (const auto& corner : { area.getTopLeft(), area.getTopRight(), area.getBottomRight(), area.getBottomLeft() })
    {
        constexpr auto length = 5.0f;
        juce::Path p;
        p.startNewSubPath (corner.x, corner.y + length);
        p.lineTo (corner);
        p.lineTo (corner.x + length, corner.y);
        p.applyTransform (juce::AffineTransform::rotation (radian, corner.x, corner.y));
        borderPath.addPath (p);

        radian += juce::MathConstants<float>::halfPi;
    };
}
