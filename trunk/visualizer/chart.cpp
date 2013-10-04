#include "chart.h"

Chart::Chart() {};

/* void init(vector<Enemy>* pEnemies_, int* columns_): initializes an uncleared level with time 0.
	Parameters:
	vector<Enemy>* pEnemies_: pointer to a vector of enemies that will be modified in the level.
	int* columns: x positions of the columns containing the notes. */

void Chart::init(vector<GameObject>* pNotes_, float* columns_) {
	pNotes = pNotes_;
	columns = columns_;
	target.left = 10;
	target.top = 27;
	target.right = 50;
	target.bottom = 36;
	lastCol = -1;
	offset = 0;
}

// setter for speed modifier used when loading the chart
void Chart::setSpeed(float speed_) {
	initialSpeed = speed_;
}

/* void placeObject(vector<GameObject>* vec, float x, float y, float z, RECT bounds, float type):
	Places a game object at a given position and defines its attributes.
	Parameters:
	vector<GameObject>* vec: a vector of objects to add the new object to.
	float x, y, z: The coordinates to place the object at.
	RECT bounds: The bounding box to use for the object.
	float type: the type of note to place (quarter, eighth, etc.) */
void Chart::placeObject(vector<GameObject>* vec, float x, float y, float z, RECT bounds, float type) {
	GameObject next;
	next.init(x,y,z,bounds,type);
	vec->push_back(next);
}

/* void placeObject(vector<GameObject>* vec, float beat, float column, RECT bounds, float type):
	Overload of placeObject. Places a game object according to a chosen beat of a song and defines its attributes.
	Parameters:
	vector<GameObject>* vec: a vector of objects to add the new object to.
	float beat: The beat of the song the object should represent; the position is calculated from this.
	RECT bounds: The bounding box to use for the object.
	float type: the type of note to place (quarter, eighth, etc.) */
void Chart::placeObject(vector<GameObject>* vec, float beat, float column, RECT bounds, float type) {
	GameObject next;
	/* places the object at a y coordinate of the distance travelled in a beat at 170 beats per minute (the speed used to initially synchronize the note scrolling),
	adjusted by the current speed modifier and the position of the target zone (one beat's distance). */
	next.init(column,(float)BEATDIST*beat*initialSpeed+(float)BEATDIST,0,bounds,type);
	vec->push_back(next);
}

/* void setNotes(float start, float end, float type):
	Places notes randomly with a given timing between two chosen beats.
	Parameters:
	float start: the beat to start placing notes at.
	float end: the beat to stop placing notes at (not included in the range).
	float type: the note timing (quarter, eighth, etc.) */
void Chart::setNotes(float start, float end, float type) {
	for (float i = start; i < end; i+=type) {
		float nextCol = columns[rand()%4];
		// place each note in a different column than the last
		while (nextCol == lastCol)
			nextCol = columns[rand()%4];
		lastCol = nextCol;
		float nextType = (i-offset) - (int)(i);
		if (nextType == 0)
			nextType = QUARTER;
		else if (fmodf(nextType, 0.5) == 0)
			nextType = EIGHTH;
		else if (fmodf(nextType, 0.25) == 0)
			nextType = SIXTEENTH;
		else if (fmodf(nextType, 0.125) == 0)
			nextType = THIRTYSECOND;
		placeObject(pNotes, i, nextCol, target, nextType);
	}
}

/* void Chart::placeTargets(vector<GameObject>* targetZone): places the target zone at the top of the screen.
	Parameters:
	vector<GameObject>* targetZone: the vector that will contain the target objects. */
void Chart::placeTargets(vector<GameObject>* targetZone) {
	placeObject(targetZone, columns[0], BEATDIST-5, 0, target, 0);
	placeObject(targetZone, columns[1], BEATDIST-5, 0, target, 0);
	placeObject(targetZone, columns[2], BEATDIST-5, 0, target, 0);
	placeObject(targetZone, columns[3], BEATDIST-5, 0, target, 0);
}

/* void loadNoteChart(size_t song_): sets the positions of notes for a given song. 
	Parameters:
	size_t song_: the song that has been chosen. */
void Chart::loadNoteChart(size_t song_) {
	if (song_ == 0) {
		offset = 0.5;
		setNotes(8.5,25,QUARTER);
		setNotes(25,41.5,EIGHTH);
		setNotes(41.5,45.5,QUARTER);
		setNotes(45.5,47,SIXTEENTH);
		setNotes(47.5,49,SIXTEENTH);
		setNotes(49.5,53,QUARTER);
		setNotes(53.5,55,SIXTEENTH);
		setNotes(55.5,57,SIXTEENTH);
		setNotes(57.5,61.5,QUARTER);
		setNotes(61.5,63,SIXTEENTH);
		setNotes(63.5,65,SIXTEENTH);
		setNotes(65.5,69.5,QUARTER);
	}

	if (song_ == 1) {
		offset = 0.5;
		setNotes(8.5,9.5,QUARTER);
		setNotes(9.5,10,SIXTEENTH);
		setNotes(10.25,11,QUARTER);
		setNotes(12,12.75,SIXTEENTH);
		setNotes(13,15.5,EIGHTH);
		setNotes(15.5,16.25,SIXTEENTH);
		setNotes(16.5,17,QUARTER);
		setNotes(17,17.75,SIXTEENTH);
		setNotes(18.25,19,QUARTER);
		setNotes(19,20,QUARTER);
		setNotes(19.5,20.5,QUARTER);
		setNotes(20.5,22.25,SIXTEENTH);
		setNotes(22.5,23.5,QUARTER);
		setNotes(23,24,QUARTER);
		setNotes(23.5,23.8f,THIRTYSECOND);
		setNotes(24,24.2f,THIRTYSECOND);
		setNotes(24.5,25,QUARTER);
		setNotes(24.75,25.75,QUARTER);
		setNotes(25.25,26,SIXTEENTH);
		setNotes(26.5,27.5,QUARTER);
		setNotes(27.25,28.25,QUARTER);
		setNotes(27.75,28.75,QUARTER);
		setNotes(28.5,30.5,SIXTEENTH);
		setNotes(30.75,31.75,QUARTER);
		setNotes(31.25,32.25,QUARTER);
		setNotes(31.625,32,THIRTYSECOND);
		setNotes(33,49,1.0f/6.0f);
		setNotes(49.25,51,SIXTEENTH);
	}
}