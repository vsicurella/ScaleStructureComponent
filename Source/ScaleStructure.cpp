/*
  ==============================================================================

    ScaleStructure.cpp
    Created: 25 Jan 2020 5:12:32pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "ScaleStructure.h"

ScaleStructure::ScaleStructure(int periodIn)
{
	period = periodIn;
	coprimeGenerators = getCoprimes(period);
}

ScaleStructure::ScaleStructure(int periodIn, int generatorIndex, int sizeIndex, Array<int> degreeGroups)
{
	period = periodIn;

	coprimeGenerators = getCoprimes(period);
	generator = generatorIndex;

	// TODO: check for valid input
	calculateProperties();

	currentSizeSelected = sizeIndex;
	sizeGroupings = degreeGroups;
}

int ScaleStructure::getPeriod() const
{
    return period;
}

Array<int> ScaleStructure::getCoprimeGenerators() const
{
    return coprimeGenerators;
}

int ScaleStructure::getGenerator() const
{
    return generator;
}

int ScaleStructure::getNumPeriods() const
{
	return numFractionalPeriods;
}

int ScaleStructure::getFractionalPeriod() const
{
	return fPeriod;
}

int ScaleStructure::getFractionalGenerator() const
{
	return fGen;
}

Array<int> ScaleStructure::getScaleSizes() const
{
	return scaleSizes;
}

int ScaleStructure::getScaleSize(int ind) const
{
	return scaleSizes[ind];
}

Array<Point<int>> ScaleStructure::getKeyboardTypes() const
{
	return keyboardTypes;
}

Point<int> ScaleStructure::getKeyboardType(int ind) const
{
	return keyboardTypes[ind];
}

Array<PointPair<int>> ScaleStructure::getPGCoords() const
{
	return pgCoords;
}

PointPair<int> ScaleStructure::getPGCoord(int ind) const
{
	return pgCoords[ind];
}

int ScaleStructure::getGroupOfDegree(int scaleDegreeIn) const
{
	for (int g = 0; g < sizeGroupings.size(); g++)
	{
		if (degreeGroupings.getReference(g).contains(scaleDegreeIn))
			return g;
	}

	return -1;
}

void ScaleStructure::resetToPeriod(int periodIn)
{
	period = periodIn;
	coprimeGenerators = getCoprimes(period);
	generator = -1;
	currentSizeSelected = -1;

	scaleSizes.clear();
	keyboardTypes.clear();
	pgCoords.clear();
	stepSizes.clear();
	modmosProperties.clear();
}

void ScaleStructure::setGenerator(int generatorIn)
{
    generator = generatorIn;
	calculateProperties();
}

void ScaleStructure::setSizeIndex(int index)
{
	currentSizeSelected = index;
	useSuggestedSizeGrouping();
}

void ScaleStructure::setGeneratorOffset(int offsetIn)
{
	generatorOffset = offsetIn;
	useSuggestedSizeGrouping();
}

void ScaleStructure::setMODMOSProperties(Array<Point<int>> modmosPropertiesIn)
{
	modmosProperties = modmosPropertiesIn;
	fillSymmetricGrouping();
}


Point<int> ScaleStructure::getStepSizes(int kbdTypeIn) const
{
	return stepSizes[kbdTypeIn];
}

Array<int> ScaleStructure::getSizeGrouping() const
{
	return sizeGroupings;
}

Array<Array<int>> ScaleStructure::getDegreeGroupings() const
{
    return degreeGroupings;
}

Array<Point<int>> ScaleStructure::getMODMOSProperties() const
{
	return modmosProperties;
}

int ScaleStructure::naturalDegreeToScaleDegree(int naturalDegree) const
{
	naturalDegree = modulo(naturalDegree, scaleSizes[currentSizeSelected]);
	Array<int> naturalScale = degreeGroupings[0];
	naturalScale.sort();

	return naturalScale[naturalDegree];
}

int ScaleStructure::getAlterationOfDegree(int naturalDegree) const
{
	for (auto alteration : modmosProperties)
	{
		if (alteration.x == naturalDegree)
		{
			return alteration.y;
		}
	}

	return 0;
}

void ScaleStructure::setAlterationofDegree(int naturalDegree, int alteration)
{
	int currentAlteration = getAlterationOfDegree(naturalDegree);

	// Apply new MODMOS property
	if (currentAlteration != alteration)
	{
		int index = modmosProperties.indexOf(Point<int>(naturalDegree, currentAlteration));

		if (alteration == 0 && index >= 0)
		{
			modmosProperties.removeRange(index, 1);
		}
		else
		{
			if (index < 0)
			{
				modmosProperties.add(Point<int>(naturalDegree, alteration));
			}
			else
			{
				modmosProperties.set(index, Point<int>(naturalDegree, alteration));
			}
		}

		String dbgstr = "";

		for (auto alteration : modmosProperties)
		{
			dbgstr += alteration.toString() + '\n';
		}
		DBG("MODMOS Properties:\n" + dbgstr);
	}

	fillSymmetricGrouping();
}


int ScaleStructure::getSizeIndex() const
{
    return currentSizeSelected;
}

Point<int> ScaleStructure::getCurrentStepSize() const
{
	return stepSizes[currentSizeSelected];
}

void ScaleStructure::calculateProperties()
{
	// Clear all data dependent on Generator and Size choices
	scaleSizes.clear();
	keyboardTypes.clear();
	pgCoords.clear();
	modmosProperties.clear();

	// Calculate number of periods based on generator
	numFractionalPeriods = getGCD(period, generator);
	fPeriod = period / numFractionalPeriods;
	fGen = generator / numFractionalPeriods;

	DBG("CALCULATING");
	DBG("GCD of (" + String(period) + ", " + String(generator) + ") is " + String(numFractionalPeriods) + "\tfPeriod: " + String(fPeriod) + "\tfGen: " + String(fGen));

	// Calculate properties of scale
    Array<int> cf = getContinuedFraction((double)generator / period);

	// seed the sequence
	Point<int> parent1 = Point<int>(-1 + cf[0], 1);
	Point<int> parent2 = Point<int>(1, 0);
	Point<int> gp = Point<int>(cf[0], 1);

	Array<Point<int>> packet = { gp, parent2, gp + parent2 }; // makes for cleaner code
	pgCoords.add(PointPair<int>(packet[0], packet[1]));
	keyboardTypes.add(packet[2]);
	scaleSizes.add(packet[2].y);

	// find keyboard types, and their period/generator coordinates
	for (int i = 1; i < cf.size(); i++)
	{
		for (int d = 0; d < cf[i]; d++)
		{
			pgCoords.add(PointPair<int>(packet[0], packet[1]));
			keyboardTypes.add(packet[2]);
			scaleSizes.add(packet[2].y);

			parent1 = packet[0];
			parent2 = packet[1];
			gp = packet[2];

			// if previous continued fraction index is even,
			// set parent2 to previous result
			if (i % 2 == 0)
				parent1 = gp;
			else // if odd, set parent1 to previous result
				parent2 = gp;

			packet = { parent1, parent2, parent1 + parent2 };
		}
	}

	String dbgstr = "";
	for (auto s : keyboardTypes)
	{
		dbgstr += "(" + s.toString() + "), ";
	}

	DBG("Sizes available: " + dbgstr);

	calculateStepSizes();
	calculateGeneratorChain();
}

void ScaleStructure::calculateStepSizes()
{
	stepSizes.clear();

	Point<int> stepSizesOut;
	Point<int> periodCoordinate;
	Point<int> generatorCoordinate;

	for (int i = 0; i < scaleSizes.size(); i++)
	{
		generatorCoordinate = Point<int>(pgCoords[i].x.x, pgCoords[i].y.x);
		periodCoordinate = Point<int>(pgCoords[i].x.y, pgCoords[i].y.y);

		// find horiztonal step size (X)
		if (periodCoordinate.y == generatorCoordinate.y)
			stepSizesOut.setX(fPeriod - generator);
		else if (periodCoordinate.y == 0)
			stepSizesOut.setX(fPeriod);
		else if (generatorCoordinate.y == 0)
			stepSizesOut.setX(generator);
		else
			stepSizesOut.setX(fPeriod * generatorCoordinate.y - generator * periodCoordinate.y);

		// find upward right step size (Y)
		if (periodCoordinate.x == generatorCoordinate.x)
			stepSizesOut.setY(fPeriod - generator);
		else if (periodCoordinate.x == 0)
			stepSizesOut.setX(fPeriod);
		else if (generatorCoordinate.y == 0)
			stepSizesOut.setX(generator);
		else
			stepSizesOut.setY(generator * periodCoordinate.x - fPeriod * generatorCoordinate.x);

		stepSizes.add(stepSizesOut);
	}
}

void ScaleStructure::calculateGeneratorChain()
{
	generatorChain.clear();
	String dbgstr = "";
	for (int i = 0; i < fPeriod; i++)
	{
		generatorChain.add(modulo(i * generator, fPeriod));
		dbgstr += String(modulo(i * generator, fPeriod)) + ", ";
	}

	DBG("Generator chain: " + dbgstr);
}

void ScaleStructure::fillGroupingSymmetrically()
{
	DBG("Filling groups: filling degrees symmetrically");
	degreeGroupings.clear();

	Array<int> grouping;

	for (int i = 0; i < numFractionalPeriods; i++)
	{
		grouping.addArray(sizeGroupings);
	}

    degreeGroupings.resize(grouping.size());

    // Fill degree groups symmetrically

    int indexForward = generatorOffset;
    int indexBackwards = period - 1 + generatorOffset;
    int indexOffset;
    
    for (int t = 0; t < grouping.size(); t++)
    {
        for (int n = 0; n < scaleSizes[grouping[t]]; n++)
        {
            if (t % 2 == 0)
            {
                indexOffset = modulo(indexForward, period);
                indexForward++;
            }
            else
            {
                indexOffset = modulo(indexBackwards, period);
                indexBackwards--;
            }
            
            degreeGroupings.getReference(t).add(generatorChain[indexOffset]);
        }
    }

	String dbgstr = "";
	int size, sum = 0;
	for (int i = 0; i < grouping.size(); i++) {
		size = scaleSizes[grouping[i]];
		dbgstr += String(size) + ", ";
		sum += size;
	}
	dbgstr += " = " + String(sum);
	DBG("Using this size grouping: " + dbgstr);

	dbgstr = "";
	for (int group = 0; group < grouping.size(); group++)
	{
		Array<int> degreeGroup = degreeGroupings[group];
		dbgstr += "Tier " + String(group) + ": ";
		for (int deg = 0; deg < degreeGroup.size(); deg++)
		{
			dbgstr += String(degreeGroup[deg]) + ", ";
		}
		dbgstr += "\n";
	}

	DBG("Degree groupings: ");
	DBG(dbgstr);
}

void ScaleStructure::fillSymmetricGrouping()
{
	DBG("Filling groups: degrees already symmetric.");
	degreeGroupings.clear();
	degreeGroupings.resize(sizeGroupings.size() * numFractionalPeriods);

	// Fill degree groups symmetrically

	int indexOffset = modulo(generatorOffset, fPeriod);

	for (int t = 0; t < sizeGroupings.size(); t++)
	{
		for (int n = 0; n < scaleSizes[sizeGroupings[t]]; n++)
		{
			for (int f = 0; f < numFractionalPeriods; f++)
			{
				degreeGroupings.getReference(t).add(generatorChain[indexOffset] + fPeriod * f);
			}
			indexOffset = modulo(indexOffset + 1, fPeriod);
		}
	}

	// Rearrange degrees if MODMOS properties exist
	if (modmosProperties.size() > 0)
		applyMODMOSProperties();

	String dbgstr = "";
	int size, sum = 0;
	for (int i = 0; i < sizeGroupings.size(); i++) {
		size = scaleSizes[sizeGroupings[i]];
		dbgstr += String(size) + ", ";
		sum += size;
	}
	dbgstr += " = " + String(sum);
	DBG("Using this size grouping: " + dbgstr);

	dbgstr = "";
	for (int group = 0; group < sizeGroupings.size(); group++)
	{
		Array<int> degreeGroup = degreeGroupings[group];
		dbgstr += "Tier " + String(group) + ": ";
		for (int deg = 0; deg < degreeGroup.size(); deg++)
		{
			dbgstr += String(degreeGroup[deg]) + ", ";
		}
		dbgstr += "\n";
	}

	DBG("Degree groupings: ");
	DBG(dbgstr);
}

// TODO: Fix issues with generator offsets (modes).
//		- This algorithm finds and uses absolute degrees, which get affected by generator offset
//		- The proper solution would be to use relative degrees within the groups
//		- AKA swap values of degreeGrouping indicies, rather than values of generatorChain indicies
void ScaleStructure::applyMODMOSProperties()
{
	Array<int> naturalScale = degreeGroupings[0];
	naturalScale.sort();

	for (auto alteration : modmosProperties)
	{
		// Get current properties
		int naturalDegree = alteration.x;
		int scaleDegree = naturalScale[naturalDegree];
		int naturalGroupingIndex = degreeGroupings.getReference(0).indexOf(scaleDegree);

		int amount = alteration.y;
		int scaleSize = scaleSizes[currentSizeSelected];

		// Find the altered scale degree
		int originalChainIndex = generatorChain.indexOf(scaleDegree);
		int shiftAmount = amount * scaleSize;
		int shiftedIndex = generatorChain.indexOf(scaleDegree) + amount * scaleSize;
		int newGeneratorIndex = modulo(generatorChain.indexOf(scaleDegree) + amount * scaleSize, fPeriod);
		int alteredDegree = generatorChain[newGeneratorIndex];

		// Swap the scale degrees in degreeGroupings
		for (int i = 0; i < degreeGroupings.size(); i++)
		{
			Array<int>& group = degreeGroupings.getReference(i);

			if (group.contains(alteredDegree))
			{
				int indexToSwap = group.indexOf(alteredDegree);
				degreeGroupings.getReference(0).set(naturalGroupingIndex, alteredDegree);
				group.set(indexToSwap, scaleDegree);
				continue;
			}
		}
	}
}


int ScaleStructure::getSuggestedGenerator()
{
	int index = -1;
	float dif1, dif2 = 10e6;
	float interval = 1200.0f / period;
	int suggestedCents = 702;

	for (int i = 1; i < period; i++)
	{
		dif1 = abs(suggestedCents - interval * i);

		if (dif1 < dif2)
		{
			dif2 = dif1;
			index = i;
		}
	}

	return index;
}

int ScaleStructure::getSuggestedSizeIndex()
{
	int index = -1;
	int dif1, dif2 = INT_MAX;
	int scaleSize = 7 / numFractionalPeriods;

	for (int i = 0; i < scaleSizes.size(); i++)
	{
		dif1 = abs(scaleSize - scaleSizes[i]);

		if (dif1 < dif2)
		{
			dif2 = dif1;
			index = i;
		}
	}

	return index;
}

int ScaleStructure::getPrefferedSizeIndex(int prefferedSize, bool preferLarger)
{
	int index = -1;
	int dif1, dif2 = INT_MAX;
	int scaleSize = prefferedSize / numFractionalPeriods;

	for (int i = 0; i < scaleSizes.size(); i++)
	{
		dif1 = abs(scaleSize - scaleSizes[i]);

		if (dif1 < dif2 || (preferLarger && dif1 == dif2))
		{
			dif2 = dif1;
			index = i;
		}
	}

	return index;
}

Array<int> ScaleStructure::getNestedSizeGrouping()
{
	int scaleSize = scaleSizes[currentSizeSelected];
	Array<int> grouping = { currentSizeSelected };

	int notesLeft = fPeriod - scaleSize;
	int subSizeInd = currentSizeSelected;
	int subSize = scaleSize;

	while (notesLeft > 0)
	{
		int q = notesLeft / subSize;

		if (notesLeft <= subSize && scaleSizes.contains(notesLeft))
		{
			grouping.add(scaleSizes.indexOf(notesLeft));
			notesLeft = 0;
		}
		else if (q >= 2)
		{
			int numToAdd = notesLeft % subSize == 0 ? q : q - (q % 2);
			for (int n = 0; n < numToAdd; n++)
			{
				grouping.add(subSizeInd);
				notesLeft -= subSize;

				// check if notesLeft can be divided equally by next size
				if (subSizeInd > 0)
				{
					int stagedSubInd = subSizeInd - 1;
					int stagedSubSize = scaleSizes[stagedSubInd];
					int q2 = notesLeft / stagedSubSize;

					if (notesLeft % stagedSubSize == 0)
					{
						// extra check for symmetry
						if (q2 % 2 == 0)
						{
							for (int qn = 0; qn < q2; qn++)
							{
								grouping.add(stagedSubInd);
								notesLeft -= stagedSubSize;
							}

							break;
						}
					}
				}
			}
		}

		subSize = scaleSizes[--subSizeInd];
	}

	DBG("Nested group:");
	String dbgstr = "";
	for (int i = 0; i < grouping.size(); i++)
		dbgstr += String(scaleSizes[grouping[i]]) + ", ";
	DBG(dbgstr);

	return grouping;
}

// TODO: possibility for improving large scales groupings.
// When checking the "staged" sizes, there could be an additional check to see if
// a different configuration of those two sizes produce a smaller grouping
// ex. with this algorithm, "Schismatic" 69/118 scale gives (7 * 4) + (5 * 18) or 22 groups
// but this could be reduced to (7 * 14) + (5 * 4) for 18 groups
Array<int> ScaleStructure::getComplimentarySizeGrouping()
{
	int scaleSize = scaleSizes[currentSizeSelected];
	Array<int> grouping = { currentSizeSelected };

	int notesLeft = (period / numFractionalPeriods) - scaleSize;
	int subSizeInd = currentSizeSelected;
	int subSize = scaleSize;

	int q = notesLeft / subSize;
	int numToAdd = notesLeft % subSize == 0 ? q : q - (q % 2);
	int num = 0;

	while (notesLeft > 0)
	{
		// check if notesLeft can be divided equally by next size
		if (subSizeInd > 0)
		{
			int stagedSubInd = subSizeInd - 1;
			int stagedSubSize = scaleSizes[stagedSubInd];
			int q2 = notesLeft / stagedSubSize;

			if (notesLeft % stagedSubSize == 0)
			{
				// extra check for symmetry
				int groupSize = grouping.size() - 1;
				if (groupSize == 0 || q2 % 2 == 0)
				{
					for (int qn = 0; qn < q2; qn++)
					{
						grouping.add(stagedSubInd);
						notesLeft -= stagedSubSize;
					}

					break;
				}
			}
		}

		if (notesLeft <= subSize && scaleSizes.contains(notesLeft))
		{
			grouping.add(scaleSizes.indexOf(notesLeft));
			notesLeft = 0;
		}
		else if (q >= 0 && num < numToAdd)
		{
			grouping.add(subSizeInd);
			notesLeft -= subSize;
			num++;
		}
		else if (notesLeft > 0)
		{
			subSize = scaleSizes[--subSizeInd];
			q = notesLeft / subSize;
			numToAdd = notesLeft % subSize == 0 ? q : q - (q % 2);
			num = 0;
		}
	}

	DBG("Complimentary group:");
	String dbgstr = "";
	for (int i = 0; i < grouping.size(); i++)
		dbgstr += String(scaleSizes[grouping[i]]) + ", ";
	DBG(dbgstr);

	return grouping;
}

void ScaleStructure::useSuggestedSizeGrouping()
{
	Array<Array<int>> groupings;
	groupings.add(getNestedSizeGrouping());
	groupings.add(getComplimentarySizeGrouping());

	Array<float> groupingScores;

	// create score by averaging range and length of group
	for (auto g : groupings)
	{
		int range = g[0] - g[g.size() - 1];
		groupingScores.add((range + g.size()) / (float) groupings.size());
	}

	// find lowest score and return respective group
	int index = 0;
	for (int s = 0; s < groupings.size(); s++)
	{
		if (groupingScores[s] < groupingScores[index])
			index = s;
	}

	// make symmetric and fill degree groupings
	sizeGroupings = arrangeSymmetrically(groupings[index]);

	DBG("Symmetric group:");
	String dbgstr = "";
	for (int i = 0; i < sizeGroupings.size(); i++)
		dbgstr += String(scaleSizes[sizeGroupings[i]]) + ", ";
	DBG(dbgstr);

	fillSymmetricGrouping();
}

bool ScaleStructure::isValid() const
{	
	bool valid = true;

	if (generator < 0 || generator >= period)
	{
		DBG("Invalid generator");
		valid = false;
	}

	if (currentSizeSelected < 0 || currentSizeSelected >= scaleSizes.size())
	{
		DBG("Invalid scale size");
		valid = false;
	}

	int sum = 0;
	for (int s = 0; s < sizeGroupings.size(); s++)
	{
		sum += scaleSizes[sizeGroupings[s]] * numFractionalPeriods;
	}
	
	if (sum != period)
	{
		DBG("Invalid scale groupings");
		valid = false;
	}

	return valid;
}
