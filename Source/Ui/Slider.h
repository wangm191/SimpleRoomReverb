/*
  ==============================================================================

    Slider.h
    Created: 13 Apr 2025 3:52:57pm
    Author:  Myles Wang

  ==============================================================================
*/

#pragma once

#include "UseColors.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class Slider final : public juce::Component
{
    public:
        enum colorIds
    {
            foregroundArcColorId,
            backgroundArcColorId,
            needleColorId,
            borderColorId
    };
    
    explicit Slider (juce::RangedAudioParameter& param, juce::UndoManager* um = nullptr);
    
    void paint (juce::Graphics& g) override;
    void resized () override;
    
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseDoubleClick (const juce::MouseEvent& e) override;
    
    bool keyPressed (const juce::KeyPress& k) override;
    
    void focusGained (FocusChangeType cause) override;
    void focusLost (FocusChangeType cause) override;
    
    float getValue() const;
    
    void setInterval (float newInterval);
    void setFineInterval (float newFineInterval);
    
    void setTextBoxColor (juce::Colour newColor);
    void setLabelColor (juce::Colour newColor);
    void setLabelText (const juce::String& newLabelText);
    
    private:
    void updateValue (float newValue);
    
    void drawSlider (juce::Graphics& g);
    void createBorder (const juce::Rectangle<float>& bounds);
    
    juce::RangedAudioParameter& audioParam;
    juce::ParameterAttachment paramAttachment;

    float value {};
    
    static constexpr auto sensitivity { 0.01f };
    float interval { 1.0f };
    float fineInterval { 0.1f };

    static constexpr auto startAngle { juce::MathConstants<float>::pi + juce::MathConstants<float>::pi / 6.0f };
    static constexpr auto endAngle { 3.0f * juce::MathConstants<float>::pi - juce::MathConstants<float>::pi / 6.0f };

    juce::Point<float> mousePosWhenLastDragged;
    juce::Rectangle<float> mainArea;
    juce::Path borderPath;
    static constexpr auto borderThickness { 1.5f };

    juce::Label label;
    
    struct TextBox final : public juce::Label
      {
          juce::String valueShownWithEditor {};

          TextBox()
          {
              setJustificationType (juce::Justification::centred);
              setInterceptsMouseClicks (false, false);
              setColour (juce::Label::outlineWhenEditingColourId, juce::Colours::transparentWhite);
          }

          juce::TextEditor* createEditorComponent() override
          {
              auto* ed = juce::Label::createEditorComponent();

              ed->setJustification (juce::Justification::centred);
              ed->setColour (juce::TextEditor::backgroundColourId, juce::Colours::transparentWhite);
              ed->setColour (juce::CaretComponent::caretColourId, UseColors::red);
              ed->setInputRestrictions (5, "-0123456789.");
              ed->setIndents (4, 1);
              ed->onTextChange = [] { juce::Desktop::getInstance().getMainMouseSource().hideCursor(); };

              return ed;
          }

          void editorShown (juce::TextEditor* ed) override
          {
              ed->clear();
              ed->setText (valueShownWithEditor);
          }
      };

      TextBox textBox;

      JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Slider)
};

