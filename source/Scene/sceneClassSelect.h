#pragma once


/********************************************/
//	段位セレクトシーン
/********************************************/
class sceneClassSelect : public BaseScene
{
public:

	//------------------------------------------------------
	//	初期化・開放
	//------------------------------------------------------
	bool Initialize();
	~sceneClassSelect();

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	bool Update();
	void Render();

private:
	//===============================================
	//	画像ID
	//===============================================
	enum class IMAGE
	{
		BACK,
		SELECT_FRAME,
		GRADE_TEXT,
		NUMBER,
		FRAME,
		FRAME_IN_TEXT,
		MAX
	};
	enum class ANIM_IMAGE
	{
		UP_FRAME,
		MIDASHI,
		GRADE_TEXT1,
		GRADE_TEXT2,
		SELECT_FRAME,
		PASS,
		SCORE_FRAME,
		MAX
	};

	//===============================================
	//	メンバ変数
	//===============================================
	int m_WaitTimer;			// このシーンに入ってからアニメ終わるまでのだいたいの待機時間
	int m_iCursor;				// 段位のカーソル
	std::unique_ptr<tdn2DObj>  m_pImages[(int)IMAGE::MAX];	// 画像
	std::unique_ptr<tdn2DAnim> m_pAnimImages[(int)ANIM_IMAGE::MAX];
	void GradeDataSetting();	// モードマネージャーさんにデータを送信する

	
	/********************************************/
	//	枠クラス
	/********************************************/
	class Frame
	{
	public:
		Frame(int cursor, int grade, int rank, Texture2D *pFrame, Texture2D *pText);
		void Update();
		void Render();
		void SetCursor(int cursor);	// カーソルしたら呼び出す。次の座標の更新
	private:
		Vector2 pos, NextPos;
		int grade, rank;				// grade(段位の番号) rank(段位の色的な何か)
		std::unique_ptr<tdn2DAnim> m_pFrame, m_pText;	// 枠とその中の画像
	}**m_pFrames;	// [GRADE::MAX - 1]


};