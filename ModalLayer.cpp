#include "ModalLayer.h"

USING_NS_CC;

using namespace cocos2d::ui;
using namespace experimental;

Scene* ModalLayer::createScene(int bgm, float bgmTime)
{

	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = ModalLayer::create(bgm, bgmTime);

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

ModalLayer* ModalLayer::create(int bgm, float bgmTime)
{
	ModalLayer *pRet = new(std::nothrow) ModalLayer();
	if (pRet && pRet->init(bgm, bgmTime))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool ModalLayer::init(int bgm, float bgmTime)
{
	if (!Layer::init()){
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto listener = EventListenerTouchOneByOne::create();

	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch* touch, Event*event)->bool{
		return true;
	};

	auto dispatcher = Director::getInstance()->getEventDispatcher();

	dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	Sprite* sprite = Sprite::create("Textuer/pause.png");
	sprite->setAnchorPoint(Vec2(0.0f, 0.0f));
	this->addChild(sprite);

	m_bgmTime = bgmTime;
	m_bgm = bgm;

	//再開ボタンの作成
	auto reStart = Button::create("Textuer/Start2.png");
	reStart->setScale(1.5f);
	reStart->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 1.6 + origin.y));
	this->addChild(reStart, 2);
	reStart->addTouchEventListener(CC_CALLBACK_2(ModalLayer::touchEvent, this, ReStartButton));

	//セレクトシーン遷移ボタンの作成
	auto selectScene = Button::create("Textuer/SlectScene.png");
	selectScene->setScale(1.5f);
	selectScene->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2.1 + origin.y));
	this->addChild(selectScene, 2);
	selectScene->addTouchEventListener(CC_CALLBACK_2(ModalLayer::touchEvent, this, SlectSceneButton));

	//タイトルシーン遷移のボタンの作成
	auto titleScene = Button::create("Textuer/TitleScene.png");
	titleScene->setScale(1.5f);
	titleScene->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 3.1 + origin.y));
	this->addChild(titleScene, 2);
	titleScene->addTouchEventListener(CC_CALLBACK_2(ModalLayer::touchEvent, this, TitleButton));

	return true;
}

void ModalLayer::touchEvent(Ref* pSender, Widget::TouchEventType type,int button)
{
	auto director = Director::getInstance();

	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::ENDED:

		//オーディオを鳴らす
		int id = AudioEngine::play2d("Sound/se_maoudamashii_onepoint26.ogg");
		AudioEngine::setVolume(id, 3.0f);

		if (button == SlectSceneButton)
		{
			director->resume();
			this->removeFromParentAndCleanup(true);
			Scene*  stageSelect = StageSelect::createScene();
		
			//トランディション
			TransitionFade* tscene = TransitionFade::create(1.0f, stageSelect);
			director->replaceScene(tscene);
		}
		
		else if (button == ReStartButton)
		{
			//BGMの追加
			int id = AudioEngine::setCurrentTime(m_bgm, m_bgmTime);
			
			director->resume();
			this->removeFromParentAndCleanup(true);
		}

		else if (button == TitleButton)
		{
			director->resume();
			this->removeFromParentAndCleanup(true);
			Scene*  titleScene = TitleScene::createScene();
		
			//トランディション
			TransitionFade* tscene = TransitionFade::create(1.0f, titleScene);
			director->replaceScene(tscene);
		}

		break;
	}
}
