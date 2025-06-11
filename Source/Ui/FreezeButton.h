/*
  ==============================================================================

    FreezeButton.h
    Created: 13 Apr 2025 3:55:51pm
    Author:  Myles Wang

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "UseColors.h"

class FreezeButton : public juce::Component
{
public:
    
    enum colorIds
    {
        onColorId,
        offColorId,
        focusColorId
    };
    
    explicit FreezeButton(juce::RangedAudioParameter& param, juce::UndoManager* um = nullptr);
    
    void paint (juce::Graphics& g) override;
    
    void resized() override;
    
    void mouseUp (const juce::MouseEvent& event) override;
    
    void focusGained (FocusChangeType cause) override;
    void focusLost (FocusChangeType cause) override;
    
    bool keyPressed (const juce::KeyPress& key) override;
    
    void setTextBoxColor (juce::Colour newColor);
    void setLabelColor (juce::Colour newColor);
    void setLabelText (const juce::String& newLabelText);
    
private:
    void updateState (bool newState);
    
    bool state { false };
    
    void drawRectangle (juce::Graphics& g);
    void createBorder (const juce::Rectangle<float>& bounds);
    
    juce::RangedAudioParameter& audioParam;
    juce::ParameterAttachment paramAttachment;
    
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

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FreezeButton)
};
