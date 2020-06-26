/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class QuadBinDec2AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    QuadBinDec2AudioProcessor();
    ~QuadBinDec2AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void process(dsp::ProcessContextReplacing<float> context);
    void updateParameters();
    void applyGain(AudioBuffer<float> buffer, int gain1, int gain2, int gain3, int gain4);
    //front left (LR), back left (LR), back right (LR), front right (LR) convs
    //this could be likely optimized, for now I am happy if it just works.
    
    dsp::Convolution FL_Conv, BL_Conv, BR_Conv, FR_Conv;
    
    dsp::ProcessSpec spec;//my process specification
    
    AudioBuffer<float> FLbuffer, BLbuffer, BRbuffer, FRbuffer, outputBuffer;
    float wsamp, xsamp, ysamp, FLsamp, BLsamp, BRsamp, FRsamp;
    //use these buffers to copy each channel.
    //AudioBuffer<float> workingBufferW, workingBufferY, workingBufferX; //no need for Z
    
    //AudioEngine audioEngine;
    
private:
    
    //AudioBuffer<float> copyBufferW;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (QuadBinDec2AudioProcessor)
};
