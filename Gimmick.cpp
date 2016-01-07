#include "Gimmick.h"

USING_NS_CC;

//------------�萔------------
// ���s�N�Z����1���[�g����
#define PTM_RATIO 32

//�I�u�W�F�̌`
#define BIT_SQUARE		0x0002
#define BIT_TRIANGLE	0x0004
#define BIT_BALL		0x0008

//�I�u�W�F�̌`
#define BIT_HOLE		0x0016
#define BIT_GROUND		0x0032
#define BIT_GIMMICK		0x0064
#define BIT_SENSOR		0x0128

//------------------------------------------------------------
//	�@�\	:	�쐬
//
//	����	:	���[���h�̏��	,	�I�����ꂽ�X�e�[�W
//
//	�߂�l	:	Gimmick
//------------------------------------------------------------
Gimmick* Gimmick::create(b2World* pWorld, int stage)
{
	// 'layer' is an autorelease object
	Gimmick *pRet = new Gimmick();
	if (pRet && pRet->init(pWorld,stage))
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

//------------------------------------------------------------
//	�@�\	:	����������
//
//	����	:	���[���h�̏��	,	�I�����ꂽ�X�e�[�W
//
//	�߂�l	:	�Ȃ�
//------------------------------------------------------------
bool Gimmick::init(b2World* pWorld, int stage)
{
	//////////////////////////////
	// 1. super init first
	if (!Sprite::init())
	{
		return false;
	}

	m_pWorld = pWorld;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	std::stringstream ss;
	ss << "Wall" << stage << ".tmx";
	std::string st = ss.str();			//stringstream�^��string�^�ɕϊ�
	m_tileMap = cocos2d::TMXTiledMap::create(st);

	createWall(stage);

	if (stage >= 4)
	{
		createRolling(stage);
	}

	if (stage >= 6)
	{
		createSensor(stage);
	}

	return true;
}


//------------------------------------------------------------
//	�@�\	:	�ǂ̍쐬
//
//	����	:	�I�����ꂽ�X�e�[�W
//
//	�߂�l	:	�Ȃ�
//------------------------------------------------------------
void Gimmick::createWall(int stage)
{
	//this->addChild(m_tileMap);
	// �ǂ̃I�u�W�F�N�g�O���[�v���擾����B
	auto wallGroup = m_tileMap->getObjectGroup("collision");

	// �ǃI�u�W�F�N�g�̔z����擾����B
	auto wallArray = wallGroup->getObjects();

	// �ǃI�u�W�F�N�g�����X�g�ɓo�^����B
	for (int i = 0; i < wallArray.size(); i++)
	{
		//�{�f�B�̒ǉ�
		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(0.3, 0.3f);
		b2BodyDef bodyBox;
		bodyBox.type = b2_staticBody;

		// �I�u�W�F�N�g���擾����B
		Value object = wallArray.at(i);

		// object�ɐݒ肳��Ă���v���p�e�B�Ȃǂ��擾
		ValueMap objectInfo = object.asValueMap();

		// �ǃI�u�W�F�N�g���쐬����B

		auto gimmick = Sprite::create("Textuer/float.png");

		gimmick->setPosition(Vec2(
			objectInfo.at("x").asFloat(),
			objectInfo.at("y").asFloat()));

		Vec2 p = gimmick->getPosition();
		bodyBox.position.Set(p.x / PTM_RATIO, p.y / PTM_RATIO);
		bodyBox.userData = gimmick;
		auto body = m_pWorld->CreateBody(&bodyBox);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;

		fixtureDef.restitution = 1.0f;
		fixtureDef.density = 1;

		fixtureDef.filter.categoryBits = BIT_GIMMICK;
		fixtureDef.filter.maskBits = BIT_BALL | BIT_SQUARE | BIT_TRIANGLE;

		body->CreateFixture(&fixtureDef);

		addChild(gimmick);
	}
}



//------------------------------------------------------------
//	�@�\	:	�v���y���̍쐬
//
//	����	:	�I�����ꂽ�X�e�[�W
//
//	�߂�l	:	�Ȃ�
//------------------------------------------------------------
void Gimmick::createRolling(int stage)
{
	m_tilName = m_tileMap->layerNamed("rolling");

	//this->addChild(m_tileMap);
	// �ǂ̃I�u�W�F�N�g�O���[�v���擾����B
	auto TurnGroup = m_tileMap->getObjectGroup("rolling");

	//�v���y���I�u�W�F�N�g�̔z����擾����B
	auto TurnArray = TurnGroup->getObjects();

	// �v���y���I�u�W�F�N�g�����X�g�ɓo�^����B
	for (int i = 0; i < TurnArray.size(); i++)
	{
		//�{�f�B�̒ǉ�
		b2PolygonShape dynamicBox;

		dynamicBox.SetAsBox(0.3, 1.0f);

		m_Turnbody.type = b2_dynamicBody;

		b2BodyDef bodyB;

		bodyB.type = b2_staticBody;

		// �I�u�W�F�N�g���擾����B
		Value object = TurnArray.at(i);

		// object�ɐݒ肳��Ă���v���p�e�B�Ȃǂ��擾
		ValueMap objectInfo = object.asValueMap();

		auto gimmick = Sprite::create("Textuer/Pulopera.png");
		gimmick->setPosition(Vec2(
			objectInfo.at("x").asFloat(),
			objectInfo.at("y").asFloat()));


		Vec2 p = gimmick->getPosition();
		m_Turnbody.position.Set(p.x / PTM_RATIO, p.y / PTM_RATIO);
		m_Turnbody.userData = gimmick;

		bodyB.position.Set(p.x / PTM_RATIO, p.y / PTM_RATIO);

		auto b_bodyA = m_pWorld->CreateBody(&m_Turnbody);

		auto b_bodyB = m_pWorld->CreateBody(&bodyB);

		b2RevoluteJointDef jd;

		b2Vec2 jointpos = b_bodyA->GetWorldCenter();

		jd.Initialize(b_bodyA, b_bodyB, jointpos);

		jd.maxMotorTorque = 10.0f;//��]��
		jd.motorSpeed = 15.0f;//�X�s�[�h
		jd.enableMotor = true;//���[�^�[���g�����ǂ���

		m_pWorld->CreateJoint(&jd);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;

		fixtureDef.restitution = 1.0f;
		fixtureDef.density = 1;

		fixtureDef.filter.categoryBits = BIT_GIMMICK;
		fixtureDef.filter.maskBits = BIT_BALL | BIT_SQUARE | BIT_TRIANGLE;

		b_bodyA->CreateFixture(&fixtureDef);

		addChild(gimmick);
	}


}


//------------------------------------------------------------
//	�@�\	:	�v���y���̍쐬
//
//	����	:	�I�����ꂽ�X�e�[�W
//
//	�߂�l	:	�Ȃ�
//------------------------------------------------------------
void Gimmick::createSensor(int stage)
{
	m_tilName = m_tileMap->layerNamed("sensor");

	//this->addChild(m_tileMap);
	// �ǂ̃I�u�W�F�N�g�O���[�v���擾����B
	auto TurnGroup = m_tileMap->getObjectGroup("sensor");

	//�v���y���I�u�W�F�N�g�̔z����擾����B
	auto TurnArray = TurnGroup->getObjects();

	// �v���y���I�u�W�F�N�g�����X�g�ɓo�^����B
	for (int i = 0; i < TurnArray.size(); i++)
	{
		//�{�f�B�̒ǉ�
		b2CircleShape dynamicCircle;
		dynamicCircle.m_radius = 0.3f;
		m_Turnbody.type = b2_dynamicBody;
		b2BodyDef bodyB;
		bodyB.type = b2_staticBody;

		// �I�u�W�F�N�g���擾����B
		Value object = TurnArray.at(i);
		// object�ɐݒ肳��Ă���v���p�e�B�Ȃǂ��擾
		ValueMap objectInfo = object.asValueMap();

		auto gimmick = Sprite::create("Textuer/sensor.png");
		gimmick->setPosition(Vec2(
			objectInfo.at("x").asFloat(),
			objectInfo.at("y").asFloat()));


		Vec2 p = gimmick->getPosition();
		m_Turnbody.position.Set(p.x / PTM_RATIO, p.y / PTM_RATIO);
		m_Turnbody.userData = gimmick;
		bodyB.position.Set(p.x / PTM_RATIO, p.y / PTM_RATIO);

		auto b_bodyA = m_pWorld->CreateBody(&m_Turnbody);
		auto b_bodyB = m_pWorld->CreateBody(&bodyB);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicCircle;
		fixtureDef.filter.categoryBits = BIT_SENSOR;
		fixtureDef.filter.maskBits = BIT_BALL | BIT_SQUARE | BIT_TRIANGLE;
		fixtureDef.isSensor = true;
		
		//�~�ʏ�̃Z���T�[�̒ǉ�
		float radius = 3.0;
		b2Vec2 vertices[8];

		vertices[0].Set(0, 0);

		for (int i = 0; i < 7; i++)
		{
			float angle = i / 6.0 * 60;
			angle = CC_DEGREES_TO_RADIANS(angle);
			vertices[i + 1].Set(radius*cosf(angle), radius*sinf(angle));

		}

		b2PolygonShape polygonShape;

		polygonShape.Set(vertices, 8);
		fixtureDef.shape = &polygonShape;
		b_bodyA->SetAngularVelocity(CC_DEGREES_TO_RADIANS(45));
		b_bodyA->CreateFixture(&fixtureDef);


		b2RevoluteJointDef jd;
		b2Vec2 jointpos = b_bodyA->GetWorldCenter();

		jd.Initialize(b_bodyA, b_bodyB, jointpos);
		jd.maxMotorTorque = 10.0f;		//��]��
		jd.motorSpeed = 1.0f;			//�X�s�[�h
		jd.enableMotor = true;			//���[�^�[���g�����ǂ���

		m_pWorld->CreateJoint(&jd);

		addChild(gimmick);
	}
	createExit(TurnGroup,TurnArray);
}

//------------------------------------------------------------
//	�@�\	:	�o���̍쐬
//
//	����	:	�I�����ꂽ�X�e�[�W
//
//	�߂�l	:	�Ȃ�
//------------------------------------------------------------
void  Gimmick::createExit(cocos2d::TMXObjectGroup* TurnGroup,
						  std::vector<cocos2d::Value, std::allocator<cocos2d::Value>> TurnArray)
{
	//�o���̍쐬
	m_tilName = m_tileMap->layerNamed("exit");

	// �o���O���[�v���擾����B
	TurnGroup = m_tileMap->getObjectGroup("exit");

	//�o���̔z����擾����B
	TurnArray = TurnGroup->getObjects();

	// �o�������X�g�ɓo�^����B
	for (int i = 0; i < TurnArray.size(); i++)
	{
		m_exitSprite = Sprite::create("Textuer/exit.png");

		this->addChild(m_exitSprite, -1);

		// �I�u�W�F�N�g���擾����B
		Value object = TurnArray.at(i);

		ValueMap objectInfo = object.asValueMap();

		m_exitSprite->setPosition(Vec2(
			objectInfo.at("x").asFloat(),
			objectInfo.at("y").asFloat()));
	}
}

//------------------------------------------------------------
//	�@�\	:	�o���̈ʒu��Get�֐�
//
//	����	:	�Ȃ�
//
//	�߂�l	:	�o���̈ʒu
//------------------------------------------------------------
Vec2 Gimmick::GetexitSpritePosition()
{
	return m_exitSprite->getPosition();
}