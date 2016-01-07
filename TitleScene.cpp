#include "TitleScene.h"

USING_NS_CC;

using namespace experimental;
using namespace cocos2d::ui;


Scene* TitleScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = TitleScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool TitleScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object

	auto Start = Button::create("Textuer/Start2.png");
	Start->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 8 + origin.y));

	Start->setScale(1.5f);
	this->addChild(Start, 1);

	Start->addTouchEventListener(CC_CALLBACK_2(TitleScene::touchEvent, this, startButton));
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    // add "TitlScene" splash screen"
    auto sprite = Sprite::create("Textuer/Title.png");

	sprite->setOpacity(0);

	FadeIn* fadeIn = FadeIn::create(1.0f);
	sprite->runAction(fadeIn);
	FadeTo* fadeTo = FadeTo::create(1.0f, 128);

	Sequence* sequence = Sequence::create(fadeTo, fadeIn, nullptr);
	RepeatForever* repeatForever = RepeatForever::create(sequence);

	FadeIn* fadein2 = FadeIn::create(1.0f);
	FadeTo* fadeTo2 = FadeTo::create(1.0f, 128);
	Sequence* sequence2 = Sequence::create(fadeTo2, fadein2, nullptr);
	RepeatForever* repeatForever2 = RepeatForever::create(sequence2);

	Start->runAction(repeatForever);
	
    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
	m_BGM = AudioEngine::play2d("Sound/game_maoudamashii_5_town15.ogg");

	AudioEngine::setVolume(m_BGM, 3.0f);
	AudioEngine::setLoop(m_BGM,true);

    return true;
}

void TitleScene::touchEvent(Ref* pSender, Widget::TouchEventType type, int Buttan)
{
	switch (type)
	{
	case Widget::TouchEventType::ENDED:

		AudioEngine::stop(m_BGM);

		int id = AudioEngine::play2d("Sound/se_maoudamashii_onepoint26.ogg");

		AudioEngine::setVolume(id, 3.0f);

		Scene* selectScene = StageSelect::createScene();

		//トランディション
		TransitionFade* tscene = TransitionFade::create(1.0f, selectScene);

		Director::getInstance()->replaceScene(tscene);

	}
}
