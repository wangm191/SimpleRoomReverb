/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//#include "UseColors.h"

//==============================================================================
SimpleRoomReverbAudioProcessorEditor::SimpleRoomReverbAudioProcessorEditor (SimpleRoomReverbAudioProcessor& p, juce::UndoManager& um)
    : AudioProcessorEditor (&p)
    , undoManager (um)
    //, editorContent(p, um)
{
    constexpr auto ratio = static_cast<double> (defaultWidth) / defaultHeight;
    setResizable (false, true);
    getConstrainer()->setFixedAspectRatio (ratio);
    getConstrainer()->setSizeLimits (defaultWidth, defaultHeight, defaultWidth * 2, defaultHeight * 2);
    setSize (defaultWidth, defaultHeight);
    //editorContent.setSize (defaultWidth, defaultHeight);

    //addAndMakeVisible (editorContent);
}

SimpleRoomReverbAudioProcessorEditor::~SimpleRoomReverbAudioProcessorEditor()
{
}

//==============================================================================
void SimpleRoomReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(UseColors::blue);
    
}

void SimpleRoomReverbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    //const auto factor = static_cast<float>(getWidth() / defaultWidth);
    //editorContent.setTransform (juce::AffineTransform::scale (factor));
}

bool SimpleRoomReverbAudioProcessorEditor::keyPressed (const juce::KeyPress& k)
{
    if (k.isKeyCode ('Z') && k.getModifiers().isCommandDown())
    {
        if (k.getModifiers().isShiftDown())
            undoManager.redo();
        else
            undoManager.undo();

        return true;
    }

    return false;
}
