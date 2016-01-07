#include "PlayScene.h"

USING_NS_CC;

// 何ピクセルで1メートルか
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
//	機能	:	シーン作成
//
//	引数	:	ステージ数
//
//	戻り値	:	なし
//--------------------------------------------
Scene* PlayScene::createScene(int stage)
{
	// 物理空間が付いたシーンを作成する。
	auto scene = Scene::create();

	// 'layer' は autorelease オブジェクト
	auto layer = PlayScene::create(stage);

	// レイヤーをシーンの子として追加する。
	scene->addChild(layer);

	// シーンを返す。
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
//	機能	:	物理システムの初期化
//
//	引数	:	ステージ数
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::initPhysics(int stage)
{
	// 無重力設定
	m_Gravity.Set(0.0f, -1.0f);
	// ワールドを生成
	m_pWorld = new b2World(m_Gravity);

	// 表示用のインスタンスを生成
	m_pDraw = new GLESDebugDraw(PTM_RATIO);
	// 全種類表示（全bitマスク）
	uint32 flags = 0xffffffff;
	m_pDraw->SetFlags(flags);
	// 表示用インスタンスをワールドにセット
	m_pWorld->SetDebugDraw(m_pDraw);

	/*   /// 壁の作成 ///  */

	//壁全体で1つのボディにする。
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0, 0);//画面左下をセンターへ

	m_pGroundBody = m_pWorld->CreateBody(&groundBodyDef);

	//画面左下(ピクセル)
	b2Vec2 leftbottom(0, 0);

	b2Vec2 rightbottom(m_visibleSize.width+m_origin.x, 0);
	// 画面左上（ピクセル）
	b2Vec2 lefttop(0, m_visibleSize.width/1.5 + m_origin.y);
	// 画面右上（ピクセル）
	b2Vec2 righttop(m_visibleSize.width  + m_origin.x, m_visibleSize.width / 1.5+ m_origin.y);
	// ピクセル→メートルに変換
	leftbottom.x /= PTM_RATIO;
	leftbottom.y /= PTM_RATIO;
	rightbottom.x /= PTM_RATIO;
	rightbottom.y /= PTM_RATIO;
	lefttop.x /= PTM_RATIO;
	lefttop.y /= PTM_RATIO;
	righttop.x /= PTM_RATIO;
	righttop.y /= PTM_RATIO;

	// シェイプ設定用のインスタンス（使いまわす）
	b2EdgeShape groundBox;

	b2FixtureDef pGroundFixtureDef;
	
	pGroundFixtureDef.shape = &groundBox;

	pGroundFixtureDef.filter.categoryBits = BIT_GROUND;
	pGroundFixtureDef.filter.maskBits = BIT_BALL | BIT_SQUARE | BIT_TRIANGLE;
	//pGroundFixtureDef.filter.groupIndex = 0;

	// 地面
	groundBox.Set(leftbottom, rightbottom);
	//m_pGroundFixture = m_pGroundBody->CreateFixture(&groundBox, 0);
	m_pGroundBody->CreateFixture(&pGroundFixtureDef);

	// 天井
	groundBox.Set(lefttop, righttop);
	m_pGroundBody->CreateFixture(&pGroundFixtureDef);
	
	// 左端
	groundBox.Set(lefttop, leftbottom);
	m_pGroundBody->CreateFixture(&pGroundFixtureDef);

	// 右端
	groundBox.Set(rightbottom, righttop);
	m_pGroundBody->CreateFixture(&pGroundFixtureDef);
	
	//オブジェクトの生成の再帰呼び出し
	Object_Factory(0, stage);

}

//--------------------------------------------
//	機能	:	初期化処理
//
//	引数	：	ステージ数
//
//	戻り値	：	bool
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

	//物理をワールドの作成
	initPhysics(stage);
	m_Color_Array.push_back(4);

	//ギミックの作成
	if (stage>1)	//ステージが１以外ならギミックを作る。
	{
		m_gimmick = Gimmick::create(m_pWorld, stage);
		this->addChild(m_gimmick,1);
	}

	//毎フレームの作成
	scheduleUpdate();

	//最初の穴の作成
	//円の半径を指定
	b2CircleShape dynamicCircle;
	dynamicCircle.m_radius = 1.0f;
	m_objectColor = m_Color_Array.front();
	b2BodyDef bodyHole;
	bodyHole.type = b2_staticBody;								//穴はstaticBody
	b2Vec2 pos(250 / PTM_RATIO, 450 / PTM_RATIO);
	bodyHole.position.Set(250 / PTM_RATIO, 450 / PTM_RATIO);

	auto goolParticle = ParticleSystemQuad::create("Particle/particle_Hole.plist");
	this->addChild(goolParticle, 1);
	goolParticle->setPosition(225, 450);

	Color_Textur_Create* object_Sprite = 
		Color_Textur_Create::create(m_objectColor, BIT_BALL, GOOL);
	this->addChild(object_Sprite,1);
	bodyHole.userData = object_Sprite;							//userDataにスプライトを登録

	body = m_pWorld->CreateBody(&bodyHole);

	// ボディに凸面体のフィクスチャを追加
	b2FixtureDef fixtureHole;
	fixtureHole.shape = &dynamicCircle;

	//フィルターの設定
	fixtureHole.filter.categoryBits = BIT_HOLE;
	fixtureHole.filter.maskBits = BIT_BALL;
	
	//摩擦
	fixtureHole.restitution = 1.0f;
	fixtureHole.density = 1;
	body->CreateFixture(&fixtureHole);

	//キー操作を使用する。
	this->setKeyboardEnabled(true);

	//コンタクトリスナーを使用する。
	m_pWorld->SetContactListener(this);

	//ジャイロセンサーを使用する。
	setAccelerometerEnabled(true);
	//0.5秒おきにジャイロセンサーが働く。
	setAccelerometerInterval(0.5f);

	//背景の追加
	auto BackGround = Sprite::create("Textuer/Backgraund.png");
	BackGround->setAnchorPoint(Vec2(0.0f, 0.0f));
	this->addChild(BackGround, 0);


	//背景のパーティクルの追加
	auto BackGround_particle = ParticleSystemQuad::create("Particle/particle_Backgraund.plist");
	BackGround_particle->setScale(2.0f);
	BackGround_particle->setAnchorPoint(Vec2(0.0f, 0.0f));
	this->addChild(BackGround_particle, 0);

	//ポーズボタンの追加
	auto pouseButton = Button::create("Textuer/pouse.png");
	pouseButton->setScale(1.5f);
	pouseButton->setPosition(Vec2(m_origin.x + m_visibleSize.width - pouseButton->getContentSize().width / 1.6,
		m_origin.y + pouseButton->getContentSize().height / 2 ));
	this->addChild(pouseButton, 3);
	pouseButton->addTouchEventListener(CC_CALLBACK_2(PlayScene::touchEvent, this, PouseButton));

	//BGMの追加
	//m_BGM = AudioEngine::play2d("Sound/game_maoudamashii_5_village07.ogg");
	m_BGM = AudioEngine::play2d("Sound/geme_bgm.ogg");
	AudioEngine::setVolume(m_BGM, 3.0f);
	AudioEngine::setLoop(m_BGM, true);

	//ラベルの作成
	m_label = LabelTTF::create("80", "fonts/CODE2000.ttf", 20);

	//　ラベルの位置設定
	m_label->setPosition(Vec2(480, 600));
	m_label->setScale(2.5f);

	this->addChild(m_label, 3);

	return true;
}

//--------------------------------------------
//	機能	:	表示関数をオーバーライド
//
//	引数	:	Renderer *renderer, const Mat4& transform, uint32_t flags
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
{
	// 物理ワールドをデバッグ表示
	m_pWorld->DrawDebugData();
	//m_pDraw->DrawCircle(dynamicCircle.m_p, dynamicCircle.m_radius, b2Color(1.0f, 1.0f, 0.0f));
	//m_pDraw->DrawAABB(&aabb, b2Color(1.0f, 1.0f, 0.0f));
}

//--------------------------------------------
//機能	:アップデート
//--------------------------------------------
void PlayScene::update(float dt)
{
	// 物理ワールドの更新（時間を進める）
	m_pWorld->Step(1.0f / 60.0f, 8, 3);

	for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
	{
		b->SetAwake(true);
	}

	//剛体に画像をつける処理
	for (b2Body* b = m_pWorld->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != NULL) {
			Sprite* myActor = (Sprite*)b->GetUserData();
			myActor->setPosition(CCPointMake(b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO));
			myActor->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
		}
	}
	//センサーの処理
	if (m_sensorFlag == true)
	{
		if (m_stage >= 6)
		{
			//SEの再生
			int id = AudioEngine::play2d("Sound/se_maoudamashii_element_fire08.ogg");
			AudioEngine::setVolume(id, 3.0f);

			//オブジェの当たり判定を無くす
			//m_enemy->SetActive(false);

			Sensor_TransForm();

			m_sensorFlag = false;
		}
	}

	if (m_hitFlag == true)
	{
		//SEの再生
		int id = AudioEngine::play2d("Sound/se_maoudamashii_onepoint28.ogg");
		AudioEngine::setVolume(id, 3.0f);

		//個数が0以上ならば
		if (m_Object_Remaining != 0)
		{
			//穴に落ちる演出
			drop_object(m_Hole_UserDate->getPosition(), m_Object_bodyUserDate->getPosition());
		}

		//穴に当たったオブジェクトと穴自身の削除処理
		std::vector<b2Body*>::iterator it;
		for (it = m_Array_Destroy.begin(); it != m_Array_Destroy.end();)
		{
			m_pWorld->DestroyBody(*it);
			m_Array_Destroy.erase(it);
			it = m_Array_Destroy.begin();
		}

		//色の情報の削除
		vector <int>::iterator ite;
		ite = m_Color_Array.begin();
		m_Color_Array.erase(ite);
		m_objectColor = m_Color_Array.front();

		//ボールの穴の作成
		b2CircleShape dynamicCircle;
		//円の半径を指定
		dynamicCircle.m_radius = 1.0f;
		//Bodyの作成
		b2BodyDef bodyHole;
		bodyHole.type = b2_staticBody;
		b2Vec2 pos(250 / PTM_RATIO, 450 / PTM_RATIO);

		b2FixtureDef fixtureHole;



		//穴を削除するので新しく穴を作成する
		//ボールが入る穴の作成
		if (m_HoleType == BIT_BALL)
		{
			Color_Textur_Create* objectA = Color_Textur_Create::create(m_objectColor, BIT_BALL, GOOL);
			
			objectA->setPosition(250 / PTM_RATIO, 450 / PTM_RATIO);
			this->addChild(objectA, 1);
			bodyHole.userData = objectA;					//userDataにスプライトと色の情報を登録

			fixtureHole.shape = &dynamicCircle;

			//フィルターデータの設定
			fixtureHole.filter.categoryBits = BIT_HOLE;		//カテゴリービット	:	穴
			fixtureHole.filter.maskBits = BIT_BALL;		//マスクビット		:	ボール

		}

		//四角形が入る穴の作成
		if (m_HoleType == BIT_SQUARE)
		{
			Color_Textur_Create* objectB = Color_Textur_Create::create(m_objectColor, BIT_SQUARE, GOOL);
			
			objectB->setPosition(250 / PTM_RATIO, 450 / PTM_RATIO);
			this->addChild(objectB, 1);
			bodyHole.userData = objectB;					//userDataにスプライトと色の情報を登録

			fixtureHole.shape = &dynamicCircle;

			//フィルターデータの設定
			fixtureHole.filter.categoryBits = BIT_HOLE;		//カテゴリービット	:	穴
			fixtureHole.filter.maskBits = BIT_SQUARE;		//マスクビット		:	四角形

		}

		//三角形が入る穴の作成
		if (m_HoleType == BIT_TRIANGLE)
		{
			Color_Textur_Create* objectC = Color_Textur_Create::create(m_objectColor, BIT_TRIANGLE, GOOL);
			objectC->setPosition(250 / PTM_RATIO, 450 / PTM_RATIO);
			this->addChild(objectC, 1);
			bodyHole.userData = objectC;					//userDataにスプライトと色の情報を登録

			fixtureHole.shape = &dynamicCircle;

			//フィルターデータの設定
			fixtureHole.filter.categoryBits = BIT_HOLE;		//カテゴリービット	:	穴
			fixtureHole.filter.maskBits = BIT_TRIANGLE;		//マスクビット		:	三角形
		}

		if (m_objectColor==4)
		{
			auto Gool = Sprite::create("Textuer/Gool.png");
			this->addChild(Gool, 1);
			Gool->setScale(0.5f);
			
			Gool->setPosition(225, 450);

		}

		bodyHole.position.Set(250 / PTM_RATIO, 450 / PTM_RATIO);	//ポジションの設定
		body = m_pWorld->CreateBody(&bodyHole);
		body->CreateFixture(&fixtureHole);							// ボディに凸面体のフィクスチャを追加

		m_hitFlag = false;

		//オブジェクトが無くなるとクリア処理
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
		//時間の更新
		std::stringstream ss;
		ss << m_labelTime;

		//残り時間の表示
		m_label->setString(ss.str());

		//残り時間が15秒になると画面が赤く点滅する処理
		
		if (m_labelTime == 15)
		{

			AudioEngine::stop(m_BGM);
			//ゲームオーバーのオーディオを流す
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

			//すべての処理の一時停止処理
			this->pauseSchedulerAndActions();

			m_AlarmSprite->removeFromParent();


			//ゲームオーバーの画像を表示
			auto m_CrearSprite = Sprite::create("Textuer/GameOver.png");
			m_CrearSprite->setPosition(Vec2(m_visibleSize.width / 2 + m_origin.x, m_visibleSize.height / 2 + m_origin.y));
			this->addChild(m_CrearSprite, 2);

			//次のステージへ進むボタンの作成
			auto ReStart = Button::create("Textuer/ReStage.png");

			ReStart->setScale(1.5f);
			ReStart->setPosition(Vec2((m_origin.x + m_visibleSize.width) / 2, (m_origin.y + m_visibleSize.height) / 4.5));

			this->addChild(ReStart, 2);
			ReStart->addTouchEventListener(CC_CALLBACK_2(PlayScene::touchEvent, this, ReStartButton));

			//BGMを止める
			AudioEngine::stop(m_BGM);

			//ゲームオーバーのオーディオを流す
			int gemeOverId = AudioEngine::play2d("Sound/game_maoudamashii_9_jingle08.ogg");
			AudioEngine::setVolume(gemeOverId, 10.0f);

		}
	}
}

//--------------------------------------------
// 機能	:デストラクタ
//--------------------------------------------
PlayScene::~PlayScene()
{
	// 解放
	delete m_pDraw;
	delete m_pWorld;

}

//--------------------------------------------
//	機能	:	再帰呼び出しの関数
//	
//	引数	:	何回再帰するか　と　ステージの情報
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::Object_Factory(int level,int stage)
{

	if (stage>2)
	{
		stage = 2;
	}

	//階層が限界になったら終了
	if (level >= stage)
	{
		return;
	}

	//円の色をランダムに決める
	int c_Circle = rand() % 3 ;
	//色を保存する
	m_Color_Array.push_back(c_Circle);

	b2CircleShape dynamicCircleA;
	//円の半径を指定
	dynamicCircleA.m_radius = 1.0f;

	// 円の形状データを作る
	b2BodyDef bodyBall;
	bodyBall.type = b2_dynamicBody;
	b2Vec2 posA((rand() % 500 + 100) / PTM_RATIO, (rand() % 200 + 100) / PTM_RATIO);
	Color_Textur_Create* object_SpriteA = 
		Color_Textur_Create::create(c_Circle, BIT_BALL,OBJECT);	//スプライトとランダムで決定した色をuserDataに登録
	this->addChild(object_SpriteA, 3);		
	bodyBall.userData = object_SpriteA; 			//userDataにスプライトを登録
	bodyBall.position.Set(posA.x, posA.y);			//ポジションの決定

	m_ObjectBody = m_pWorld->CreateBody(&bodyBall);			// ボディに凸面体のフィクスチャを追加
	b2FixtureDef fixtureDefA;
	fixtureDefA.shape = &dynamicCircleA;

	//フィルターデータの設定
	fixtureDefA.filter.categoryBits = BIT_BALL;		//カテゴリービット	:	穴	
			//マスクビット		:	穴と　		壁と      ボールと	　四角と		三角と       ギミックと　 センサー	に当たる
	fixtureDefA.filter.maskBits = BIT_HOLE | BIT_GROUND | BIT_BALL | BIT_SQUARE | BIT_TRIANGLE | BIT_GIMMICK | BIT_SENSOR;

	//摩擦
	fixtureDefA.restitution = 0.3f;
	fixtureDefA.density = 1;

	m_ObjectBody->CreateFixture(&fixtureDefA);

	//ボディの情報を配列に入れる
	m_Array_Body.push_back(m_ObjectBody);

	m_Object_Remaining++;

	//三角形の半径を指定
	int c_Convex = rand() % 3;			//三角形の色をランダムに決める
	m_Color_Array.push_back(c_Convex);	//色を保存する

	b2PolygonShape dynamicConvex;
	
	// 頂点のリスト
	b2Vec2 points[] =
	{//       x  y
		{ -1.0, -1.0f }, // 1点目
		{ 1.0f, -1.0f }, // 2点目
		{ 0.0f, 1.0f }, // 3点目

	};
	// 頂点数
	int npoints = sizeof(points) / sizeof(b2Vec2);
	// 頂点データをセット
	dynamicConvex.Set(points, npoints);

	// 三角形の形状データを作る
	b2BodyDef bodyConvex;
	bodyConvex.type = b2_dynamicBody;

	//userDataの設定
	Color_Textur_Create* object_SpriteC = 
		Color_Textur_Create::create(c_Convex, BIT_TRIANGLE, OBJECT);	//スプライトとランダムで決定した色をuserDataに登録
	this->addChild(object_SpriteC,3);
	bodyConvex.userData = object_SpriteC;				//userDataにスプライトと色の情報を登録

	b2Vec2 posC((rand() % 500 + 100) / PTM_RATIO, (rand() % 200 + 100) / PTM_RATIO);
	bodyConvex.position.Set(posC.x, posC.y);			//ポジションの決定
	m_ObjectBody = m_pWorld->CreateBody(&bodyConvex);			// ボディに凸面体のフィクスチャを追加
	b2FixtureDef fixtureDefC;
	fixtureDefC.shape = &dynamicConvex;

	//フィルターデータの設定
	fixtureDefC.filter.categoryBits = BIT_TRIANGLE;		//カテゴリービット	:	穴
			//マスクビット		:	穴と　		壁と      ボールと	　四角と		三角と       ギミックと　 　センサー	に当たる
	fixtureDefC.filter.maskBits = BIT_HOLE | BIT_GROUND | BIT_BALL | BIT_SQUARE | BIT_TRIANGLE | BIT_GIMMICK | BIT_SENSOR;

	//摩擦
	fixtureDefC.restitution = 0.3f;
	fixtureDefC.density = 1;
	m_ObjectBody->CreateFixture(&fixtureDefC);
	//ボディの情報を配列に入れる
	m_Array_Body.push_back(m_ObjectBody);
	m_Object_Remaining++;

	// 四角の形状データを作る
	int c_Box = rand() % 3;
	m_Color_Array.push_back(c_Box);

	b2PolygonShape dynamicBox;

	dynamicBox.SetAsBox(1.0, 1.0f);

	b2BodyDef bodyBox;
	bodyBox.type = b2_dynamicBody;

	b2Vec2 posB((rand() % 500 + 100) / PTM_RATIO, (rand() % 200 + 100) / PTM_RATIO);

	Color_Textur_Create* object_SpriteB = Color_Textur_Create::create(c_Box, BIT_SQUARE, OBJECT);
	this->addChild(object_SpriteB,3);

	bodyBox.userData = object_SpriteB;					 //userDataにスプライトと色の情報を登録
	bodyBox.position.Set(posB.x, posB.y);				//ポジションの決定
	m_ObjectBody = m_pWorld->CreateBody(&bodyBox);

	b2FixtureDef fixtureDefB;							// ボディに凸面体のフィクスチャを追加
	fixtureDefB.shape = &dynamicBox;					// ボディに凸面体のフィクスチャを追加
	
	//フィルターデータの設定
	fixtureDefB.filter.categoryBits = BIT_SQUARE;		//カテゴリービット	:	穴
			//マスクビット		:	穴と　		壁と      ボールと	　四角と		三角と       ギミックと　 　センサー	に当たる
	fixtureDefB.filter.maskBits = BIT_HOLE | BIT_GROUND | BIT_BALL | BIT_SQUARE | BIT_TRIANGLE | BIT_GIMMICK | BIT_SENSOR;

	//摩擦
	fixtureDefB.restitution = 0.3f;
	fixtureDefB.density = 1;
	m_ObjectBody->CreateFixture(&fixtureDefB);
	//ボディの情報を配列に入れる
	m_Array_Body.push_back(m_ObjectBody);

	m_Object_Remaining++;


	Object_Factory(level + 1,stage);
}

//--------------------------------------------
//	機能	:	衝突判定
//	
//	引数	:	コンタクトの情報
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::BeginContact(b2Contact* contact)
{
	//二つのオブジェクトのフィクスチャデータを得る
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();

	Color_Textur_Create* bodyUserDateA = (Color_Textur_Create*)fixtureA->GetBody()->GetUserData();
	Color_Textur_Create* bodyUserDateB = (Color_Textur_Create*)fixtureB->GetBody()->GetUserData();

	//二つのオブジェクトのフィルターのデータを得る
	b2Filter filterA = fixtureA->GetFilterData();
	b2Filter filterB = fixtureB->GetFilterData();

	//	オブジェクトAに穴が当たった時	 ||  オブジェクトBに穴が当たった時
	if (filterA.categoryBits == BIT_HOLE || filterB.categoryBits == BIT_HOLE)
	{
		if (bodyUserDateA->GetColor() == bodyUserDateB->GetColor())
		{
			//穴とオブジェクト両方のボディをm_Array_Destroyに入れる
			m_Array_Destroy.push_back(fixtureA->GetBody());
			m_Array_Destroy.push_back(fixtureB->GetBody());

			//UserDateの中にあるスプライトと色の情報を削除する。

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

			
			//削除したUserDateにnullptrを入れる
			contact->GetFixtureA()->GetBody()->SetUserData(nullptr);
			contact->GetFixtureB()->GetBody()->SetUserData(nullptr);

			m_hitFlag = true;

			bool hit = true;

			if (m_HoleType == BIT_BALL)
			{
				//三角形にする
				if (hit)
				{
					m_HoleType = BIT_TRIANGLE;
					hit = false;
				}
			}

			if (m_HoleType == BIT_TRIANGLE)
			{
				//四角形にする
				if (hit)
				{
					m_HoleType = BIT_SQUARE;
					hit = false;
				}
			}

			if (m_HoleType == BIT_SQUARE)
			{
				//ボールにする
				if (hit)
				{
					m_HoleType = BIT_BALL;
					hit = false;
				}
			}

			//オブジェクトの個数を減らす。
			m_Object_Remaining--;
		}
	}

	
	//センサーの衝突判定の呼び出し
	if (m_stage >= 6)
	{
		sensorContact(fixtureA, fixtureB);
	}
	
}

//--------------------------------------------
//	機能	:	ジャイロセンサー
//	
//	引数	:	Acceleration* acc, Event* unused_event
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::onAcceleration(Acceleration* acc, Event* unused_event)
{

	float j_x = (float)acc->x;
	float j_y = (float)acc->y;

	b2Vec2 gravity(0, 0);

	//最初の傾きを記録しておく
	if (m_Gyro_Sensor_Flag == true)
	{
		m_First_gravity.Set(j_x, j_y);
		m_Gyro_Sensor_Flag = false;
	}

	//無重力設定
	if (j_x == 0 || j_y == 0)
	{
		gravity.Set(0.0f, 0.0f);
		m_pWorld->SetGravity(gravity);
		return;
	}


	if (j_y <= -0.7f)
	{
		//下への初速設定
		gravity.Set(j_x * 10, j_y * 10);
		m_pWorld->SetGravity(gravity);

		return;
	}
	//重力設定
	//{(引数から取得した　x　,　y　)-(最初の傾き x , y )} * 20 = 傾けた重力
	gravity.Set((j_x - m_First_gravity.x) * 20, (j_y - m_First_gravity.y) * 20);
	m_pWorld->SetGravity(gravity);


}

//--------------------------------------------
//	機能	:	キー入力した時の処理
//	
//	引数	:	Acceleration* acc, Event* unused_event
//
//	戻り値	:	EventKeyboard::KeyCode keyCode, Event* event
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
//	機能	:	キーを離した時の処理
//	
//	引数	:	Acceleration* acc, Event* unused_event
//
//	戻り値	:	なし
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
//	機能	:	Wキーを押した時の処理
//	
//	引数	:	float dt
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::onKeyDownW(float dt)
{
	// 重力設定
	m_Gravity.Set(0.0f, 20.0f);
	m_pWorld->SetGravity(m_Gravity);
}

//--------------------------------------------
//	機能	:	Aキーを押した時の処理
//	
//	引数	:	float dt
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::onKeyDownA(float dt)
{
	// 重力設定
	m_Gravity.Set(-20.0f, 0.0f);
	m_pWorld->SetGravity(m_Gravity);
}

//--------------------------------------------
//	機能	:	Sキーを押した時の処理
//	
//	引数	:	float dt
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::onKeyDownS(float dt)
{
	// 重力設定
	m_Gravity.Set(0.0f, -20.0f);
	m_pWorld->SetGravity(m_Gravity);
}

//--------------------------------------------
//	機能	:	Dキーを押した時の処理
//	
//	引数	:	float dt
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::onKeyDownD(float dt)
{
	// 重力設定
	m_Gravity.Set(20.0f, 0.0f);
	m_pWorld->SetGravity(m_Gravity);
}

//--------------------------------------------
//	機能	:	ボタンを押した時の処理
//	
//	引数	:	Ref* pSender, Widget::TouchEventType type, int Buttan
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::touchEvent(Ref* pSender, Widget::TouchEventType type, int Buttan)
{
	switch (type)
	{
	case Widget::TouchEventType::ENDED:

		float Audio_time = AudioEngine::getCurrentTime(m_BGM);

		//オーディオの取得と再生
		AudioEngine::stop(m_BGM);

		if (m_labelTime <= 15)
		{
			AudioEngine::stop(PinchId);
		}

		int id = AudioEngine::play2d("Sound/se_maoudamashii_onepoint26.ogg");
		AudioEngine::setVolume(id, 3.0f);

		//ネクストステージボタンが押されたならば
		if (Buttan == NextStageButton)
		{
			//ステージが8ならばタイトルへ
			if (m_stage == 8)
			{
				Scene* playScene = TitleScene::createScene();

				//トランディション
				TransitionFade* tscene = TransitionFade::create(1.0f, playScene);

				Director::getInstance()->replaceScene(tscene);
			}

			//それ以外ならば
			else
			{
				//次のステージへ
				m_stage++;

				Scene* playScene = PlayScene::createScene(m_stage);

				//トランディション
				TransitionFade* tscene = TransitionFade::create(1.0f, playScene);

				Director::getInstance()->replaceScene(tscene);
			}
			break;
		}


		//ボタンがポーズボタンならばポーズする
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

			//トランディション
			TransitionFade* tscene = TransitionFade::create(1.0f, playScene);

			Director::getInstance()->replaceScene(tscene);

		}
		break;
	}
}

//--------------------------------------------
//	機能	:	センサー処理
//	
//	引数	:	衝突した物体の情報　A　&　B
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::sensorContact(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
	//センサーの当たり判定

	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();

	Color_Textur_Create* bodyUserDateA = (Color_Textur_Create*)fixtureA->GetBody()->GetUserData();
	Color_Textur_Create* bodyUserDateB = (Color_Textur_Create*)fixtureB->GetBody()->GetUserData();

	//片方だけがセンサーであるケース以外の時	||
	// フィルターデータA,Bが 穴の時				||
	// フィルターデータA,Bが 壁の時	は除外
	if (!(sensorA^sensorB) ||
		fixtureA->GetFilterData().categoryBits == BIT_HOLE || 
		fixtureB->GetFilterData().categoryBits == BIT_HOLE||
		fixtureA->GetFilterData().categoryBits == BIT_GROUND||
		fixtureB->GetFilterData().categoryBits == BIT_GROUND)
	{
		return;
	}

	//ユーザーデータを取り出す。
	b2Body* entityA = fixtureA->GetBody();
	b2Body* entityB = fixtureB->GetBody();
	
	//センサーがAならば
	if (sensorA)
	{
		//フィクスチャBがオブジェクトである
		m_rader = entityA;
		m_Array_Sensor_Object.push_back(entityB);
		
		m_Object_bodyUserDate = bodyUserDateB;

		m_sensorFlag = true;
	}

	//センサーがBならば
	if (sensorB)
	{
		//フィクスチャAがオブジェクトである
		m_rader = entityB;
		m_Array_Sensor_Object.push_back(entityA);

		m_Object_bodyUserDate = bodyUserDateA;

		m_sensorFlag = true;
	}

}

//--------------------------------------------
//	機能	:	クリアした時の処理
//	
//	引数	:	float dt
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::Crear(float dt)
{
	if (m_labelTime <= 15)
	{
		AudioEngine::stop(PinchId);


		m_AlarmSprite->removeFromParent();
	}
	//画面のポーズ処理
	this->pauseSchedulerAndActions();
	//BGMを鳴らす
	AudioEngine::stop(m_BGM);
	crearid = AudioEngine::play2d("Sound/se_maoudamashii_jingle06.ogg");
	AudioEngine::setVolume(crearid, 3.0f);

	//パーティクルの作成
	auto particle = ParticleSystemQuad::create("Particle/particle_hanabi.plist");
	// パーティクルを表示する場所の設定
	particle->setPosition(Vec2(480, 320));
	particle->setAutoRemoveOnFinish(true);
	// パーティクルを配置
	this->addChild(particle, 3);

	//クリアの画像を表示
	auto m_CrearSprite = Sprite::create("Textuer/GameClear.png");
	m_CrearSprite->setPosition(Vec2(m_visibleSize.width / 2 + m_origin.x, m_visibleSize.height / 2 + m_origin.y));
	this->addChild(m_CrearSprite, 3);

	//次のステージへ進むボタンの作成
	auto NextStage = Button::create("Textuer/Next_Stage.png");
	NextStage->setScale(1.5f);
	NextStage->setPosition(Vec2((m_origin.x + m_visibleSize.width) / 2, (m_origin.y + m_visibleSize.height) / 4.5));
	this->addChild(NextStage, 3);
	NextStage->addTouchEventListener(CC_CALLBACK_2(PlayScene::touchEvent, this, NextStageButton));
}

//--------------------------------------------
//	機能	:	オブジェが穴に落ちた時にはじける処理
//	
//	引数	:	穴の位置　,　オブジェの位置
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::drop_object(Vec2 HollPosition, Vec2 ObjectPosition)
{

	//UserDateから色の情報をメンバ変数に保存する
	m_object_color = m_Object_bodyUserDate->GetColor();
	//UserDateから形の情報をメンバ変数に保存する
	m_object_Form = m_Object_bodyUserDate->GetForm();

	//---------------------------------------------------
	//穴に当たるとオブジェがはじけるパーティクルの作成
	//---------------------------------------------------

	//四角のパーティクルを作る
	ParticleSystemQuad* particle_Crash = ParticleSystemQuad::create("Particle/particle_Crash.plist");

	//usedata　が球体なら球体のパーティクルを作成
	if (m_object_Form==Form_maru)
	{
		particle_Crash = ParticleSystemQuad::create("Particle/particle_Crash_maru.plist");

	}
	//usedata　が三角形なら三角形のパーティクルを作成
	else
	{
		if (m_object_Form == Form_sankaku)
		{
			particle_Crash = ParticleSystemQuad::create("Particle/particle_Crash_sankaku.plist");

		}
	}
	//オブジェが消える位置にパーティクルのセット
	particle_Crash->setPosition(Vec2(ObjectPosition.x, ObjectPosition.y));
	//パーティクルの大きさを小さくする。
	particle_Crash->setScale(0.4f);

	//ユーザーデータの削除
	m_Hole_UserDate->removeFromParent();
	m_Object_bodyUserDate->removeFromParent();

	//色の設定
	//usedata　の色が黄色なら色を黄色にする。
	if (m_object_color == Color_Yellow)
	{
		particle_Crash->setStartColor(Color4F::YELLOW);		//パーティクルが始まる色を黄色にする 
		particle_Crash->setEndColor(Color4F::YELLOW);		//パーティクルが終わる色を黄色にする
	}
	//usedata　の色が赤色なら色を赤色にする。
	else
	{
		if (m_object_color == Color_Red)
		{
			particle_Crash->setStartColor(Color4F::RED);	//パーティクルが始まる色を赤色にする 
			particle_Crash->setEndColor(Color4F::RED);		//パーティクルが終わる色を赤色にする
		}
	}
	this->addChild(particle_Crash, 3);

	//はじけるパーティクルの終了時間を得る。
	float Crash_time = particle_Crash->getDuration();

	//パーティクルが終了時間を1.5倍した時間まで次のアクションを止めておく
	auto delay = DelayTime::create(Crash_time*1.2);

	//particle_Convergence関数をアクションに追加する。
	CallFuncN* callfunc = CallFuncN::create(this, callfuncN_selector(PlayScene::particle_Convergence));
	MoveTo* move = MoveTo::create(0.5f,HollPosition);

	//DelayTime　と　CallFuncNのアクションを順番に流す。
	Sequence*sequence = Sequence::create(delay, callfunc,nullptr);

	Spawn* spawn = Spawn::create(sequence,move,nullptr);

	particle_Crash->runAction(spawn);
}

//--------------------------------------------
//	機能	:	はじけたオブジェが穴に集まる処理
//	
//	引数	:	Node* sender
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::particle_Convergence(Node* sender)
{
	//--------------------------------------------------
	//ボールが穴に吸い込まれるパーティクルの作成
	//--------------------------------------------------

	//四角形のパーティクルの作成
	ParticleSystemQuad* particle_Convergence = ParticleSystemQuad::create("Particle/particle_Convergence.plist");
	
	//メンバ変数に保存している形が球体ならば球体のパーティクルを作成
	if (m_object_Form == Form_maru)
	{
		particle_Convergence = ParticleSystemQuad::create("Particle/particle_Convergence_maru.plist");

	}
	//メンバ変数に保存している形が三角形ならば三角形のパーティクルを作成
	else
	{
		if (m_object_Form == Form_sankaku)
		{
			particle_Convergence = ParticleSystemQuad::create("Particle/particle_Convergence_sankaku.plist");

		}
	}
	
	//穴の位置にパーティクルをセットする。
	particle_Convergence->setPosition(Vec2(225,450));
	//パーティクルを縮小する。
	particle_Convergence->setScale(0.1f);

	//メンバ変数に保存してある色の情報が黄色ならばパーティクルの色を黄色にする。
	if (m_object_color == Color_Yellow)
	{
		particle_Convergence->setStartColor(Color4F::YELLOW);	//パーティクルが始まる色を黄色にする
		particle_Convergence->setEndColor(Color4F::YELLOW);		//パーティクルが終わる色を黄色にする
	}

	//メンバ変数に保存してある色の情報が赤色ならばパーティクルの色を赤色にする。
	else
	{
		if (m_object_color == Color_Red)
		{
			particle_Convergence->setStartColor(Color4F::RED);	//パーティクルが始まる色を赤色にする
			particle_Convergence->setEndColor(Color4F::RED);	//パーティクルが終わる色を赤色にする
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

		//UserDateから色の情報をメンバ変数に保存する
		m_object_color = m_Object_bodyUserDate->GetColor();

		if (m_Object_bodyUserDate->GetColor() == Color_Yellow)
		{
			particle->setStartColor(Color4F::YELLOW);	//パーティクルが始まる色を黄色にする
			particle->setEndColor(Color4F::YELLOW);		//パーティクルが終わる色を黄色にする
		}

		else if (m_Object_bodyUserDate->GetColor() == Color_Blue)
		{
			particle->setStartColor(Color4F::BLUE);	//パーティクルが始まる色を黄色にする
			particle->setEndColor(Color4F::BLUE);		//パーティクルが終わる色を黄色にする
		}

		b2Vec2 obje_position = m_array_sensor_object[0]->GetPosition();
		particle->setPosition(Vec2(obje_position.x * PTM_RATIO, obje_position.y * PTM_RATIO));
		
		float delay_time = particle->getDuration();
		auto delay = DelayTime::create(delay_time * 1.5);

		//Sensor_Second_ObjectTransform関数を予約する。
		CallFunc* callfunc = CallFunc::create(std::bind(&PlayScene::Sensor_Second_ObjectTransform, this, m_array_sensor_object.front()));

		//delayとcallfuncの合成
		Sequence*sequence = Sequence::create(delay, callfunc, nullptr);
		this->runAction(sequence);
		particle->setScale(0.3f);
		addChild(particle, 3);

		//出口の位置を保存する。
		Vec2 exitPosition = m_gimmick->GetexitSpritePosition();

		MoveTo* move = MoveTo::create(0.8f, exitPosition);
		particle->runAction(move);

		m_array_sensor_object.erase(it);
		it = m_array_sensor_object.begin();

	}

	//Sensor_First_ObjectTransform関数をアクションに追加する。
	Sensor_First_ObjectTransform();

	//オブジェを出口に移動させる。

}

//--------------------------------------------
//	機能	:	センサーにオブジェが当たった時の処理
//	
//	引数	:	float dt
//
//	戻り値	:	なし
//--------------------------------------------
void PlayScene::Sensor_First_ObjectTransform()
{

	std::vector<b2Body*>::iterator it;

	//センサーに当たったら別の場所にワープする。
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
	////センサーに当たったら別の場所にワープする。
	Vec2 exitPosition = m_gimmick->GetexitSpritePosition();

	pbody->SetTransform(b2Vec2(exitPosition.x / PTM_RATIO, exitPosition.y / PTM_RATIO), 0);
	pbody->SetActive(true);
}