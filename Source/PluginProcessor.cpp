/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    const auto percentageLabels = juce::AudioParameterFloatAttributes().withStringFromValueFunction (
            // Format the number to always display three digits like "0.01 %", "10.0 %", "100 %".
            [] (auto value, auto)
            {
                constexpr auto unit = " %";

                if (auto v { std::round (value * 100.0f) / 100.0f }; v < 10.0f)
                    return juce::String { v, 2 } + unit;

                if (auto v { std::round (value * 10.0f) / 10.0f }; v < 100.0f)
                    return juce::String { v, 1 } + unit;

                return juce::String { std::round ( value ) } + unit;
            });
    
    const auto frequencyLabels = juce::AudioParameterFloatAttributes().withStringFromValueFunction (
            [] (auto value, auto)
            {
                constexpr auto unit = " Hz";
                return juce::String { std::round ( value ) } + unit;
            });
    
    layout.add(std::make_unique<juce::AudioParameterFloat>  (juce::ParameterID { Parameters::size, 1 },
                                                            Parameters::size,
                                                            juce::NormalisableRange { 0.0f, 100.0f, 0.01f, 1.0f },
                                                            50.0f,
                                                            percentageLabels));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>  (juce::ParameterID { Parameters::damp, 1 },
                                                            Parameters::damp,
                                                            juce::NormalisableRange { 0.0f, 100.0f, 0.01f, 1.0f },
                                                            50.0f,
                                                            percentageLabels));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>  (juce::ParameterID { Parameters::width, 1 },
                                                            Parameters::width,
                                                            juce::NormalisableRange { 0.0f, 100.0f, 0.01f, 1.0f },
                                                            50.0f,
                                                            percentageLabels));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>  (juce::ParameterID { Parameters::mix, 1 },
                                                            Parameters::mix,
                                                            juce::NormalisableRange { 0.0f, 100.0f, 0.01f, 1.0f },
                                                            50.0f,
                                                            percentageLabels));
    
    layout.add(std::make_unique<juce::AudioParameterBool>   (juce::ParameterID { Parameters::freeze, 1},
                                                            Parameters::freeze,
                                                            false));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>  (juce::ParameterID { Parameters::lowPass, 1},
                                                            Parameters::lowPass,
                                                            juce::NormalisableRange { 20.0f, 20000.0f, 1.0f, 0.2f},
                                                            20000.0f,
                                                            frequencyLabels));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>  (juce::ParameterID { Parameters::highPass, 1},
                                                            Parameters::highPass,
                                                            juce::NormalisableRange { 20.0f, 20000.0f, 1.0f, 0.2f},
                                                            20.0f,
                                                            frequencyLabels));
    
    layout.add(std::make_unique<juce::AudioParameterBool>   (juce::ParameterID { Parameters::bypass, 1},
                                                            Parameters::bypass,
                                                            false));
    
    return layout;
}

//==============================================================================
SimpleRoomReverbAudioProcessor::SimpleRoomReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts (*this, &undoManager, "pluginParameters", createParameterLayout())
#endif
{
}

SimpleRoomReverbAudioProcessor::~SimpleRoomReverbAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleRoomReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleRoomReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleRoomReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleRoomReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleRoomReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleRoomReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleRoomReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleRoomReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleRoomReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleRoomReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleRoomReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    lastSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec {};
    
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    reverb.prepare(spec);
    lowPassFilter.prepare(spec);
    highPassFilter.prepare(spec);
    
    if (firstTimeInitializing) {
        reverb.reset();
        lowPassFilter.reset();
        highPassFilter.reset();
        
        firstTimeInitializing = false;
    }
}

void SimpleRoomReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleRoomReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

Settings getSettings(juce::AudioProcessorValueTreeState& apvts){
    Settings settings;
    
    // Multiply by 0.01 for scaling refactor as juce::dsp::Reverb::Parameters expects a value between 0.0 and 1.0.
    settings.size = apvts.getRawParameterValue(Parameters::size)->load() * 0.01f;
    settings.damp = apvts.getRawParameterValue(Parameters::damp)->load() * 0.01f;
    settings.width = apvts.getRawParameterValue(Parameters::width)->load() * 0.01f;
    settings.wetLevel = apvts.getRawParameterValue(Parameters::mix)->load() * 0.01f;
    settings.dryLevel = 1.0f - settings.wetLevel;
    settings.freeze = apvts.getRawParameterValue(Parameters::freeze)->load();
    settings.lowPassFreq = apvts.getRawParameterValue(Parameters::lowPass)->load();
    settings.highPassFreq = apvts.getRawParameterValue(Parameters::highPass)->load();
    settings.bypass = apvts.getRawParameterValue(Parameters::bypass)->load();
    
    return settings;
}

bool SimpleRoomReverbAudioProcessor::isBypassed(){
    auto settings = getSettings(apvts);
    return settings.bypass;
}

void SimpleRoomReverbAudioProcessor::updateFilters() {
    auto settings = getSettings(apvts);
    
    float resonance = 1 / sqrt(2);
    float lowPassFreq = settings.lowPassFreq;
    float highPassFreq = settings.highPassFreq;
    
    lowPassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    lowPassFilter.setResonance(resonance);
    lowPassFilter.setCutoffFrequency(lowPassFreq);
    
    highPassFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highPassFilter.setResonance(resonance);
    highPassFilter.setCutoffFrequency(highPassFreq);
}

void SimpleRoomReverbAudioProcessor::updateReverbParameters() {
    auto settings = getSettings(apvts);
    
    reverbParameters.roomSize = settings.size;
    reverbParameters.damping = settings.damp;
    reverbParameters.width = settings.width;
    reverbParameters.wetLevel = settings.wetLevel;
    reverbParameters.dryLevel = settings.dryLevel;
    reverbParameters.freezeMode = settings.freeze;
    
    reverb.setParameters(reverbParameters);
}

void SimpleRoomReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateFilters();
    updateReverbParameters();
    
    if (isBypassed())
    {
        return;
    }

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    reverb.process(context);
    lowPassFilter.process(context);
    highPassFilter.process(context);
}

//==============================================================================
bool SimpleRoomReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleRoomReverbAudioProcessor::createEditor()
{
    return new SimpleRoomReverbAudioProcessorEditor (*this, undoManager);
    //return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleRoomReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream (mos);
}

void SimpleRoomReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    if (const auto tree = juce::ValueTree::readFromData(data, static_cast<size_t>(sizeInBytes)); tree.isValid())
        apvts.replaceState (tree);
        updateFilters();
        updateReverbParameters();
}

juce::AudioProcessorValueTreeState& SimpleRoomReverbAudioProcessor::getPluginState() { return apvts; }

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleRoomReverbAudioProcessor();
}
