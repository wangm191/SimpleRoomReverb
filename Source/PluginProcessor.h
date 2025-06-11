/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

struct Settings {
    float size { 0 };
    float damp { 0 };
    float width { 0 };
    float wetLevel { 0 };
    float dryLevel { 0 };
    bool freeze { false };
    float lowPassFreq { 0 };
    float highPassFreq { 0 };
    bool bypass { false };
};

Settings getSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class SimpleRoomReverbAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleRoomReverbAudioProcessor();
    ~SimpleRoomReverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    juce::AudioProcessorValueTreeState& getPluginState();
    
private:
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;
    
    juce::AudioParameterFloat* size { nullptr };
    juce::AudioParameterFloat* damp { nullptr };
    juce::AudioParameterFloat* width { nullptr };
    juce::AudioParameterFloat* mix { nullptr };
    juce::AudioParameterBool* freeze { nullptr };
    juce::AudioParameterFloat* lowPass { nullptr };
    juce::AudioParameterFloat* highPass { nullptr };
    juce::AudioParameterBool* bypass { nullptr };
    
    bool firstTimeInitializing;
    
    bool isBypassed();
    void updateFilters();
    void updateReverbParameters();
    
    float lastSampleRate;
    
    Settings settings;
    
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParameters;
    
    juce::dsp::StateVariableTPTFilter<float> lowPassFilter;
    juce::dsp::StateVariableTPTFilter<float> highPassFilter;
    
    juce::UndoManager undoManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleRoomReverbAudioProcessor)
};
