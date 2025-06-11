/*
  ==============================================================================

    EditorContent.h
    Created: 12 Apr 2025 4:39:34pm
    Author:  Myles Wang

  ==============================================================================
*/

#pragma once

#include "../PluginProcessor.h"
#include "UndoManagerButton.h"
#include "BypassButton.h"
#include "FreezeButton.h"
#include "Slider.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class EditorContent final : public juce::Component
{
public:
    EditorContent (SimpleRoomReverbAudioProcessor& p, juce::UndoManager& um);
    
    void resized() override;
    bool keyPressed (const juce::KeyPress& k) override;
    
private:
    juce::AudioProcessorValueTreeState& apvts;
    
    Slider sizeSlider;
    Slider dampSlider;
    Slider widthSlider;
    Slider mixSlider;
    Slider lowPassSlider;
    Slider highPassSlider;
    
    FreezeButton freezeButton;
    
    BypassButton bypassButton;
    
    UndoManagerButton undoButton;
    UndoManagerButton redoButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorContent)
};

