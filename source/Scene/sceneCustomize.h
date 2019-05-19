#pragma once

class sceneCustomize;

/********************************************/
//	カスタマイズ設定委譲
/********************************************/
namespace SelectCustomize
{
	//===============================================
	//	基底
	//===============================================
	class Base
	{
	protected:
		static const Vector2 FRAME_OFFSET_XY;	// 右上枠の開始座標
		tdnText2 *m_pText;
		Vector2 m_CurrentCursorPos, m_NextCursorPos;	// カーソル枠の補間で使う
		
		sceneCustomize *m_pCustomize;	// 委譲元
		int m_iCursor;
		int m_CursorAnimFrame;
		const int m_NumCustomize;	// カスタマイズの数
		const int m_iCustomizeType;	// カスタマイズの種類
		bool UpdateCursor();		// カーソルしたらtrue,何もしないフレームならfalse
		void RenderCursor();		// カーソル描画
	public:
		Base(sceneCustomize *pCustomize, int iCustomizeType);
		virtual void Initialize();
		virtual ~Base();
		virtual void Release();
		virtual void Update(){ UpdateCursor(); }
		virtual void Render() = 0;
	};

	//===============================================
	//	スキン変更
	//===============================================
	class Skin :public Base
	{
	public:
		Skin(sceneCustomize *pCustomize);
		void Render();
	};

	//===============================================
	//	ノーツ変更
	//===============================================
	class Note :public Base
	{
	public:
		Note(sceneCustomize *pCustomize);
		void Render();
	};

	//===============================================
	//	ボム変更
	//===============================================
	class Explosion :public Base
	{
	public:
		Explosion(sceneCustomize *pCustomize);
		void Render();
	};

	//===============================================
	//	サドプラ変更
	//===============================================
	class Sudden :public Base
	{
	public:
		Sudden(sceneCustomize *pCustomize);
		void Initialize();
		//~Sudden();
		void Sudden::Release();
		void Render();
	};

	//===============================================
	//	選曲BGM変更
	//===============================================
	class SelectMusic :public Base
	{
	public:
		SelectMusic(sceneCustomize *pCustomize);
		void Initialize();
		//~SelectMusic(){ }
		void Release();
		void Update();
		void Render();
	private:
		tdnStreamSound *m_pStreamSound;	// 選曲BGM用ストリーミング
	};
}


/********************************************/
//	カスタマイズのプレビュー担当
/********************************************/
class CustomizeTestPlayer
{
public:
	//===============================================
	//	コンストラクタ・デストラクタ
	//===============================================
	CustomizeTestPlayer(sceneCustomize *pCustomize) :m_pCustomize(pCustomize){ Reset(); }
	~CustomizeTestPlayer(){ Clear(); }

	//===============================================
	//	更新・描画
	//===============================================
	void Update();
	void Render();

	//===============================================
	//	初期化
	//===============================================
	void Clear();
	void Reset();

	int m_NumExplosionPanel;
private:

	/********************************************/
	//	落ちてくるノーツ
	/********************************************/
	class Note
	{
	public:
		Note(int x, bool bBlack) : posY(133), posX(x), ExplosionFrame(0), bNoteEnd(false), bBlack(bBlack) {}
		bool Update(int NumExplosionPanel);
		void Render(tdn2DObj *pNote, tdn2DObj *pExplosion);
		void Reset() { ExplosionFrame = 0; }
	private:
		int posY;
		const int posX;
		int ExplosionFrame;	// 爆発エフェクトフレーム
		bool bNoteEnd;		// ノーツ落ち終わって、爆発中
		const bool bBlack;		// 黒鍵フラク
	};

	//===============================================
	//	メンバ変数
	//===============================================
	sceneCustomize *m_pCustomize;	// 委譲元
	std::list<Note*> m_NoteList;	// 落ちてるノーツのリスト
};



/********************************************/
//	カスタマイズシーン
/********************************************/
class	sceneCustomize : public BaseScene
{
public:
	//------------------------------------------------------
	//	初期化・開放
	//------------------------------------------------------
	bool Initialize();
	~sceneCustomize();

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	bool Update();
	void Render();

	//===============================================
	//	画像ID
	//===============================================
	enum IMAGE
	{
		BACK,
		KEYLINE,
		UP_FRAME,
		MIDASHI,
		LEFT_FRAME,
		RIGHT_FRAME,
		RIGHT_UP_FRAME,
		CURSOR_FRAME,
		CURSOR_FRAME_S,
		MUSIC,
		MAX
	};

	//===============================================
	//	メンバ変数
	//===============================================
	UINT *m_NumCustomizes;			// 各カスタマイズの数(可変配列ポインタにする。なぜ配列で宣言しないかは、ヘッダインクルードを防止するため)
	int m_iSelectCustomizeType;		// 選択するカスタマイズのタイプ
	int *m_iSelectCustomizes;		// カスタマイズごとの選択してるやつ
	float m_fNextSudden;			// サドゥン割合(サドゥン選択したら1にして最大にする)
	bool m_bSelect;					// 種類選んだか
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];			// カスタマイズシーンの基本画像
	CustomizeTestPlayer *m_pCustomizeTest;
	tdn2DObj **m_pSkins;			// 背景のスキンたち
	tdn2DObj **m_pExplosions;		// 爆発エフェクトたち
	tdn2DObj **m_pNotes;			// ノーツ画像たち
	tdn2DObj **m_pSuddens;			// サドゥンたち
	tdnSoundSE *m_pSE;				// あんまり来ることなさそうなので、ここ用のSEを随時確保解放
	void Close()
	{
		//SAFE_DELETE(m_pSelectCustomize); 
		m_pSelectCustomizes[m_iSelectCustomizeType]->Release();
	}

private:
	//===============================================
	//	メンバ変数
	//===============================================
	float m_fSuddenPercent;							// サドゥン割合
	float m_CurrentCursorPosY, m_NextCursorPosY;	// カーソル枠の補間で使う
	SelectCustomize::Base **m_pSelectCustomizes;	// [CUSTOMIZE_TYPE::MAX];
	std::unique_ptr<tdn2DObj> m_pPlayScreen;		// プレイ画面スクリーン
	Surface *m_pSurface;

};
