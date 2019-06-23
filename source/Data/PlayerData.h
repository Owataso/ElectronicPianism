#pragma once

struct MusicRecord;
enum class GAUGE_OPTION;


//===============================================
//	段位の定数
//===============================================
enum class GRADE
{
	NONE = -1,		// 何もない状態
	FIRST,		// 初段
	SECOND,		// 二段
	THERD,		// 三段
	FOURTH,		// 四段
	FIFTH,		// 五段
	SIXTH,		// 六段
	SEVENTH,	// 七段
	EIGHTH,		// 八段
	NINTH,		// 九段
	TENTH,		// 十段
	BRILLIANCE,	// 皆伝
	MAX
};

//===============================================
//	スキンデータの最大数
//===============================================
static const int SKINDATA_MAX = 63;


/********************************************/
//	プレイヤーに関する情報の構造体
/********************************************/
namespace PlayerData
{
	//===============================================
	//	プレイ情報
	//===============================================
	struct Infomation
	{
		char PlayerName[11];		// プレイヤーの名前(10文字) string型にしないのは、データ書き出しによって固定長にするため。
		int PlayCount;				// 遊んだ回数
		//DWORD id;					// ネットワークでデータ管理をすると想定した場合のユーザーID
		DWORD PlayTime;				// プレイ時間
	};

	//===============================================
	//	曲のスコア記録
	//===============================================
	struct MusicRecord
	{
		unsigned int ClearLamp : 4;	// クリアランプ(現状7種類だから、4ビット(15)あればいいかという安直な考え)
		unsigned int HiScore : 17;	// ハイスコア(最高100000なので、17ビット(131071)
	};

	//===============================================
	//	段位情報
	//===============================================
	struct GradeRecord
	{
		unsigned int bPass : 1;				// 合格(0or1)(未挑戦は達成率で判定とる)
		unsigned int AchievementPct : 7;	// 達成率(0～100なので、7ビット(127))
	};

	//===============================================
	//	段位のデータ1枠余ったので、再利用
	//===============================================
	struct Other
	{
		unsigned int version : 2;			// データのバージョン 3ビット(7)
		unsigned int empty : 6;				// 空き枠
	};


	//===============================================
	//	プレイヤーのオプション
	//===============================================
	// 
	//struct Option
	//{
	//	unsigned int bSudden		:1;		// サドゥン
	//	unsigned int SuddenPos		:10;	// サドゥン座標(Y)10ビット(1023)
	//	unsigned int HiSpeed		:5;		// ハイスピ 5ビット(31) * 0.25
	//	unsigned int iGaugeOption	:3;		// ゲージ(enumだと処理系依存なので危険) 3ビット(7)
	//	//unsigned int BasisOctave	:7;		// 開始オクターブ 7ビット(127) ※結構頻繁に変わりそうなので、テキストで管理する
	//	unsigned int bMoviePlay		:1;		// 演奏時に動画を再生するかどうか
	//	unsigned int MovieAlpha		:8;		// 演奏背景で流す動画の透明度(255)
	//	unsigned int bAutoPlay		:1;		// DJ_AUTOさん
	//	unsigned int bGuide			:1;		// ガイドありかなしか
	//};
	struct OptionV2
	{
		unsigned int bSudden		:1;		// サドゥン
		unsigned int SuddenPos		:10;	// サドゥン座標(Y)10ビット(1023)
		unsigned int HiSpeed		:5;		// ハイスピ 5ビット(31) * 0.25
		unsigned int iGaugeOption	:3;		// ゲージ(enumだと処理系依存なので危険) 3ビット(7)
		//unsigned int BasisOctave	:7;		// 開始オクターブ 7ビット(127) ※結構頻繁に変わりそうなので、テキストで管理する
		unsigned int bMoviePlay		:1;		// 演奏時に動画を再生するかどうか
		unsigned int MovieAlpha		:8;		// 演奏背景で流す動画の透明度(255)
		unsigned int bAutoPlay		:1;		// DJ_AUTOさん
		unsigned int bGuide			:1;		// ガイドありかなしか
		unsigned int bRegulSpeed	:1;		// レギュラースピード(ソフランなし)		
		unsigned int bPushStartGuide:1;		// この鍵盤から始まるよ！の有り無し
		unsigned int bShowTiming	:1;		// 判定タイミング表示
		unsigned int bMIDISoundOn : 1;		// ピアノ音
		unsigned int Other1 : 13;			// 今後の追加を想定した空き枠
		unsigned int Other2 : 16;
		unsigned int Other3 : 16;
	};

	//===============================================
	//	スキン
	//===============================================
	struct CustomizeRecord
	{
		int no;			// 選んでるスキンの番号
		Mybool bUnlock;	// 解禁
	};

}


/********************************************/
//	プレイヤーデータ用(シングルトン)
/********************************************/
class PlayerDataManager
{
public:
	//===============================================
	//	実体取得
	//===============================================
	static PlayerDataManager *GetInstance(){ static PlayerDataManager instance; return &instance; }


	//===============================================
	//	セーブフラグ
	//===============================================
	void OnSaveFlag(){ m_bSaveFlag = true; }

	//===============================================
	//	初期化(セーブデータのロード)
	//===============================================
	bool Initialize();

	//===============================================
	//	メンバ変数(アクセスを楽にしたかったのでpublic)
	//===============================================
	PlayerData::Infomation m_PlayerInfo;				// プレイ情報
	PlayerData::Other m_OtherData;						// 段位の空き枠でその他の情報リサイクル
	PlayerData::GradeRecord **m_GradeRecords;			// 段位情報(ゲーム開始時の段位はここから探す)
	GRADE m_grade;										// 自分が取得している最大の段位
	Mybool *m_bUnlockMusics;							// 曲の解禁データ(定数MAX_MUSICをヘッダで宣言したくないので、ポインタ可変長にする)
	PlayerData::CustomizeRecord **m_CustomizeRecords;	// スキンの情報(どのスキン)
	PlayerData::OptionV2 m_PlayerOption;				// プレイヤーのオプション


	//===============================================
	//	デストラクタ
	//===============================================
	~PlayerDataManager();


	//===============================================
	//	新規データ作成
	//===============================================
	void NewCreate(char *PlayerName);

	//===============================================
	//	セーブ
	//===============================================
	void SavePlayerData();

	//===============================================
	//	曲の記録の取得
	//===============================================
	PlayerData::MusicRecord *GetMusicRecord(int id, int iDifficulty){ return &m_MusicRecords[id][iDifficulty]; }

private:
	//===============================================
	//	メンバ関数
	//===============================================
	bool LoadPlayerData();
	void ClearData();		// やったらあかんやつ(データ全消去)


	//===============================================
	//	メンバ変数
	//===============================================
	clock_t m_StartClock;	// プレイ時間加算用
	bool m_bSaveFlag;		// trueにしたら消したときにセーブする
	PlayerData::MusicRecord	**m_MusicRecords;			// 曲のハイスコアとか保存する用のデータ(ダブルポインタだけど、m_MusicRecords[id][difficlyt].scoreという使い方をする)


	PlayerDataManager();
};

/********************************************/
//	インスタンス簡略化
/********************************************/
#define PlayerDataMgr (PlayerDataManager::GetInstance())