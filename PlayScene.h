/**
* @file PlayScene.h
* @brief プレイシーンの作成
* @author 中谷	駿佑
* @date 2015 /	9	/	1
*/

#ifndef __PLAYSENE_H__
#define __PLAYSENE_H__

#include "cocos2d.h"

#include "GLES-Render.h"
#include "Box2D/Box2D.h"
#include "ui/CocosGUI.h"
#include "Color_Textur_Create.h"
#include "Gimmick.h"
#include "TitleScene.h"
#include "Help.h"
#include "ModalLayer.h"


#include <vector>

enum TagOfButton
{
	Up = 1,
	Right = 2,
	Left = 3,
	Down = 4
	
};

enum TagOfButton2
{
	SlectSceneButton,
	ReStartButton,
	NextStageButton,
	PouseButton,
	TitleButton

};


class PlayScene : public cocos2d::Layer, public b2ContactListener
{
public:
	//定数
	static const bool GOOL = true;
	static const bool OBJECT = false;

	//--------------------------------------------
	//! @brief	コンストラクタ
	//--------------------------------------------
	PlayScene();

	//--------------------------------------------
	//! @brief	シーンの作成
	//--------------------------------------------
	static cocos2d::Scene* createScene(int Stage);
	static PlayScene* create(int stage);

	//--------------------------------------------
	//! @brief	物理システムの初期化
	//! @param[int]  stage	ステージ数
	//--------------------------------------------
	void initPhysics(int stage);

	//--------------------------------------------
	//! @brief	初期化処理
	//! @param[int] stage
	//--------------------------------------------
	virtual bool init(int stage);

	//--------------------------------------------
	//! @brief	表示関数のオーバーライド
	//! @param[Renderer]	renderer
	//! @param[Mat4]		transform
	//! @param[uint32_t]	flags
	//--------------------------------------------
	void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags);

	//--------------------------------------------
	//! @brief	アップデート
	//! @param[float] dt
	//--------------------------------------------
	void update(float dt);
	
	//--------------------------------------------
	//! @brief	デストラクタ
	//--------------------------------------------
	~PlayScene();

	//--------------------------------------------
	//! @brief	ジャイロセンサー
	//! @param	[Acceleration*] acc
	//! @param  [Event*] unused_event
	//--------------------------------------------
	void onAcceleration(cocos2d::Acceleration* acc, cocos2d::Event* unused_event) override;

	//--------------------------------------------
	//! @brief オブジェクトの生成の再帰呼び出し
	//! @param[int] level 
	//! @param[int] stage ステージ数
	//--------------------------------------------
	void Object_Factory(int level, int stage);

	//--------------------------------------------
	//キー入力での重力操作
	//--------------------------------------------
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyDownW(float dt);
	void onKeyDownA(float dt);
	void onKeyDownS(float dt);
	void onKeyDownD(float dt);

	//--------------------------------------------
	//! @brief	衝突判定
	//! @param	[b2Contact*] contact
	//--------------------------------------------
	void BeginContact(b2Contact* contact);

	// implement the "static create()" method manually
//	CREATE_FUNC(PlayScene);


	//--------------------------------------------
	//! @brief	次のステージへのシーン遷移
	//--------------------------------------------
	void touchEvent(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type, int Buttan);

	//--------------------------------------------
	//! @brief センサー処理
	//! @param[b2Fixture] fixtureA
	//! @param[b2Fixture] fixtureB
	//--------------------------------------------
	void sensorContact(b2Fixture* fixtureA, b2Fixture* fixtureB);

	//--------------------------------------------
	//! @brief クリア処理
	//! @param[float] dt
	//--------------------------------------------
	void Crear(float dt);

	//--------------------------------------------
	//! @brief オブジェが穴に落ちた時にはじける処理
	//! @param[:Vec2]  HollPosition
	//! @param[:Vec2]  ObjectPosition
	//--------------------------------------------
	void drop_object(cocos2d::Vec2 HollPosition, cocos2d::Vec2 ObjectPosition);

	//--------------------------------------------
	//! @brief はじけたオブジェが穴に集まる処理
	//! @param[Node*] sender
	//--------------------------------------------
	void particle_Convergence(Node* sender);

	void Sensor_TransForm();

	//--------------------------------------------
	//! @brief センサーにオブジェが当たった時の処理
	//! @param[float] dt
	//--------------------------------------------
	void Sensor_First_ObjectTransform();
	void Sensor_Second_ObjectTransform(b2Body* pbody);


private:

	b2Body *body;
	b2Body *m_ObjectBody;
	b2Body* m_pGroundBody;					//壁の変数
	b2Body* m_rader;						//センサーの情報

	b2World* m_pWorld;						//物理ワールド

	GLESDebugDraw* m_pDraw;					//Drow変数
	
	cocos2d::Sprite* m_CrearSprite;

	std::vector<b2Body*>m_Array_Destroy;	//オブジェクトを消すための配列
	std::vector<int>m_Color_Array;			//オブジェクトの色の情報を入れる配列
	std::vector<b2Body*>m_Array_Body;
	std::vector<b2Body*> m_Array_Sensor_Object;

	int m_objectColor;						//オブジェクトの色
	int m_HoleType;							//当たった時の穴の形
	int m_Object_Remaining;					//残っているオブジェクトの数
	int m_stage;
	int crearid;							//クリアした時のオーディオ
	int m_BGM;								//プレイシーンに常時流すオーディオの変数
	int m_timeCount;						//時間のカウント変数
	int m_labelTime;						//ラベルによる時間のカウント変数
	int m_pauseFlag;						//ポーズするかどうかのフラグ変数
	int PinchId;

	bool m_hitFlag;							//当たっているかどうかのフラグ
	bool m_sensorFlag;						//センサーに当たったかどうかのフラグ
	bool m_Gyro_Sensor_Flag;				//最初の傾きかどうかを調べるためのフラグ

	b2Vec2 m_Gravity;						// 物理ワールドの重力を設定
	b2Vec2 m_First_gravity; 				// 始めの傾きを保存しておく変数

	cocos2d::Size m_visibleSize;			//目に見えるサイズ
	cocos2d::Vec2 m_origin;					//原点
	cocos2d::LabelTTF* m_label;				//残り時間を表示するためのラベル変数
	cocos2d::ui::Button* pouseButton;		//ポーズボタンのUI変数
	cocos2d::RepeatForever* m_repeat;
	cocos2d::Sprite* m_AlarmSprite;


	Gimmick* m_gimmick;						//ギミックの変数

	Color m_object_color;					//色の情報を保存しておく

	Form m_object_Form;						//形の情報を保存しておく

	Color_Textur_Create* m_Hole_UserDate;			//穴のユーザーデータを保存しておく
	Color_Textur_Create* m_Object_bodyUserDate;		//オブジェのユーザーデータを保存しておく
};


#endif // __PLAYSENE_H__
