/*
  ==============================================================================

    GroupingCircle.h
    Created: 26 Apr 2020 4:59:12pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class GroupingCircle    : public Component
{
public:

	enum ControlMode
	{
		Layout = 0,
		Notes
	};

public:
    GroupingCircle(const Array<int>& generatorChainIn, const Array<int> degreeGroupSizesIn, Array<Colour>& groupColoursIn);
    ~GroupingCircle();

	ControlMode getControlMode();

	void setControlMode(ControlMode controlModeIn);

    void paint (Graphics&) override;
    void resized() override;

	class Listener
	{
	public:
		~Listener() {};

		
	};

protected:
	ListenerList<Listener> listeners;

private:
	OwnedArray<Label> generatorChainLabels;
	OwnedArray<Label> degreeGroupLabels;

	// The chain of scale degrees by stacking the chosen generator
	const Array<int>& generatorChain;

	// Starting from the top (scale size selected) going clockwise
	const Array<int>& degreeGroupSizes;

	// Starting from the top going clockwise
	const Array<Colour>& groupColours;

	ControlMode controlModeSelected = ControlMode::Layout;
	
	bool showDegreesInLayoutMode = true;

private:

	void updateChildren();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GroupingCircle)
};
