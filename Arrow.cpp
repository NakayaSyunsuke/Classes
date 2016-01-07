#include "Arrow.h"

using namespace std;

USING_NS_CC;
#define PTM_RATIO  32

Arrow* Arrow::create(b2World* pWorld)
{
	// 'layer' is an autorelease object
	Arrow *pRet = new Arrow();
	if (pRet && pRet->init(pWorld))
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

	// return the scene
	return pRet;
}

// on "init" you need to initialize your instance
bool Arrow::init(b2World* pWorld)
{
	//////////////////////////////
	// 1. super init first
	if (!Sprite::init())
	{
		return false;
	}
	m_pWorld = pWorld;
	
	auto sprite=Sprite::create("Arrow.png");
	//this->addChild(sprite);

	//ボディの追加
	b2PolygonShape dynamicBox;

	dynamicBox.SetAsBox(((sprite->getContentSize().width / 2)) / PTM_RATIO,
		((sprite->getContentSize().height / 2)) / PTM_RATIO);

	b2BodyDef Turnbody;

	Turnbody.type = b2_dynamicBody;

	b2BodyDef bodyB;

	bodyB.type = b2_dynamicBody;

	Turnbody.position.Set(100 / PTM_RATIO, 540 / PTM_RATIO);
	Turnbody.userData = nullptr;

	bodyB.position.Set(100 / PTM_RATIO, 540 / PTM_RATIO);

	auto b_bodyA = m_pWorld->CreateBody(&Turnbody);

	auto b_bodyB = m_pWorld->CreateBody(&bodyB);

	b2RevoluteJointDef jd;

	b2Vec2 jointpos = b_bodyA->GetWorldCenter();

	jd.Initialize(b_bodyA, b_bodyB, jointpos);

	jd.enableMotor = true;//モーターを使うかどうか

	m_pWorld->CreateJoint(&jd);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;

	fixtureDef.restitution = 1.0f;
	fixtureDef.density = 1;

	b_bodyA->CreateFixture(&fixtureDef);

}