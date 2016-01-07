#ifndef __COLOR_TEXYUR_CREATE_H__
#define __COLOR_TEXYUR_CREATE_H__

#include "cocos2d.h"

#include "GLES-Render.h"


enum  Color
{
	Color_Red,
	Color_Blue,
	Color_Yellow
};

enum Form
{
	Form_maru,
	Form_sankaku,
	Form_sikaku
};

class Color_Textur_Create : public cocos2d::Sprite
{
public:
	//定数
	static const bool GOOL = true;					
	static const bool COLOR_TEXYUR_CREATE = false;	

	//--------------------------------------------
	//! @brief シーンの作成
	//! @param[int]	 color
	//! @param[int]	 type
	//! @param[bool] objectflag
	//--------------------------------------------
	static Color_Textur_Create* create(int color,int type,bool objectflag);

	//--------------------------------------------
	//! @brief 初期化処理
	//! @param[int]	 color
	//! @param[int]	 type
	//! @param[bool] objectflag
	//--------------------------------------------
	bool init(int color, int type,bool objectflag);

	//--------------------------------------------
	//! @brief 形と色をセットする関数
	//! @param[Color]	color
	//! @param[Form]	form
	//! @param[string]	st
	//--------------------------------------------
	void Set_Color_Textur(Color color, Form form, const std::string textuerName);

	//--------------------------------------------
	//! @brief 色と形のGet関数
	//--------------------------------------------
	Color GetColor();
	Form GetForm();

private:
	
	Color m_Color;	//色の情報
	Form m_Form;	//形の情報
};


#endif // __COLOR_TEXYUR_CREATE_SCENE_H__