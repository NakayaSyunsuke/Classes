#include "PlayScene.h"

USING_NS_CC;

// ���s�N�Z����1���[�g����
#define PTM_RATIO 32

#define BIT_SQUARE		0x0002
#define BIT_TRIANGLE	0x0004
#define BIT_BALL		0x0008
#define BIT_HOLE		0x0016
#define BIT_GROUND		0x0032
#define BIT_GIMMICK		0x0064
#define BIT_SENSOR		0x0128

using namespace experimental;
using namespace cocos2d::ui;
using namespace std;

USING_NS_CC;

PlayScene::PlayScene()
:m_Object_Remaining(0)
, m_HoleType(BIT_BALL)
, m_hitFlag(false)
, m_sensorFlag(false)
, m_pauseFlag(0)
,m_timeCount(0)
,m_labelTime(80)
, m_Gyro_Sensor_Flag(true)
{

}

//--------------------------------------------
//	�@�\	:	�V�[���쐬
//
//	����	:	�X�e�[�W��
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
Scene* PlayScene::createScene(int stage)
{
	// ������Ԃ��t�����V�[�����쐬����B
	auto scene = Scene::create();

	// 'layer' �� autorelease �I�u�W�F�N�g
	auto layer = PlayScene::create(stage);

	// ���C���[���V�[���̎q�Ƃ��Ēǉ�����B
	scene->addChild(layer);

	// �V�[����Ԃ��B
	return scene;
}

PlayScene* PlayScene::create(int stage)
{
	PlayScene *pRet = new(std::nothrow) PlayScene();
	if (pRet && pRet->init(stage))
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


//--------------------------------------------
//	�@�\	:	�����V�X�e���̏�����
//
//	����	:	�X�e�[�W��
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::initPhysics(int stage)
{
	// ���d�͐ݒ�
	m_Gravity.Set(0.0f, -1.0f);
	// ���[���h�𐶐�
	m_pWorld = new b2World(m_Gravity);

	// �\���p�̃C���X�^���X�𐶐�
	m_pDraw = new GLESDebugDraw(PTM_RATIO);
	// �S��ޕ\���i�Sbit�}�X�N�j
	uint32 flags = 0xffffffff;
	m_pDraw->SetFlags(flags);
	// �\���p�C���X�^���X�����[���h�ɃZ�b�g
	m_pWorld->SetDebugDraw(m_pDraw);

	/*   /// �ǂ̍쐬 ///  */

	//�ǑS�̂�1�̃{�f�B�ɂ���B
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);//��ʍ������Z���^�[��

	m_pGroundBody = m_pWorld->CreateBody(&groundBodyDef);

	//��ʍ���(�s�N�Z��)
	b2Vec2 leftbottom(0, 0);

	b2Vec2 rightbottom(m_visibleSize.width+m_origin.x, 0);
	// ��ʍ���i�s�N�Z���j
	b2Vec2 lefttop(0, m_visibleSize.width/1.5 + m_origin.y);
	// ��ʉE��i�s�N�Z���j
	b2Vec2 righttop(m_visibleSize.width  + m_origin.x, m_visibleSize.width / 1.5+ m_origin.y);
	// �s�N�Z�������[�g���ɕϊ�
	leftbottom.x /= PTM_RATIO;
	leftbottom.y /= PTM_RATIO;
	rightbottom.x /= PTM_RATIO;
	rightbottom.y /= PTM_RATIO;
	lefttop.x /= PTM_RATIO;
	lefttop.y /= PTM_RATIO;
	righttop.x /= PTM_RATIO;
	righttop.y /= PTM_RATIO;

	// �V�F�C�v�ݒ�p�̃C���X�^���X�i�g���܂킷�j
	b2EdgeShape groundBox;

	b2FixtureDef pGroundFixtureDef;
	
	pGroundFixtureDef.shape = &groundBox;

	pGroundFixtureDef.filter.categoryBits = BIT_GROUND;
	pGroundFixtureDef.filter.maskBits = BIT_BALL | BIT_SQUARE | BIT_TRIANGLE;
	//pGroundFixtureDef.filter.groupIndex = 0;

	// �n��
	groundBox.Set(leftbottom, rightbottom);
	//m_pGroundFixture = m_pGroundBody->CreateFixture(&groundBox, 0);
	m_pGroundBody->CreateFixture(&pGroundFixtureDef);

	// �V��
	groundBox.Set(lefttop, righttop);
	m_pGroundBody->CreateFixture(&pGroundFixtureDef);
	
	// ���[
	groundBox.Set(lefttop, leftbottom);
	m_pGroundBody->CreateFixture(&pGroundFixtureDef);

	// �E�[
	groundBox.Set(rightbottom, righttop);
	m_pGroundBody->CreateFixture(&pGroundFixtureDef);
	
	//�I�u�W�F�N�g�̐����̍ċA�Ăяo��
	Object_Factory(0, stage);

}

//--------------------------------------------
//	�@�\	:	����������
//
//	����	�F	�X�e�[�W��
//
//	�߂�l	�F	bool
//--------------------------------------------
// on "init" you need to initialize your instance
bool PlayScene::init(int stage)
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	m_stage = stage;
	m_visibleSize = Director::getInstance()->getVisibleSize();
	m_origin = Director::getInstance()->getVisibleOrigin();

	//���������[���h�̍쐬
	initPhysics(stage);
	m_Color_Array.push_back(4);

	//�M�~�b�N�̍쐬
	if (stage>1)	//�X�e�[�W���P�ȊO�Ȃ�M�~�b�N�����B
	{
		m_gimmick = Gimmick::create(m_pWorld, stage);
		this->addChild(m_gimmick,1);
	}

	//���t���[���̍쐬
	scheduleUpdate();

	//�ŏ��̌��̍쐬
	//�~�̔��a���w��
	b2CircleShape dynamicCircle;
	dynamicCircle.m_radius = 1.0f;
	m_objectColor = m_Color_Array.front();
	b2BodyDef bodyHole;
	bodyHole.type = b2_staticBody;								//����staticBody
	b2Vec2 pos(250 / PTM_RATIO, 450 / PTM_RATIO);
	bodyHole.position.Set(250 / PTM_RATIO, 450 / PTM_RATIO);

	auto goolParticle = ParticleSystemQuad::create("Particle/particle_Hole.plist");
	this->addChild(goolParticle, 1);
	goolParticle->setPosition(225, 450);

	Color_Textur_Create* object_Sprite = 
		Color_Textur_Create::create(m_objectColor, BIT_BALL, GOOL);
	this->addChild(object_Sprite,1);
	bodyHole.userData = object_Sprite;							//userData�ɃX�v���C�g��o�^

	body = m_pWorld->CreateBody(&bodyHole);

	// �{�f�B�ɓʖʑ̂̃t�B�N�X�`����ǉ�
	b2FixtureDef fixtureHole;
	fixtureHole.shape = &dynamicCircle;

	//�t�B���^�[�̐ݒ�
	fixtureHole.filter.categoryBits = BIT_HOLE;
	fixtureHole.filter.maskBits = BIT_BALL;
	
	//���C
	fixtureHole.restitution = 1.0f;
	fixtureHole.density = 1;
	body->CreateFixture(&fixtureHole);

	//�L�[������g�p����B
	this->setKeyboardEnabled(true);

	//�R���^�N�g���X�i�[���g�p����B
	m_pWorld->SetContactListener(this);

	//�W���C���Z���T�[���g�p����B
	setAccelerometerEnabled(true);
	//0.5�b�����ɃW���C���Z���T�[�������B
	setAccelerometerInterval(0.5f);

	//�w�i�̒ǉ�
	auto BackGround = Sprite::create("Textuer/Backgraund.png");
	BackGround->setAnchorPoint(Vec2(0.0f, 0.0f));
	this->addChild(BackGround, 0);


	//�w�i�̃p�[�e�B�N���̒ǉ�
	auto BackGround_particle = ParticleSystemQuad::create("Particle/particle_Backgraund.plist");
	BackGround_particle->setScale(2.0f);
	BackGround_particle->setAnchorPoint(Vec2(0.0f, 0.0f));
	this->addChild(BackGround_particle, 0);

	//�|�[�Y�{�^���̒ǉ�
	auto pouseButton = Button::create("Textuer/pouse.png");
	pouseButton->setScale(1.5f);
	pouseButton->setPosition(Vec2(m_origin.x + m_visibleSize.width - pouseButton->getContentSize().width / 1.6,
		m_origin.y + pouseButton->getContentSize().height / 2 ));
	this->addChild(pouseButton, 3);
	pouseButton->addTouchEventListener(CC_CALLBACK_2(PlayScene::touchEvent, this, PouseButton));

	//BGM�̒ǉ�
	//m_BGM = AudioEngine::play2d("Sound/game_maoudamashii_5_village07.ogg");
	m_BGM = AudioEngine::play2d("Sound/geme_bgm.ogg");
	AudioEngine::setVolume(m_BGM, 3.0f);
	AudioEngine::setLoop(m_BGM, true);

	//���x���̍쐬
	m_label = LabelTTF::create("80", "fonts/CODE2000.ttf", 20);

	//�@���x���̈ʒu�ݒ�
	m_label->setPosition(Vec2(480, 600));
	m_label->setScale(2.5f);

	this->addChild(m_label, 3);

	return true;
}

//--------------------------------------------
//	�@�\	:	�\���֐����I�[�o�[���C�h
//
//	����	:	Renderer *renderer, const Mat4& transform, uint32_t flags
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	// �������[���h���f�o�b�O�\��
	m_pWorld->DrawDebugData();
	//m_pDraw->DrawCircle(dynamicCircle.m_p, dynamicCircle.m_radius, b2Color(1.0f, 1.0f, 0.0f));
	//m_pDraw->DrawAABB(&aabb, b2Color(1.0f, 1.0f, 0.0f));
}

//--------------------------------------------
//�@�\	:�A�b�v�f�[�g
//--------------------------------------------
void PlayScene::update(float dt)
{
	// �������[���h�̍X�V�i���Ԃ�i�߂�j
	m_pWorld->Step(1.0f / 60.0f, 8, 3);

	for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
	{
		b->SetAwake(true);
	}

	//���̂ɉ摜�����鏈��
	for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL) {
			Sprite* myActor = (Sprite*)b->GetUserData();
			myActor->setPosition(CCPointMake(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
			myActor->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
		}
	}
	//�Z���T�[�̏���
	if (m_sensorFlag == true)
	{
		if (m_stage >= 6)
		{
			//SE�̍Đ�
			int id = AudioEngine::play2d("Sound/se_maoudamashii_element_fire08.ogg");
			AudioEngine::setVolume(id, 3.0f);

			//�I�u�W�F�̓����蔻��𖳂���
			//m_enemy->SetActive(false);

			Sensor_TransForm();

			m_sensorFlag = false;
		}
	}

	if (m_hitFlag == true)
	{
		//SE�̍Đ�
		int id = AudioEngine::play2d("Sound/se_maoudamashii_onepoint28.ogg");
		AudioEngine::setVolume(id, 3.0f);

		//����0�ȏ�Ȃ��
		if (m_Object_Remaining != 0)
		{
			//���ɗ����鉉�o
			drop_object(m_Hole_UserDate->getPosition(), m_Object_bodyUserDate->getPosition());
		}

		//���ɓ��������I�u�W�F�N�g�ƌ����g�̍폜����
		std::vector<b2Body*>::iterator it;
		for (it = m_Array_Destroy.begin(); it != m_Array_Destroy.end();)
		{
			m_pWorld->DestroyBody(*it);
			m_Array_Destroy.erase(it);
			it = m_Array_Destroy.begin();
		}

		//�F�̏��̍폜
		vector <int>::iterator ite;
		ite = m_Color_Array.begin();
		m_Color_Array.erase(ite);
		m_objectColor = m_Color_Array.front();

		//�{�[���̌��̍쐬
		b2CircleShape dynamicCircle;
		//�~�̔��a���w��
		dynamicCircle.m_radius = 1.0f;
		//Body�̍쐬
		b2BodyDef bodyHole;
		bodyHole.type = b2_staticBody;
		b2Vec2 pos(250 / PTM_RATIO, 450 / PTM_RATIO);

		b2FixtureDef fixtureHole;



		//�����폜����̂ŐV���������쐬����
		//�{�[�������錊�̍쐬
		if (m_HoleType == BIT_BALL)
		{
			Color_Textur_Create* objectA = Color_Textur_Create::create(m_objectColor, BIT_BALL, GOOL);
			
			objectA->setPosition(250 / PTM_RATIO, 450 / PTM_RATIO);
			this->addChild(objectA, 1);
			bodyHole.userData = objectA;					//userData�ɃX�v���C�g�ƐF�̏���o�^

			fixtureHole.shape = &dynamicCircle;

			//�t�B���^�[�f�[�^�̐ݒ�
			fixtureHole.filter.categoryBits = BIT_HOLE;		//�J�e�S���[�r�b�g	:	��
			fixtureHole.filter.maskBits = BIT_BALL;		//�}�X�N�r�b�g		:	�{�[��

		}

		//�l�p�`�����錊�̍쐬
		if (m_HoleType == BIT_SQUARE)
		{
			Color_Textur_Create* objectB = Color_Textur_Create::create(m_objectColor, BIT_SQUARE, GOOL);
			
			objectB->setPosition(250 / PTM_RATIO, 450 / PTM_RATIO);
			this->addChild(objectB, 1);
			bodyHole.userData = objectB;					//userData�ɃX�v���C�g�ƐF�̏���o�^

			fixtureHole.shape = &dynamicCircle;

			//�t�B���^�[�f�[�^�̐ݒ�
			fixtureHole.filter.categoryBits = BIT_HOLE;		//�J�e�S���[�r�b�g	:	��
			fixtureHole.filter.maskBits = BIT_SQUARE;		//�}�X�N�r�b�g		:	�l�p�`

		}

		//�O�p�`�����錊�̍쐬
		if (m_HoleType == BIT_TRIANGLE)
		{
			Color_Textur_Create* objectC = Color_Textur_Create::create(m_objectColor, BIT_TRIANGLE, GOOL);
			objectC->setPosition(250 / PTM_RATIO, 450 / PTM_RATIO);
			this->addChild(objectC, 1);
			bodyHole.userData = objectC;					//userData�ɃX�v���C�g�ƐF�̏���o�^

			fixtureHole.shape = &dynamicCircle;

			//�t�B���^�[�f�[�^�̐ݒ�
			fixtureHole.filter.categoryBits = BIT_HOLE;		//�J�e�S���[�r�b�g	:	��
			fixtureHole.filter.maskBits = BIT_TRIANGLE;		//�}�X�N�r�b�g		:	�O�p�`
		}

		if (m_objectColor==4)
		{
			auto Gool = Sprite::create("Textuer/Gool.png");
			this->addChild(Gool, 1);
			Gool->setScale(0.5f);
			
			Gool->setPosition(225, 450);

		}

		bodyHole.position.Set(250 / PTM_RATIO, 450 / PTM_RATIO);	//�|�W�V�����̐ݒ�
		body = m_pWorld->CreateBody(&bodyHole);
		body->CreateFixture(&fixtureHole);							// �{�f�B�ɓʖʑ̂̃t�B�N�X�`����ǉ�

		m_hitFlag = false;

		//�I�u�W�F�N�g�������Ȃ�ƃN���A����
		if (m_Object_Remaining == 0)
		{
			drop_object(m_Hole_UserDate->getPosition(), m_Object_bodyUserDate->getPosition());
			this->scheduleOnce(schedule_selector(PlayScene::Crear), 2.0f);

			//m_Object_bodyUserDate->removeFromParent();
		}

	}
	m_timeCount++;

	if (m_timeCount >= 60)
	{

		m_timeCount = 0;

		if (m_Object_Remaining!=0)
		{
			m_labelTime--;
		}
		//���Ԃ̍X�V
		std::stringstream ss;
		ss << m_labelTime;

		//�c�莞�Ԃ̕\��
		m_label->setString(ss.str());

		//�c�莞�Ԃ�15�b�ɂȂ�Ɖ�ʂ��Ԃ��_�ł��鏈��
		
		if (m_labelTime == 15)
		{

			AudioEngine::stop(m_BGM);
			//�Q�[���I�[�o�[�̃I�[�f�B�I�𗬂�
			PinchId = AudioEngine::play2d("Sound/se_maoudamashii_retro29.ogg");
			//AudioEngine::setVolume(PinchId, 5.0f);
			AudioEngine::setLoop(PinchId,true);

			m_AlarmSprite = Sprite::create("Textuer/Over_Pinch.png");

			m_AlarmSprite->setAnchorPoint(Vec2(0.0f, 0.0f));
			this->addChild(m_AlarmSprite, 3);

			FadeIn* fadeIn = FadeIn::create(0.5f);
			FadeOut* fadeOut = FadeOut::create(0.5f);
			Sequence*sequence = Sequence::create(fadeOut, fadeIn, nullptr);
			m_repeat = RepeatForever::create(sequence);

			m_AlarmSprite->runAction(m_repeat);

		}

		if (m_labelTime == 0)
		{
			AudioEngine::stop(PinchId);

			//���ׂĂ̏����̈ꎞ��~����
			this->pauseSchedulerAndActions();

			m_AlarmSprite->removeFromParent();


			//�Q�[���I�[�o�[�̉摜��\��
			auto m_CrearSprite = Sprite::create("Textuer/GameOver.png");
			m_CrearSprite->setPosition(Vec2(m_visibleSize.width / 2 + m_origin.x, m_visibleSize.height / 2 + m_origin.y));
			this->addChild(m_CrearSprite, 2);

			//���̃X�e�[�W�֐i�ރ{�^���̍쐬
			auto ReStart = Button::create("Textuer/ReStage.png");

			ReStart->setScale(1.5f);
			ReStart->setPosition(Vec2((m_origin.x + m_visibleSize.width) / 2, (m_origin.y + m_visibleSize.height) / 4.5));

			this->addChild(ReStart, 2);
			ReStart->addTouchEventListener(CC_CALLBACK_2(PlayScene::touchEvent, this, ReStartButton));

			//BGM���~�߂�
			AudioEngine::stop(m_BGM);

			//�Q�[���I�[�o�[�̃I�[�f�B�I�𗬂�
			int gemeOverId = AudioEngine::play2d("Sound/game_maoudamashii_9_jingle08.ogg");
			AudioEngine::setVolume(gemeOverId, 10.0f);

		}
	}
}

//--------------------------------------------
// �@�\	:�f�X�g���N�^
//--------------------------------------------
PlayScene::~PlayScene()
{
	// ���
	delete m_pDraw;
	delete m_pWorld;

}

//--------------------------------------------
//	�@�\	:	�ċA�Ăяo���̊֐�
//	
//	����	:	����ċA���邩�@�Ɓ@�X�e�[�W�̏��
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::Object_Factory(int level,int stage)
{

	if (stage>2)
	{
		stage = 2;
	}

	//�K�w�����E�ɂȂ�����I��
	if (level >= stage)
	{
		return;
	}

	//�~�̐F�������_���Ɍ��߂�
	int c_Circle = rand() % 3 ;
	//�F��ۑ�����
	m_Color_Array.push_back(c_Circle);

	b2CircleShape dynamicCircleA;
	//�~�̔��a���w��
	dynamicCircleA.m_radius = 1.0f;

	// �~�̌`��f�[�^�����
	b2BodyDef bodyBall;
	bodyBall.type = b2_dynamicBody;
	b2Vec2 posA((rand() % 500 + 100) / PTM_RATIO, (rand() % 200 + 100) / PTM_RATIO);
	Color_Textur_Create* object_SpriteA = 
		Color_Textur_Create::create(c_Circle, BIT_BALL,OBJECT);	//�X�v���C�g�ƃ����_���Ō��肵���F��userData�ɓo�^
	this->addChild(object_SpriteA, 3);		
	bodyBall.userData = object_SpriteA; 			//userData�ɃX�v���C�g��o�^
	bodyBall.position.Set(posA.x, posA.y);			//�|�W�V�����̌���

	m_ObjectBody = m_pWorld->CreateBody(&bodyBall);			// �{�f�B�ɓʖʑ̂̃t�B�N�X�`����ǉ�
	b2FixtureDef fixtureDefA;
	fixtureDefA.shape = &dynamicCircleA;

	//�t�B���^�[�f�[�^�̐ݒ�
	fixtureDefA.filter.categoryBits = BIT_BALL;		//�J�e�S���[�r�b�g	:	��	
			//�}�X�N�r�b�g		:	���Ɓ@		�ǂ�      �{�[����	�@�l�p��		�O�p��       �M�~�b�N�Ɓ@ �Z���T�[	�ɓ�����
	fixtureDefA.filter.maskBits = BIT_HOLE | BIT_GROUND | BIT_BALL | BIT_SQUARE | BIT_TRIANGLE | BIT_GIMMICK | BIT_SENSOR;

	//���C
	fixtureDefA.restitution = 0.3f;
	fixtureDefA.density = 1;

	m_ObjectBody->CreateFixture(&fixtureDefA);

	//�{�f�B�̏���z��ɓ����
	m_Array_Body.push_back(m_ObjectBody);

	m_Object_Remaining++;

	//�O�p�`�̔��a���w��
	int c_Convex = rand() % 3;			//�O�p�`�̐F�������_���Ɍ��߂�
	m_Color_Array.push_back(c_Convex);	//�F��ۑ�����

	b2PolygonShape dynamicConvex;
	
	// ���_�̃��X�g
	b2Vec2 points[] =
	{//       x  y
		{ -1.0, -1.0f }, // 1�_��
		{ 1.0f, -1.0f }, // 2�_��
		{ 0.0f, 1.0f }, // 3�_��

	};
	// ���_��
	int npoints = sizeof(points) / sizeof(b2Vec2);
	// ���_�f�[�^���Z�b�g
	dynamicConvex.Set(points, npoints);

	// �O�p�`�̌`��f�[�^�����
	b2BodyDef bodyConvex;
	bodyConvex.type = b2_dynamicBody;

	//userData�̐ݒ�
	Color_Textur_Create* object_SpriteC = 
		Color_Textur_Create::create(c_Convex, BIT_TRIANGLE, OBJECT);	//�X�v���C�g�ƃ����_���Ō��肵���F��userData�ɓo�^
	this->addChild(object_SpriteC,3);
	bodyConvex.userData = object_SpriteC;				//userData�ɃX�v���C�g�ƐF�̏���o�^

	b2Vec2 posC((rand() % 500 + 100) / PTM_RATIO, (rand() % 200 + 100) / PTM_RATIO);
	bodyConvex.position.Set(posC.x, posC.y);			//�|�W�V�����̌���
	m_ObjectBody = m_pWorld->CreateBody(&bodyConvex);			// �{�f�B�ɓʖʑ̂̃t�B�N�X�`����ǉ�
	b2FixtureDef fixtureDefC;
	fixtureDefC.shape = &dynamicConvex;

	//�t�B���^�[�f�[�^�̐ݒ�
	fixtureDefC.filter.categoryBits = BIT_TRIANGLE;		//�J�e�S���[�r�b�g	:	��
			//�}�X�N�r�b�g		:	���Ɓ@		�ǂ�      �{�[����	�@�l�p��		�O�p��       �M�~�b�N�Ɓ@ �@�Z���T�[	�ɓ�����
	fixtureDefC.filter.maskBits = BIT_HOLE | BIT_GROUND | BIT_BALL | BIT_SQUARE | BIT_TRIANGLE | BIT_GIMMICK | BIT_SENSOR;

	//���C
	fixtureDefC.restitution = 0.3f;
	fixtureDefC.density = 1;
	m_ObjectBody->CreateFixture(&fixtureDefC);
	//�{�f�B�̏���z��ɓ����
	m_Array_Body.push_back(m_ObjectBody);
	m_Object_Remaining++;

	// �l�p�̌`��f�[�^�����
	int c_Box = rand() % 3;
	m_Color_Array.push_back(c_Box);

	b2PolygonShape dynamicBox;

	dynamicBox.SetAsBox(1.0, 1.0f);

	b2BodyDef bodyBox;
	bodyBox.type = b2_dynamicBody;

	b2Vec2 posB((rand() % 500 + 100) / PTM_RATIO, (rand() % 200 + 100) / PTM_RATIO);

	Color_Textur_Create* object_SpriteB = Color_Textur_Create::create(c_Box, BIT_SQUARE, OBJECT);
	this->addChild(object_SpriteB,3);

	bodyBox.userData = object_SpriteB;					 //userData�ɃX�v���C�g�ƐF�̏���o�^
	bodyBox.position.Set(posB.x, posB.y);				//�|�W�V�����̌���
	m_ObjectBody = m_pWorld->CreateBody(&bodyBox);

	b2FixtureDef fixtureDefB;							// �{�f�B�ɓʖʑ̂̃t�B�N�X�`����ǉ�
	fixtureDefB.shape = &dynamicBox;					// �{�f�B�ɓʖʑ̂̃t�B�N�X�`����ǉ�
	
	//�t�B���^�[�f�[�^�̐ݒ�
	fixtureDefB.filter.categoryBits = BIT_SQUARE;		//�J�e�S���[�r�b�g	:	��
			//�}�X�N�r�b�g		:	���Ɓ@		�ǂ�      �{�[����	�@�l�p��		�O�p��       �M�~�b�N�Ɓ@ �@�Z���T�[	�ɓ�����
	fixtureDefB.filter.maskBits = BIT_HOLE | BIT_GROUND | BIT_BALL | BIT_SQUARE | BIT_TRIANGLE | BIT_GIMMICK | BIT_SENSOR;

	//���C
	fixtureDefB.restitution = 0.3f;
	fixtureDefB.density = 1;
	m_ObjectBody->CreateFixture(&fixtureDefB);
	//�{�f�B�̏���z��ɓ����
	m_Array_Body.push_back(m_ObjectBody);

	m_Object_Remaining++;


	Object_Factory(level + 1,stage);
}

//--------------------------------------------
//	�@�\	:	�Փ˔���
//	
//	����	:	�R���^�N�g�̏��
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::BeginContact(b2Contact* contact)
{
	//��̃I�u�W�F�N�g�̃t�B�N�X�`���f�[�^�𓾂�
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	Color_Textur_Create* bodyUserDateA = (Color_Textur_Create*)fixtureA->GetBody()->GetUserData();
	Color_Textur_Create* bodyUserDateB = (Color_Textur_Create*)fixtureB->GetBody()->GetUserData();

	//��̃I�u�W�F�N�g�̃t�B���^�[�̃f�[�^�𓾂�
	b2Filter filterA = fixtureA->GetFilterData();
	b2Filter filterB = fixtureB->GetFilterData();

	//	�I�u�W�F�N�gA�Ɍ�������������	 ||  �I�u�W�F�N�gB�Ɍ�������������
	if (filterA.categoryBits == BIT_HOLE || filterB.categoryBits == BIT_HOLE)
	{
		if (bodyUserDateA->GetColor() == bodyUserDateB->GetColor())
		{
			//���ƃI�u�W�F�N�g�����̃{�f�B��m_Array_Destroy�ɓ����
			m_Array_Destroy.push_back(fixtureA->GetBody());
			m_Array_Destroy.push_back(fixtureB->GetBody());

			//UserDate�̒��ɂ���X�v���C�g�ƐF�̏����폜����B

			if (fixtureA->GetFilterData().categoryBits == BIT_HOLE)
			{
				m_Hole_UserDate = bodyUserDateA;
				m_Object_bodyUserDate = bodyUserDateB;
			}

			if (fixtureB->GetFilterData().categoryBits == BIT_HOLE)
			{
				m_Hole_UserDate = bodyUserDateB;
				m_Object_bodyUserDate = bodyUserDateA;
			}

			
			//�폜����UserDate��nullptr������
			contact->GetFixtureA()->GetBody()->SetUserData(nullptr);
			contact->GetFixtureB()->GetBody()->SetUserData(nullptr);

			m_hitFlag = true;

			bool hit = true;

			if (m_HoleType == BIT_BALL)
			{
				//�O�p�`�ɂ���
				if (hit)
				{
					m_HoleType = BIT_TRIANGLE;
					hit = false;
				}
			}

			if (m_HoleType == BIT_TRIANGLE)
			{
				//�l�p�`�ɂ���
				if (hit)
				{
					m_HoleType = BIT_SQUARE;
					hit = false;
				}
			}

			if (m_HoleType == BIT_SQUARE)
			{
				//�{�[���ɂ���
				if (hit)
				{
					m_HoleType = BIT_BALL;
					hit = false;
				}
			}

			//�I�u�W�F�N�g�̌������炷�B
			m_Object_Remaining--;
		}
	}

	
	//�Z���T�[�̏Փ˔���̌Ăяo��
	if (m_stage >= 6)
	{
		sensorContact(fixtureA, fixtureB);
	}
	
}

//--------------------------------------------
//	�@�\	:	�W���C���Z���T�[
//	
//	����	:	Acceleration* acc, Event* unused_event
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::onAcceleration(Acceleration* acc, Event* unused_event)
{

	float j_x = (float)acc->x;
	float j_y = (float)acc->y;

	b2Vec2 gravity(0, 0);

	//�ŏ��̌X�����L�^���Ă���
	if (m_Gyro_Sensor_Flag == true)
	{
		m_First_gravity.Set(j_x, j_y);
		m_Gyro_Sensor_Flag = false;
	}

	//���d�͐ݒ�
	if (j_x == 0 || j_y == 0)
	{
		gravity.Set(0.0f, 0.0f);
		m_pWorld->SetGravity(gravity);
		return;
	}


	if (j_y <= -0.7f)
	{
		//���ւ̏����ݒ�
		gravity.Set(j_x * 10, j_y * 10);
		m_pWorld->SetGravity(gravity);

		return;
	}
	//�d�͐ݒ�
	//{(��������擾�����@x�@,�@y�@)-(�ŏ��̌X�� x , y )} * 20 = �X�����d��
	gravity.Set((j_x - m_First_gravity.x) * 20, (j_y - m_First_gravity.y) * 20);
	m_pWorld->SetGravity(gravity);


}

//--------------------------------------------
//	�@�\	:	�L�[���͂������̏���
//	
//	����	:	Acceleration* acc, Event* unused_event
//
//	�߂�l	:	EventKeyboard::KeyCode keyCode, Event* event
//--------------------------------------------
void PlayScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_W:

		this->schedule(schedule_selector(PlayScene::onKeyDownW));
		break;

	case EventKeyboard::KeyCode::KEY_A:

		this->schedule(schedule_selector(PlayScene::onKeyDownA));
		break;

	case EventKeyboard::KeyCode::KEY_S:

		this->schedule(schedule_selector(PlayScene::onKeyDownS));
		break;
	case EventKeyboard::KeyCode::KEY_D:

		this->schedule(schedule_selector(PlayScene::onKeyDownD));
		break;
	}
}

//--------------------------------------------
//	�@�\	:	�L�[�𗣂������̏���
//	
//	����	:	Acceleration* acc, Event* unused_event
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void  PlayScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_W:

		this->unschedule(schedule_selector(PlayScene::onKeyDownW));
		break;

	case EventKeyboard::KeyCode::KEY_A:

		this->unschedule(schedule_selector(PlayScene::onKeyDownA));
		break;

	case EventKeyboard::KeyCode::KEY_S:

		this->unschedule(schedule_selector(PlayScene::onKeyDownS));
		break;
	case EventKeyboard::KeyCode::KEY_D:

		this->unschedule(schedule_selector(PlayScene::onKeyDownD));
		break;
	}
}

//--------------------------------------------
//	�@�\	:	W�L�[�����������̏���
//	
//	����	:	float dt
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::onKeyDownW(float dt)
{
	// �d�͐ݒ�
	m_Gravity.Set(0.0f, 20.0f);
	m_pWorld->SetGravity(m_Gravity);
}

//--------------------------------------------
//	�@�\	:	A�L�[�����������̏���
//	
//	����	:	float dt
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::onKeyDownA(float dt)
{
	// �d�͐ݒ�
	m_Gravity.Set(-20.0f, 0.0f);
	m_pWorld->SetGravity(m_Gravity);
}

//--------------------------------------------
//	�@�\	:	S�L�[�����������̏���
//	
//	����	:	float dt
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::onKeyDownS(float dt)
{
	// �d�͐ݒ�
	m_Gravity.Set(0.0f, -20.0f);
	m_pWorld->SetGravity(m_Gravity);
}

//--------------------------------------------
//	�@�\	:	D�L�[�����������̏���
//	
//	����	:	float dt
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::onKeyDownD(float dt)
{
	// �d�͐ݒ�
	m_Gravity.Set(20.0f, 0.0f);
	m_pWorld->SetGravity(m_Gravity);
}

//--------------------------------------------
//	�@�\	:	�{�^�������������̏���
//	
//	����	:	Ref* pSender, Widget::TouchEventType type, int Buttan
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::touchEvent(Ref* pSender, Widget::TouchEventType type, int Buttan)
{
	switch (type)
	{
	case Widget::TouchEventType::ENDED:

		float Audio_time = AudioEngine::getCurrentTime(m_BGM);

		//�I�[�f�B�I�̎擾�ƍĐ�
		AudioEngine::stop(m_BGM);

		if (m_labelTime <= 15)
		{
			AudioEngine::stop(PinchId);
		}

		int id = AudioEngine::play2d("Sound/se_maoudamashii_onepoint26.ogg");
		AudioEngine::setVolume(id, 3.0f);

		//�l�N�X�g�X�e�[�W�{�^���������ꂽ�Ȃ��
		if (Buttan == NextStageButton)
		{
			//�X�e�[�W��8�Ȃ�΃^�C�g����
			if (m_stage == 8)
			{
				Scene* playScene = TitleScene::createScene();

				//�g�����f�B�V����
				TransitionFade* tscene = TransitionFade::create(1.0f, playScene);

				Director::getInstance()->replaceScene(tscene);
			}

			//����ȊO�Ȃ��
			else
			{
				//���̃X�e�[�W��
				m_stage++;

				Scene* playScene = PlayScene::createScene(m_stage);

				//�g�����f�B�V����
				TransitionFade* tscene = TransitionFade::create(1.0f, playScene);

				Director::getInstance()->replaceScene(tscene);
			}
			break;
		}


		//�{�^�����|�[�Y�{�^���Ȃ�΃|�[�Y����
		if (Buttan == PouseButton)
		{
			auto director = Director::getInstance();
			director->pause();
			auto modalLayer = ModalLayer::create(m_BGM, Audio_time);
			this->addChild(modalLayer,3);

		}
		if (Buttan == ReStartButton)
		{
			Scene* playScene = PlayScene::createScene(m_stage);

			//�g�����f�B�V����
			TransitionFade* tscene = TransitionFade::create(1.0f, playScene);

			Director::getInstance()->replaceScene(tscene);

		}
		break;
	}
}

//--------------------------------------------
//	�@�\	:	�Z���T�[����
//	
//	����	:	�Փ˂������̂̏��@A�@&�@B
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::sensorContact(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	//�Z���T�[�̓����蔻��

	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();

	Color_Textur_Create* bodyUserDateA = (Color_Textur_Create*)fixtureA->GetBody()->GetUserData();
	Color_Textur_Create* bodyUserDateB = (Color_Textur_Create*)fixtureB->GetBody()->GetUserData();

	//�Е��������Z���T�[�ł���P�[�X�ȊO�̎�	||
	// �t�B���^�[�f�[�^A,B�� ���̎�				||
	// �t�B���^�[�f�[�^A,B�� �ǂ̎�	�͏��O
	if (!(sensorA^sensorB) ||
		fixtureA->GetFilterData().categoryBits == BIT_HOLE || 
		fixtureB->GetFilterData().categoryBits == BIT_HOLE||
		fixtureA->GetFilterData().categoryBits == BIT_GROUND||
		fixtureB->GetFilterData().categoryBits == BIT_GROUND)
	{
		return;
	}

	//���[�U�[�f�[�^�����o���B
	b2Body* entityA = fixtureA->GetBody();
	b2Body* entityB = fixtureB->GetBody();
	
	//�Z���T�[��A�Ȃ��
	if (sensorA)
	{
		//�t�B�N�X�`��B���I�u�W�F�N�g�ł���
		m_rader = entityA;
		m_Array_Sensor_Object.push_back(entityB);
		
		m_Object_bodyUserDate = bodyUserDateB;

		m_sensorFlag = true;
	}

	//�Z���T�[��B�Ȃ��
	if (sensorB)
	{
		//�t�B�N�X�`��A���I�u�W�F�N�g�ł���
		m_rader = entityB;
		m_Array_Sensor_Object.push_back(entityA);

		m_Object_bodyUserDate = bodyUserDateA;

		m_sensorFlag = true;
	}

}

//--------------------------------------------
//	�@�\	:	�N���A�������̏���
//	
//	����	:	float dt
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::Crear(float dt)
{
	if (m_labelTime <= 15)
	{
		AudioEngine::stop(PinchId);


		m_AlarmSprite->removeFromParent();
	}
	//��ʂ̃|�[�Y����
	this->pauseSchedulerAndActions();
	//BGM��炷
	AudioEngine::stop(m_BGM);
	crearid = AudioEngine::play2d("Sound/se_maoudamashii_jingle06.ogg");
	AudioEngine::setVolume(crearid, 3.0f);

	//�p�[�e�B�N���̍쐬
	auto particle = ParticleSystemQuad::create("Particle/particle_hanabi.plist");
	// �p�[�e�B�N����\������ꏊ�̐ݒ�
	particle->setPosition(Vec2(480, 320));
	particle->setAutoRemoveOnFinish(true);
	// �p�[�e�B�N����z�u
	this->addChild(particle, 3);

	//�N���A�̉摜��\��
	auto m_CrearSprite = Sprite::create("Textuer/GameClear.png");
	m_CrearSprite->setPosition(Vec2(m_visibleSize.width / 2 + m_origin.x, m_visibleSize.height / 2 + m_origin.y));
	this->addChild(m_CrearSprite, 3);

	//���̃X�e�[�W�֐i�ރ{�^���̍쐬
	auto NextStage = Button::create("Textuer/Next_Stage.png");
	NextStage->setScale(1.5f);
	NextStage->setPosition(Vec2((m_origin.x + m_visibleSize.width) / 2, (m_origin.y + m_visibleSize.height) / 4.5));
	this->addChild(NextStage, 3);
	NextStage->addTouchEventListener(CC_CALLBACK_2(PlayScene::touchEvent, this, NextStageButton));
}

//--------------------------------------------
//	�@�\	:	�I�u�W�F�����ɗ��������ɂ͂����鏈��
//	
//	����	:	���̈ʒu�@,�@�I�u�W�F�̈ʒu
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::drop_object(Vec2 HollPosition, Vec2 ObjectPosition)
{

	//UserDate����F�̏��������o�ϐ��ɕۑ�����
	m_object_color = m_Object_bodyUserDate->GetColor();
	//UserDate����`�̏��������o�ϐ��ɕۑ�����
	m_object_Form = m_Object_bodyUserDate->GetForm();

	//---------------------------------------------------
	//���ɓ�����ƃI�u�W�F���͂�����p�[�e�B�N���̍쐬
	//---------------------------------------------------

	//�l�p�̃p�[�e�B�N�������
	ParticleSystemQuad* particle_Crash = ParticleSystemQuad::create("Particle/particle_Crash.plist");

	//usedata�@�����̂Ȃ狅�̂̃p�[�e�B�N�����쐬
	if (m_object_Form==Form_maru)
	{
		particle_Crash = ParticleSystemQuad::create("Particle/particle_Crash_maru.plist");

	}
	//usedata�@���O�p�`�Ȃ�O�p�`�̃p�[�e�B�N�����쐬
	else
	{
		if (m_object_Form == Form_sankaku)
		{
			particle_Crash = ParticleSystemQuad::create("Particle/particle_Crash_sankaku.plist");

		}
	}
	//�I�u�W�F��������ʒu�Ƀp�[�e�B�N���̃Z�b�g
	particle_Crash->setPosition(Vec2(ObjectPosition.x, ObjectPosition.y));
	//�p�[�e�B�N���̑傫��������������B
	particle_Crash->setScale(0.4f);

	//���[�U�[�f�[�^�̍폜
	m_Hole_UserDate->removeFromParent();
	m_Object_bodyUserDate->removeFromParent();

	//�F�̐ݒ�
	//usedata�@�̐F�����F�Ȃ�F�����F�ɂ���B
	if (m_object_color == Color_Yellow)
	{
		particle_Crash->setStartColor(Color4F::YELLOW);		//�p�[�e�B�N�����n�܂�F�����F�ɂ��� 
		particle_Crash->setEndColor(Color4F::YELLOW);		//�p�[�e�B�N�����I���F�����F�ɂ���
	}
	//usedata�@�̐F���ԐF�Ȃ�F��ԐF�ɂ���B
	else
	{
		if (m_object_color == Color_Red)
		{
			particle_Crash->setStartColor(Color4F::RED);	//�p�[�e�B�N�����n�܂�F��ԐF�ɂ��� 
			particle_Crash->setEndColor(Color4F::RED);		//�p�[�e�B�N�����I���F��ԐF�ɂ���
		}
	}
	this->addChild(particle_Crash, 3);

	//�͂�����p�[�e�B�N���̏I�����Ԃ𓾂�B
	float Crash_time = particle_Crash->getDuration();

	//�p�[�e�B�N�����I�����Ԃ�1.5�{�������Ԃ܂Ŏ��̃A�N�V�������~�߂Ă���
	auto delay = DelayTime::create(Crash_time*1.2);

	//particle_Convergence�֐����A�N�V�����ɒǉ�����B
	CallFuncN* callfunc = CallFuncN::create(this, callfuncN_selector(PlayScene::particle_Convergence));
	MoveTo* move = MoveTo::create(0.5f,HollPosition);

	//DelayTime�@�Ɓ@CallFuncN�̃A�N�V���������Ԃɗ����B
	Sequence*sequence = Sequence::create(delay, callfunc,nullptr);

	Spawn* spawn = Spawn::create(sequence,move,nullptr);

	particle_Crash->runAction(spawn);
}

//--------------------------------------------
//	�@�\	:	�͂������I�u�W�F�����ɏW�܂鏈��
//	
//	����	:	Node* sender
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::particle_Convergence(Node* sender)
{
	//--------------------------------------------------
	//�{�[�������ɋz�����܂��p�[�e�B�N���̍쐬
	//--------------------------------------------------

	//�l�p�`�̃p�[�e�B�N���̍쐬
	ParticleSystemQuad* particle_Convergence = ParticleSystemQuad::create("Particle/particle_Convergence.plist");
	
	//�����o�ϐ��ɕۑ����Ă���`�����̂Ȃ�΋��̂̃p�[�e�B�N�����쐬
	if (m_object_Form == Form_maru)
	{
		particle_Convergence = ParticleSystemQuad::create("Particle/particle_Convergence_maru.plist");

	}
	//�����o�ϐ��ɕۑ����Ă���`���O�p�`�Ȃ�ΎO�p�`�̃p�[�e�B�N�����쐬
	else
	{
		if (m_object_Form == Form_sankaku)
		{
			particle_Convergence = ParticleSystemQuad::create("Particle/particle_Convergence_sankaku.plist");

		}
	}
	
	//���̈ʒu�Ƀp�[�e�B�N�����Z�b�g����B
	particle_Convergence->setPosition(Vec2(225,450));
	//�p�[�e�B�N�����k������B
	particle_Convergence->setScale(0.1f);

	//�����o�ϐ��ɕۑ����Ă���F�̏�񂪉��F�Ȃ�΃p�[�e�B�N���̐F�����F�ɂ���B
	if (m_object_color == Color_Yellow)
	{
		particle_Convergence->setStartColor(Color4F::YELLOW);	//�p�[�e�B�N�����n�܂�F�����F�ɂ���
		particle_Convergence->setEndColor(Color4F::YELLOW);		//�p�[�e�B�N�����I���F�����F�ɂ���
	}

	//�����o�ϐ��ɕۑ����Ă���F�̏�񂪐ԐF�Ȃ�΃p�[�e�B�N���̐F��ԐF�ɂ���B
	else
	{
		if (m_object_color == Color_Red)
		{
			particle_Convergence->setStartColor(Color4F::RED);	//�p�[�e�B�N�����n�܂�F��ԐF�ɂ���
			particle_Convergence->setEndColor(Color4F::RED);	//�p�[�e�B�N�����I���F��ԐF�ɂ���
		}
	}
	this->addChild(particle_Convergence, 3);

}

void PlayScene::Sensor_TransForm()
{
	std::vector<b2Body*>::iterator it;
	std::vector<b2Body*> m_array_sensor_object;

	m_array_sensor_object = m_Array_Sensor_Object;

	for (it = m_array_sensor_object.begin(); it != m_array_sensor_object.end();)
	{
		auto particle = ParticleSystemQuad::create("Particle/particle_Sencer.plist");

		//UserDate����F�̏��������o�ϐ��ɕۑ�����
		m_object_color = m_Object_bodyUserDate->GetColor();

		if (m_Object_bodyUserDate->GetColor() == Color_Yellow)
		{
			particle->setStartColor(Color4F::YELLOW);	//�p�[�e�B�N�����n�܂�F�����F�ɂ���
			particle->setEndColor(Color4F::YELLOW);		//�p�[�e�B�N�����I���F�����F�ɂ���
		}

		else if (m_Object_bodyUserDate->GetColor() == Color_Blue)
		{
			particle->setStartColor(Color4F::BLUE);	//�p�[�e�B�N�����n�܂�F�����F�ɂ���
			particle->setEndColor(Color4F::BLUE);		//�p�[�e�B�N�����I���F�����F�ɂ���
		}

		b2Vec2 obje_position = m_array_sensor_object[0]->GetPosition();
		particle->setPosition(Vec2(obje_position.x * PTM_RATIO, obje_position.y * PTM_RATIO));
		
		float delay_time = particle->getDuration();
		auto delay = DelayTime::create(delay_time * 1.5);

		//Sensor_Second_ObjectTransform�֐���\�񂷂�B
		CallFunc* callfunc = CallFunc::create(std::bind(&PlayScene::Sensor_Second_ObjectTransform, this, m_array_sensor_object.front()));

		//delay��callfunc�̍���
		Sequence*sequence = Sequence::create(delay, callfunc, nullptr);
		this->runAction(sequence);
		particle->setScale(0.3f);
		addChild(particle, 3);

		//�o���̈ʒu��ۑ�����B
		Vec2 exitPosition = m_gimmick->GetexitSpritePosition();

		MoveTo* move = MoveTo::create(0.8f, exitPosition);
		particle->runAction(move);

		m_array_sensor_object.erase(it);
		it = m_array_sensor_object.begin();

	}

	//Sensor_First_ObjectTransform�֐����A�N�V�����ɒǉ�����B
	Sensor_First_ObjectTransform();

	//�I�u�W�F���o���Ɉړ�������B

}

//--------------------------------------------
//	�@�\	:	�Z���T�[�ɃI�u�W�F�������������̏���
//	
//	����	:	float dt
//
//	�߂�l	:	�Ȃ�
//--------------------------------------------
void PlayScene::Sensor_First_ObjectTransform()
{

	std::vector<b2Body*>::iterator it;

	//�Z���T�[�ɓ���������ʂ̏ꏊ�Ƀ��[�v����B
	for (it = m_Array_Sensor_Object.begin(); it != m_Array_Sensor_Object.end();)
	{
		m_Array_Sensor_Object[0]->SetTransform(b2Vec2(-100, -100), 0);
		m_Array_Sensor_Object[0]->SetActive(false);
		m_Array_Sensor_Object.erase(it);

		it = m_Array_Sensor_Object.begin();
	}
}


void  PlayScene::Sensor_Second_ObjectTransform(b2Body* pbody)
{
	////�Z���T�[�ɓ���������ʂ̏ꏊ�Ƀ��[�v����B
	Vec2 exitPosition = m_gimmick->GetexitSpritePosition();

	pbody->SetTransform(b2Vec2(exitPosition.x / PTM_RATIO, exitPosition.y / PTM_RATIO), 0);
	pbody->SetActive(true);
}