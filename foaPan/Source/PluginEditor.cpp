/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FoaPanAudioProcessorEditor::FoaPanAudioProcessorEditor (FoaPanAudioProcessor& p)
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

FoaPanAudioProcessorEditor::~FoaPanAudioProcessorEditor()
{
    //destroy reference to GUI objects in VTS
    azimuthSliderAttachment = nullptr;
    elevationSliderAttachment = nullptr;
    
}

//==============================================================================
void FoaPanAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    
    // fill the whole window silver
    g.fillAll (Colours::silver);
    // set the current drawing colour to blueviolet
    g.setColour (Colours::blueviolet);
    // set the font size and draw text to the screen
    g.setFont (15.0f);
    g.drawFittedText ("FOA Pan", 0, 0, getWidth(), 30, Justification::centred, 1);
    g.setFont (10.0f);
    g.drawFittedText ("ACN Ordering [WYZX]",
                      0, 0, getWidth(), 60, Justification::centred, 1);
    g.setFont (12.0f);
    g.drawFittedText ("Azimuth", 20, 215, 200, 20, Justification::centred, 1);
    g.drawFittedText ("Elevation", 200, 40, 200, 20, Justification::centred, 1);
    
    //it feels like this representation should also control the azimuth, can you make the sphere "grabbable"?
    //draw the circle/"sphere"
    //ellipses depict position of source on azimuth plane
    g.setColour (Colours::blueviolet);
    g.drawEllipse(50, 50, 150, 150, 2);//main big circle

    //go to corner of large circle, then its origin, then to the position, multiply it by radius.
    //minus 90 degrees (pi/2) justifies the position so that center is above rather than on the right
    aziPosX = 50 + 150/2 + cos(aziVal * (M_PI / 180) - M_PI/2) * 150/2;
    aziPosY = 50 + 150/2 + sin(aziVal * (M_PI / 180) - M_PI/2) * 150/2;
    
    g.setColour (Colours::skyblue);
    //subtract the w and h of circle to have origin of mini circle at the circumference
    g.drawEllipse(aziPosX - 15/2, aziPosY - 15/2, 15, 15, 2);
    repaint();
    
}

void FoaPanAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // sets the position and size of the slider with arguments (x, y, width, height)
    azimuthSlider.setBounds (20, 235, 200, 40);
    elevationSlider.setBounds (250, 60, 100, 200);
}

//==============================================================================
void FoaPanAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
//    processor.sliderValue = mySlider.getValue();
    
    //get value of slider to draw mini filled ellipse
    aziVal = azimuthSlider.getValue();
    elevVal = elevationSlider.getValue();
    
}

