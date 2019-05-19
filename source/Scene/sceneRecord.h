#pragma once

#include "../Data/PlayerData.h"

// 前方宣言
class AlphabetRenderer;



/********************************************/
//	記録閲覧シーン
/********************************************/
class sceneRecord : public BaseScene
{
public:
	//------------------------------------------------------
	//	初期化・開放
	//------------------------------------------------------
	bool Initialize();
	~sceneRecord();

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	bool Update();
	void Render();

private:
	//------------------------------------------------------
	//	画像ID
	//------------------------------------------------------
	enum IMAGE
	{
		BACK,
		UP_FRAME,
		MIDASHI,
		NUMBER,
		GRADE_TEXT,
		ALPHABET,
		MAX
	};

	//------------------------------------------------------
	//	メンバ変数
	//------------------------------------------------------
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];	// 画像
	AlphabetRenderer *m_pAlphabetRenderer;				// プレイヤーの名前描画用
	PlayerData::Infomation *m_pPlayerInfo;				// プレイヤーのプレイ情報
	GRADE m_grade;										// プレイヤーが所持している段位
};