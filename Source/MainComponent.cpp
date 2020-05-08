/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
	scaleStructure.reset(new ScaleStructure(12));

    scaleController.reset(new ScaleStructureComponent(*scaleStructure.get(), colourTable));
    scaleController->setName("ScaleStructureComponent");

	addAndMakeVisible(scaleController.get());
    
    setSize (800, 800);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), Justification::centred, true);
}

void MainComponent::resized()
{
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    scaleController->setBounds(0, 0, proportionOfWidth(1.0f), proportionOfHeight(1.0f));
}
