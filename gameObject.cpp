#include "gameObject.h"

GameObject::GameObject() :pos(0,0,0), active(false) {

}

/* void init(float x, float y, float z, RECT bounds_, int type_, float speed_):
	Initializes a game object with the following characteristics:
	Parameters:
	float x, y, z: the coordinates of the object's starting position.
	RECT bounds_: the bounding box of the enemy.
	int type_: the kind of object this is. 
	float speed: A higher value makes the enemy move faster. */

void GameObject::init(float x, float y, float z, RECT bounds_, float type_, float speed_) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
	startPos.x = x;
	startPos.y = y;
	startPos.z = z;
	s = 0.0f;
	t = 0.0f;
	drawingBounds.left = bounds_.left;
	drawingBounds.top = bounds_.top;
	drawingBounds.right = bounds_.right;
	drawingBounds.bottom = bounds_.bottom;
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = bounds_.right - bounds_.left;
	bounds.bottom = bounds_.bottom - bounds_.top;
	active = true;
	exploding = false;
	animTime = 10;
	type = type_;
	speed = speed_;
}

/* bool inBounds(GameObject test, float x, float y): checks if this object's bounding box overlaps with 
the bounding box of another object.
	Parameters:
	GameObject test: the object to check against.
	float x, y: translates the bounding box to be checked by x and y. */

bool GameObject::inBounds(GameObject test, float x, float y) {
	testRect = test.getBounds();
	if (testRect.right + x + test.getPos(0) < bounds.left + pos.x || testRect.bottom + y + test.getPos(1) < bounds.top + pos.y || 
		testRect.left + x + test.getPos(0) > bounds.right + pos.x || testRect.top + y + test.getPos(1) > bounds.bottom + pos.y)
		return false;
	else return true;
}

// getter and setter for whether this object is active
bool GameObject::isActive() {
	if (active)
		return true;
	else return false;
}
void GameObject::setActive(bool setting) {
	active = setting;
}

/* getters and setters for the position of the object */
// returns the position vector
D3DXVECTOR3 GameObject::getPos() {
	return pos;
}
// returns an individual component of the position vector
float GameObject::getPos(int coord) {
	switch(coord) {
		case 0:
			return pos.x;
		case 1:
			return pos.y;
		case 2:
			return pos.z;	
		default:
			return pos.x;
	}
}
// set all position vector components 
void GameObject::setPos(float x, float y, float z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
}
// set an individual component of the position vector
void GameObject::setPos(int coord, float value) {
	switch(coord) {
		case 0:
			pos.x = value;
			break;
		case 1:
			pos.y = value;
			break;
		case 2:
			pos.z = value;	
			break;
		default:
			pos.x = value;
			break;
	}
}

// adds the parameter values to the position vector instead of setting the vector to them
void GameObject::move(float x, float y, float z) {
	pos.x += x;
	pos.y += y;
	pos.z += z;
}

// getter and setter for the initial position vector of the object
D3DXVECTOR3 GameObject::getStartPos() {
	return startPos;
}
void GameObject::setStartPos(D3DXVECTOR3 _startPos) {
	startPos = _startPos;
}

// getter and setter for the position of the object along its movement path
float GameObject::getS() {
	return s;
}
void GameObject::setS(float s_) {
	s = s_;
}

// getter and setter for the position of the object if it is moving in a spiral function
float GameObject::getT() {
	return t;
}
void GameObject::setT(float t_) {
	t = t_;
}

// getter and setter for the bounding box of the object
RECT GameObject::getBounds() {
	return bounds;
}
void GameObject::setBounds(RECT bounds_) {
	bounds.left = 0;
	bounds.top = 0;
	bounds.right = bounds_.right - bounds_.left;
	bounds.bottom = bounds_.bottom - bounds_.top;
}

// getter for the coordinates of the bounding box on the sprite sheet
RECT GameObject::getDrawingBounds() {
	return drawingBounds;
}

// getter and setter for whether the object is destroyed
bool GameObject::isExploding() {
	return exploding;
}
void GameObject::setExploding(bool exploding_) {
	exploding = exploding_;
}

// getter and setter for the length of time to display animation frames for the object
int GameObject::getAnimTime() {
	return animTime;
}
void GameObject::setAnimTime(int time) {
	animTime = time;
}

// getter and setter for the object's type
float GameObject::getType() {
	return type;
}
void GameObject::setType(float type_) {
	type = type_;
}

// getter and setter for the object's speed
float GameObject::getSpeed() {
	return speed;
}
void GameObject::setSpeed(float speed_) {
	speed = speed_;
}