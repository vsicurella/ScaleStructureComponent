/*
  ==============================================================================

    GroupingCircle.cpp
    Created: 26 Apr 2020 4:59:12pm
    Author:  Vincenzo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GroupingCircle.h"

//==============================================================================
GroupingCircle::GroupingCircle(const Array<int>& generatorChainIn, const Array<int>& degreeGroupSizesIn, Array<Colour>& groupColoursIn)
	:	generatorChain(generatorChainIn),
		degreeGroupSizes(degreeGroupSizesIn),
		colourTable(groupColoursIn)
{
	//jassert(degreeGroupSize.size() == colourTable.size())
	//addMouseListener(this, true);
}

GroupingCircle::~GroupingCircle()
{
}

float GroupingCircle::getInnerRadius() const
{
	return degreeInnerRadius;
}

float GroupingCircle::getMiddleRadius() const
{
	return degreeOuterRadius;
}

float GroupingCircle::getOuterRadius() const
{
	return groupOuterRadius;
}

Point<float> GroupingCircle::getCenter() const
{
	return center;
}

Point<int> GroupingCircle::getPositionFromCenter(float radius, float angle) const
{
	return center.getPointOnCircumference(radius, angle).toInt();
}

GroupingCircle::ControlMode GroupingCircle::getControlMode()
{
	return controlModeSelected;
}

Value& GroupingCircle::getOffsetValue()
{
	return generatorOffset;
}

void GroupingCircle::setControlMode(ControlMode controlModeIn)
{
	controlModeSelected = controlModeIn;
}

void GroupingCircle::setGeneratorOffset(int offsetIn, bool upgradeDegreeLabels)
{
	generatorOffset.setValue(offsetIn);

	if (upgradeDegreeLabels)
		updateGenerator();
}

void GroupingCircle::setOffsetLimit(int offsetLimitIn)
{
	offsetLimit = offsetLimitIn;
}

void GroupingCircle::paint (Graphics& g)
{
	g.fillAll(Colour());

	// Draw outline
	g.setColour(Colours::black);

	strokeType.setStrokeThickness(2.0f);

	if (controlModeSelected == ControlMode::Notes)
	{
	}
	else
	{
	}

	Colour groupColour;
	Colour degreeColour;
	int degIndex = 0;
	for (int i = 0; i < groupArcPaths.size(); i++)
	{
		// Draw groups
		Path& groupPath = groupArcPaths.getReference(i);
		groupColour = colourTable[i];

		if (groupColour.isTransparent())
			groupColour = Colours::lightgrey;

		if (groupSectorMouseOver[i])
			groupColour = groupColour.contrasting(highlightContrastRatio);

		g.setColour(groupColour);
		g.fillPath(groupPath);

		g.setColour(Colours::black);
		g.strokePath(groupPath, strokeType);

		groupSizeLabels[i]->setColour(Label::ColourIds::textColourId, groupColour.contrasting(labelContrastRatio));

		// Draw degrees
		for (int d = 0; d < degreeGroupSizes[i]; d++)
		{
			degreeColour = groupColour;

			if (degreeSectorMouseOver[degIndex])
				degreeColour = groupColour.contrasting(highlightContrastRatio);

			Path& degreePath = degreeArcPaths.getReference(degIndex);
			
			g.setColour(degreeColour.brighter());
			g.fillPath(degreePath);

			g.setColour(degreeColour.darker());
			g.strokePath(degreePath, strokeType);

			degreeLabels[degIndex]->setColour(Label::ColourIds::textColourId, degreeColour.contrasting(labelContrastRatio));
			degIndex++;
		}
	}

	if (controlModeSelected == ControlMode::Layout)
	{
		// TODO: Outline degree group edges
	}
}

void GroupingCircle::resized()
{
	center = Point<float>(getWidth() / 2.0f, getHeight() / 2.0f);

	groupOuterRadius = getWidth() * borderRatio / 2.0f;
	degreeOuterRadius = groupOuterRadius * degreeToGroupRatio;
	degreeInnerRadius = degreeOuterRadius * degreeOuterToInnerRatio;

	groupRingWidth = groupOuterRadius - degreeOuterRadius;
	degreeRingWidth = degreeOuterRadius - degreeInnerRadius;

	groupMiddleRadius = (degreeOuterRadius + groupOuterRadius) / 2.0f;
	degreeMiddleRadius = (degreeInnerRadius + degreeOuterRadius) / 2.0f;

	groupOuterCircleBounds = getBounds().toFloat().reduced(proportionOfWidth(1 - borderRatio));
	groupInnerCircleBounds = groupOuterCircleBounds.reduced(groupRingWidth);
	degreeInnerCircleBounds = groupInnerCircleBounds.reduced(degreeRingWidth);
	
	angleIncrement = 2 * double_Pi / generatorChain.size();
	angleHalf = angleIncrement / 2.0f;

	// determine circle offset, based off of middle angle of first degree group
	circleOffset = degreeGroupSizes[0] * angleIncrement / 2.0f;

	int groupIndex = 0;
	int groupDegreesPassed = 0;
	
	float degreeLabelSize = jmin(degreeRingWidth, 2 * float_Pi * degreeMiddleRadius / degreeLabels.size() ) * sectorLabelSizeRatio;
	float groupLabelSize = groupRingWidth * sectorLabelSizeRatio;

	float angle, angleTo, groupAngleFrom = -circleOffset; 
	float degLabelAngle, groupLabelAngle;

	Path degreePath, groupPath;
	Label* degreeLabel, *groupLabel;
	float degreeLabelWidth, groupLabelWidth;

	groupArcPaths.clear();
	degreeArcPaths.clear();
	for (int i = 0; i < degreeLabels.size(); i++)
	{
		angle = angleIncrement * i - circleOffset;
		angleTo = angle + angleIncrement;
	
		// draw arc sections
		degreePath = Path();
		addArcToPath(degreePath, degreeInnerCircleBounds, angle, angleTo, true);
		addArcToPath(degreePath, groupInnerCircleBounds, angleTo, angle);
		degreePath.closeSubPath();
		degreeArcPaths.add(degreePath);

		// place labels
		degreeLabel = degreeLabels[i];
		degreeLabel->setFont(Font().withHeight(degreeLabelSize));
		degreeLabelWidth = degreeLabel->getFont().getStringWidthFloat(" " + degreeLabel->getText() + " ");
		degreeLabel->setSize(jmax(degreeLabelWidth, degreeLabelSize), degreeLabelSize);

		degLabelAngle =  angleTo - angleHalf - float_HalfPi;

		degreeLabel->setCentrePosition(Point<int>(
			round(center.x + cosf(degLabelAngle) * degreeMiddleRadius),
			round(center.y + sinf(degLabelAngle) * degreeMiddleRadius)
		));

		int groupSize = degreeGroupSizes[groupIndex];
		if ((i + 1) - groupDegreesPassed >= groupSize && groupSize > 0)
		{
			groupPath = Path();
			addArcToPath(groupPath, groupInnerCircleBounds, groupAngleFrom, angleTo, true);
			addArcToPath(groupPath, groupOuterCircleBounds, angleTo, groupAngleFrom);
			groupPath.closeSubPath();
			groupArcPaths.add(groupPath);

			groupLabelAngle = (groupAngleFrom + angleTo) / 2.0f - float_HalfPi;

			groupLabel = groupSizeLabels[groupIndex];
			groupLabel->setFont(Font().withHeight(groupLabelSize));
			groupLabelWidth = groupLabel->getFont().getStringWidthFloat(groupLabel->getText());
			groupLabel->setSize(jmax(groupLabelWidth, groupLabelSize), groupLabelSize);

			groupLabel->setCentrePosition(Point<int>(
				round(center.x + cosf(groupLabelAngle) * groupMiddleRadius),
				round(center.y + sinf(groupLabelAngle) * groupMiddleRadius)
			));

			groupDegreesPassed += groupSize;
			groupIndex++;
			groupSize = degreeGroupSizes[groupIndex];

			groupAngleFrom = angleTo;
		}
	}

	DBG("Circle: Resized");
}

void GroupingCircle::mouseMove(const MouseEvent& event)
{
	// TODO: find more efficent method

	// Brute Force
	bool dirty = false;
	float mouseRadius = event.position.getDistanceFrom(center);

	// If mouse is not in rings, remove highlights
	if (mouseRadius < degreeInnerRadius || mouseRadius > groupOuterRadius)
	{
		degreeSectorMouseOver.fill(false);
		groupSectorMouseOver.fill(false);
		dirty = true;
	}

	else
	{
		int degreeIndex = mouseInDegreeSector(event);
		int groupIndex = mouseInGroupSector(event);

		// Turn off sectors if not in appropriate ring
		if (degreeIndex < 0)
		{
			degreeSectorMouseOver.fill(false);
			dirty = true;
		}

		if (groupIndex < 0)
		{
			groupSectorMouseOver.fill(false);
			dirty = true;
		}

		// Check Degree Sectors
		if (mouseRadius < degreeOuterRadius)
		{
			int lastDegreeSector = degreeSectorMouseOver.indexOf(true);
			if (lastDegreeSector != degreeIndex)
			{
				if (lastDegreeSector > -1)
				{
					degreeSectorMouseOver.set(lastDegreeSector, false);
				}

				degreeSectorMouseOver.set(degreeIndex, true);
				dirty = true;
			}
		}

		// Check Group Sectors
		else if (mouseRadius < groupOuterRadius)
		{
			int lastGroupSector = groupSectorMouseOver.indexOf(true);
			if (lastGroupSector != groupIndex)
			{
				if (lastGroupSector > -1)
				{
					groupSectorMouseOver.set(lastGroupSector, false);
				}

				groupSectorMouseOver.set(groupIndex, true);
				dirty = true;
			}
		}
	}

	if (dirty)
		repaint();
}

void GroupingCircle::mouseDrag(const MouseEvent& event)
{
	// TODO: search for more efficient method

	float mouseRadius = event.position.getDistanceFrom(center);
	
	// Only check if the mouse is in the circle rings
	if (mouseRadius < degreeInnerRadius || mouseRadius >= degreeOuterRadius)
		return;

	else
	{
		// Brute Force
		bool dirty = false;
		for (int deg = 0; deg < degreeLabels.size(); deg++)
		{
			// If the mouse was over a sector when drag started, but not anymore

			if (degreeSectorMouseOver[deg] && !degreeArcPaths.getReference(deg).contains(event.position))
			{
				// Search for the current sector it's over and find index offset
				for (int degTo = 0; degTo < degreeLabels.size(); degTo++)
				{
					if (degreeArcPaths.getReference(degTo).contains(event.position))
					{
						int offset = degTo - deg + (int) generatorOffset.getValue();

						if (offset > -1 && offset <= offsetLimit)
						{
							//degreeSectorMouseOver.set(deg, false);
							//degreeSectorMouseOver.set(modulo(deg + offset, degreeLabels.size()), true);

							generatorOffset.setValue(offset);
							DBG("Moved by " + String(degTo - deg) + "\tNew offset: " + generatorOffset.getValue().toString());
							dirty = true;
						}

						break;
					}
				}
			}
		}

		if (dirty)
		{
			resized();
			/*repaint();*/
			mouseMove(event);
		}
	}
}


void GroupingCircle::updatePeriod(int periodIn)
{
	DBG("Circle: Updating");

	removeAllChildren();

	degreeLabels.clear();

	for (int i = 0; i < periodIn; i++)
	{
		Label* l = degreeLabels.add(new Label());
		l->setJustificationType(Justification::centred);
		l->setInterceptsMouseClicks(false, false);
		addAndMakeVisible(l);
	}

	degreeSectorMouseOver.resize(periodIn);
	degreeSectorMouseOver.fill(false);
}

void GroupingCircle::updateGenerator()
{
	DBG("Circle: Generator changed, updating labels");
	for (int i = 0; i < degreeLabels.size(); i++)
	{
		degreeLabels[i]->setText(String(generatorChain[i]), dontSendNotification);
		//degreeLabels[i]->setColour(Label::ColourIds::outlineColourId, Colours::white);
	}

	groupSizeLabels.clear();
	for (auto groupSize : degreeGroupSizes)
	{
		Label* l = groupSizeLabels.add(new Label());
		l->setJustificationType(Justification::centred);
		l->setText(String(groupSize), dontSendNotification);
		l->setInterceptsMouseClicks(false, false);
		//l->setColour(Label::ColourIds::outlineColourId, Colours::white);
		addAndMakeVisible(l);
	}

	groupSectorMouseOver.resize(groupSizeLabels.size());
	groupSectorMouseOver.fill(false);

	resized();
	repaint();
}

int GroupingCircle::mouseInDegreeSector(const MouseEvent& event)
{
	for (int deg = 0; deg < degreeArcPaths.size(); deg++)
	{
		if (degreeArcPaths.getReference(deg).contains(event.position))
			return deg;
	}

	return -1;
}

int GroupingCircle::mouseInGroupSector(const MouseEvent& event)
{
	for (int group = 0; group < groupArcPaths.size(); group++)
	{
		if (groupArcPaths.getReference(group).contains(event.position))
			return group;
	}

	return -1;
}

void GroupingCircle::addArcToPath(Path& pathIn, Rectangle<float>& ellipseBounds, float fromRadians, float toRadians, bool startAsNewSubPath)
{
	pathIn.addArc(
		ellipseBounds.getX(),
		ellipseBounds.getY(),
		ellipseBounds.getWidth(),
		ellipseBounds.getHeight(),
		fromRadians,
		toRadians,
		startAsNewSubPath
	);
}

int GroupingCircle::modulo(int num, int mod)
{
	return ((num % mod) + mod) % mod;
}
