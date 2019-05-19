#include "tdnlib.h"
#include "../Music/MusicInfo.h"
#include "Judge.h"
#include "../Music/Note.h"
#include "JudgeAction.h"
#include "../Data/GameScore.h"
#include "Gauge.h"

//*****************************************************************************************************************************
//	判定して結果を返す
//*****************************************************************************************************************************
JUDGE_TYPE JudgeManager::ComputeJudgeResult(int iPosY, BOOL bUp)
{
	// 各判定幅
	static const int l_ciSuperWidth(40);
	static const int l_ciGreadWidth(80);
	static const int l_ciGoodWidth(150);
	static const int l_ciBadWidth(180);

	if (iPosY < l_ciSuperWidth * (bUp + 1))	return JUDGE_TYPE::SUPER;
	if (iPosY < l_ciGreadWidth * (bUp + 1)) return JUDGE_TYPE::GREAT;
	if (iPosY < l_ciGoodWidth * (bUp + 1))	return JUDGE_TYPE::GOOD;
	if (iPosY < l_ciBadWidth * (bUp + 1))	return JUDGE_TYPE::BAD;
	else
	{
		return JUDGE_TYPE::MAX;
	}
}


//*****************************************************************************************************************************
//	初期化・解放
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
JudgeManager::JudgeManager() : m_pRefNoteMgr(nullptr), m_pJudgeActMgr(nullptr), m_GameScore(nullptr), m_iCombo(0), m_bCN(new bool[NUM_KEYBOARD])
{

}
void JudgeManager::Initialize(NoteManager *pNoteMgr, GAUGE_OPTION op, bool bShowTiming)
{
	m_pRefNoteMgr = pNoteMgr;
	m_pJudgeActMgr = new JudgeActionManager(pNoteMgr->GetNumNorts(), this);
	m_pJudgeActMgr->Initialize(bShowTiming);

	m_iCombo = 0;

	// ゲームスコア管理クラスのスコアをアドレス参照
	m_GameScore = GameScoreMgr->GetGameScore();

	// ゲージくん初期化
	m_GaugeManager = new GaugeManager(op, &m_GameScore->FinesseGauge);

	// なんかリセット
	Reset();
}

//------------------------------------------------------
//	解放
//------------------------------------------------------
JudgeManager::~JudgeManager()
{
	delete m_pJudgeActMgr;
	delete m_GaugeManager;
	delete[] m_bCN;
}

//*****************************************************************************************************************************
//		更新・描画
//*****************************************************************************************************************************
//------------------------------------------------------
//	更新
//------------------------------------------------------
bool JudgeManager::Update()
{
	m_pJudgeActMgr->Update();

	// 閉店フラグを返す
	return m_GaugeManager->isFailed();
}

//------------------------------------------------------
//	描画
//------------------------------------------------------
void JudgeManager::Render()
{
	m_pJudgeActMgr->Render();
}


//*****************************************************************************************************************************
//		ノート判定
//*****************************************************************************************************************************
void JudgeManager::JudgeNote(char cLaneNo)
{
	// ノーツリストループ(最適化により、ピンポイントにレーンで判定を取ることが可能に)
	for (auto &it : *m_pRefNoteMgr->GetList(cLaneNo))
	{
		int l_iPosY;
		m_pRefNoteMgr->GetNoteAbsolutePos((*it), &l_iPosY);	// ノーツの絶対位置

		JUDGE_TIMING l_eJudgeTiming((l_iPosY > 0) ? JUDGE_TIMING::SLOW : JUDGE_TIMING::FAST);

		// 絶対値処理
		l_iPosY = (l_iPosY < 0) ? l_iPosY * -1 : l_iPosY;

		JUDGE_TYPE JudgeResult(ComputeJudgeResult(l_iPosY, FALSE));

		if(JudgeResult == JUDGE_TYPE::SUPER)
		{
			l_eJudgeTiming = JUDGE_TIMING::JUST;
		}

		// 何かしらの判定を起こしていたら
		if (JudgeResult != JUDGE_TYPE::MAX)
		{
			// 判定個数更新
			m_GameScore->JudgeCount[(int)JudgeResult]++;

			// コンボ数更新
			m_iCombo = (JudgeResult == JUDGE_TYPE::BAD) ? 0 : m_iCombo + 1;
			// 最大コンボ数更新
			m_GameScore->MaxCombo = max(m_GameScore->MaxCombo, m_iCombo);

			// ジャッジアクション君にデータ送信
			m_pJudgeActMgr->Set(cLaneNo, m_iCombo, JudgeResult, l_eJudgeTiming);

			// ゲージ増減
			m_GaugeManager->Compute(JudgeResult);

			// スコア計算
			m_GameScore->ComputeScore(m_pRefNoteMgr->GetNumNorts());

			// ノーツ消す
			it->erase = true;

			// ノーツがCN開始だったらCNフラグON
			if (it->note.cNoteType == (int)NOTE_TYPE::CN_START) m_bCN[cLaneNo] = true;

			break;
		}
	}
}

void JudgeManager::JudgeUpKey(char cLaneNo)
{
	// CN押してなかったら出ていけぇ！！
	if (!m_bCN[cLaneNo]) return;

	int l_iPosY;
	auto it = *m_pRefNoteMgr->GetList(cLaneNo)->begin();
	m_pRefNoteMgr->GetNoteAbsolutePos((*it), &l_iPosY);	// ノーツの絶対位置

	JUDGE_TIMING l_eJudgeTiming((l_iPosY > 0) ? JUDGE_TIMING::SLOW : JUDGE_TIMING::FAST);

	// 絶対値処理
	l_iPosY = (l_iPosY < 0) ? l_iPosY * -1 : l_iPosY;

	JUDGE_TYPE JudgeResult(ComputeJudgeResult(l_iPosY, TRUE));

	if(JudgeResult == JUDGE_TYPE::SUPER)
	{
		l_eJudgeTiming = JUDGE_TIMING::JUST;
	}

	if (JudgeResult == JUDGE_TYPE::MAX) JudgeResult = JUDGE_TYPE::BAD;

	// 判定個数更新
	m_GameScore->JudgeCount[(int)JudgeResult]++;

	// コンボ数更新
	m_iCombo = (JudgeResult == JUDGE_TYPE::BAD) ? 0 : m_iCombo + 1;
	// 最大コンボ数更新
	m_GameScore->MaxCombo = max(m_GameScore->MaxCombo, m_iCombo);

	// ジャッジアクション君にデータ送信
	m_pJudgeActMgr->Set(cLaneNo, m_iCombo, JudgeResult, l_eJudgeTiming);

	// ゲージ増減
	m_GaugeManager->Compute(JudgeResult);

	// スコア計算
	m_GameScore->ComputeScore(m_pRefNoteMgr->GetNumNorts());

	// ノーツ消す
	it->erase = true;

	// フラグ終了
	m_bCN[cLaneNo] = false;
}

void JudgeManager::LostNote(char cLaneNo)
{
	// BAD判定カウント
	m_GameScore->JudgeCount[(int)JUDGE_TYPE::BAD]++;

	// コンボ数更新
	m_iCombo = 0;

	// ジャッジアクション君にデータ送信
	m_pJudgeActMgr->Set(cLaneNo, m_iCombo, JUDGE_TYPE::BAD, JUDGE_TIMING::SLOW);

	// ゲージ減
	m_GaugeManager->Compute(JUDGE_TYPE::BAD);

	// CN押し越しを想定して、CNフラグをオフにする
	m_bCN[cLaneNo] = false;
}

void JudgeManager::Reset()
{
	// スコアとゲージリセット
	GameScoreMgr->ResetExceptGauge();	// 段位用にゲージを残しての初期化を行う

	// ゲージオプションに応じて初期ゲージの値を設定
	int StartGauge;
	switch (m_GaugeManager->GetGaugeOption())
	{
		// 22%スタート
	case GAUGE_OPTION::NORMAL:
	case GAUGE_OPTION::EASY:
		StartGauge = 22;
		break;

		// 100%スタート
	case GAUGE_OPTION::HARD:
	case GAUGE_OPTION::EX_HARD:
	case GAUGE_OPTION::HAZARD:
		StartGauge = 100;
		break;

		// 前のゲージからのスタート
	case GAUGE_OPTION::GRADE:
		StartGauge = GameScoreMgr->GetGameScore()->FinesseGauge;
		break;
	}
	m_GaugeManager->GaugeSet(StartGauge);
	m_iCombo = 0;

	// 判定アクションクリア
	m_pJudgeActMgr->Clear();

	// ノーツ回復量設定
	m_GaugeManager->SetNumNorts(m_pRefNoteMgr->GetNumNorts());

	// CNフラグオフ
	memset(m_bCN, false, sizeof(bool)*NUM_KEYBOARD);
}

// フルコン演出中かどうか
bool JudgeManager::isFullComboNow(){ return m_pJudgeActMgr->isFullComboNow(); }
bool JudgeManager::isFullComboEnd(){ return m_pJudgeActMgr->isFullComboEnd(); }