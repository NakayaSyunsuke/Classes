#ifndef __ARROW_H__
#define __ARROW_H__

#include "cocos2d.h"
#include "GLES-Render.h"
#include "Box2D/Box2D.h"
#include "ui/CocosGUI.h"

class Arrow : public cocos2d::Sprite
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static Arrow* create(b2World* pWorld);

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init(b2World* pWorld);

	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Sprite* createSprite();

	// implement the "static create()" method manually
	//CREATE_FUNC(Arrow);

private:
	b2World* m_pWorld;

};

#endif // __ARROW_H__