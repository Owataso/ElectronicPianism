#pragma once

#include "../Music/MusicInfo.h"

//===============================================
//	ゲームのスコアの情報
//===============================================
struct GameScore
{
	int MaxCombo;							// 最大コンボ数
	int score;								// スコア(最大100000)
	int JudgeCount[(int)JUDGE_TYPE::MAX];	// 判定数。
	int FinesseGauge;						// ゲージ(偶数刻み)

	// スコア計算
	void ComputeScore(int NumNorts);
};

/********************************************/
//	スコア、コンボ数等の情報を保存するクラス<Singleton>
/********************************************/
class GameScoreManager
{
public:
	//===============================================
	//	実体取得
	//===============================================
	static GameScoreManager *GetInstance(){ static GameScoreManager instance; return &instance; }


	//===============================================
	//	段位開始
	//===============================================
	void StartGrade()
	{
		Reset();
		m_tagGameScore.FinesseGauge = 100;
	}


	//===============================================
	//	スコアリセット
	//===============================================
	void Reset(){ ZeroMemory(&m_tagGameScore, sizeof(GameScore)); }


	//===============================================
	//	ゲージ以外の情報をリセット※段位認定で使う
	//===============================================
	void ResetExceptGauge()
	{
		int gauge = m_tagGameScore.FinesseGauge;
		Reset();
		m_tagGameScore.FinesseGauge = gauge;
	}

	//===============================================
	//	スコアデータのポインタを渡す
	//===============================================
	GameScore* GetGameScore(){ return &m_tagGameScore; }

private:
	//===============================================
	//	スコアデータの実体
	//===============================================
	GameScore m_tagGameScore;


	//===============================================
	//	シングルトンの作法の作法
	//===============================================
	GameScoreManager(){ Reset(); }
	GameScoreManager(const GameScoreManager&){}
	GameScoreManager &operator =(const GameScoreManager&){}
};

/********************************************/
//	インスタンス簡略化
/********************************************/
#define GameScoreMgr (GameScoreManager::GetInstance())
