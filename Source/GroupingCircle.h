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
    GroupingCircle(const Array<Array<int>>& degreeGroupingsIn, Array<Colour>& colourTableIn);
    ~GroupingCircle();

	ControlMode getControlMode();
	Value& getOffsetValue();

	float getInnerRadius() const;
	float getMiddleRadius() const;
	float getOuterRadius() const;

	Rectangle<float> getInnerCircleBounds() const;
	Rectangle<float> getMiddleCircleBounds() const;
	Rectangle<float> getOuterCircleBounds() const;

	Point<float> getCenter() const;
	Point<float> getFloatPointFromCenter(float radius, float angle) const;
	Point<int> getIntPointFromCenter(float radius, float angle) const;

	void setControlMode(ControlMode controlModeIn);

	void setGeneratorOffset(int offsetIn, bool updateDegreeLabels = true);
	void setOffsetLimit(int offsetLimitIn);

	void updatePeriod(int periodIn);
	void updateGenerator();

    void paint (Graphics&) override;
    void resized() override;

	void mouseMove(const MouseEvent& event) override;
	void mouseDrag(const MouseEvent& event) override;

	class Listener
	{
	public:
		~Listener() {};

		
	};

	// Arc path helper
	static void addArcToPath(Path& pathIn, Rectangle<float>& ellipseBounds, float fromRadians, float toRadians, bool startAsNewSubPath = false);

protected:
	ListenerList<Listener> listeners;

private:
	OwnedArray<Label> degreeLabels;
	OwnedArray<Label> groupSizeLabels;

	// The groups of scale degree arrays
	const Array<Array<int>>& degreeGroupings;

	// The chain of scale degrees by stacking the chosen generator
	Array<int> generatorChain;

	// Starting from the top (scale size selected) going clockwise
	Array<int> groupSizes;

	// Starting from the top going clockwise
	const Array<Colour>& colourTable;

	// TODO: decide whether or not to have control modes
	ControlMode controlModeSelected = ControlMode::Layout;
	
	Value generatorOffset;
	int offsetLimit = 7;

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
	const float float_HalfPi = float_Pi / 2;
	const float float_Tau = float_Pi * 2;

	Array<Line<float>> radiLines;
	Array<Path> degreeArcPaths;
	Array<Path> groupArcPaths;
	PathStrokeType strokeType = PathStrokeType(2.0f);

	float sectorLabelSizeRatio = 0.875f;

	Array<bool> degreeSectorMouseOver;
	Array<bool> groupSectorMouseOver;
	int lastDegreeSectorMouseIn = -1;
	int lastGroupSectorMouseIn = -1;

	float highlightContrastRatio = 1.0f / 6.0f;
	float labelContrastRatio = 2.0f / 3.0f;

private:

	/*
		Returns the angle of the mouse based on the circle's center, with 0 starting at the counter-clockwise edge 
		of the first degree group's degree, and moving clockwise.
	*/
	float getNormalizedMouseAngle(const MouseEvent& event);

	/*
		Returns the degree index of the degree's corresponding sector that the mouse is in.
	*/
	int mouseInDegreeSector(const MouseEvent& event, float angle);

	/*
		Returns the degree index that the mouse is in if in the area of a degree ring sector.
		Returns -1 if outside of ring sector.
	*/
	int mouseInDegreeRingSector(const MouseEvent& event, float radiusFromCenter, float angle);

	/*
		Returns the group size index of the group's corresponding sector that the mouse is in
	*/
	int mouseInGroupSector(int degreeIndex);

	/*
		Returns the group size index that the mouse is in if in the area of a group size ring sector.
		Returns -1 if outside of ring sector.
	*/
	int mouseInGroupRingSector(const MouseEvent& event, float radiusFromCenter, float angle);
	int mouseInGroupRingSector(const MouseEvent& event, float radiusFromCenter, int degreeIndex);

	static int modulo(int num, int mod);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GroupingCircle)
};
