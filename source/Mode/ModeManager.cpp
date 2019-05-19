#include "TDNLIB.h"
#include "../Data/SelectMusicData.h"
#include "../Music/MusicInfo.h"
#include "../Data/GameScore.h"
#include "ModeManager.h"

void ModeManager::SetSelectGrade(int SelectGrade)
{
	assert(m_eModeType == PLAY_MODE::GRADE);	// 申し訳程度のエラーチェック
	ZeroMemory(&m_tagGradeData, sizeof(GradeData));
	GameScoreMgr->StartGrade();	// 段位用のスコアデータ設定

	std::ifstream ifs("DATA/Text/grade.txt");
	assert(ifs);	// ファイルオープン失敗

	while (!ifs.eof())
	{
		int l_iGrade;
		SelectMusicData datas[NUM_GRADE_MUSIC];
		ifs >> l_iGrade;
		FOR(NUM_GRADE_MUSIC)
		{
			ifs >> datas[i].id;
			int iDifficulty;
			ifs >> iDifficulty;
			datas[i].difficulty = (DIFFICULTY)iDifficulty;
		}

		// 選択した段位と同じだったら設定して抜ける
		if (l_iGrade == SelectGrade)
		{
			m_tagGradeData.Initialize(SelectGrade, datas);
			break;
		}
	}
}

void ModeManager::SettingGradeMusic()
{
	// 選曲管理さんに曲の情報を渡してあげる
	SelectMusicMgr->Set(m_tagGradeData.MusicDatas[m_iStep]);
}

GradeData::GradeData() :iSelectGrade(0), MusicDatas(nullptr)
{
	memset(FinesseGauge, 0, sizeof(FinesseGauge));
	memset(ClearRate, 0, sizeof(ClearRate));
}
void GradeData::Initialize(int SelectGrade, const SelectMusicData *data)
{
	iSelectGrade = SelectGrade;
	if (!MusicDatas) MusicDatas = new SelectMusicData[NUM_GRADE_MUSIC];
	memcpy_s(MusicDatas, sizeof(SelectMusicData)*NUM_GRADE_MUSIC, data, sizeof(SelectMusicData)*NUM_GRADE_MUSIC);
	memset(FinesseGauge, 0, sizeof(FinesseGauge));
	memset(ClearRate, 0, sizeof(ClearRate));
}

GradeData::~GradeData(){ if(MusicDatas)delete[] MusicDatas; }

void GradeData::ComputeClearRate(int step, int RestGauge, int JudgeCounts[], int NumNorts)
{
	FinesseGauge[step] = RestGauge;	// 残りゲージ

	// 判定取得率
	int JudgePercentages[(int)JUDGE_TYPE::MAX];
	FOR((int)JUDGE_TYPE::MAX) JudgePercentages[i] = (int)(((float)JudgeCounts[i] / NumNorts) * 100);	// 0～1なので、*100すること。

	// ▼合格（単曲通過）の場合　　　　有効サンプル数 ： SP94、DP45
	if (RestGauge > 0)
	{
		// まず、基本値を出します。 基本値 = PG率×1.1 + GR率×1.09 + GD率×0.8 　　　　　－(BD率×2.6 + PR率×4.25 + 空PR率×5)
		// 100を基準にし、それぞれの判定の取得率に応じて増減する。全てPGREATなら110、全てGOODなら80
		int BaseValue(0);
		FOR((int)JUDGE_TYPE::MAX)
		{
			static const float rates[(int)JUDGE_TYPE::MAX] = { 1.1f, 1.09f, .8f, 2.6f };	// 掛ける値
			const float percentage = JudgePercentages[i] * rates[i];			// 判定の取得率
			if (i != (int)JUDGE_TYPE::BAD) BaseValue += (int)percentage;			// BAD以外なら足す
			else  BaseValue -= (int)percentage;										// BADなら引く
		}

		// ※SP・DP共通 
		// ・基本値が70以上の場合　（すべてのサンプルで±0%~+ 1 % ）						→達成率 = 基本値 （100以上ならば100%）。
		// ・基本値が70以下の場合　（±1%の誤差あり。 - 4 % (DP)となるサンプルが1つあり）	→達成率 = 基本値×2 - 70 （40未満ならば40%）
		// （例）： 基本値が64 → 64×2 - 70 = 58% 
		//if (BaseValue >= 70)
		//{
		//	ClearRate[step] = min(BaseValue, 100);
		//}
		//else
		//{
		//	ClearRate[step] = max((BaseValue * 2) - 70, 40);
		//}
		ClearRate[step] = (BaseValue >= 70) ? min(BaseValue, 100) : max(BaseValue * 2 - 70, 40);

	}
	// ▼不合格の場合　　　　有効サンプル数 ： SP6、DP4
	else
	{
		// 単曲達成率 = (PG率 + GR率 + GD率 + BD率 + PR率)×40% = (曲の進行度)×40% 判定の取得率は、TOTAL NOTESに対する比率で良い。 空POORは含まない。
		// → 落ちた曲の単曲達成率は、必ず40%未満となる。
		float sum(0);
		FOR((int)JUDGE_TYPE::MAX) sum += JudgePercentages[i];
		ClearRate[step] = (int)(sum * 0.4f);
	}



}