#ifndef __GIMMICK_H__
#define __GIMMICK_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
//#include "GLES-Render.h"

class Gimmick : public cocos2d::Sprite
{
public:
	//--------------------------------------------
	//! @brief �N���G�C�g�֐�
	//--------------------------------------------
	static Gimmick* create(b2World* pWorld,int stage);

	//--------------------------------------------
	//! @brief ����������
	//! @param[:b2pWorld]	pWorld
	//! @param[:int]		stage
	//--------------------------------------------
	virtual bool init(b2World* pWorld, int stage);

	//--------------------------------------------
	//! @brief �M�~�b�N��create�֐�
	//--------------------------------------------
	void createWall(int stage);
	void createRolling(int stage);
	void createSensor(int stage);
	void createExit(cocos2d::TMXObjectGroup* TurnGroup, 
					std::vector<cocos2d::Value,std::allocator<cocos2d::Value>> TurnArray);

	//--------------------------------------------
	//! @brief �o���̈ʒu��Get�֐�
	//--------------------------------------------
	cocos2d::Vec2 GetexitSpritePosition();

	b2BodyDef m_Turnbody;

private:

	//�������[���h
	b2World* m_pWorld;

	Sprite* m_exitSprite;

	cocos2d::TMXLayer* m_tilName;			// �^�C���̖��O
	cocos2d::TMXTiledMap* m_tileMap;		//	�^�C���}�b�v
};

#endif // __GIMMICK_H__
