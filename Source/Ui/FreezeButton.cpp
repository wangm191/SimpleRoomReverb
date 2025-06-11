/*
  ==============================================================================

    FreezeButton.cpp
    Created: 13 Apr 2025 3:55:56pm
    Author:  Myles Wang

  ==============================================================================
*/

#include "FreezeButton.h"
#include "UseColors.h"

FreezeButton::FreezeButton (juce::RangedAudioParameter& param, juce::UndoManager* um)
: audioParam(param)
, paramAttachment (audioParam, [&] (float v) { updateState (static_cast<bool> (v)); }, um)
{
    setWantsKeyboardFocus (true);
    setRepaintsOnMouseActivity (true);
    setColour(onColorId, UseColors::red);
    setColour(offColorId, UseColors::yellow);
    setColour(focusColorId, UseColors::green);
    
    setLabelColor(UseColors::green);
    label.setText(audioParam.getName(8), juce::NotificationType::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setInterceptsMouseClicks (false, false);
    
    setTextBoxColor (UseColors::green);
    textBox.onTextChange = [&]
    {
        const auto newNormValue = audioParam.getValueForText (textBox.getText());
        const auto newDenormValue = audioParam.convertFrom0to1 (newNormValue);
        paramAttachment.setValueAsCompleteGesture (newDenormValue);
        textBox.setText (audioParam.getCurrentValueAsText(), juce::NotificationType::dontSendNotification);
    };
    
    addAndMakeVisible(label);
    addAndMakeVisible(textBox);
    
    paramAttachment.sendInitialUpdate();
};

void FreezeButton::resized()
{
    auto bounds = getLocalBounds().toFloat();
    
    borderPath.clear();
    createBorder(bounds);
    
    const auto subAreaHeight = bounds.getHeight() / 4.0f;
    
    label.setBounds(bounds.removeFromTop(subAreaHeight).toNearestInt());
    label.setFont(juce::FontOptions { static_cast<float>(label.getHeight()) * 0.7f });
    
    textBox.setBounds (bounds.removeFromBottom (subAreaHeight).toNearestInt());
    textBox.setFont (juce::FontOptions { static_cast<float> (textBox.getHeight()) * 0.7f });
    
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(10);
    bounds.removeFromRight(10);
    
    mainArea = bounds.expanded (1.0f).withY (bounds.getY() + 1);
}

void FreezeButton::paint(juce::Graphics& g)
{
    
    drawRectangle(g);
    
    if ( hasKeyboardFocus(true))
    {
        g.setColour(findColour(focusColorId));
        g.strokePath(borderPath, juce::PathStrokeType {borderThickness});
    }
}

void FreezeButton::mouseUp(const juce::MouseEvent& event)
{
    juce::ignoreUnused (event);
    
    paramAttachment.setValueAsCompleteGesture(! state);
    
    textBox.setText (audioParam.getCurrentValueAsText(), juce::NotificationType::dontSendNotification);
}

void FreezeButton::focusGained (FocusChangeType cause)
{
    juce::ignoreUnused (cause);
    repaint();
}

void FreezeButton::focusLost (FocusChangeType cause)
{
    juce::ignoreUnused (cause);
    repaint();
}

void FreezeButton::setTextBoxColor (juce::Colour newColor)
{
    textBox.setColour (juce::Label::textColourId, newColor);
    textBox.setColour (juce::Label::textWhenEditingColourId, newColor);
}

void FreezeButton::setLabelColor (juce::Colour newColor)
{
    label.setColour (juce::Label::textColourId, newColor);
}

bool FreezeButton::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::returnKey)
    {
        paramAttachment.setValueAsCompleteGesture(! state);
        return true;
    }
    
    return false;
}

void FreezeButton::updateState(bool newState)
{
    state = newState;
    textBox.setText (audioParam.getCurrentValueAsText(), juce::NotificationType::dontSendNotification);
    repaint();
}

void FreezeButton::drawRectangle(juce::Graphics &g)
{
    auto fillColor = state ? findColour(onColorId) : findColour(offColorId);
    g.setColour(fillColor);
    
    constexpr float cornerRadius = 6.0f;
    g.fillRoundedRectangle(mainArea, cornerRadius);
}

void FreezeButton::createBorder(const juce::Rectangle<float> &bounds)
{
    const auto area = bounds.reduced(borderThickness);
    
    auto radian = 0.0f;
    
    for (const auto& corner : { area.getTopLeft(), area.getTopRight(), area.getBottomRight(), area.getBottomLeft() })
    {
        constexpr auto length = 5.0f;
        juce::Path p;
        p.startNewSubPath(corner.x, corner.y + length);
        p.lineTo(corner);
        p.lineTo(corner.x + length, corner.y);
        p.applyTransform(juce::AffineTransform::rotation(radian, corner.x, corner.y));
        borderPath.addPath(p);
        
        radian += juce::MathConstants<float>::halfPi;
    };
}
