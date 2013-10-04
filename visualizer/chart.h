/* class Chart
This class controls the placement of notes in a note chart for a song. */

#ifndef CHART_H
#define CHART_H

#include <vector>
#include <string>
#include <map>
#include "gameObject.h"

#define BEATDIST 41.75
#define QUARTER 1
#define EIGHTH 0.5
#define SIXTEENTH 0.25
#define THIRTYSECOND 0.125

using namespace std;

class Chart {
private:
	// pointer to a container for notes to use in the chart
	vector<GameObject>* pNotes; 
	float offset, bpm, initialSpeed, lastCol;
	float* columns; 
	RECT note, target;

public:
	Chart();
	void init(vector<GameObject>* pNotes_, float* columns);
	void placeTargets(vector<GameObject>* targetZone);
	void setSpeed(float speed_);
	void placeObject(vector<GameObject>* vec, float x, float y, float z, RECT bounds, float type);
	void placeObject(vector<GameObject>* vec, float beat, float column, RECT bounds, float type);
	void setNotes(float start, float end, float type);
	void loadNoteChart(size_t song_);
};

#endif