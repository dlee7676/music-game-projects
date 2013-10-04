/* class GameObject
Contains information required for describing all elements that interact on the game screen, like the player,
enemies, and bullets.  Functions in the class modify or access the information stored in these objects. */  

#ifndef GO_H
#define GO_H

#include <d3d9.h>
#include <d3dx9.h>

class GameObject {

private:
	D3DXVECTOR3 pos, startPos;
	RECT bounds, drawingBounds;
	bool active, hit;
	float s, t, speed, type;
	int animTime;
	RECT testRect;

public:
	GameObject::GameObject();
	void init(float x, float y, float z, RECT bounds_, float type_);
	bool isActive();
	void setActive(bool setting);
	float getPos(int coord);
	D3DXVECTOR3 getPos();
	void setPos(float x, float y, float z);
	void setPos(int coord, float value);
	void move(float x, float y, float z);
	void setBounds(RECT bounds_);
	void setBounds(RECT bounds_, float x, float y); 
	RECT getBounds();
	RECT getDrawingBounds();
	bool inBounds(GameObject test, float x, float y);
	bool inBounds(RECT test, int x, int y, int xMod, int yMod); 
	D3DXVECTOR3 getStartPos();
	void setStartPos(D3DXVECTOR3 _startPos);
	float getS();
	void setS(float s_);
	float getT();
	void setT(float t_);
	bool isHit();
	void setHit(bool exploding_);
	int getAnimTime();
	void setAnimTime(int time);
	float getType();
	void setType(float type_);
	float getSpeed();
	void setSpeed(float speed_);
};
	
#endif

