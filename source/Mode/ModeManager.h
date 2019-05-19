#pragma once

//===============================================
//	前方宣言
//===============================================
enum class DIFFICULTY;
struct SelectMusicData;

//===============================================
//	定数
//===============================================
enum class PLAY_MODE
{
	STANDARD,	// STANDARD系
	FREE,		// FREE系
	GRADE,		// 段位認定系
	MAX
};
static const int NUM_GRADE_MUSIC(3);// 段位認定の曲数


/********************************************/
//	段位認定専用の構造体
/********************************************/
struct GradeData
{
	// 選択した段位
	int iSelectGrade;

	// 曲の情報
	SelectMusicData *MusicDatas;

	// 達成率関連
	int FinesseGauge[NUM_GRADE_MUSIC];	// 完走時のゲージ
	int ClearRate[NUM_GRADE_MUSIC];		// 特殊な計算から導き出される、単曲達成率(総合達成率はこれの平均をとる)
	GradeData();
	~GradeData();
	void Initialize(int SelectGrade, const SelectMusicData *data);
	void ComputeClearRate(int step, int RestGauge, int JudgeCounts[], int NumNorts);
	UINT GetAverageClearRate()
	{
		UINT sum(0);
		FOR(NUM_GRADE_MUSIC) sum += ClearRate[i];
		return sum / NUM_GRADE_MUSIC;
	}
};


/********************************************/
//	シーン移動時の分岐用<Singleton>
/********************************************/
class ModeManager
{
public:
	//===============================================
	//	実体取得
	//===============================================
	static ModeManager *GetInstance(){ static ModeManager i; return &i; }

	//===============================================
	//	アクセサ
	//===============================================
	void SetMode(PLAY_MODE m)
	{
		m_iStep = 0;
		m_eModeType = m;

		if (m != PLAY_MODE::GRADE) { ZeroMemory(&m_tagGradeData, sizeof(GradeData)); }
	}
	PLAY_MODE GetMode(){ return m_eModeType; }
	int GetStep(){ return m_iStep; }
	void AddStep(){ m_iStep++; }
	GradeData *GetGradeData(){ return &m_tagGradeData; }

	//===============================================
	//	段位認定
	//===============================================
	void SetSelectGrade(int SelectGrade);
	void SettingGradeMusic();
	void SetGradeClearRate(int RestGauge, int JudgeCounts[], int NumNorts) { m_tagGradeData.ComputeClearRate(m_iStep, RestGauge, JudgeCounts, NumNorts); }

private:
	//===============================================
	//	メンバ変数
	//===============================================
	PLAY_MODE m_eModeType;			// プレイヤーが選択しているモード
	int m_iStep;					// スタンダードのステージだったり、段位の何曲目だったり
	GradeData m_tagGradeData;		// 段位用データ

	//===============================================
	//	シングルトンの作法
	//===============================================
	ModeManager() :m_eModeType(PLAY_MODE::MAX), m_iStep(0){}
	ModeManager(const ModeManager&){}
	ModeManager &operator=(const ModeManager&){}
};

/********************************************/
//	インスタンス簡略化
/********************************************/
#define ModeMgr (ModeManager::GetInstance())