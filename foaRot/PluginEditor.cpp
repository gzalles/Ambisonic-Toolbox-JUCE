/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FoaRotAudioProcessorEditor::FoaRotAudioProcessorEditor (FoaRotAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    // these define the parameters of our azimuth slider object
    azimuthSlider.setRange(-180.0, 180.0);
    azimuthSlider.setSliderStyle (Slider::LinearHorizontal);
    azimuthSlider.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
    azimuthSlider.addListener(this);
    addAndMakeVisible (&azimuthSlider);
    
    // these define the parameters of our elevation slider object
    elevationSlider.setRange(-180.0, 180.0);
    elevationSlider.setSliderStyle (Slider::LinearVertical);
    elevationSlider.setTextBoxStyle (Slider::TextBoxBelow, false, 100, 20);
    elevationSlider.addListener(this);
    addAndMakeVisible (&elevationSlider);
    
    //add the sliders to the VTS
    azimuthSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "azimuth", azimuthSlider);
    elevationSliderAttachment = new AudioProcessorValueTreeState::SliderAttachment (processor.parameters, "elevation", elevationSlider);
}

FoaRotAudioProcessorEditor::~FoaRotAudioProcessorEditor()
{
    //destroy reference to GUI objects in VTS
    azimuthSliderAttachment = nullptr;
    elevationSliderAttachment = nullptr;
}

//==============================================================================
void FoaRotAudioProcessorEditor::paint (Graphics& g)
{
    // fill the whole window silver
    g.fillAll (Colours::gold);
    // set the current drawing colour to blueviolet
    g.setColour (Colours::blueviolet);
    // set the font size and draw text to the screen
    g.setFont (15.0f);
    g.drawFittedText ("FOA Rotate", 0, 0, getWidth(), 30, Justification::centred, 1);
    g.setFont (10.0f);
    g.drawFittedText ("ACN Ordering [WYZX]", 0, 0, getWidth(), 60, Justification::centred, 1);
    g.setFont (12.0f);
    g.drawFittedText ("Azimuth", 20, 230, 200, 20, Justification::centred, 1);
    g.drawFittedText ("Elevation", 200, 40, 200, 20, Justification::centred, 1);
}

void FoaRotAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // sets the position and size of the slider with arguments (x, y, width, height)
    azimuthSlider.setBounds (20, 250, 200, 40);
    elevationSlider.setBounds (250, 60, 100, 200);
}

//==============================================================================
void FoaRotAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    //    processor.sliderValue = mySlider.getValue();
}

