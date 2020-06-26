/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FoaRotAudioProcessorEditor  : public AudioProcessorEditor, private Slider::Listener
{
public:
    FoaRotAudioProcessorEditor (FoaRotAudioProcessor&);
    ~FoaRotAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider* slider) override; // [3]
    
    Slider azimuthSlider; // [1]
    Slider elevationSlider; // [1]
    
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> azimuthSliderAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> elevationSliderAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FoaRotAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FoaRotAudioProcessorEditor)
};
