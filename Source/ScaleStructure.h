/*
  ==============================================================================

    ScaleStructure.h
    Created: 25 Jan 2020 5:12:32pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once

#include "ScaleMath.h"

class ScaleStructure
{
	int period;
	int generator = -1;
	
	int numFractionalPeriods;
	int fPeriod;
	int fGen = -1;

	Array<int> coprimeGenerators; // not needed anymore

	Array<int> scaleSizes;
	Array<Point<int>> keyboardTypes;
	Array<PointPair<int>> pgCoords;
	Array<Point<int>> stepSizes;

	Array<int> sizeGroupings;
	Array<Array<int>> degreeGroupings;
	Array<int> generatorChain;
	Array<Point<int>> modmosProperties;

	int currentSizeSelected = -1;
	int generatorOffset = 0;

	void calculateProperties();
	void calculateStepSizes();
	void calculateGeneratorChain();
	void fillGroupingSymmetrically();
	void fillSymmetricGrouping();
	void applyMODMOSProperties();

public:

	ScaleStructure(int periodIn);
	ScaleStructure(int periodIn, int generatorIndex, int sizeIndex, Array<int> degreeGroups=Array<int>());
	ScaleStructure(const ScaleStructure& scaleToCopy);
	ScaleStructure(ValueTree scaleStructureProperties);
    
    int getPeriod() const;
	int getGenerator() const;

	int getNumPeriods() const;
	int getFractionalPeriod() const;
	int getFractionalGenerator() const;

	Array<int> getCoprimeGenerators() const;

	Array<int> getScaleSizes() const;
	int getScaleSize(int ind) const;

	Array<Point<int>> getKeyboardTypes() const;
	Point<int> getKeyboardType(int ind) const;

	Array<PointPair<int>> getPGCoords() const;
	PointPair<int> getPGCoord(int ind) const;

	Point<int> getStepSizes(int kbdTypeIn) const;
	int getGroupOfDegree(int scaleDegreeIn) const;

	Array<int> getSizeGrouping() const;
    Array<Array<int>> getDegreeGroupings() const;

	const Array<int>& getGeneratorChainReference();
	const Array<int>& getGroupingSizesReference();

	Array<Point<int>> getMODMOSProperties() const;
	int naturalDegreeToScaleDegree(int naturalDegree) const;
	int getAlterationOfDegree(int naturalDegree) const;
	void setAlterationofDegree(int naturalDegree, int alteration);
    
    int getSizeIndex() const;
	Point<int> getCurrentStepSize() const;

	bool isValid() const;

	void resetToPeriod(int periodIn);

	void setGenerator(int generatorIn);
	void setSizeIndex(int index);
	void setGeneratorOffset(int offsetIn);

	/*
		Input a mapping of scale degrees and chroma alteration values. 
		x of Point is a scale degree between 0 and scaleSize, in numerical order (not chain of generators)
		y of Point is the amount of chroma alterations, usually +/- 1
	*/
	void setMODMOSProperties(Array<Point<int>> modmosPropertiesIn);

	// returns the index whose generator is closest to a perfect fifth
	int getSuggestedGenerator();

	// returns the index whose size is closest to 7
	int getSuggestedSizeIndex();
	int getPrefferedSizeIndex(int prefferedSize, bool preferLarger = true);

	Array<int> getNestedSizeGrouping();
	Array<int> getComplimentarySizeGrouping();
	void useSuggestedSizeGrouping();
};
