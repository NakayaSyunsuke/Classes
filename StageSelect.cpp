#include "StageSelect.h"

USING_NS_CC;

using namespace experimental;
using namespace cocos2d::ui;
using namespace std;



Scene* StageSelect::createScene()
{

	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = StageSelect::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

StageSelect::~StageSelect()
{

}


// on "init" you need to initialize your instance
bool StageSelect::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//ステージボタン1の作成
	auto stage1 = Button::create("Textuer/SelectButtan1.png");
	stage1->setPosition(Vec2(visibleSize.width / 5 + origin.x, visibleSize.height / 1.5 + origin.y));
	stage1->setScale(1.5f);
	this->addChild(stage1, 0);

	//ステージボタン2の作成
	auto stage2 = Button::create("Textuer/SelectButtan2.png");
	stage2->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 1.5 + origin.y));
	stage2->setScale(1.5f);
	this->addChild(stage2, 0);

	//ステージボタン3の作成
	auto stage3 = Button::create("Textuer/SelectButtan3.png");
	stage3->setPosition(Vec2(visibleSize.width / 1.25 + origin.x, visibleSize.height / 1.5 + origin.y));
	stage3->setScale(1.5f);
	this->addChild(stage3, 0);

	//ステージボタン4の作成
	auto stage4 = Button::create("Textuer/SelectButtan4.png");
	stage4->setPosition(Vec2(visibleSize.width / 5 + origin.x, visibleSize.height / 2 + origin.y));
	stage4->setScale(1.5f);
	this->addChild(stage4, 0);

	//ステージボタン5作成
	auto stage5 = Button::create("Textuer/SelectButtan5.png");
	stage5->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	stage5->setScale(1.5f);
	this->addChild(stage5, 0);

	//ステージボタン6作成
	auto stage6 = Button::create("Textuer/SelectButtan6.png");
	stage6->setPosition(Vec2(visibleSize.width / 1.25 + origin.x, visibleSize.height / 2 + origin.y));
	stage6->setScale(1.5f);
	this->addChild(stage6, 0);

	//ステージボタン7作成
	auto stage7 = Button::create("Textuer/SelectButtan7.png");
	stage7->setPosition(Vec2(visibleSize.width / 2.8 + origin.x, visibleSize.height / 3 + origin.y));
	stage7->setScale(1.5f);
	this->addChild(stage7, 0);

	//ステージボタン8作成
	auto stage8 = Button::create("Textuer/SelectButtan8.png");
	stage8->setPosition(Vec2(visibleSize.width / 1.5 + origin.x, visibleSize.height / 3 + origin.y));
	stage8->setScale(1.5f);
	this->addChild(stage8, 0);

	//Helpボタンの作成
	auto helpButton = Button::create("Textuer/HelpButton.png");
	helpButton->setPosition(Vec2(origin.x + visibleSize.width - helpButton->getContentSize().width / 1.7,
		origin.y + helpButton->getContentSize().height / 2));
	helpButton->setScale(1.4f);
	this->addChild(helpButton, 0);


	//それぞれのボタンイベント
	stage1->addTouchEventListener(CC_CALLBACK_2(StageSelect::touchEvent, this, STAGE1,NULL));
	stage2->addTouchEventListener(CC_CALLBACK_2(StageSelect::touchEvent, this, STAGE2, NULL));
	stage3->addTouchEventListener(CC_CALLBACK_2(StageSelect::touchEvent, this, STAGE3, NULL));
	stage4->addTouchEventListener(CC_CALLBACK_2(StageSelect::touchEvent, this, STAGE4, NULL));
	stage5->addTouchEventListener(CC_CALLBACK_2(StageSelect::touchEvent, this, STAGE5, NULL));
	stage6->addTouchEventListener(CC_CALLBACK_2(StageSelect::touchEvent, this, STAGE6, NULL));
	stage7->addTouchEventListener(CC_CALLBACK_2(StageSelect::touchEvent, this, STAGE7, NULL));
	stage8->addTouchEventListener(CC_CALLBACK_2(StageSelect::touchEvent, this, STAGE8, NULL));
	helpButton->addTouchEventListener(CC_CALLBACK_2(StageSelect::touchEvent, this, NULL, HELPBUTTON));

	auto sprite = Sprite::create("Textuer/Select.png");

	sprite->setPosition(visibleSize.width / 2 + origin.x, visibleSize.height / 1.2 + origin.y);

	sprite->setScale(1.5f);

	this->addChild(sprite, 0);
	
	// 作成したパーティクルのプロパティリストを読み込み
	m_particle = ParticleSystemQuad::create("Particle/particle_SelectSceneUp.plist");

	// パーティクルを開始
	m_particle->resetSystem();

	// パーティクルを表示する場所の設定
	m_particle->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height + origin.y));

	/*
	m_particle->setStartColor(Color4F::RED);

	m_particle->setEndColor(Color4F::RED);
	*/
	m_particle->setAutoRemoveOnFinish(true);
	// パーティクルを配置
	this->addChild(m_particle, -1);

	ParticleSystemQuad* m_particle2 = ParticleSystemQuad::create("Particle/particle_SelectSceneUnder.plist");

	// パーティクルを開始
	m_particle2->resetSystem();

	// パーティクルを表示する場所の設定
	m_particle2->setPosition(Vec2(480, -100));

	m_particle2->setAutoRemoveOnFinish(true);
	// パーティクルを配置
	this->addChild(m_particle2, -1);

	UserDefault*userDefault = UserDefault::getInstance();

	m_D_flag = userDefault->getIntegerForKey("Flag", TRUE);
	
	if (m_D_flag == TRUE)
	{
		Help* description = Help::create();

		this->addChild(description);
	}

	return true;
}

void StageSelect::touchEvent(Ref* pSender, Widget::TouchEventType touchType, int stage, int button)
{
	switch (touchType)
	{
	case Widget::TouchEventType::ENDED:
		int id = AudioEngine::play2d("Sound/se_maoudamashii_onepoint26.ogg");

		AudioEngine::setVolume(id, 3.0f);

		if (button == HELPBUTTON)
		{
			Help* description = Help::create();

			this->addChild(description);
		}
		else
		{
			Scene* playScene = PlayScene::createScene(stage);

			//トランディション
			TransitionFade* tscene = TransitionFade::create(1.0f, playScene);

			Director::getInstance()->replaceScene(tscene);
		}
		break;
	}
}
