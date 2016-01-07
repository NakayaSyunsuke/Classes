#ifndef __STAGESELECT_H__
#define __STAGESELECT_H__

#include "cocos2d.h"
#include "audio/include/AudioEngine.h"
#include "PlayScene.h"
#include"ui/CocosGUI.h"

#define TRUE	1
#define FALSE	0

enum
{
	STAGE1 = 1,
	STAGE2 = 2,
	STAGE3 = 3,
	STAGE4 = 4,
	STAGE5 = 5,
	STAGE6 = 6,
	STAGE7 = 7,
	STAGE8 = 8,
	HELPBUTTON=9
};

class StageSelect : public cocos2d::Layer
{
private:

	int m_D_flag;	//ルール説明をするかどうかのフラグ

public:

	~StageSelect();

	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// a selector callback
	//void menuCloseCallback(cocos2d::Ref* pSender);

	//--------------------------------------------
	//! @brief	次のステージへのシーン遷移
	//--------------------------------------------
	void touchEvent(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type, int stage,int button);

	// implement the "static create()" method manually
	CREATE_FUNC(StageSelect);

private:
	cocos2d::ParticleSystemQuad* m_particle;

};


#endif // __STAGESELECT_H__