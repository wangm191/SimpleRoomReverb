/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//#include "EditorContent.h"
//#include "EditorResize.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/**
*/
class SimpleRoomReverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleRoomReverbAudioProcessorEditor (SimpleRoomReverbAudioProcessor& p, juce::UndoManager& um);
    ~SimpleRoomReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    bool keyPressed (const juce::KeyPress& k) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    juce::UndoManager& undoManager;
    
    //EditorContent editorContent;
    
    static constexpr auto defaultWidth { 690 };
    static constexpr auto defaultHeight { 250 };
    
//    struct SharedLnf
//       {
//            SharedLnf() { juce::LookAndFeel::setDefaultLookAndFeel (&editorResize); }
//           ~SharedLnf() { juce::LookAndFeel::setDefaultLookAndFeel (nullptr); }
//
//        EditorResize editorResize;
//       };

//       juce::SharedResourcePointer<SharedLnf> lnf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleRoomReverbAudioProcessorEditor)
};
