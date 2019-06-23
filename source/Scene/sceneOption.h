#pragma once

//------------------------------------------------------
//	前方宣言
//------------------------------------------------------
class sceneOption;
class tdnMovie;
namespace PlayerData
{
	struct OptionV2;
}

/********************************************/
//	オプション設定委譲
/********************************************/
namespace SelectOption
{
	//------------------------------------------------------
	//	基底
	//------------------------------------------------------
	class Base
	{
	protected:
		static const Vector2 FRAME_OFFSET_XY;	// 右上枠の開始座標
		tdnText2 *m_pText;
		Vector2 m_CurrentCursorPos, m_NextCursorPos;	// カーソル枠の補間で使う

		sceneOption *m_pOption;	// 委譲元
		int m_iCursor;
		int m_CursorAnimFrame;
		void RenderCursor();		// カーソル描画
		const int m_NumCursor;
		bool m_bSettingOption;	// オプションを選択して、設定をいじっている状態
		void UpdateCursor();
	public:
		Base(sceneOption *pOption, int NumCursor);
		virtual ~Base();
		virtual void Update() = 0;
		virtual void Render() = 0;
	};

	//------------------------------------------------------
	//	ハイスピード関連
	//------------------------------------------------------
	class HiSpeed :public Base
	{
	public:
		HiSpeed(sceneOption *pOption);
		void Update();
		void Render();
	private:
		const Mybool m_bOrgRegulSpeed;
		const int m_iOrgHiSpeed;
	};

	//------------------------------------------------------
	//	動画関連
	//------------------------------------------------------
	class Movie :public Base
	{
	public:
		Movie(sceneOption *pOption);
		void Update();
		void Render();
	private:
		const BYTE m_byOrgMovieAlpha;
		const Mybool m_bOrgMovieOn;

	};

	//------------------------------------------------------
	//	音関連
	//------------------------------------------------------
	class Sound :public Base
	{
	public:
		Sound(sceneOption *pOption);
		void Update();
		void Render();
	private:
		const Mybool m_bOrgMIDISoundOn;
	};

	//------------------------------------------------------
	//	ガイド関連
	//------------------------------------------------------
	class Guide :public Base
	{
	public:
		Guide(sceneOption *pOption);
		void Update();
		void Render();
	private:
		const Mybool m_bOrgGuide;
	};
}



/********************************************/
//	オプションプレビュー
/********************************************/
class OptionTestPlayer
{
public:
	//------------------------------------------------------
	//	コンストラクタ・デストラクタ
	//------------------------------------------------------
	OptionTestPlayer(sceneOption *pOption, tdn2DObj *pExplosion) :m_pOption(pOption), m_NumExplosionPanel(pExplosion->GetWidth() / 128), m_iStartClock(clock()){}
	~OptionTestPlayer(){ Clear(); }

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	void Update();
	void Render(tdn2DObj *pNote, tdn2DObj *pExplosion);

	//------------------------------------------------------
	//	初期化
	//------------------------------------------------------
	void Clear();

	int m_NumExplosionPanel;
private:

	/********************************************/
	//	落ちてくるノーツ
	/********************************************/
	class Note
	{
	public:
		Note(int x, float fBPM) : fBPM(fBPM), iEventTime(3000), iRestTime(3000), posX(x), ExplosionFrame(0), bNoteEnd(false) {}
		bool Update(int NumExplosionPanel);
		void Render(tdn2DObj *pNote, tdn2DObj *pExplosion);
		void Reset() { ExplosionFrame = 0; }
		int iEventTime;	// ノーツのイベント時間
		int iRestTime;
		int posY;			// ノーツのY座標
		const int posX;		// ノーツのX座標
		const float fBPM;	// ノーツのBPM
		int ExplosionFrame;	// 爆発エフェクトフレーム
		bool bNoteEnd;		// ノーツ落ち終わって、爆発中
	};
	//------------------------------------------------------
	//	メンバ変数
	//------------------------------------------------------
	clock_t m_iStartClock;			// ノーツ位置計算用
	sceneOption *m_pOption;			// 委譲元
	std::list<Note*> m_NoteList;	// ノーツリスト
};



/********************************************/
//	オプションシーン
/********************************************/
class	sceneOption : public BaseScene
{
public:
	//------------------------------------------------------
	//	初期化・開放
	//------------------------------------------------------
	bool Initialize();
	~sceneOption();

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	bool Update();
	void Render();


	//------------------------------------------------------
	//	定数
	//------------------------------------------------------
	enum class SELECT_OPTION
	{
		HI_SPEED,
		MOVIE,
		GUIDE,
		SOUND,
		MAX
	};

	//------------------------------------------------------
	//	画像ID
	//------------------------------------------------------
	enum class IMAGE
	{
		BACK,
		UP_FRAME,
		MIDASHI,
		LEFT_FRAME,
		RIGHT_FRAME,
		RIGHT_UP_FRAME,
		CURSOR_FRAME,
		CURSOR_FRAME_S,
		MOVIE,

		// テスト描画に必要なもの
		SKIN,
		NOTE,
		EXPLOSION,
		KEYLINE,
		MAX
	};

	void Close() { SAFE_DELETE(m_pSelectOption); }

	//------------------------------------------------------
	//	メンバ変数
	//------------------------------------------------------
	int m_iSelectOptionType;								// 選択するオプションのタイプ
	tdnMovie *m_pMovie;										// テスト動画
	PlayerData::OptionV2 *m_pPlayerOption;					// プレイヤーがセットするオプション(プレイヤーデータとは別で持つ)
	std::unique_ptr<tdn2DObj> m_pImages[(int)IMAGE::MAX];	// オプションシーンの基本画像
	std::unique_ptr<OptionTestPlayer> m_pOptionTest;		// プレビュー君
	std::unique_ptr<tdnSoundSE> m_pSE;						// あんまり来ることなさそうなので、ここ用のSEを随時確保解放
private:
	std::unique_ptr<tdn2DObj> m_pPlayScreen;				// プレイ画面スクリーン
	SelectOption::Base *m_pSelectOption;					// 設定委譲
	Surface *m_pSurface;
	float m_CurrentCursorPosY, m_NextCursorPosY;			// カーソル枠の補間で使う
};
