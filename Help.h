#ifndef __HELP_SCENE_H__
#define __HELP_SCENE_H__

#include "cocos2d.h"
#include"StageSelect.h"
#include "audio/include/AudioEngine.h"

class Help : public cocos2d::Layer
{
public:

	//--------------------------------------------
	//! @brief �V�[���쐬
	//--------------------------------------------
	static cocos2d::Scene* createScene();

	//--------------------------------------------
	//! @brief ����������
	//--------------------------------------------
	virtual bool init();

	//--------------------------------------------
	//! @brief ���j���[
	//! @param[Ref] pSender
	//--------------------------------------------
	void menuCloseCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(Help);

private:

	cocos2d::Sprite* sprite;�@	//�X�v���C�g
	cocos2d::Menu* menu;		//���j���[
};


#endif // __HELP_SCENE_H__