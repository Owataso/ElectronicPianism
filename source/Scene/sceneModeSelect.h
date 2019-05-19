#pragma once

#include "../Effect/KeyboardGuide.h"

class tdnMovie;



/********************************************/
//	選択したモード毎
/********************************************/
namespace SelectMode
{
	//------------------------------------------------------
	//	基底
	//------------------------------------------------------
	class Base
	{
	public:
		~Base();
		//virtual void Initialize() = 0;	// 全員強制で、m_pNextSceneの実体をnewする。そしてモードのコメントを設定する	※リターンでnewを返すようにしたので必要なくなった

		// ゲッター
		virtual BaseScene *GetNextScene() = 0;	// 各それぞれnewのシーンを返す
		//const char *GetDescription();
	protected:
		//std::string m_ModeDescription;	// モードの説明文
	};

	//------------------------------------------------------
	//	スタンダード
	//------------------------------------------------------
	class Standard : public Base
	{
	public:
		Standard();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// 段位認定
	//------------------------------------------------------
	class Grade : public Base
	{
	public:
		Grade();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// フリー
	//------------------------------------------------------
	class Free : public Base
	{
	public:
		Free();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// チュートリアル
	//------------------------------------------------------
	class Tutorial : public Base
	{
	public:
		Tutorial();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// スキンカスタマイズ
	//------------------------------------------------------
	class Customize: public Base
	{
	public:
		Customize();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// オプション
	//------------------------------------------------------
	class Option : public Base
	{
	public:
		Option();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// プレイヤーの記録
	//------------------------------------------------------
	class Record : public Base
	{
	public:
		Record();
		BaseScene *GetNextScene();
	};
}



/********************************************/
//	モードセレクトシーン
/********************************************/
class sceneModeSelect : public BaseScene
{
public:
	//------------------------------------------------------
	//	初期化・開放
	//------------------------------------------------------
	bool Initialize();
	~sceneModeSelect();

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	bool Update();
	void Render();

private:
	//------------------------------------------------------
	//	定数
	//------------------------------------------------------
	enum class SELECT_MODE
	{
		TUTORIAL,	// チュートリアル
		STANDARD,	// アーケード
		GRADE,		// 段位認定
		FREE,		// フリーモード
		CUSTMIZE,	// スキンカスタマイズ
		RECORD,		// プレイ記録
		OPTION,		// オプション
		MAX
	};
	
	//------------------------------------------------------
	//	画像ID
	//------------------------------------------------------
	enum class IMAGE
	{
		BACK,	// 背景	
		MODE,	// 7つのモードの画像
		MAX
	};
	enum class ANIM_IMAGE
	{
		MODE1,
		MODE2, 
		MODE3,
		MODE4,
		MODE5,
		MODE6,
		MODE7,
		MIDASHI,
		FRAME, 
		MODE_STR,
		UP_FRAME,
		DESCRIPTION,
		MAX
	};

	//------------------------------------------------------
	//	メンバ変数
	//------------------------------------------------------
	static int m_ModeCursor;												// モード選択カーソル(++とかするので、int型にする)
	int m_WaitTimer;														// このシーンに入ってからアニメ終わるまでのだいたいの待機時間
	std::unique_ptr<SelectMode::Base> m_pSelectMode[(int)SELECT_MODE::MAX];	// 選ぶモードのポインタ
	std::unique_ptr<tdn2DObj> m_pImages[(int)IMAGE::MAX];					// tdn画像たち
	std::unique_ptr<tdn2DAnim> m_pAnimImages[(int)ANIM_IMAGE::MAX];			// アニメーション画像たち
	std::unique_ptr<KeyboardGuide> m_pKeyboardGuide;						// ガイド
	tdnMovie *m_pMovie;														// 動画
};