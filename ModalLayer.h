#ifndef __MODALLAYER__H__
#define __MODALLAYER__H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "StageSelect.h"

class ModalLayer :public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene(int bgm, float bgmTime);

	static ModalLayer* create(int bgm, float bgmTime);

	virtual bool init(int bgm, float bgmTime);

	void touchEvent(Ref* pSender,cocos2d::ui::Widget::TouchEventType type,int button);

private:

	float m_bgmTime;

	int m_bgm;
};

#endif//#defndef __MODALLAYER__MODALLAYER__o