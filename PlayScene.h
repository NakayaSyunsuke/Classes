/**
* @file PlayScene.h
* @brief �v���C�V�[���̍쐬
* @author ���J	�x�C
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
	//�萔
	static const bool GOOL = true;
	static const bool OBJECT = false;

	//--------------------------------------------
	//! @brief	�R���X�g���N�^
	//--------------------------------------------
	PlayScene();

	//--------------------------------------------
	//! @brief	�V�[���̍쐬
	//--------------------------------------------
	static cocos2d::Scene* createScene(int Stage);
	static PlayScene* create(int stage);

	//--------------------------------------------
	//! @brief	�����V�X�e���̏�����
	//! @param[int]  stage	�X�e�[�W��
	//--------------------------------------------
	void initPhysics(int stage);

	//--------------------------------------------
	//! @brief	����������
	//! @param[int] stage
	//--------------------------------------------
	virtual bool init(int stage);

	//--------------------------------------------
	//! @brief	�\���֐��̃I�[�o�[���C�h
	//! @param[Renderer]	renderer
	//! @param[Mat4]		transform
	//! @param[uint32_t]	flags
	//--------------------------------------------
	void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags);

	//--------------------------------------------
	//! @brief	�A�b�v�f�[�g
	//! @param[float] dt
	//--------------------------------------------
	void update(float dt);
	
	//--------------------------------------------
	//! @brief	�f�X�g���N�^
	//--------------------------------------------
	~PlayScene();

	//--------------------------------------------
	//! @brief	�W���C���Z���T�[
	//! @param	[Acceleration*] acc
	//! @param  [Event*] unused_event
	//--------------------------------------------
	void onAcceleration(cocos2d::Acceleration* acc, cocos2d::Event* unused_event) override;

	//--------------------------------------------
	//! @brief �I�u�W�F�N�g�̐����̍ċA�Ăяo��
	//! @param[int] level 
	//! @param[int] stage �X�e�[�W��
	//--------------------------------------------
	void Object_Factory(int level, int stage);

	//--------------------------------------------
	//�L�[���͂ł̏d�͑���
	//--------------------------------------------
	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyDownW(float dt);
	void onKeyDownA(float dt);
	void onKeyDownS(float dt);
	void onKeyDownD(float dt);

	//--------------------------------------------
	//! @brief	�Փ˔���
	//! @param	[b2Contact*] contact
	//--------------------------------------------
	void BeginContact(b2Contact* contact);

	// implement the "static create()" method manually
//	CREATE_FUNC(PlayScene);


	//--------------------------------------------
	//! @brief	���̃X�e�[�W�ւ̃V�[���J��
	//--------------------------------------------
	void touchEvent(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type, int Buttan);

	//--------------------------------------------
	//! @brief �Z���T�[����
	//! @param[b2Fixture] fixtureA
	//! @param[b2Fixture] fixtureB
	//--------------------------------------------
	void sensorContact(b2Fixture* fixtureA, b2Fixture* fixtureB);

	//--------------------------------------------
	//! @brief �N���A����
	//! @param[float] dt
	//--------------------------------------------
	void Crear(float dt);

	//--------------------------------------------
	//! @brief �I�u�W�F�����ɗ��������ɂ͂����鏈��
	//! @param[:Vec2]  HollPosition
	//! @param[:Vec2]  ObjectPosition
	//--------------------------------------------
	void drop_object(cocos2d::Vec2 HollPosition, cocos2d::Vec2 ObjectPosition);

	//--------------------------------------------
	//! @brief �͂������I�u�W�F�����ɏW�܂鏈��
	//! @param[Node*] sender
	//--------------------------------------------
	void particle_Convergence(Node* sender);

	void Sensor_TransForm();

	//--------------------------------------------
	//! @brief �Z���T�[�ɃI�u�W�F�������������̏���
	//! @param[float] dt
	//--------------------------------------------
	void Sensor_First_ObjectTransform();
	void Sensor_Second_ObjectTransform(b2Body* pbody);


private:

	b2Body *body;
	b2Body *m_ObjectBody;
	b2Body* m_pGroundBody;					//�ǂ̕ϐ�
	b2Body* m_rader;						//�Z���T�[�̏��

	b2World* m_pWorld;						//�������[���h

	GLESDebugDraw* m_pDraw;					//Drow�ϐ�
	
	cocos2d::Sprite* m_CrearSprite;

	std::vector<b2Body*>m_Array_Destroy;	//�I�u�W�F�N�g���������߂̔z��
	std::vector<int>m_Color_Array;			//�I�u�W�F�N�g�̐F�̏�������z��
	std::vector<b2Body*>m_Array_Body;
	std::vector<b2Body*> m_Array_Sensor_Object;

	int m_objectColor;						//�I�u�W�F�N�g�̐F
	int m_HoleType;							//�����������̌��̌`
	int m_Object_Remaining;					//�c���Ă���I�u�W�F�N�g�̐�
	int m_stage;
	int crearid;							//�N���A�������̃I�[�f�B�I
	int m_BGM;								//�v���C�V�[���ɏ펞�����I�[�f�B�I�̕ϐ�
	int m_timeCount;						//���Ԃ̃J�E���g�ϐ�
	int m_labelTime;						//���x���ɂ�鎞�Ԃ̃J�E���g�ϐ�
	int m_pauseFlag;						//�|�[�Y���邩�ǂ����̃t���O�ϐ�
	int PinchId;

	bool m_hitFlag;							//�������Ă��邩�ǂ����̃t���O
	bool m_sensorFlag;						//�Z���T�[�ɓ����������ǂ����̃t���O
	bool m_Gyro_Sensor_Flag;				//�ŏ��̌X�����ǂ����𒲂ׂ邽�߂̃t���O

	b2Vec2 m_Gravity;						// �������[���h�̏d�͂�ݒ�
	b2Vec2 m_First_gravity; 				// �n�߂̌X����ۑ����Ă����ϐ�

	cocos2d::Size m_visibleSize;			//�ڂɌ�����T�C�Y
	cocos2d::Vec2 m_origin;					//���_
	cocos2d::LabelTTF* m_label;				//�c�莞�Ԃ�\�����邽�߂̃��x���ϐ�
	cocos2d::ui::Button* pouseButton;		//�|�[�Y�{�^����UI�ϐ�
	cocos2d::RepeatForever* m_repeat;
	cocos2d::Sprite* m_AlarmSprite;


	Gimmick* m_gimmick;						//�M�~�b�N�̕ϐ�

	Color m_object_color;					//�F�̏���ۑ����Ă���

	Form m_object_Form;						//�`�̏���ۑ����Ă���

	Color_Textur_Create* m_Hole_UserDate;			//���̃��[�U�[�f�[�^��ۑ����Ă���
	Color_Textur_Create* m_Object_bodyUserDate;		//�I�u�W�F�̃��[�U�[�f�[�^��ۑ����Ă���
};


#endif // __PLAYSENE_H__
