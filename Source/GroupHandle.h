/*
  ==============================================================================

    GroupHandle.h
    Created: 29 May 2020 4:33:46pm
    Author:  Vincenzo

	Data structure regarding UI with resizing degree groups

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class GroupHandle
{

public:

	GroupHandle(int groupIndexIn, bool addsGroupIn, bool clockwiseDragIn);
	~GroupHandle() {};

	int getGroupIndex() const;

	bool addsGroupWhenDragged() const;

	bool isDraggingClockwise() const;

	/*
		Returns polar coordinates
	*/
	Point<float> getPosition() const;

	Colour getColour() const;

	float getSize() const;

	/*
		Returns the path that represents this handle as a dot
	*/
	Path getDot() const;

	/*
		Returns the path that represents this handle as a line (edge)
	*/
	Path getLine() const;

	/*
		Returns the path that represents this handle depending on the addsGroup member
	*/
	Path getPath() const;

	/*
		Returns thickness value.
		If represents an edge (addsGroup = false), this is thickness. If it's a dot, it's a radius.
	*/

	void setDraggingClockwise(bool clockwiseDragIn);

	/*
		Set center position in polar coordinates
	*/

	void setPosition(float radius, float angle, Point<float> centerIn=Point<float>());
	void setPosition(Point<float> polarCoordinates, Point<float> centerIn = Point<float>());

	/*
		If represents an edge (addsGroup = false), this is the line length. If it's a dot, it's a radius.
	*/
	void setSize(float sizeIn);

	void setColour(Colour colourIn);

public:

	static Line<float> getGroupEdgeLine(Point<float> centerCircle, Point<float> polarCoords, float ringWidthRatio);

private:

	int groupIndex;
	bool addsGroup;
	bool clockwiseDrag;

	Point<float> position;
	Point<float> center;

	float size;
	Colour colour;
};