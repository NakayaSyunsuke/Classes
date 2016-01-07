#include "Help.h"

USING_NS_CC;

using namespace experimental;


//--------------------------------------------
//	機能	:	シーン作成
//
//	引数	:	なし
//
//	戻り値	:	なし
//--------------------------------------------
Scene* Help::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Help::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}


//--------------------------------------------
//	機能	:	初期化
//
//	引数	:	なし
//
//	戻り値	:	真偽
//--------------------------------------------
bool Help::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"Textuer/Start.png",
		"Textuer/Start.png",
		CC_CALLBACK_1(Help::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 1.7,
		origin.y + closeItem->getContentSize().height / 2));

	closeItem->setScale(1.5f);

	// create menu, it's an autorelease object
	menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label

	auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);

	// add "Help" splash screen"
	sprite = Sprite::create("Textuer/Description.png");

	sprite->setScale(0.8f);
	// position the sprite on the center of the screen
	sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

	// add the sprite as a child to this layer
	this->addChild(sprite, 1);

	return true;
}

//--------------------------------------------
//	機能	:	ボタンを押してのシーン遷移
//
//	引数	:	Ref* pSender
//
//	戻り値	:	なし
//--------------------------------------------
void Help::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif

	removeChild(menu);

	int id = AudioEngine::play2d("Sound/se_maoudamashii_onepoint26.ogg");

	AudioEngine::setVolume(id, 3.0f);

	FadeOut*fadeOut = FadeOut::create(1.0f);
	FadeOut*fadeOut2 = FadeOut::create(1.0f);

	menu->runAction(fadeOut);
	sprite->runAction(fadeOut2);

	UserDefault*userDefault = UserDefault::getInstance();
	
	//ユーザーデフォルトにデータの格納
	userDefault->setIntegerForKey("Flag", FALSE);

	//ファイルデータを書き込む
	userDefault->flush();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

}
