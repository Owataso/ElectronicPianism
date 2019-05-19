#pragma once

//------------------------------------------------------
//	前方宣言
//------------------------------------------------------
struct Note;
struct Soflan;
struct MusicInfo;
class NoteManager;
class SoflanManager;
enum class JUDGE_TYPE;
enum class GAUGE_OPTION;
struct GameScore;
class BeatBar;


/********************************************/
//	演奏画面のUI管理クラス
/********************************************/
class UI
{
public:
	//------------------------------------------------------
	//	コンストラクタ
	//------------------------------------------------------
	UI(char OctaveWidth);

	//------------------------------------------------------
	//	初期化
	//------------------------------------------------------
	void Initialize(BeatBar *pBeatBar, NoteManager *pNoteMgr, SoflanManager *pSoflanMgr, GAUGE_OPTION GaugeOP);

	//------------------------------------------------------
	//	描画
	//------------------------------------------------------
	void Render();
	void RenderBack();	// 背景描画(UIで動画を扱わないために分けた)
	void RenderMusicName(char *name);


	//------------------------------------------------------
	//	アクセサ
	//------------------------------------------------------
	void SetBeam(char no, bool on);
	void SetBeam(bool list[]);
	void SetOctaveWidth(char width){ m_cOctaveWidth = width; }

private:
	//------------------------------------------------------
	//	画像のID
	//------------------------------------------------------
	enum IMAGE
	{
		BACK,
		GUIDE,
		NOTE_GUIDE,
		SKIN,
		STAGE,
		NOTE,
		BEAM,
		SUDDEN,
		NUMBER,
		DIFFICULTY,
		GAUGE,
		JUDGE_BAR,
		ON_LIGHT,	// 鍵盤を押して青く光る画像
		MAX
	};

	//------------------------------------------------------
	//	メンバ変数
	//------------------------------------------------------
	int m_iGaugeSrcY;				// ゲージのとってくる座標(画像節約)
	char m_cOctaveWidth;			// この曲の演奏オクターブ数
	bool m_bBeam[NUM_KEYBOARD];		// ビーム
	NoteManager *m_pNoteMgr;		// 参照するだけNote
	SoflanManager *m_pSoflanMgr;	// 参照するだけSoflan
	BeatBar *m_pBeatBar;			// ドゥンドゥン
	GameScore *m_pGameScore;		// 参照するだけScore
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];	// 画像実体

	//------------------------------------------------------
	//	メンバ関数
	//------------------------------------------------------
	void InfoRender();		// 基本的な情報の描画
	void SuddenRender();	// サドプラの描画
	void OptionRender();	// スペースキー押されたら呼ぶ。ハイスピ、サドプラの数字とかの表示
	void NoteRender();		// ノーツ描画
	void BeamRender();		// 打鍵ビーム描画
	void PushRender();		// ごみくそ
	void GuideNoteRender(int x, int y, int lane);	// ノーツのガイド描画
	BOOL isBlackKey(int lane);	// 黒鍵かどうか
};
