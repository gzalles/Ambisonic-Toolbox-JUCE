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

//==============================================================================
void QuadBinDec2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    //use to pass a stereo signal to the convolver with one copy plus the original channel
    workingBufferW.setSize(2, samplesPerBlock);
    workingBufferY.setSize(2, samplesPerBlock);
    //dont need Z
    workingBufferX.setSize(2, samplesPerBlock);
    
    //specs for conv engines
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;//since we have stereo IRs
    
    //pass process specifications
    FL_ConvW.prepare(spec);BL_ConvW.prepare(spec); BR_ConvW.prepare(spec); FR_ConvW.prepare(spec);//W
    FL_ConvY.prepare(spec); BL_ConvY.prepare(spec); BR_ConvY.prepare(spec); FR_ConvY.prepare(spec);//Y
    // no Z
    FL_ConvX.prepare(spec); BL_ConvX.prepare(spec); BR_ConvX.prepare(spec); FR_ConvX.prepare(spec);//X
    
    //load our 8 HRTFs
    //p: file, file size, wants stereo, wants trimming, size after loading (0 = full), normalize
    bool norm = false; //leave as false!
    int IRlen = 0;
    
    //real
    FL_ConvW.loadImpulseResponse(BinaryData:: _45_wav, BinaryData:: _45_wavSize, true, false, IRlen, norm);//45
    BL_ConvW.loadImpulseResponse(BinaryData::_135_wav, BinaryData::_135_wavSize, true, false, IRlen, norm);//135
    BR_ConvW.loadImpulseResponse(BinaryData::_225_wav, BinaryData::_225_wavSize, true, false, IRlen, norm);//225
    FR_ConvW.loadImpulseResponse(BinaryData::_315_wav, BinaryData::_315_wavSize, true, false, IRlen, norm);//315
    
    FL_ConvY.loadImpulseResponse(BinaryData:: _45_wav, BinaryData:: _45_wavSize, true, false, IRlen, norm);//45
    BL_ConvY.loadImpulseResponse(BinaryData::_135_wav, BinaryData::_135_wavSize, true, false, IRlen, norm);//135
    BR_ConvY.loadImpulseResponse(BinaryData::_225_wav, BinaryData::_225_wavSize, true, false, IRlen, norm);//225
    FR_ConvY.loadImpulseResponse(BinaryData::_315_wav, BinaryData::_315_wavSize, true, false, IRlen, norm);//315
    //no Z
    FL_ConvX.loadImpulseResponse(BinaryData:: _45_wav, BinaryData:: _45_wavSize, true, false, IRlen, norm);//45
    BL_ConvX.loadImpulseResponse(BinaryData::_135_wav, BinaryData::_135_wavSize, true, false, IRlen, norm);//135
    BR_ConvX.loadImpulseResponse(BinaryData::_225_wav, BinaryData::_225_wavSize, true, false, IRlen, norm);//225
    FR_ConvX.loadImpulseResponse(BinaryData::_315_wav, BinaryData::_315_wavSize, true, false, IRlen, norm);//315
    
    //test
    //    FL_ConvW.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//45
    //    BL_ConvW.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//135
    //    BR_ConvW.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//225
    //    FR_ConvW.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//315
    //
    //    FL_ConvY.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//45
    //    BL_ConvY.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//135
    //    BR_ConvY.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//225
    //    FR_ConvY.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//315
    //
    //    //no Z
    //
    //    FL_ConvX.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//45
    //    BL_ConvX.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//135
    //    BR_ConvX.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//225
    //    FR_ConvX.loadImpulseResponse(BinaryData::test_wav, BinaryData::test_wavSize, true, false, 1024, norm);//315
    
}

void QuadBinDec2AudioProcessor::releaseResources()
{
    //Reset the convolution engines
    FL_ConvW.reset();BL_ConvW.reset();BR_ConvW.reset();FR_ConvW.reset();
    FL_ConvY.reset();BL_ConvY.reset();BR_ConvY.reset();FR_ConvY.reset();
    //no Z
    FL_ConvX.reset();BL_ConvX.reset();BR_ConvX.reset();FR_ConvX.reset();
    
    //simpleConvolution is also empty
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool QuadBinDec2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    
    //#if JucePlugin_IsMidiEffect
    //    ignoreUnused (layouts);
    //    return true;
    //#else
    //    // This is the place where you check if the layout is supported.
    //    // In this template code we only support mono or stereo.
    //    if (layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
    //        return false;
    //
    //    // This checks if the input layout matches the output layout (it should return true)
    //#if ! JucePlugin_IsSynth
    //    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    //        return true;
    //#endif
    //
    //    return true;
    //#endif
    return false;
}
#endif

void QuadBinDec2AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    //auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    int index;
    
    //create write pointers for left and right output channels
    auto* leftOutput = buffer.getWritePointer (0);//0L, 1R
    auto* rightOutput = buffer.getWritePointer (1);//0L, 1R
    
    //create read pointers
    //cycle through read pointers (input)
    auto* inDataW = buffer.getReadPointer (0);//0W, 1Y, 2Z, 3X
    auto* inDataY = buffer.getReadPointer (1);//0W, 1Y, 2Z, 3X
    auto* inDataZ = buffer.getReadPointer (2);//0W, 1Y, 2Z, 3X
    auto* inDataX = buffer.getReadPointer (3);//0W, 1Y, 2Z, 3X
    
    //there will be 4 input channels
    for (int channel = 0; channel < totalNumInputChannels; channel++)
    {
        //switch based on channel
        switch (channel) {
            case 0: { //W : coef = 0.25 FL+,BL+,BR+,FR+
                
                //"mute" channel
                /*
                 for(index = 0; index < buffer.getNumSamples(); index++) {
                 leftOutput[index] = leftOutput[index] + inDataW[index] * 0.0f;
                 rightOutput[index] = leftOutput[index] + inDataW[index] * 0.0f;
                 }
                 // */
                
                // /* (un)comment to debug
                
                dsp::AudioBlock<float> blockW(buffer);
                blockW.getSingleChannelBlock(channel);
                
                //multiply the block by 0.25 (since we have four channels)
                blockW.multiply(blockW, 0.25f);
                
                //create context information that is passed into the algorithm's process method
                dsp::ProcessContextReplacing<float> contextW(blockW);
                
                //convolve using our 8 HRTFs
                FL_ConvW.process(contextW);
                BL_ConvW.process(contextW);
                BR_ConvW.process(contextW);
                FR_ConvW.process(contextW);
                
                //*/
                
                break;
            }
            case 1: { //Y : coef = 0.1768 FL+,BL+,BR-,FR-
                // /*
                for(index = 0; index < buffer.getNumSamples(); index++) {
                    leftOutput[index] = leftOutput[index] + inDataY[index] * 0.0f;
                    rightOutput[index] = leftOutput[index] + inDataY[index] * 0.0f;
                }
                // */
                
                /*
                 //copy data from buffer's Y channel to working buffer twice
                 //params: dest chan, dest strt samp, source, num samples
                 //workingBufferY.copyFrom(0, 0, inDataY, buffer.getNumSamples());
                 //workingBufferY.copyFrom(1, 0, inDataY, buffer.getNumSamples());
                 
                 //create two audio blocks that points to the data in the buffer
                 dsp::AudioBlock<float> blockYPos(buffer);
                 dsp::AudioBlock<float> blockYNeg(buffer);
                 
                 blockYPos.getSingleChannelBlock(channel);
                 blockYNeg.getSingleChannelBlock(channel);
                 
                 //multiply one block by 0.1768, the other by -0.1768
                 blockYPos.multiply(blockYPos, +0.1768f);
                 blockYNeg.multiply(blockYNeg, -0.1768f);
                 
                 //create context information that is passed into the algorithm's process method (+/-)
                 dsp::ProcessContextReplacing<float> contextPosY(blockYPos);
                 dsp::ProcessContextReplacing<float> contextNegY(blockYNeg);
                 
                 //convolve using 8 HRTFs (FL+,BL+,BR-,FR-)
                 FL_ConvY.process(contextPosY);//+
                 BL_ConvY.process(contextPosY);//+
                 BR_ConvY.process(contextNegY);//-
                 FR_ConvY.process(contextNegY);//-
                 
                 //I am assuming JUCE will sum these outputs correspondingly (L & R)
                 
                 //clear the buffer since the dummyBuffer needs to be used again.
                 //workingBufferY.clear();
                 
                 // */
                
                break;
            }
            case 2: { //Z
                
                // buffer.clear(channel, 0, buffer.getNumSamples());
                
                //mute the channel ?
                // /*
                for(index = 0; index < buffer.getNumSamples(); index++) {
                    leftOutput[index] = leftOutput[index] + inDataZ[index] * 0.0f;
                    rightOutput[index] = leftOutput[index] + inDataZ[index] * 0.0f;
                }
                // */
                break;
            }
            case 3: {//X : coef = 0.1768 FL+,BL-,BR-,FR+
                
                // /*
                for(index = 0; index < buffer.getNumSamples(); index++) {
                    leftOutput[index] = leftOutput[index] + inDataX[index] * 0.0f;
                    rightOutput[index] = leftOutput[index] + inDataX[index] * 0.0f;
                }
                // */
                
                /*
                 //copy data from buffer's X channel to working buffer twice
                 //params: dest chan, dest strt samp, source, num samples
                 workingBufferX.copyFrom(0, 0, inDataX, buffer.getNumSamples());
                 workingBufferX.copyFrom(1, 0, inDataX, buffer.getNumSamples());
                 
                 //create two audio blocks that points to the data in the working buffer (2 chans)
                 dsp::AudioBlock<float> blockXPos(workingBufferX);
                 dsp::AudioBlock<float> blockXNeg(workingBufferX);
                 
                 //multiply one block by 0.1768, the other by -0.1768
                 blockXPos.multiply(blockXPos, +0.1768f);
                 blockXNeg.multiply(blockXNeg, -0.1768f);
                 
                 //create context information that is passed into the algorithm's process method (+/-)
                 dsp::ProcessContextReplacing<float> contextPosX(blockXPos);
                 dsp::ProcessContextReplacing<float> contextNegX(blockXNeg);
                 
                 //convolve using 8 HRTFs (FL+,BL-,BR-,FR+)
                 FL_ConvX.process(contextPosX);//+
                 BL_ConvX.process(contextNegX);//-
                 BR_ConvX.process(contextNegX);//-
                 FR_ConvX.process(contextPosX);//+
                 
                 //I am assuming JUCE will sum these outputs correspondingly (L & R)
                 
                 //clear the buffer since the dummyBuffer needs to be used again.
                 //workingBufferX.clear();
                 
                 // */
                
                break;
            }
            default: {
                break;
            }
        }
    }
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

