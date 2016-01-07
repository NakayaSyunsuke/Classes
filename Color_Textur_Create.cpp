#include "Color_Textur_Create.h"

USING_NS_CC;

#define BIT_SQUARE		0x0002
#define BIT_TRIANGLE	0x0004
#define BIT_BALL		0x0008


//------------------------------------------------------------
//	�@�\	:	�V�[���쐬
//
//	����	:	�F�̏��,�I�u�W�F�̌`,�I�u�W�F�������̃t���O
//
//	�߂�l	:	�Ȃ�
//------------------------------------------------------------
Color_Textur_Create* Color_Textur_Create::create(int color, int type, bool objectflag)
{
	Color_Textur_Create *pRet = new Color_Textur_Create();
	if (pRet && pRet->init(color, type,objectflag))
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

//------------------------------------------------------------
//	�@�\	:	������
//
//	����	:	�F�̏��@,�@�I�u�W�F�̌`�@,�@�I�u�W�F�������̃t���O
//
//	�߂�l	:	�^�U
//------------------------------------------------------------
bool Color_Textur_Create::init(int color, int type, bool objectflag)
{
	Sprite::init();

	//�F���Ԃ̎��@�Ɓ@�`���{�[���Ȃ�΁@�ԐF�ƁuRedCircle.png�v���Z�b�g���鏈��
	if (color == Color_Red&&type == BIT_BALL)
	{
		Set_Color_Textur(Color_Red, Form_maru, "Textuer/RedCircle.png");
	}
	//�F���Ԃ̎��@�Ɓ@�`���l�p�`�Ȃ�΁@�ԐF�ƁuRedBlock.png�v���Z�b�g���鏈��
	if (color == Color_Red&&type == BIT_SQUARE)
	{
		Set_Color_Textur(Color_Red, Form_sikaku, "Textuer/RedBlock.png");
	}

	//�F���Ԃ̎��@�Ɓ@�`���l�p�`�Ȃ�΁@�ԐF�ƁuRedBlock.png�v���Z�b�g���鏈��
	if (color == Color_Red&&type == BIT_TRIANGLE)
	{
		Set_Color_Textur(Color_Red, Form_sankaku, "Textuer/RedTriangle.png");
	}

	//�F���̎��@�Ɓ@�`���{�[���Ȃ�΁@�F�ƁuBlueCircle.png�v���Z�b�g���鏈��
	if (color == Color_Blue&&type == BIT_BALL)
	{
		Set_Color_Textur(Color_Blue, Form_maru, "Textuer/BlueCircle.png");
	}

	//�F���̎��@�Ɓ@�`���l�p�`�Ȃ�΁@�F�ƁuBlueBlock.png�v���Z�b�g���鏈��
	if (color == Color_Blue&&type == BIT_SQUARE)
	{
		Set_Color_Textur(Color_Blue, Form_sikaku, "Textuer/BlueBlock.png");
	}

	//�F���̎��@�Ɓ@�`���O�p�`�Ȃ�΁@�F�ƁuBlueTriangle.png�v���Z�b�g���鏈��
	if (color == Color_Blue&&type == BIT_TRIANGLE)
	{
		Set_Color_Textur(Color_Blue, Form_sankaku, "Textuer/BlueTriangle.png");
	}

	//�F�����̎��@�Ɓ@�`���{�[���Ȃ�΁@���F�ƁuYellowCircle.png�v���Z�b�g���鏈��
	if (color == Color_Yellow&&type == BIT_BALL)
	{
		Set_Color_Textur(Color_Yellow, Form_maru, "Textuer/YellowCircle.png");
	}

	//�F�����̎��@�Ɓ@�`���l�p�`�Ȃ�΁@���F�ƁuYellowBlock.png�v���Z�b�g���鏈��
	if (color == Color_Yellow&&type == BIT_SQUARE)
	{
		Set_Color_Textur(Color_Yellow, Form_sikaku, "Textuer/YellowBlock.png");
	}

	//�F�����̎��@�Ɓ@�`���O�p�`�Ȃ�΁@���F�ƁuYellowTriangle.png�v���Z�b�g���鏈��
	if (color == Color_Yellow&&type == BIT_TRIANGLE)
	{
		Set_Color_Textur(Color_Yellow, Form_sankaku, "Textuer/YellowTriangle.png");
	}

	//�t���O�����Ȃ�΃e�N�X�`���𔼕��̑傫���ɂ���B
	if (objectflag==GOOL)
	{
		this->setScale(0.5f);
	}
	
	return true;
}

//------------------------------------------------------------
//	�@�\	:	�`�ƐF���Z�b�g����֐�
//
//	����	:	�F�̏��@,�@�I�u�W�F�̌`�@,�@�e�N�X�`���̖��O
//
//	�߂�l	:	�Ȃ�
//------------------------------------------------------------
void Color_Textur_Create::Set_Color_Textur(Color color, Form form, std::string textuerName)
{
	m_Color = color;				//�F�̃Z�b�g

	m_Form = form;					//�`���Z�b�g

	this->setTexture(textuerName);	//�e�N�X�`���̖��O���Z�b�g
}

//------------------------------------------------------------
//	�@�\	:	�F�̃Q�b�g�֐�
//
//	����	:	�Ȃ�
//
//	�߂�l	:	�F�̏��
//------------------------------------------------------------
Color Color_Textur_Create::GetColor()
{
	return m_Color;
}


//------------------------------------------------------------
//	�@�\	:	�`�̃Q�b�g�֐�
//
//	����	:	�Ȃ�
//
//	�߂�l	:	�`�̏��
//------------------------------------------------------------
Form Color_Textur_Create::GetForm()
{
	return m_Form;
}
