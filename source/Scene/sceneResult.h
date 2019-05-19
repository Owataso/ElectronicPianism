#pragma once

#include "../Effect/CyberCircle.h"
#include "../Effect/KeyboardGuide.h"



/********************************************/
//	モードごとのリザルト委譲
/********************************************/
namespace Result
{
	//------------------------------------------------------
	//	定数
	//------------------------------------------------------
	enum class TYPE
	{
		FREE
	};

	//------------------------------------------------------
	//	基底
	//------------------------------------------------------
	class Base
	{
	public:
		Base() :m_bEnable(false){}
		virtual ~Base(){}
		virtual bool Update() = 0;
		virtual void Render() = 0;
		virtual void Enable(){ m_bEnable = true; }
		virtual TYPE GetType() = 0;
	protected:
		bool m_bEnable;	// 稼働状態(要は表示するかしないか)
	};

	//------------------------------------------------------
	//	フリーモードの時の動き
	//------------------------------------------------------
	class Free: public Base
	{
	public:
		Free(tdn2DObj *pWindow) :Base(), m_CursorAnim(0), m_pWindow(pWindow), m_pCursor(new tdn2DObj("DATA/UI/Other/cursor_frame2.png")), m_pKeyboardGuide(new KeyboardGuide(Vector2(200, 360), 3, 0.1f, 0.5f, 1.0f, 60))
		{
			m_CurrentCursorY = m_NextCursorY = 52 + m_iSelectCursor * 50.0f;
		}
		bool Update();
		void Render();
		void Enable();
		TYPE GetType(){ return TYPE::FREE; }
	private:
		enum class SELECT
		{
			CONTINUE,		// もう一回
			MUSIC_SELECT,	// 曲セレクト
			EXIT,			// モード選択に戻る
			MAX,
		};
		static int m_iSelectCursor;
		int m_CursorAnim;
		float m_CurrentCursorY, m_NextCursorY;	// カーソル補間
		std::unique_ptr<tdn2DObj> m_pCursor;
		tdn2DObj *m_pWindow;	// 参照するだけなので生ポインタ(スマートポインタにしたら落ちた)

		// ガイド
		std::unique_ptr<KeyboardGuide> m_pKeyboardGuide;
	};

}


/********************************************/
//	リザルトシーン
/********************************************/
class sceneResult : public BaseScene
{
public:

	//------------------------------------------------------
	//	初期化・開放
	//------------------------------------------------------
	sceneResult(bool bClear, bool bPractice = false);
	bool Initialize();
	~sceneResult();

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
		NUMBER_L,
		UI_MAP,
		MUSIC_NAME,	// 曲名
		DIF,		// 難易度
		MAX
	};
	enum ANIM_IMAGE
	{
		FRAME1,	// 上フレーム
		FRAME2,	// 左下フレーム
		FRAME3,	// 右下フレーム
		FULLCOMBO,
		NEW_RECORD,
		RANK,	// ランク
		RANK_RIPPLE,	// ランクリップル
		ANIM_MAX,
	};

	//------------------------------------------------------
	//	メンバ変数
	//------------------------------------------------------
	int m_iStep;			// ステップ
	const int m_iRank;		// ランク
	const bool m_bCleared;	// クリアしたか
	const int m_iScore;		// スコア
	const int m_iHiScore;	// ハイスコア
	const int m_combo;		// コンボ
	const int m_notes;		// トータルノーツ
	int m_iJudgeCount[4];	// 配列だからイニシャライザで全部初期化できない！！！ううううううくやしいいいいいいいいいいいぃ”！！！！！！
	bool m_bPractice;		// 練習フラグ
	bool m_bNewRecord;		// スコア記録更新
	bool m_bFullCombo;		// フルコンしたか
	Result::Base *m_pResult;// 委譲ポインタ
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];				// 画像
	std::unique_ptr<tdn2DAnim> m_pAnimImages[ANIM_IMAGE::ANIM_MAX];	// アニメ―ション画像
	std::unique_ptr<CyberCircle> m_pCircle;							// ランクの下に置く円エフェクト
	// スコアとして描画するようの数字
	int m_iRenderScore, 
		m_iRenderCombo, 
		m_iRenderJudgeCount[4];

	//------------------------------------------------------
	//	メンバ関数
	//------------------------------------------------------
	void RenovationRecord();	// プレイヤーデータ更新
};


struct GradeData;

// 段位の総合リザルト
class sceneGradeResult : public BaseScene
{
public:
	//------------------------------------------------------
	//	初期化と開放
	//------------------------------------------------------
	sceneGradeResult(bool bPass);
	bool Initialize();
	~sceneGradeResult();

	//------------------------------------------------------
	//	更新と描画
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
		RESULT,
		IMG_GRADE,
		UP_FRAME,
		MIDASHI,
		FRAME,
		NUMBER,
		NUMBER_L,
		MAX
	};

	//------------------------------------------------------
	//	メンバ変数
	//------------------------------------------------------
	bool m_bPassed;										// クリアフラグ
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];	// 画像
	GradeData *m_pGradeData;							// モード管理さんのを参照する
	UINT m_Achievement;									// 総合達成率
	tdnSoundSE *m_pSE;									// リザルトのリアクション(ここでしか使わんやろ)


	//------------------------------------------------------
	//	メンバ関数
	//------------------------------------------------------
	void RenovationRecord();	// プレイヤーデータ更新
};