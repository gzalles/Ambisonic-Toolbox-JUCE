/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
QuadBinDec2AudioProcessor::QuadBinDec2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::ambisonic(1), true)//4 channels
#endif
                  .withOutput ("Output", AudioChannelSet::stereo(), true)//2 channels
#endif
                  )
#endif
{
}

QuadBinDec2AudioProcessor::~QuadBinDec2AudioProcessor()
{
}

//==============================================================================
const String QuadBinDec2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool QuadBinDec2AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool QuadBinDec2AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool QuadBinDec2AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double QuadBinDec2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int QuadBinDec2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int QuadBinDec2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void QuadBinDec2AudioProcessor::setCurrentProgram (int index)
{
}

const String QuadBinDec2AudioProcessor::getProgramName (int index)
{
    return {};
}

void QuadBinDec2AudioProcessor::changeProgramName (int index, const String& newName)
{
}

void QuadBinDec2AudioProcessor::updateParameters()
{
    //update params for processor? //audio programmer
}

void QuadBinDec2AudioProcessor::process(dsp::ProcessContextReplacing<float> context)
{
    //do processing //audio programmer
}

//==============================================================================
void QuadBinDec2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    //specs for conv engines
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;
    //spec.numChannels = getTotalNumOutputChannels();//? 
    
    //pass process specifications
    FL_Conv.prepare(spec);
    BL_Conv.prepare(spec);
    BR_Conv.prepare(spec);
    FR_Conv.prepare(spec);
    
    //load our 8 HRTFs (4 stereo files)
    //p: file, file size, wants stereo, wants trimming, size after loading (0 = full), normalize
    bool stereo = true;
    bool trim = false;
    bool norm = true; //normalization
    int IRlen = samplesPerBlock; //0 = full
    
    //real
    
    FL_Conv.loadImpulseResponse(BinaryData::  _45_wav,  BinaryData::  _45_wavSize, stereo, trim, IRlen, norm);//_45
    BL_Conv.loadImpulseResponse(BinaryData:: _135_wav,  BinaryData:: _135_wavSize, stereo, trim, IRlen, norm);//_135
    BR_Conv.loadImpulseResponse(BinaryData:: _225_wav,  BinaryData:: _225_wavSize, stereo, trim, IRlen, norm);//_225
    FR_Conv.loadImpulseResponse(BinaryData:: _315_wav,  BinaryData:: _315_wavSize, stereo, trim, IRlen, norm);//_315

    FLbuffer = AudioBuffer<float>(2, samplesPerBlock);
    BLbuffer = AudioBuffer<float>(2, samplesPerBlock);
    BRbuffer = AudioBuffer<float>(2, samplesPerBlock);
    FRbuffer = AudioBuffer<float>(2, samplesPerBlock);
    
    outputBuffer = AudioBuffer<float>(2, samplesPerBlock);
    
    wsamp = xsamp = ysamp = FLsamp = BLsamp = BRsamp = FRsamp = 0.0f;
}

void QuadBinDec2AudioProcessor::releaseResources()
{
    //Reset the convolution engines
    FL_Conv.reset();
    BL_Conv.reset();
    BR_Conv.reset();
    FR_Conv.reset();
    
    //simpleConvolution is also empty
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool QuadBinDec2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return true;
}
#endif

void QuadBinDec2AudioProcessor::applyGain(AudioBuffer<float> buffer, int gain1, int gain2, int gain3, int gain4)
{
    
}

void QuadBinDec2AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    //process(dsp::ProcessContextReplacing<float> (block)); //audio programmer
    
    //create write pointers for left and right output channels
    auto* leftOutput = buffer.getWritePointer (0);//0L, 1R
    auto* rightOutput = buffer.getWritePointer (1);//0L, 1R
    
    //apply init dec mat (FL chan = 0)
    buffer.applyGain(0, 0, buffer.getNumSamples(), 0.25f);
    buffer.applyGain(1, 0, buffer.getNumSamples(), 0.17f);
    buffer.applyGain(2, 0, buffer.getNumSamples(), 0.00f);
    buffer.applyGain(3, 0, buffer.getNumSamples(), 0.17f);
    //apply dec mat to other chans, set samples inside buffers
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        wsamp = buffer.getSample(0, i);//w
        ysamp = buffer.getSample(1, i);//y
        xsamp = buffer.getSample(3, i);//x
        //BL 3-, BR 1-, 3-, FR 1-
        FLbuffer.setSample(0, i, wsamp+ysamp+xsamp);
        BLbuffer.setSample(0, i, wsamp+ysamp+xsamp*(-1.0f));
        BRbuffer.setSample(0, i, wsamp+ysamp*(-1.0f)+xsamp*(-1.0f));
        FRbuffer.setSample(0, i, wsamp+ysamp*(-1.0f)+xsamp);
    }
    //copy samples from left channel to right
    FLbuffer.copyFrom(1, 0, FLbuffer, 0, 0, buffer.getNumSamples());
    BLbuffer.copyFrom(1, 0, BLbuffer, 0, 0, buffer.getNumSamples());
    BRbuffer.copyFrom(1, 0, BRbuffer, 0, 0, buffer.getNumSamples());
    FRbuffer.copyFrom(1, 0, FRbuffer, 0, 0, buffer.getNumSamples());
    //get pointer to buffer
    dsp::AudioBlock<float> FLblock = dsp::AudioBlock<float>(FLbuffer);
    dsp::AudioBlock<float> BLblock = dsp::AudioBlock<float>(BLbuffer);
    dsp::AudioBlock<float> BRblock = dsp::AudioBlock<float>(BRbuffer);
    dsp::AudioBlock<float> FRblock = dsp::AudioBlock<float>(FRbuffer);
    //create contexts and process buffers
    dsp::ProcessContextReplacing<float> FLcontext = dsp::ProcessContextReplacing<float>(FLblock);
    dsp::ProcessContextReplacing<float> BLcontext = dsp::ProcessContextReplacing<float>(BLblock);
    dsp::ProcessContextReplacing<float> BRcontext = dsp::ProcessContextReplacing<float>(BRblock);
    dsp::ProcessContextReplacing<float> FRcontext = dsp::ProcessContextReplacing<float>(FRblock);
    //convolving
    FL_Conv.process(FLcontext);
    BL_Conv.process(BLcontext);
    BR_Conv.process(BRcontext);
    FR_Conv.process(FRcontext);
    //get output block from engine
    dsp::AudioBlock<float> outputBlockFL = FLcontext.getOutputBlock();
    dsp::AudioBlock<float> outputBlockBL = BLcontext.getOutputBlock();
    dsp::AudioBlock<float> outputBlockBR = BRcontext.getOutputBlock();
    dsp::AudioBlock<float> outputBlockFR = FRcontext.getOutputBlock();
    
    
    //outputBlockFL.copyTo(buffer, 0, 0, buffer.getNumSamples()); //this "works"
    
    //setSample as sum of outputs?
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        for (int j = 0; j < 2; j++)//channels
        {
            FLsamp = outputBlockFL.getSample(j, i);
            BLsamp = outputBlockBL.getSample(j, i);
            BRsamp = outputBlockBR.getSample(j, i);
            FRsamp = outputBlockFR.getSample(j, i);
            
            buffer.setSample(j, i, FLsamp+BLsamp+BRsamp+FRsamp);
        }
    } //it works!, i think...
    
    
//    //create pointer to outputBlock
//    dsp::AudioBlock<float> outputBlock = dsp::AudioBlock<float>(outputBuffer);
//    //get output block samples from process (L and R channel)
//    for (int i = 0; i < buffer.getNumSamples(); i++)//sampleIndex
//    {
//        for (int j = 0; j < 2; j++ )//channels
//        {
//            //addSample (int destChannel, int destSample, SampleType valueToAdd)
//            //getSample (int channel, int sampleIndex)
//            outputBlock.addSample(j, i, outputBlockFL.getSample(j, i));
////            outputBlock.addSample(j, i, outputBlockBL.getSample(j, i));
////            outputBlock.addSample(j, i, outputBlockBR.getSample(j, i));
////            outputBlock.addSample(j, i, outputBlockFR.getSample(j, i));
//        }
//    }
//
//    //copy samples from outputBuffer var to output using write pointers
//    for (int i = 0; i < buffer.getNumSamples(); i++)
//    {
//        leftOutput[i] = outputBuffer.getSample(0, i);//L
//        rightOutput[i] = outputBuffer.getSample(1, i);//R
//    } //dont do this, it caused feedback, just use the buffer that gets passed in.

  //////////////////////////////////////////////////////////////////////////
//    dsp::AudioBlock<float> block = dsp::AudioBlock<float>(buffer);
//    dsp::AudioBlock<float> blockw = block.getSingleChannelBlock(0);//W
//    dsp::AudioBlock<float> blocky = block.getSingleChannelBlock(1);//Y
//    dsp::AudioBlock<float> blockx = block.getSingleChannelBlock(3);//X
//    dsp::AudioBlock<float> block2 = block.getSubsetChannelBlock(0, 2);//WY (stereo)
//
////  dsp::AudioBlock<float> blockYY = block.getSubsetChannelBlock(1, 2);
////  dsp::AudioBlock<float> blockXX = block.getSubsetChannelBlock(0, 2);
//
//    for (int i = 0; i < buffer.getNumSamples(); i++)
//    {
//        float wsamp = blockw.getSample(0, i);
//        float ysamp = blocky.getSample(0, i);
//        float xsamp = blockx.getSample(0, i);
//        block2.setSample(0, i, wsamp+ysamp+xsamp);//W
//        block2.setSample(1, i, wsamp+ysamp+xsamp);//W -probably wrong
//    }
    
}

//==============================================================================
bool QuadBinDec2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* QuadBinDec2AudioProcessor::createEditor()
{
    return new QuadBinDec2AudioProcessorEditor (*this);
}

//==============================================================================
void QuadBinDec2AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void QuadBinDec2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new QuadBinDec2AudioProcessor();
}
