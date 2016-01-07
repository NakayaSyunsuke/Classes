#ifndef __HELP_SCENE_H__
#define __HELP_SCENE_H__

#include "cocos2d.h"
#include"StageSelect.h"
#include "audio/include/AudioEngine.h"

class Help : public cocos2d::Layer
{
public:

	//--------------------------------------------
	//! @brief シーン作成
	//--------------------------------------------
	static cocos2d::Scene* createScene();

	//--------------------------------------------
	//! @brief 初期化処理
	//--------------------------------------------
	virtual bool init();

	//--------------------------------------------
	//! @brief メニュー
	//! @param[Ref] pSender
	//--------------------------------------------
	void menuCloseCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(Help);

private:

	cocos2d::Sprite* sprite;　	//スプライト
	cocos2d::Menu* menu;		//メニュー
};


#endif // __HELP_SCENE_H__