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
    GroupingCircle(const Array<int>& generatorChainIn, const Array<int>& degreeGroupSizesIn, Array<Colour>& groupColoursIn);
    ~GroupingCircle();

	ControlMode getControlMode();

	void setControlMode(ControlMode controlModeIn);

	void updatePeriod(int periodIn);
	void updateGenerator();

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
	OwnedArray<Label> degreeLabels;
	OwnedArray<Label> groupSizeLabels;

	// The chain of scale degrees by stacking the chosen generator
	const Array<int>& generatorChain;

	// Starting from the top (scale size selected) going clockwise
	const Array<int>& degreeGroupSizes;

	// Starting from the top going clockwise
	const Array<Colour>& groupColours;

	ControlMode controlModeSelected = ControlMode::Layout;
	
	bool showDegreesInLayoutMode = true;

	// Drawing related members
	float borderRatio = 127.0f / 128.0f;
	float degreeToGroupRatio = 4.0f / 5.0f;
	float degreeOuterToInnerRatio = 5.0f / 6.0f;

	float groupOuterRadius;
	float groupMiddleRadius;

	float degreeOuterRadius;
	float degreeInnerRadius;
	float degreeMiddleRadius;

	Point<float> center;
	float circleOffset = float_Pi / 2.0f;

	float groupRingWidth;
	float degreeRingWidth;

	Rectangle<float> groupOuterCircleBounds;
	Rectangle<float> groupInnerCircleBounds;
	Rectangle<float> degreeInnerCircleBounds;

	double angleIncrement;
	double angleHalf;

	Array<Line<float>> radiLines;
	Path arcDegreeSections;
	Path arcGroupSections;
	Array<Path> degreeArcPaths;
	Array<Path> groupArcPaths;
	PathStrokeType strokeType = PathStrokeType(2.0f);

private:

	// Arc path helper
	static void addArcToPath(Path& pathIn, Rectangle<float>& ellipseBounds, float fromRadians, float toRadians, bool startAsNewSubPath = false);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GroupingCircle)
};
