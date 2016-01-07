#ifndef __GIMMICK_H__
#define __GIMMICK_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
//#include "GLES-Render.h"

class Gimmick : public cocos2d::Sprite
{
public:
	//--------------------------------------------
	//! @brief クリエイト関数
	//--------------------------------------------
	static Gimmick* create(b2World* pWorld,int stage);

	//--------------------------------------------
	//! @brief 初期化処理
	//! @param[:b2pWorld]	pWorld
	//! @param[:int]		stage
	//--------------------------------------------
	virtual bool init(b2World* pWorld, int stage);

	//--------------------------------------------
	//! @brief ギミックのcreate関数
	//--------------------------------------------
	void createWall(int stage);
	void createRolling(int stage);
	void createSensor(int stage);
	void createExit(cocos2d::TMXObjectGroup* TurnGroup, 
					std::vector<cocos2d::Value,std::allocator<cocos2d::Value>> TurnArray);

	//--------------------------------------------
	//! @brief 出口の位置のGet関数
	//--------------------------------------------
	cocos2d::Vec2 GetexitSpritePosition();

	b2BodyDef m_Turnbody;

private:

	//物理ワールド
	b2World* m_pWorld;

	Sprite* m_exitSprite;

	cocos2d::TMXLayer* m_tilName;			// タイルの名前
	cocos2d::TMXTiledMap* m_tileMap;		//	タイルマップ
};

#endif // __GIMMICK_H__
