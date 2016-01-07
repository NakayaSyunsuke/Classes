#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

#include"StageSelect.h"
#include "audio/include/AudioEngine.h"
#include "cocos2d.h"


enum TitleButton
{
	startButton,
	ru_ruButton
};

class TitleScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // implement the "static create()" method manually
	CREATE_FUNC(TitleScene);

	void touchEvent(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type, int Buttan);


private:
	int m_BGM;
};

#endif // __HELLOWORLD_SCENE_H__
