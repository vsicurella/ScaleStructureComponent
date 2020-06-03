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
GroupingCircle::GroupingCircle(const ScaleStructure& structureIn, Array<Colour>& groupColoursIn)
	:	scaleStructure(structureIn),
		colourTable(groupColoursIn)
{

}

GroupingCircle::~GroupingCircle()
{
	listeners.clear();
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

Rectangle<float> GroupingCircle::getInnerCircleBounds() const
{
	return degreeInnerCircleBounds;
}

Rectangle<float> GroupingCircle::getMiddleCircleBounds() const
{
	return groupInnerCircleBounds;
}

Rectangle<float> GroupingCircle::getOuterCircleBounds() const
{
	return groupOuterCircleBounds;
}

Point<float> GroupingCircle::getCenter() const
{
	return center;
}

Point<float> GroupingCircle::getFloatPointFromCenter(float radius, float angle) const
{
	return center.getPointOnCircumference(radius, angle);

}

Point<int> GroupingCircle::getIntPointFromCenter(float radius, float angle) const
{
	return getFloatPointFromCenter(radius, angle).toInt();
}

void GroupingCircle::updatePeriod()
{
	DBG("CIRCLE: Updating Period.");

	removeAllChildren();

	degreeLabels.clear();

	for (int i = 0; i < scaleStructure.getPeriod(); i++)
	{
		TextEditor* l = degreeLabels.add(new TextEditor());
		l->setJustification(Justification::centred);
		l->setReadOnly(true);
		l->setInterceptsMouseClicks(false, false);
		l->setColour(TextEditor::ColourIds::backgroundColourId, Colour());
		l->setColour(TextEditor::ColourIds::outlineColourId, Colour());
		addAndMakeVisible(l);
	}

	degreeSectorMouseOver = -1;
}

void GroupingCircle::updateGenerator()
{
	DBG("CIRCLE: Updating degree ring.");
	cancelDegreeMods();

	groupSizes = scaleStructure.getDegreeGroupSizes();
	groupChain = scaleStructure.getGroupChain();
	chromaAlterations = scaleStructure.getChromaAlterations();
	degreeAlterations = scaleStructure.getDegreeAlterations();

	DBG("\t" + arrayToString(groupChain));

	// Set up group size labels
	groupSizeLabels.clear();
	for (int i = 0; i < groupSizes.size(); i++)
	{
		Label* l = groupSizeLabels.add(new Label());
		l->setJustificationType(Justification::centred);
		l->setText(String(groupSizes[i]), dontSendNotification);
		l->setInterceptsMouseClicks(false, false);
		//l->setColour(Label::ColourIds::outlineColourId, Colours::white);

		// First one will be replaced by scaleSizeSelector
		if (i > 0)
			addAndMakeVisible(l);
	}

	// Set up group handles
	groupHandles.clear();
	highlightedEdges.clear();
	int degIndex = 0;
	for (int i = 1; i < groupSizes.size(); i++)
	{
		degIndex += groupSizes[i - 1];

		groupHandles.add(new GroupHandle(i, degIndex, true, false));
		groupHandles.add(new GroupHandle(i, degIndex, true, true));

		if (i > 1)
		{
			groupHandles.add(new GroupHandle(i, degIndex, false, false));
		}
	}

	highlightedEdges.resize(groupSizes.size());

	groupSectorMouseOver = -1;

	resized();
	repaint();
}

void GroupingCircle::paint (Graphics& g)
{
	g.fillAll(Colour());

	// Draw outline
	g.setColour(Colours::black);

	solidStroke.setStrokeThickness(2.0f);

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

		if (i == groupSectorMouseOver)
			groupColour = groupColour.contrasting(highlightContrastRatio);

		g.setColour(groupColour);
		g.fillPath(groupPath);

		g.setColour(Colours::black);
		g.strokePath(groupPath, solidStroke);

		groupSizeLabels[i]->setColour(Label::ColourIds::textColourId, groupColour.contrasting(labelContrastRatio));

		// Draw degrees
		for (int d = 0; d < groupSizes[i]; d++)
		{
			degreeColour = groupColour;

			if (degreeIndexToMod > -1 && isDegreeSectorIndexModCandidate(degIndex))
			{
				// Maybe outline instead of recolor
				degreeColour = groupColour.contrasting(Colours::mediumvioletred, 1.0f / 3);
			}

			if (degIndex == degreeSectorMouseOver)
				degreeColour = groupColour.contrasting(highlightContrastRatio);

			Path& degreePath = degreeArcPaths.getReference(degIndex);
			
			g.setColour(degreeColour.brighter());
			g.fillPath(degreePath);

			g.setColour(degreeColour.darker());
			g.strokePath(degreePath, solidStroke);

			degreeLabels[degIndex]->applyColourToAllText(degreeColour.contrasting(labelContrastRatio));
			degIndex++;
		}
	}

	// Draw edge handles
	for (int i = 0; i < groupHandles.size(); i++)
	{
		GroupHandle* handle = groupHandles.getUnchecked(i);
		
		g.setColour(handle->getColour());
		g.fillPath(handle->getPath());
	}

	float dashPattern[2] = {
		(groupRingWidth + degreeRingWidth) / 20.0f,
		(groupRingWidth + degreeRingWidth) / 20.0f
	};

	// Highlight degree edges
	int edgePath = 0;
	for (auto index : highlightDegreeIndicies)
	{
		g.setColour(colourTable[scaleStructure.getGroupOfDegreeIndex(index)].contrasting(0.75f));

		if (adjacentEdges.x == index || adjacentEdges.y == index)
			g.setColour(Colours::red);

		g.drawDashedLine(highlightedEdges[edgePath], dashPattern, 2, 2.0f);

		edgePath++;
	}
}

void GroupingCircle::resized()
{
	center = Point<float>(getWidth() / 2.0f, getHeight() / 2.0f);

	groupRingWidth = getWidth() * groupWidthRatio;
	degreeRingWidth = getWidth() * degreeWidthRatio;

	groupOuterRadius = getWidth() * borderRatio / 2.0f;
	degreeOuterRadius = groupOuterRadius - groupRingWidth;
	degreeInnerRadius = degreeOuterRadius - degreeRingWidth;

	groupMiddleRadius = (groupOuterRadius + degreeOuterRadius) / 2.0f;
	degreeMiddleRadius = (degreeOuterRadius + degreeInnerRadius) / 2.0f;

	groupOuterCircleBounds = getBounds().toFloat().reduced(proportionOfWidth(1 - borderRatio));
	groupInnerCircleBounds = groupOuterCircleBounds.reduced(groupRingWidth);
	degreeInnerCircleBounds = groupInnerCircleBounds.reduced(degreeRingWidth);
	
	angleIncrement = 2 * double_Pi / groupChain.size();
	angleHalf = angleIncrement / 2.0f;

	handleDragThreshold = angleHalf * groupMiddleRadius;
	handleDotAngRatio = angleIncrement / 10.0f;
	handleDotRadius = groupRingWidth / 25.0f;

	// determine circle offset, based off of middle angle of first degree group
	circleOffset = groupSizes[0] * angleIncrement / 2.0f;

	int groupIndex = 0;
	int groupDegreesPassed = 0;

	float degreeLabelSize = jmin(degreeRingWidth, float_Tau * degreeMiddleRadius / degreeLabels.size()) * sectorLabelSizeRatio;
	float groupLabelSize = groupRingWidth * sectorLabelSizeRatio;

	float angle, angleTo, groupAngleFrom = -circleOffset; 
	float degLabelAngle, groupLabelAngle;

	Path degreePath, groupPath;
	TextEditor* degreeLabel;
	String degreeText;
	int degree;
	Point<int> alteration;

	Label* groupLabel;
	float degreeLabelWidth, groupLabelWidth;

	// Resize group & degree arc sections
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

		// find label size
		degreeLabel = degreeLabels[i];
		degreeLabel->clear();
		degreeLabel->setFont(Font().withHeight(degreeLabelSize));

		// fill labels with text
		degree = groupChain[i];
		degreeLabel->insertTextAtCaret(String(degree));
		
		alteration = degreeAlterations[degree];
		if (alteration.x >= 0 && alteration.y != 0)
		{
			degreeText = "";
			if (alteration.y > 0)
				degreeText += "+";
		
			degreeText += String(alteration.y);
			degreeLabel->setFont(Font(round(degreeLabelSize / 4.0f)));
			degreeLabel->insertTextAtCaret(degreeText);
		}		

		degreeLabelWidth = degreeLabel->getTextWidth();
		degreeLabel->setSize(jmax(degreeLabelWidth, degreeLabelSize), degreeLabelSize);

		degLabelAngle =  angleTo - angleHalf - float_HalfPi;

		degreeLabel->setCentrePosition(Point<int>(
			round(center.x + cosf(degLabelAngle) * degreeMiddleRadius),
			round(center.y + sinf(degLabelAngle) * degreeMiddleRadius)
		));

		int groupSize = groupSizes[groupIndex];
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
			groupSize = groupSizes[groupIndex];

			groupAngleFrom = angleTo;
		}
	}

	float thetaOffset = -circleOffset - float_HalfPi;
	// Resize group handles
	for (int i = 0; i < groupHandles.size(); i++)
	{
		GroupHandle* handle = groupHandles.getUnchecked(i);
		float theta = angleIncrement * handle->getDegreeIndex() + thetaOffset;

		if (handle->addsGroupWhenDragged())
		{
			if (handle->isDraggingClockwise())
			{
				handle->setPosition(Point<float>(theta + handleDotAngRatio, groupMiddleRadius), center);
			}
			else
			{
				theta += groupSizes[handle->getGroupIndex()] * angleIncrement;
				handle->setPosition(Point<float>(theta - handleDotAngRatio, groupMiddleRadius), center);
			}

			handle->setSize(handleDotRadius);
		}
		else
		{
			handle->setPosition(Point<float>(theta, degreeInnerRadius), center);
			handle->setSize(groupOuterRadius / degreeInnerRadius);
		}
	}

	// Resize degree edge lines if groups are being resized
	if (highlightDegreeIndicies.size() > 0)
	{
		highlightedEdges.clear();

		float length = groupOuterRadius / degreeInnerRadius;
		int index;
		Line<float> line;
		
		for (int i = 0; i < highlightDegreeIndicies.size(); i++)
		{
			index = highlightDegreeIndicies[i];
			line = GroupHandle::getGroupEdgeLine(center, Point<float>(angleIncrement * index + thetaOffset, degreeInnerRadius), length);
			highlightedEdges.add(line);
		}
	}
}

void GroupingCircle::mouseMove(const MouseEvent& event)
{
	mouseRadius = event.position.getDistanceFrom(center);
	bool dirty = false;

	// If mouse is not in rings, remove highlights
	if (mouseRadius < degreeInnerRadius || mouseRadius > groupOuterRadius)
	{
		lastDegreeSectorMouseIn = degreeSectorMouseOver;
		degreeSectorMouseOver = -1;

		lastGroupSectorMouseIn = groupSectorMouseOver;
		groupSectorMouseOver = -1;

		if (handleBeingDragged)
		{
			handleBeingDragged->setMouseOver(false);
			handleBeingDragged = nullptr;
			highlightDegreeIndicies.clear();
		}
		
		dirty = true;
	}

	else
	{
		float angle = getNormalizedMouseAngle(event);
		int degreeIndex = degreeSectorOfAngle(angle);

		// Check Degree Sectors
		if (mouseRadius < degreeOuterRadius)
		{
			if (degreeSectorMouseOver != degreeIndex)
			{
				lastDegreeSectorMouseIn = degreeSectorMouseOver;
				degreeSectorMouseOver = degreeIndex;
				dirty = true;
			}

			if (groupSectorMouseOver > -1)
			{
				lastGroupSectorMouseIn = groupSectorMouseOver;
				groupSectorMouseOver = -1;
				dirty = true;
			}

			if (handleBeingDragged)
			{
				handleBeingDragged->setMouseOver(false);
				handleBeingDragged = nullptr;
				highlightDegreeIndicies.clear();
				dirty = true;
			}
		}

		// Check Group Sectors
		else if (mouseRadius < groupOuterRadius)
		{
			int groupIndex = mouseInGroupSector(degreeIndex);

			if (groupSectorMouseOver != groupIndex)
			{
				lastGroupSectorMouseIn = groupSectorMouseOver;
				groupSectorMouseOver = groupIndex;
				dirty = true;
			}

			if (degreeSectorMouseOver > -1)
			{
				lastDegreeSectorMouseIn = degreeSectorMouseOver;
				degreeSectorMouseOver = -1;
				dirty = true;
			}

			// check if over a handle
			// TODO: improve detecting?
			int handleIndex;
			for (handleIndex = 0; handleIndex < groupHandles.size(); handleIndex++)
			{
				GroupHandle* handle = groupHandles.getUnchecked(handleIndex);

				if (handleBeingDragged != handle && handle->isMouseOver(event))
				{
					handleBeingDragged = handle;
					dirty = true;
					break;
				}
			}

			if (handleBeingDragged && !handleBeingDragged->isMouseOver(event))
			{
				handleBeingDragged = nullptr;
				highlightDegreeIndicies.clear();

				dirty = true;
			}
		}
	}

	// TODO: implement ring sectors as components so only certain ones need to be repainted
	if (dirty)
		repaint();
}

void GroupingCircle::mouseDown(const MouseEvent& event)
{
	bool cancelMods = true;

	// If mouse is on a group or degree section
	if (mouseRadius > degreeInnerRadius && mouseRadius < groupOuterRadius)
	{
		// If mouse on a degree
		if (mouseRadius < degreeOuterRadius)
		{
			degreeSectorMouseOver = degreeSectorOfAngle(getNormalizedMouseAngle(event));
			int degree = groupChain[degreeSectorMouseOver];
			// Show menu
			if (event.mods.isRightButtonDown())
			{
				PopupMenu::Options options = PopupMenu::Options().withMaximumNumColumns(1);

				int degreeIndexMouseOn = degreeSectorMouseOver;
				bool& hideMods = cancelMods;
								
				auto selectionCallback = [this](int degIndex) {
					degreeToModSelectedCallback(degIndex);
				};
				
				auto alterationCallback = [this](int degIndex, Point<int> alteration) {
					listeners.call(&Listener::degreeIndexAltered, degIndex, alteration);
					updateGenerator();
				};

				auto resetCallback = [this]() {
					listeners.call(&Listener::allModificationsReset);
					updateGenerator();
				};

				degreeMenu.clear();
				degreeMenu.addItem("Show modifiable degrees", true, false, [selectionCallback, degreeIndexMouseOn, &hideMods](void) {
					selectionCallback(degreeIndexMouseOn);
					hideMods = false;
				});

				degreeMenu.addItem("Reset degree modifications", isDegreeSectorIndexAltered(degreeSectorMouseOver), false, [degreeIndexMouseOn, alterationCallback](void) {
					alterationCallback(degreeIndexMouseOn, Point<int>(-1, 0));
				});

				degreeMenu.addItem("Reset all modifications", true, false, resetCallback);

				degreeMenu.showMenuAsync(options);
			}

			// Mouse left-clicked on mod candidate
			else if (isDegreeSectorIndexModCandidate(degreeSectorMouseOver))
			{
				listeners.call(&Listener::degreeIndexAltered, degreeIndexToMod, degreeModCandidates[degree]);
				updateGenerator();
			}
		}

		// If mouse in a group area
		else if (mouseRadius < groupOuterRadius)
		{
			// TODO: move most calculations to ScaleStructure class
			// Show possible degrees to drag handle to
			if (handleBeingDragged)
			{
				int groupIndex = handleBeingDragged->getGroupIndex();
				int groupSize = groupSizes[groupIndex];
				int handleDeg = handleBeingDragged->getDegreeIndex();

				// will minimize the difference of possible degree edges
				// to find the edges adjacent to the handle
				adjacentEdges = Point<int>(-1, -1);
				adjacentEdgeIndicies = adjacentEdges;

				Array<Point<int>> newGroupSizes;
				int newDegreeEdge;
				highlightDegreeIndicies.clear();

				if (handleBeingDragged->addsGroupWhenDragged())
				{
					newGroupSizes = scaleStructure.findValidGroupSizeRemainders(handleBeingDragged->getGroupIndex());
					for (int i = 0; i <  newGroupSizes.size(); i++)
					{
						Point<int> sizePair = newGroupSizes[i];

						if (sizePair.x < groupSize)
						{
							if (handleBeingDragged->isDraggingClockwise())
							{
								newDegreeEdge = sizePair.x + handleDeg;

								if (adjacentEdges.y < 0 || newDegreeEdge < adjacentEdges.y)
								{
									adjacentEdges.setY(newDegreeEdge);
									adjacentEdgeIndicies.setY(i);
								}
							}
							else
							{
								newDegreeEdge = groupSize - sizePair.x + handleDeg;

								if (adjacentEdges.x < 0 || newDegreeEdge > adjacentEdges.x)
								{
									adjacentEdges.setX(newDegreeEdge);
									adjacentEdgeIndicies.setX(i);
								}

							}

							highlightDegreeIndicies.add(newDegreeEdge);
						}
					}
				}

				// TODO: add checking for edges that go beyond adjacent groups
				else
				{
					newGroupSizes = scaleStructure.findValidGroupSize(handleBeingDragged->getGroupIndex(), true);
					newGroupSizes.addArray(scaleStructure.findValidGroupSize(handleBeingDragged->getGroupIndex(), false));
					for (int i = 0; i < newGroupSizes.size(); i++)
					{
						Point<int> sizePair = newGroupSizes[i];
						if (groupSize != sizePair.x)
						{
							newDegreeEdge = groupSize - sizePair.x + handleDeg;

							if (newDegreeEdge < handleDeg && (adjacentEdges.x < 0 || handleDeg - newDegreeEdge < handleDeg - adjacentEdges.x))
							{
								adjacentEdges.setX(newDegreeEdge);
								adjacentEdgeIndicies.setX(highlightDegreeIndicies.size());
							}

							else if (newDegreeEdge > handleDeg && (adjacentEdges.y < 0 || newDegreeEdge - handleDeg < adjacentEdges.y - handleDeg))
							{
								adjacentEdges.setY(newDegreeEdge);
								adjacentEdgeIndicies.setY(highlightDegreeIndicies.size());
							}

							highlightDegreeIndicies.add(newDegreeEdge);
						}
					}
				}

				resized();

				DBG("CIRCLE: Handle grabbed. Available degree edges:");
				String dbgstr;
				for (auto hdi : highlightDegreeIndicies)
				{
					dbgstr = "\t" + String(hdi);
					if (adjacentEdges.x == hdi)
						dbgstr += " * ccw adj";
					else if (adjacentEdges.y == hdi)
						dbgstr += " * cw adj";
					DBG(dbgstr);
				}
			}
		}
	}

	if (cancelMods)
	{
		cancelDegreeMods();
		repaint();
	}
}

void GroupingCircle::mouseUp(const MouseEvent& event)
{

	if (handleBeingDragged)
	{
		handleBeingDragged = nullptr;
		highlightDegreeIndicies.clear();
		repaint();
	}
}

void GroupingCircle::mouseDrag(const MouseEvent& event)
{
	mouseDownRadius = event.mouseDownPosition.getDistanceFrom(center);
	mouseRadius = event.position.getDistanceFrom(center);
	bool dirty = false;

	if (mouseDownRadius >= degreeInnerRadius)
	{
		float angle = getNormalizedMouseAngle(event);
		int degreeIndex = degreeSectorOfAngle(angle);

		if (mouseDownRadius < degreeOuterRadius)
		{
			if (degreeSectorMouseOver != degreeIndex)
			{
				int offset = degreeIndex - degreeSectorMouseOver + scaleStructure.getGeneratorOffset();

				if (offset > -1 && offset < scaleStructure.getScaleSize())
				{
					//generatorOffset.setValue(offset);
					listeners.call(&Listener::offsetChanged, offset);

					DBG("Moved by " + String(degreeIndex - degreeSectorMouseOver) + "\tNew offset: " + String(offset));
					dirty = true;
				}

				lastDegreeSectorMouseIn = degreeSectorMouseOver;
				degreeSectorMouseOver = degreeIndex;

			}
		}

		else if (mouseDownRadius <= groupOuterRadius)
		{
			if (handleBeingDragged)
			{
				float distanceX = 10e6, distanceY = 10e6;
				float distance;
				int edgeIndex;
				if (adjacentEdgeIndicies.x > -1)
				{
					distanceX = abs(
						highlightedEdges[adjacentEdgeIndicies.x]
						.findNearestPointTo(event.position)
						.getDistanceFrom(event.position)
					);

					DBG("Distance from CCW edge: " + String(distanceX));
				}


				if (adjacentEdgeIndicies.y > -1)
				{
					distanceY = abs(
						highlightedEdges[adjacentEdgeIndicies.y]
						.findNearestPointTo(event.position)
						.getDistanceFrom(event.position)
					);

					DBG("Distance from CW edge: " + String(distanceY));
				}

				distance = jmin(distanceX, distanceY);

				if (distance <= handleDragThreshold)
				{

					// Figure out type of change (merge, resize, split)


					DBG("Passed threshold: " + String(handleDragThreshold));

					if (distance == distanceX)
					{


					}
					else
					{


					}

					// TODO: degreeGroupingChanged callback

					dirty = true;
				}
			}
		}
	}

	if (dirty)
	{
		updateGenerator();
	}
}

void GroupingCircle::addListener(Listener* listenerToAdd)
{
	listeners.add(listenerToAdd);
}

void GroupingCircle::removeListener(Listener* listenerToRemove)
{
	listeners.remove(listenerToRemove);
}

void GroupingCircle::degreeToModSelectedCallback(int degreeIndex)
{
	degreeIndexToMod = degreeIndex;
	degreeModCandidates = scaleStructure.findDegreeMods(degreeIndexToMod, -1);
	TextEditor* label = degreeLabels[degreeIndexToMod];
	label->setFont(label->getFont().withHeight(label->getHeight() / 3.0f));
	label->insertTextAtCaret("*");
	repaint();
}

void GroupingCircle::cancelDegreeMods()
{
	if (degreeIndexToMod > -1 && degreeIndexToMod < scaleStructure.getPeriod())
	{
		TextEditor* label = degreeLabels[degreeIndexToMod];
		label->setHighlightedRegion(Range<int>(label->getText().length() - 1, label->getText().length()));
		label->insertTextAtCaret(String());
		degreeIndexToMod = -1;
		degreeModCandidates.clear();
	}
	
}

float GroupingCircle::getNormalizedMouseAngle(const MouseEvent& event) const
{
	float angle = atan2f(event.position.x - center.x, center.y - event.position.y) + (groupSizes[0] / 2.0f * angleIncrement);
	if (angle < 0) angle += float_Tau;
	if (angle >= float_Tau) angle -= float_Tau;

	return angle;
}

int GroupingCircle::degreeSectorOfAngle(float angle) const
{
	return (int)(angle / angleIncrement) % groupChain.size();
}

int GroupingCircle::mouseInDegreeRingSector(const MouseEvent& event, float radiusFromCenter, float angle) const
{
	if (radiusFromCenter >= degreeInnerRadius && radiusFromCenter < degreeOuterRadius)
		return degreeSectorOfAngle(angle);

	return -1;
}

int GroupingCircle::mouseInGroupSector(int degreeIndex) const
{
	int groupIndex = 0;
	int groupSize = groupSizes[groupIndex];
	while (degreeIndex >= groupSize)
	{
		degreeIndex -= groupSize;

		if (degreeIndex < 0 || groupIndex == groupSizes.size() - 1)
			return -1;

		groupSize = groupSizes[++groupIndex];
	}
	
	return groupIndex;
}

int GroupingCircle::mouseInGroupRingSector(const MouseEvent& event, float radiusFromCenter, float angle) const
{
	if (radiusFromCenter >= degreeOuterRadius && radiusFromCenter < groupOuterRadius)
		mouseInGroupSector(degreeSectorOfAngle(angle));

	return -1;
}

int GroupingCircle::mouseInGroupRingSector(const MouseEvent& event, float radiusFromCenter, int degreeIndex) const
{
	if (radiusFromCenter >= degreeOuterRadius && radiusFromCenter < groupOuterRadius)
		mouseInGroupSector(degreeIndex);

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

/*
	Returns true if degree sector is a modification candidate
*/
bool GroupingCircle::isDegreeSectorIndexModCandidate(int degreeSectorIndexIn) const
{
	if (degreeSectorIndexIn >= 0 && degreeSectorIndexIn < degreeModCandidates.size())
	{
		const Point<int>& alteration = degreeModCandidates.getReference(groupChain[degreeSectorIndexIn]);
		return alteration.x >= 0 && alteration.y != 0;
	}
	return false;
}

/*
	Returns true if degree sector is altered
*/
bool GroupingCircle::isDegreeSectorIndexAltered(int degreeSectorIndexIn) const
{
	if (degreeSectorIndexIn >= 0 && degreeSectorIndexIn < degreeAlterations.size())
	{
		const Point<int>& alteration = degreeAlterations.getReference(groupChain[degreeSectorIndexIn]);
		return alteration.x >= 0 && alteration.y != 0;
	}
	return false;
}