#pragma once

#include "TDNLIB.h"

/********************************************/
//	定数や構造体などの情報の倉庫
/********************************************/


/********************************************/
//	定数
/********************************************/
static const int MAX_MUSIC = 127;	// 曲の最大数
static const int MAX_OCTAVE = 4;
static const int JUDGE_LINE = 618;	// 判定Y座標
static const int NUM_KEYBOARD = 12 * MAX_OCTAVE;
static const float HI_SPEED_WIDTH = .5f;	// ハイスピの刻み幅
static const int NOTE_WIDTH = 40;		// ノーツの幅
static const int KEYLANE_LEFT = 0;		// キーライン左
static const int KEYLANE_TOP = 116;		// キーライン上
static const int KEYLANE_HEIGHT = 497;	// 落ちてくるやつの上下幅
static const int MAX_SCORE = 100000;	// スコアの最大数


/********************************************/
//	ノーツ座標計算(横)
/********************************************/
#define NOTE_POS_COMPTE(key) (key*(NOTE_WIDTH / 2) + ((key % 12 >= 5) ? (NOTE_WIDTH / 2) : 0) + key / 12 * NOTE_WIDTH)

//------------------------------------------------------
//	スコア
//------------------------------------------------------
enum class SCORE_RANK
{
	AAA, AA, A, B, C, D, E, F, MAX
};
static const int SCORE_BORDER[(int)SCORE_RANK::MAX] =
{
	90000,	// AAA
	80000,	// AA
	70000,	// A
	60000,	// B
	50000,	// C
	40000,	// D
	30000,	// E
	// Fはそれ以下
};
SCORE_RANK ScoreRankCheck(int score);// 引数のスコアから、そのスコアのランクを吐き出す


//------------------------------------------------------
//	難易度
//------------------------------------------------------
enum class DIFFICULTY
{
	NORMAL,		// まだいける
	HYPER,		// 結構歯ごたえある
	EXPERT,		// めっちゃむずい
	VIRTUOSO,	// 超絶技巧(クリアしたら立派なピアニストです)
	MAX
};
static const DWORD DifficultyColor[(int)DIFFICULTY::MAX] =
{
	0xff10ff10,	// 緑
	0xffffff10,	// 黄
	0xffff1010,	// 赤
	0xffca10ca	// 紫
};

//------------------------------------------------------
//	クリアランプ
//------------------------------------------------------
enum class CLEAR_LAMP
{
	NO_PLAY,	// 未プレイ
	FAILED,		// 未クリア
	EASY,		// イージー
	CLEAR,		// ノマゲ
	HARD,		// 難
	EX_HARD,	// EX難
	FULL_COMBO,	// フル婚
	MAX
};


//------------------------------------------------------
//	ゲージオプション
//------------------------------------------------------
enum class GAUGE_OPTION
{
	NORMAL,
	EASY,
	HARD,
	EX_HARD,
	HAZARD,
	GRADE,
	MAX
};

//------------------------------------------------------
//	判定
//------------------------------------------------------
enum class JUDGE_TYPE
{
	SUPER,	// 最高
	GREAT,	// 良い
	GOOD,	// ふつう
	BAD,	// 悪い
	MAX
};
enum class JUDGE_TIMING
{
	JUST,
	FAST,
	SLOW,
	MAX
};


//------------------------------------------------------
//	ジャンル
//------------------------------------------------------
enum class MUSIC_GENRE
{
	ORIGINAL,	// オリジナル
	GAME,		// ゲーム音楽
	CLASSIC,	// クラシック
	VOCALOID,	// ボカロ
	POPS,		// アニメとか版権の類
	OTHER,		// その他
	MAX
};


//------------------------------------------------------
//	ノーツ
//------------------------------------------------------
enum class NOTE_TYPE
{
	NORMAL,		// CNなし(通常ノーツ)
	CN_START,	// CN開始
	CN_END,		// CN終了
};
struct NoteInfo
{
	UINT iEventTime;		// イベント時間
	char cLaneNo;			// レーン番号
	char cNoteType;			// ノーツタイプを格納(上のenumを格納する)	// char型にしたのは、前の使ってた情報(tone)を使いまわせるからという無理やり
};

// 速度変化
struct SoflanInfo
{
	UINT iEventTime;
	float fBPM;
};

// フォルダーの情報基底(エディット用と通常用の別で分ける)
class FolderBase
{
public:
	std::string sMusicName;				// 曲名(フォルダーの名前でもある)
	int level[(int)DIFFICULTY::MAX];	// レベル

	FolderBase() :sMusicName(""){}
	virtual ~FolderBase(){}
};


class FolderInfo : public FolderBase
{
public:
	bool bNameEnglish;								// 曲名が英語かどうか
	bool bMovieExist;								// 動画があるかどうか
	int iOctaveWidths[(int)DIFFICULTY::MAX];			// オクターブ幅
	std::string sSortMusicName;						// ソート用のジャンル名
	float fMinBPM, fMaxBPM;	// 選曲情報でBPM100~200とかのソフランで使う

	BYTE byID;				// ID(0～255)
	std::string sArtistName;	// 作曲者名
	MUSIC_GENRE eMusicGenre;	// ジャンル

	FolderInfo() :FolderBase(), sArtistName(""), bNameEnglish(true){}
};

// エディット用のフォルダーデータ
class EditFolderInfo : public FolderBase
{
public:
	bool bMusicExistance[(int)DIFFICULTY::MAX];			// 曲が入ってるか(基本的に0番だけの利用となるが、Anotherで曲変わるとかやりたいので難易度ぶん保持する)ただ本当に基本的には0にだけtrueを入れる
	bool bAudition;										// 試聴用のwavデータが入ってるか
	bool bDifficultyExistance[(int)DIFFICULTY::MAX];	// 難易度が存在しているか

	EditFolderInfo(){
		assert(0);// デフォルトコンストラクタは呼び出さないで
	}
	EditFolderInfo(LPCSTR FileName);
};

// ファイル読み書き用おわたそ譜面構造体(ビットフィールド)
struct OMSInfo
{
	//unsigned int level		 : 5;	// 曲のレベル 5ビット(31)
	unsigned int PlayMusicNo : 3;	// フォルダの中のどの番号の曲を再生するか(1～4の値、0なら曲なし) 3ビット(7)
	unsigned int PlayTime	 : 10;	// 再生時間(秒単位、曲がある場合不要) 10ビット(1023) 17分5秒
	unsigned int NumNotes	 : 14;	// ノート数 14ビット(16383)
	unsigned int NumSoflans	 : 10;	// ソフラン回数 10ビット(1023)
	signed int shift		 : 16;	// 曲と譜面の差を調整する用 符号あり16ビット(-32768 ～ 32767)
	unsigned int division	 : 16;	// 分能値 16ビット(65535)
	unsigned int BaseOctave	 : 7;	// 開始オクターブ 7ビット(127)
};

struct OMSInfoV2
{
	unsigned int PlayMusicNo : 3;	// フォルダの中のどの番号の曲を再生するか(1～4の値、0なら曲なし) 3ビット(7)
	unsigned int PlayTime : 10;	// 再生時間(秒単位、曲がある場合不要) 10ビット(1023) 17分5秒
	unsigned int NumNotes : 14;	// ノート数 14ビット(16383)
	unsigned int NumSoflans : 10;	// ソフラン回数 10ビット(1023)
	signed int shift : 16;	// 曲と譜面の差を調整する用 符号あり16ビット(-32768 ～ 32767)
	unsigned int division : 16;	// 分能値 16ビット(65535)
	unsigned int BaseOctave : 7;	// 開始オクターブ 7ビット(127)
	unsigned int OctaveWidth : 3;	// オクターブ幅(1～4) 3ビット(7)
	unsigned int Other1 : 16;		// 今後の追加を想定した空き枠
	unsigned int Other2 : 16;
	unsigned int Other3 : 16;
};

struct OMSInfoV3
{
	BYTE byPlayMusicNo;	// フォルダの中のどの番号の曲を再生するか
	WORD wNumNotes;		// ノーツ数
	WORD wNumSoflans;	// ソフラン回数
	WORD wDivision;		// 分能値
	int other1;			// 今後の追加を想定した空き枠
	int other2;
	int other3;
};

// 曲の情報
struct MusicInfo
{
	bool bMovieExist;			// 動画があるかどうか
	DIFFICULTY eDifficulty;
	BYTE byID;						// ID(0～255)選曲画面から移動時に値を保持したかったから

	OMSInfoV2 omsInfo;

	NoteInfo *notes;				// ノーツイベント(可変長)
	SoflanInfo *soflans;			// BPM変化イベント(可変長)
};