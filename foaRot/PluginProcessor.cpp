/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FoaRotAudioProcessor::FoaRotAudioProcessor() : parameters(*this, nullptr)
#ifndef JucePlugin_PreferredChannelConfigurations
, AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::ambisonic(1), true)
#endif
                  .withOutput ("Output", AudioChannelSet::ambisonic(1), true)
#endif
                  )
#endif
{
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>("azimuth",       // parameter ID
                                                                           "azimuth",       // parameter name
                                                                           NormalisableRange<float> (-180.0f, 180.0f),      // range
                                                                           0.0f,         // default value
                                                                           "degrees"));
    
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>("elevation",       // parameter ID
                                                                           "elevation",       // parameter name
                                                                           NormalisableRange<float> (-180.0f, 180.0f),      // range
                                                                           0.0f,         // default value
                                                                           "degrees"));
    
    parameters.state = ValueTree (Identifier ("FoaRotVT"));
}

FoaRotAudioProcessor::~FoaRotAudioProcessor()
{
}

//==============================================================================
const String FoaRotAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FoaRotAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FoaRotAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool FoaRotAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double FoaRotAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FoaRotAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int FoaRotAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FoaRotAudioProcessor::setCurrentProgram (int index)
{
}

const String FoaRotAudioProcessor::getProgramName (int index)
{
    return {};
}

void FoaRotAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FoaRotAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    azimuth = 0;
    elevation = 0;
    twopi = 8.0 * atan(1.0);
}

void FoaRotAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FoaRotAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return false;
}
#endif

void FoaRotAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
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
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    /*
     
     This takes 4 channels in ACN order and spits back out 4 new channels after rotation.
     
     Eq:
     
     yaw (Z) : X' = X * cos(a) - Y * sin(a) & Y' = X * sin(a) + Y * cos(a)
     pitch(Y) : X' = X * cos(b) - Z * sin(b) & Z' = X * sin(b) + Z * cos(b)
     roll (X) : Y’ = Y * cos(b) – Z * sin(b) & Z’ = Y * sin(b) + Z * cos(b)
     
     where b is elevation
     */
    
    int index;
    
    azimuth = *parameters.getRawParameterValue("azimuth");
    azimuth = azimuth * (M_PI / 180);//convert to radians
    
    elevation = *parameters.getRawParameterValue("elevation");
    elevation = elevation * (M_PI / 180);
    
    //make 4 write pointers (output)
    auto* outDataW = buffer.getWritePointer (0);
    auto* outDataY = buffer.getWritePointer (1);
    auto* outDataZ = buffer.getWritePointer (2);
    auto* outDataX = buffer.getWritePointer (3);
    
    //make 4 read pointers (input)
    auto* inDataW = buffer.getReadPointer (0);
    auto* inDataY = buffer.getReadPointer (1);
    auto* inDataZ = buffer.getReadPointer (2);
    auto* inDataX = buffer.getReadPointer (3);
    
    //traverse samples
    for(index = 0; index < buffer.getNumSamples(); index++) {
        
        outDataW[index] = inDataW[index]; //W is the same
        
        // Y = y * cos(e) – z * sin(e) + x * sin(a) + y * cos(a)
        outDataY[index] = inDataY[index] * cos(elevation) - inDataZ[index] * sin(elevation) +
                          inDataX[index] * sin(azimuth) + inDataY[index] * cos(azimuth);
        
        // Z = x * sin(e) + z * cos(e) + y * sin(e) + z * cos(e)
        outDataZ[index] = inDataX[index] + sin(elevation) + inDataZ[index] * cos(elevation) +
                          inDataY[index] * sin(elevation) + inDataZ[index] * cos(elevation) ;
        
        // X = x * cos(a) - y * sin(a) + x * cos(e) - z * sin(e)
        outDataX[index] = inDataX[index] * cos(azimuth) - inDataY[index] * sin(azimuth) +
                          inDataX[index] * cos(elevation) - inDataZ[index] * sin(elevation);
    }
}

//==============================================================================
bool FoaRotAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FoaRotAudioProcessor::createEditor()
{
    return new FoaRotAudioProcessorEditor (*this);
}

//==============================================================================
void FoaRotAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FoaRotAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FoaRotAudioProcessor();
}
