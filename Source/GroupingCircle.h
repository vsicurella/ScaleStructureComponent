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
    GroupingCircle(const Array<int>& generatorChainIn, const Array<int>& degreeGroupSizesIn, Array<Colour>& colourTableIn);
    ~GroupingCircle();

	ControlMode getControlMode();
	Value& getOffsetValue();

	float getInnerRadius() const;
	float getMiddleRadius() const;
	float getOuterRadius() const;

	Point<float> getCenter() const;
	Point<int> getPositionFromCenter(float radius, float angle) const;

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

	Array<Line<float>> radiLines;
	Array<Path> degreeArcPaths;
	Array<Path> groupArcPaths;
	PathStrokeType strokeType = PathStrokeType(2.0f);

	float sectorLabelSizeRatio = 0.875f;

	Array<bool> degreeSectorMouseOver;
	Array<bool> groupSectorMouseOver;

	float highlightContrastRatio = 1.0f / 6.0f;
	float labelContrastRatio = 2.0f / 3.0f;

private:

	/*
		Returns the degree sector index that the mouse is in. Uses Path data.
		Returns -1 if not found.
	*/
	int mouseInDegreeSector(const MouseEvent& event);

	/*
		Returns the group sector index that the mouse is in. Uses Path data.
		Returns -1 if not found.
	*/
	int mouseInGroupSector(const MouseEvent& event);

	// Arc path helper
	static void addArcToPath(Path& pathIn, Rectangle<float>& ellipseBounds, float fromRadians, float toRadians, bool startAsNewSubPath = false);

	static int modulo(int num, int mod);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GroupingCircle)
};
