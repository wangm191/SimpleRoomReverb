/*
  ==============================================================================

    BypassButton.cpp
    Created: 3 Jun 2025 10:37:06pm
    Author:  Myles Wang

  ==============================================================================
*/

#include "BypassButton.h"
#include "UseColors.h"

BypassButton::BypassButton (juce::RangedAudioParameter& param, juce::UndoManager* um)
    : juce::TextButton("Bypass")
    , audioParam(param)
    , paramAttachment (audioParam, [this](float v) { updateState (static_cast<bool> (v)); }, um)
{
    onClick = [this]() { onClicked(); };
    setColour(buttonColourId, UseColors::red);
    paramAttachment.sendInitialUpdate();
};

BypassButton::~BypassButton() = default;

void BypassButton::onClicked()
{
    const bool newState = !state;
    paramAttachment.setValueAsCompleteGesture(static_cast<float>(newState));
}

void BypassButton::updateState(bool newState)
{
    state = newState;
    setColour(buttonColourId, state ? UseColors::red : UseColors::green);
    setColour(textColourOffId, state ? UseColors::yellow : UseColors::blue);
    repaint();
}


