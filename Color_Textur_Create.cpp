#include "Color_Textur_Create.h"

USING_NS_CC;

#define BIT_SQUARE		0x0002
#define BIT_TRIANGLE	0x0004
#define BIT_BALL		0x0008


//------------------------------------------------------------
//	機能	:	シーン作成
//
//	引数	:	色の情報,オブジェの形,オブジェか穴かのフラグ
//
//	戻り値	:	なし
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
//	機能	:	初期化
//
//	引数	:	色の情報　,　オブジェの形　,　オブジェか穴かのフラグ
//
//	戻り値	:	真偽
//------------------------------------------------------------
bool Color_Textur_Create::init(int color, int type, bool objectflag)
{
	Sprite::init();

	//色が赤の時　と　形がボールならば　赤色と「RedCircle.png」をセットする処理
	if (color == Color_Red&&type == BIT_BALL)
	{
		Set_Color_Textur(Color_Red, Form_maru, "Textuer/RedCircle.png");
	}
	//色が赤の時　と　形が四角形ならば　赤色と「RedBlock.png」をセットする処理
	if (color == Color_Red&&type == BIT_SQUARE)
	{
		Set_Color_Textur(Color_Red, Form_sikaku, "Textuer/RedBlock.png");
	}

	//色が赤の時　と　形が四角形ならば　赤色と「RedBlock.png」をセットする処理
	if (color == Color_Red&&type == BIT_TRIANGLE)
	{
		Set_Color_Textur(Color_Red, Form_sankaku, "Textuer/RedTriangle.png");
	}

	//色が青の時　と　形がボールならば　青色と「BlueCircle.png」をセットする処理
	if (color == Color_Blue&&type == BIT_BALL)
	{
		Set_Color_Textur(Color_Blue, Form_maru, "Textuer/BlueCircle.png");
	}

	//色が青の時　と　形が四角形ならば　青色と「BlueBlock.png」をセットする処理
	if (color == Color_Blue&&type == BIT_SQUARE)
	{
		Set_Color_Textur(Color_Blue, Form_sikaku, "Textuer/BlueBlock.png");
	}

	//色が青の時　と　形が三角形ならば　青色と「BlueTriangle.png」をセットする処理
	if (color == Color_Blue&&type == BIT_TRIANGLE)
	{
		Set_Color_Textur(Color_Blue, Form_sankaku, "Textuer/BlueTriangle.png");
	}

	//色が黄の時　と　形がボールならば　黄色と「YellowCircle.png」をセットする処理
	if (color == Color_Yellow&&type == BIT_BALL)
	{
		Set_Color_Textur(Color_Yellow, Form_maru, "Textuer/YellowCircle.png");
	}

	//色が黄の時　と　形が四角形ならば　黄色と「YellowBlock.png」をセットする処理
	if (color == Color_Yellow&&type == BIT_SQUARE)
	{
		Set_Color_Textur(Color_Yellow, Form_sikaku, "Textuer/YellowBlock.png");
	}

	//色が黄の時　と　形が三角形ならば　黄色と「YellowTriangle.png」をセットする処理
	if (color == Color_Yellow&&type == BIT_TRIANGLE)
	{
		Set_Color_Textur(Color_Yellow, Form_sankaku, "Textuer/YellowTriangle.png");
	}

	//フラグが穴ならばテクスチャを半分の大きさにする。
	if (objectflag==GOOL)
	{
		this->setScale(0.5f);
	}
	
	return true;
}

//------------------------------------------------------------
//	機能	:	形と色をセットする関数
//
//	引数	:	色の情報　,　オブジェの形　,　テクスチャの名前
//
//	戻り値	:	なし
//------------------------------------------------------------
void Color_Textur_Create::Set_Color_Textur(Color color, Form form, std::string textuerName)
{
	m_Color = color;				//色のセット

	m_Form = form;					//形をセット

	this->setTexture(textuerName);	//テクスチャの名前をセット
}

//------------------------------------------------------------
//	機能	:	色のゲット関数
//
//	引数	:	なし
//
//	戻り値	:	色の情報
//------------------------------------------------------------
Color Color_Textur_Create::GetColor()
{
	return m_Color;
}


//------------------------------------------------------------
//	機能	:	形のゲット関数
//
//	引数	:	なし
//
//	戻り値	:	形の情報
//------------------------------------------------------------
Form Color_Textur_Create::GetForm()
{
	return m_Form;
}
