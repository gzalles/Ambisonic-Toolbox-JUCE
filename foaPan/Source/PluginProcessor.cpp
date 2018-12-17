/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FoaPanAudioProcessor::FoaPanAudioProcessor() : parameters (*this, nullptr)
#ifndef JucePlugin_PreferredChannelConfigurations
, AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::mono(), true)
#endif
                  .withOutput ("Output", AudioChannelSet::ambisonic(1), true)
#endif
                  )
#endif
{
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>("azimuth",// parameter ID
                                                                           "azimuth",// parameter name
                                                                           NormalisableRange<float>
                                                                           (-180.0f, 180.0f),// range
                                                                           0.0f,// default value
                                                                           "degrees"));
    
    parameters.createAndAddParameter(std::make_unique<AudioParameterFloat>("elevation",// parameter ID
                                                                           "elevation",// parameter name
                                                                           NormalisableRange<float> (-180.0f, 180.0f),// range
                                                                           0.0f,// default value
                                                                           "degrees"));
    
    parameters.state = ValueTree (Identifier ("FoaPanVT"));
}

FoaPanAudioProcessor::~FoaPanAudioProcessor()
{
}

//==============================================================================
const String FoaPanAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FoaPanAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FoaPanAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool FoaPanAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double FoaPanAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FoaPanAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int FoaPanAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FoaPanAudioProcessor::setCurrentProgram (int index)
{
}

const String FoaPanAudioProcessor::getProgramName (int index)
{
    return {};
}

void FoaPanAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FoaPanAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    azimuth = 0;
    elevation = 0;
    twopi = 8.0f * atan(1.0f);
}

void FoaPanAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

//THIS MIGHT BE A PROBLEM
#ifndef JucePlugin_PreferredChannelConfigurations
bool FoaPanAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    
        return false;//force plug in to take one input and spit out 4
}
#endif

void FoaPanAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
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
    
    //we do have more outputs than inputs, leave this code here.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    /* [note: this is ACN order!!!]
     W: 1/k * s (1/sqrt(2))
     Y: 1/k * s(sin(a)cos(e))
     Z: 1/k * s(sin(e))
     X: 1/k * s(cos(a)cos(e))
     */
    
    //use this variable to traverse samples
    int index;
    
    //get the azi val from the slider
    azimuth = *parameters.getRawParameterValue("azimuth");
    azimuth = azimuth * (M_PI / 180.0f);//convert degrees to radians
    
    //get the elev val from the slider
    elevation = *parameters.getRawParameterValue("elevation");
    elevation = elevation * (M_PI / 180.0f);//convert degrees to radians
    
    //the code we wrote wants azimuth values increasing counterclockwise from 0 (at the center) to 359 (1 degree to the right). In order to compensate for that (and to keep our sliders) we multiply negative values by -1 and subtract 360 (and take the absolute value) for positive values.
    if (azimuth < 0) {
        azimuth = azimuth * -1.0f;
    }

    if (azimuth > 0) {
        azimuth = abs(azimuth - 360.0f);
    }
    
    //traverse the output channels
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        //get write pointers (output)
        auto* outData = buffer.getWritePointer(channel);
        //always reading from mono source (input)
        auto* inData = buffer.getReadPointer (0);
        
        //traverse samples
        for(index = 0; index < buffer.getNumSamples(); index++)
        {
            switch (channel) { //switch based on channel count
                case 0:
                    outData[index] = inData[index]*(1/(sqrt(2)));                       //W : ACN channel 0
                    break;
                case 1:
                    outData[index] = inData[index]*(sin(azimuth)*cos(elevation));       //Y : ACN channel 1
                    break;
                case 2:
                    outData[index] = inData[index]*(sin(elevation));                    //Z : ACN channel 2
                    break;
                case 3:
                    outData[index] = inData[index]*(cos(azimuth)*cos(elevation));       //X : ACN channel 3
                    break;
                default:
                    break;
            }
        }
    }
}

//==============================================================================
bool FoaPanAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FoaPanAudioProcessor::createEditor()
{
    return new FoaPanAudioProcessorEditor (*this);
}

//==============================================================================
void FoaPanAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FoaPanAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FoaPanAudioProcessor();
}
