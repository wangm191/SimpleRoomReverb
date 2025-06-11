/*
  ==============================================================================

    BypassButton.h
    Created: 3 Jun 2025 10:37:06pm
    Author:  Myles Wang

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class BypassButton final : public juce::TextButton
{
public:
    
    BypassButton(juce::RangedAudioParameter& param, juce::UndoManager* um = nullptr);
    ~BypassButton() override;
    
private:
    void updateState (bool newState);
    
    void onClicked();
    
    juce::RangedAudioParameter& audioParam;
    juce::ParameterAttachment paramAttachment;
    bool state = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BypassButton)
};
