#include "TDNLIB.h"
#include "system/Framework.h"
#include "../Sound/SoundManager.h"
#include "sceneTitle.h"
#include "SceneResult.h"
#include "../Data/PlayerData.h"
#include "sceneEnding.h"
#include "sceneMusicSelect.h"
#include "sceneModeSelect.h"
#include "scenePlaying.h"
#include "../Number/Number.h"
#include "../Data/GameScore.h"
#include "../Data/SelectMusicData.h"
#include "../Mode/ModeManager.h"
#include "../Input/InputMIDI.h"
#include "../Fade/Fade.h"


//*****************************************************************************************************************************
//
//		曲のリザルト
//
//*****************************************************************************************************************************

// プレイヤーデータ更新
void sceneResult::RenovationRecord()
{
	const int id(SelectMusicMgr->Get()->byID);
	const int dif((int)SelectMusicMgr->Get()->eDifficulty);
	bool bSave(false);	// trueになったらデータセーブフラグON

	// スコア更新
	if (m_iHiScore < m_iScore)
	{
		PlayerDataMgr->GetMusicRecord(id, dif)->HiScore = m_iScore;
		bSave = m_bNewRecord = true;
	}

	// クリアしてたらクリアランプ更新
	if (m_bCleared)
	{
		// フルコン判定
		if (m_combo == m_notes)
		{
			if (PlayerDataMgr->GetMusicRecord(id,dif)->ClearLamp != (int)CLEAR_LAMP::FULL_COMBO) bSave = true;
			PlayerDataMgr->GetMusicRecord(id,dif)->ClearLamp = (int)CLEAR_LAMP::FULL_COMBO;
			m_bFullCombo = true;
		}
		else if (ModeMgr->GetMode() != PLAY_MODE::GRADE)	// 段位ならクリアランプ更新はなし
		{
			// ゲージ⇔ランプ用の連想配列
			std::map<GAUGE_OPTION, CLEAR_LAMP> LampMap;
			LampMap[GAUGE_OPTION::EASY] = CLEAR_LAMP::EASY;
			LampMap[GAUGE_OPTION::NORMAL] = CLEAR_LAMP::CLEAR;
			LampMap[GAUGE_OPTION::HARD] = CLEAR_LAMP::HARD;
			LampMap[GAUGE_OPTION::EX_HARD] = CLEAR_LAMP::EX_HARD;
			LampMap[GAUGE_OPTION::HAZARD] = CLEAR_LAMP::FULL_COMBO;

			// 前のランプよりランク高かったら更新
			if (PlayerDataMgr->GetMusicRecord(id,dif)->ClearLamp < (UINT)LampMap[(GAUGE_OPTION)PlayerDataMgr->m_PlayerOption.iGaugeOption])
			{
				PlayerDataMgr->GetMusicRecord(id,dif)->ClearLamp = (UINT)LampMap[(GAUGE_OPTION)PlayerDataMgr->m_PlayerOption.iGaugeOption];
				bSave = true;
			}
		}
	}
	else if (PlayerDataMgr->GetMusicRecord(id,dif)->ClearLamp == (int)CLEAR_LAMP::NO_PLAY)
	{
		PlayerDataMgr->GetMusicRecord(id,dif)->ClearLamp = (int)CLEAR_LAMP::FAILED;
		bSave = true;
	}

	// セーブフラグオン
	if (bSave) PlayerDataMgr->OnSaveFlag();
}

//=============================================================================================
//		初	期	化	と	開	放
sceneResult::sceneResult(bool bCleared, bool bPractice) :
m_bPractice(bPractice),
m_bCleared(bCleared),
m_iScore(GameScoreMgr->GetGameScore()->score),
m_iHiScore(PlayerDataMgr->GetMusicRecord(SelectMusicMgr->Get()->byID, (int)SelectMusicMgr->Get()->eDifficulty)->HiScore),
m_iRank((int)ScoreRankCheck(GameScoreMgr->GetGameScore()->score)),
m_combo(GameScoreMgr->GetGameScore()->MaxCombo),
m_notes(SelectMusicMgr->Get()->omsInfo.NumNotes),
m_pCircle(new CyberCircle(Vector2(950, 478),.8f, .8f)),
m_bFullCombo(false), m_bNewRecord(false), m_pResult(nullptr),
m_iRenderCombo(0), m_iRenderScore(0), 
m_iStep(0)
{
	memcpy_s(m_iJudgeCount, sizeof(int)* (int)JUDGE_TYPE::MAX, GameScoreMgr->GetGameScore()->JudgeCount, sizeof(int)* (int)JUDGE_TYPE::MAX);
	memset(m_iRenderJudgeCount, 0, sizeof(int)* (int)JUDGE_TYPE::MAX);
}
bool sceneResult::Initialize()
{
	// フェード初期化
	Fade::Set(Fade::MODE::FADE_IN, 10, 0x00000000);

	// オートじゃなかったらハイスコアに設定
	if (PlayerDataMgr->m_PlayerOption.bAutoPlay == FALSE && !m_bPractice) RenovationRecord();

	// 画像初期化
	char *BackImagePath;
	if (m_bCleared)BackImagePath = "DATA/UI/Result/cleared.png";
	else BackImagePath = "DATA/UI/Result/failed.png";
	m_pImages[IMAGE::BACK]			= std::make_unique<tdn2DObj>(BackImagePath);
	m_pImages[IMAGE::UI_MAP]		= std::make_unique<tdn2DObj>("DATA/UI/Result/map.png");
	m_pImages[IMAGE::UP_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Other/up_frame.png");
	m_pImages[IMAGE::MIDASHI]		= std::make_unique<tdn2DObj>("DATA/UI/Other/midashi.png");
	m_pImages[IMAGE::NUMBER]		= std::make_unique<tdn2DObj>("DATA/Number/Number.png");
	m_pImages[IMAGE::NUMBER_L]		= std::make_unique<tdn2DObj>("DATA/Number/NumberL.png");

	m_pImages[IMAGE::MUSIC_NAME]	= std::make_unique<tdn2DObj>((SelectMusicMgr->GetMusicPath() + "title.png").c_str());
	m_pImages[IMAGE::DIF]			= std::make_unique<tdn2DObj>("DATA/UI/Other/difficulty.png");

	m_pAnimImages[ANIM_IMAGE::FRAME1]	= std::make_unique<tdn2DAnim>(m_pImages[IMAGE::UI_MAP]->GetTexture());
	m_pAnimImages[ANIM_IMAGE::FRAME1]->OrderStretch(16, 1, 0);
	m_pAnimImages[ANIM_IMAGE::FRAME1]->Action(5);

	m_pAnimImages[ANIM_IMAGE::FRAME2]	= std::make_unique<tdn2DAnim>(m_pImages[IMAGE::UI_MAP]->GetTexture());
	m_pAnimImages[ANIM_IMAGE::FRAME2]->OrderStretch(6, 1, 0);
	m_pAnimImages[ANIM_IMAGE::FRAME2]->Action(20);

	m_pAnimImages[ANIM_IMAGE::FRAME3]	= std::make_unique<tdn2DAnim>(m_pImages[IMAGE::UI_MAP]->GetTexture());
	m_pAnimImages[ANIM_IMAGE::FRAME3]->OrderStretch(16, 1, 0);

	if (m_bFullCombo)
	{
		m_pAnimImages[ANIM_IMAGE::FULLCOMBO] = std::make_unique<tdn2DAnim>(m_pImages[IMAGE::UI_MAP]->GetTexture());
		m_pAnimImages[ANIM_IMAGE::FULLCOMBO]->OrderShrink(12, 1, 2);
		m_pAnimImages[ANIM_IMAGE::FULLCOMBO]->SetAlpha(0);
	}

	if (m_bNewRecord)
	{
		m_pAnimImages[ANIM_IMAGE::NEW_RECORD] = std::make_unique<tdn2DAnim>(m_pImages[IMAGE::UI_MAP]->GetTexture());
		m_pAnimImages[ANIM_IMAGE::NEW_RECORD]->OrderShrink(12, 1, 2);
		m_pAnimImages[ANIM_IMAGE::NEW_RECORD]->SetAlpha(0);
	}

	m_pAnimImages[ANIM_IMAGE::RANK]		= std::make_unique<tdn2DAnim>(m_pImages[IMAGE::UI_MAP]->GetTexture());
	m_pAnimImages[ANIM_IMAGE::RANK]->OrderShrink(12, 1, 3.5f);
	m_pAnimImages[ANIM_IMAGE::RANK]->SetAlpha(0);

	m_pAnimImages[ANIM_IMAGE::RANK_RIPPLE] = std::make_unique<tdn2DAnim>(m_pImages[IMAGE::UI_MAP]->GetTexture());
	m_pAnimImages[ANIM_IMAGE::RANK_RIPPLE]->OrderRipple(12, 1.0f, 0.1f);

	// リザルト委譲的な何か
	switch (ModeMgr->GetMode())
	{
	case PLAY_MODE::FREE:
		m_pResult = new Result::Free(m_pImages[IMAGE::UI_MAP].get());
		break;
	}

	return true;
}

sceneResult::~sceneResult()
{
	SAFE_DELETE(m_pResult);
}
//=============================================================================================


//=============================================================================================
//		更			新
bool sceneResult::Update()
{
	// フェード更新
	Fade::Update();

	// 画像アニメ更新
	FOR((int)ANIM_IMAGE::ANIM_MAX)
	{
		// 入ってなかったら飛ばす
		if (!m_pAnimImages[i])continue;

		// アクションが発動した瞬間
		if (m_pAnimImages[i]->GetAction()->GetDelayFrame() == 1)
		{
			switch ((ANIM_IMAGE)i)
			{
			case sceneResult::FRAME1:
			case sceneResult::FRAME2:
			case sceneResult::FRAME3:
				g_pSE->Play("ウィンドウ2");
				break;
			}
		}

		m_pAnimImages[i]->Update();
	}

	// サークル更新
	m_pCircle->Update();

	// 委譲くんに処理を託してたら
	if (m_pResult)
	{
		if (m_pResult->Update()) return true;	// シーンチェンジの後に処理をさせないための処理
	}

	switch (m_iStep)
	{
		case 0:	/* 左下の枠にコンボと判定の数値が加算されている間 */
			if (m_pAnimImages[ANIM_IMAGE::FRAME2]->GetAction()->IsEnd())
			{
				// スコア加算処理
				if (m_iRenderJudgeCount[(int)JUDGE_TYPE::SUPER] < m_iJudgeCount[(int)JUDGE_TYPE::SUPER]) m_iRenderJudgeCount[(int)JUDGE_TYPE::SUPER] = min(m_iRenderJudgeCount[(int)JUDGE_TYPE::SUPER] + 222, m_iJudgeCount[(int)JUDGE_TYPE::SUPER]);
				else if (m_iRenderJudgeCount[(int)JUDGE_TYPE::GREAT] < m_iJudgeCount[(int)JUDGE_TYPE::GREAT]) m_iRenderJudgeCount[(int)JUDGE_TYPE::GREAT] = min(m_iRenderJudgeCount[(int)JUDGE_TYPE::GREAT] + 222, m_iJudgeCount[(int)JUDGE_TYPE::GREAT]);
				else if (m_iRenderJudgeCount[(int)JUDGE_TYPE::GOOD] < m_iJudgeCount[(int)JUDGE_TYPE::GOOD])  m_iRenderJudgeCount[(int)JUDGE_TYPE::GOOD] = min(m_iRenderJudgeCount[(int)JUDGE_TYPE::GOOD] + 222, m_iJudgeCount[(int)JUDGE_TYPE::GOOD]);
				else if (m_iRenderJudgeCount[(int)JUDGE_TYPE::BAD] < m_iJudgeCount[(int)JUDGE_TYPE::BAD])   m_iRenderJudgeCount[(int)JUDGE_TYPE::BAD] = min(m_iRenderJudgeCount[(int)JUDGE_TYPE::BAD] + 222, m_iJudgeCount[(int)JUDGE_TYPE::BAD]);
				else if (m_iRenderCombo < m_combo) m_iRenderCombo = min(m_iRenderCombo + 222, m_combo);
				else
				{
					// 3つめの枠を解禁
					m_pAnimImages[ANIM_IMAGE::FRAME3]->Action(1);

					// フルコンしてたらフルコン画像アニメーション発動
					if (m_pAnimImages[ANIM_IMAGE::FULLCOMBO])m_pAnimImages[ANIM_IMAGE::FULLCOMBO]->Action();

					// 次のステップへ
					m_iStep++;
				}
			}
			break;

		case 1: /* 右下の枠にスコアが加算されている間 */
			if (m_pAnimImages[ANIM_IMAGE::FRAME3]->GetAction()->IsEnd())
			{
				if (m_iRenderScore < m_iScore) m_iRenderScore = min(m_iRenderScore + 3333, m_iScore);
				else
				{
					// ランクのアニメーション発動
					m_pAnimImages[ANIM_IMAGE::RANK]->Action();

					// 記録更新してたら記録更新画像アニメーション発動
					if (m_pAnimImages[ANIM_IMAGE::NEW_RECORD])m_pAnimImages[ANIM_IMAGE::NEW_RECORD]->Action();

					// 次のステップへ
					m_iStep++;
				}
			}
			break;

		case 2:
			if (m_pAnimImages[ANIM_IMAGE::RANK]->GetAction()->IsEnd())
			{
				// ランクのアニメーション発動
				m_pAnimImages[ANIM_IMAGE::RANK]->OrderShake(8, 23.0f, 35.0f, 2);
				m_pAnimImages[ANIM_IMAGE::RANK]->Action();
				m_pAnimImages[ANIM_IMAGE::RANK_RIPPLE]->Action();

				// サークル発動
				m_pCircle->Action();

				// SE
				g_pSE->Play("ランク");

				// 次のステップへ
				m_iStep++;
			}
			break;

		case 3:	/* もう出て行っていいよ */
			if (InputMIDIMgr->GetAnyTrigger())
			{
				// モードでシーン分岐
				switch (ModeMgr->GetMode())
				{
				case PLAY_MODE::STANDARD:
					// クリアしてたら
					if (m_bCleared)
					{
						// ステップ増やす
						ModeMgr->AddStep();

						// 完走
						if (ModeMgr->GetStep() >= 4)
						{
							// エンディングにいく
							MainFrame->ChangeScene(new sceneEnding);
						}
						// 続行
						else
						{
							MainFrame->ChangeScene(new sceneMusicSelect);
						}
					}
					// 死んだ
					else
					{
						// タイトルに行く
						MainFrame->ChangeScene(new sceneTitle);
					}

					break;
				case PLAY_MODE::FREE:
					m_pResult->Enable();	// 有効化
					break;
				case PLAY_MODE::GRADE:
				{
										 GameScore *pScoreData = GameScoreMgr->GetGameScore();
										 int JudgeCounts[(int)JUDGE_TYPE::MAX];
										 FOR((int)JUDGE_TYPE::MAX) JudgeCounts[i] = pScoreData->JudgeCount[i];
										 ModeMgr->SetGradeClearRate(pScoreData->FinesseGauge, JudgeCounts, m_notes);	// モードさんに成績を渡す
				}
					// クリアしてたら
					if (m_bCleared)
					{
						// ステップ増やす
						ModeMgr->AddStep();

						// 完走
						if (ModeMgr->GetStep() >= NUM_GRADE_MUSIC)
						{
							// 段位合格
							MainFrame->ChangeScene(new sceneGradeResult(true));
						}
						// 続行
						else
						{
							ModeMgr->SettingGradeMusic();		// 選曲管理さんに曲を設定させる
							MainFrame->ChangeScene(new scenePlaying);
						}
					}
					// 死んだ
					else
					{
						// 段位不合格
						MainFrame->ChangeScene(new sceneGradeResult(false));
					}
					break;

				}	// switch

			}	// なんか押したらif文
			break;
	}



	return true;
}
//
//=============================================================================================


//=============================================================================================
//		描			画
void sceneResult::Render()
{
	tdnView::Clear();

	// 背景
	m_pImages[IMAGE::BACK]->Render(0, 0);

	// 上フレーム
	m_pImages[IMAGE::UP_FRAME]->Render(0, 0);

	// 上の見出し
	m_pImages[IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 384, 400, 64);

	// 結果(CREARED or FAILED)
	m_pImages[IMAGE::UI_MAP]->Render(722, 38, 340, 64, 256, 933 + (m_bCleared ? 0 : 64), 340, 64);


	// 枠
	m_pAnimImages[ANIM_IMAGE::FRAME1]->Render(220, 100, 813, 151, 0,   0,   813, 151);
	m_pAnimImages[ANIM_IMAGE::FRAME2]->Render(36,  220, 638, 391, 280, 542, 638, 391);
	m_pAnimImages[ANIM_IMAGE::FRAME3]->Render(688, 220, 564, 391, 280, 151, 564, 391);

	// 1枠目が終わったら
	if (m_pAnimImages[ANIM_IMAGE::FRAME1]->GetAction()->IsEnd())
	{
		// 曲名と難易度
		m_pImages[IMAGE::MUSIC_NAME]->Render(280, 136, 700, 90, 0, 0, 700, 90);
		m_pImages[IMAGE::DIF]->Render(512, 224, 230, 32, 0, (int)SelectMusicMgr->Get()->eDifficulty * 32, 256, 32);
	}

	// 2枠目が終わったら
	if (m_pAnimImages[ANIM_IMAGE::FRAME2]->GetAction()->IsEnd())
	{
		/* 数字たち */
		static const int X = 540;

		static const int COMBO_OFFSET_Y(550);

		// MAXCOMBO
		NumberRender(m_pImages[IMAGE::NUMBER].get(), X, COMBO_OFFSET_Y, 24, 32, m_iRenderCombo, 4, false);

		// TOTAL NOTES
		NumberRender(m_pImages[IMAGE::NUMBER].get(), X, COMBO_OFFSET_Y + 48, 24, 32, m_notes, 4, false);



		// フルコンおめでとう
		if (m_pAnimImages[ANIM_IMAGE::FULLCOMBO])m_pAnimImages[ANIM_IMAGE::FULLCOMBO]->Render(350, COMBO_OFFSET_Y + 96, 340, 32, 256, 933 + 160, 340, 32);

		// 各判定
		static const int JUDGE_OFFSET_Y(337);
		for (int i = 0; i < (int)JUDGE_TYPE::MAX; i++)
		{
			NumberRender(m_pImages[IMAGE::NUMBER].get(), X, JUDGE_OFFSET_Y + i * 50, 24, 32, m_iRenderJudgeCount[i], 4, false);
		}
	}

	// 3枠目が終わったら
	if (m_pAnimImages[ANIM_IMAGE::FRAME3]->GetAction()->IsEnd())
	{
		// ハイスコア
		NumberRender(m_pImages[IMAGE::NUMBER].get(), 1082, 368, 24, 32, m_iHiScore, 6, false);

		// スコア
		NumberRender(m_pImages[IMAGE::NUMBER_L].get(), 1118, 445, 32, 42, m_iRenderScore, 6, false);

		// ニューレコードおめでとう
		if (m_pAnimImages[ANIM_IMAGE::NEW_RECORD])
		{
			m_pAnimImages[ANIM_IMAGE::NEW_RECORD]->Render(884, 412, 342, 32, 256, 933 + 128, 340, 32);

			// ニューレコードアニメーションが終わったら
			if (m_pAnimImages[ANIM_IMAGE::NEW_RECORD]->GetAction()->IsEnd())
			{
				// ハイスコアからの差分を表示
				const int iDifferenceScore(m_iScore - m_iHiScore);
				tdnText::Draw(1150, 414, 0xff80f0f0, "+%d", m_iScore - m_iHiScore);
			}
		}

		// サークル描画
		m_pCircle->Render();

		// ランク
		m_pAnimImages[ANIM_IMAGE::RANK]->Render(940, 554, 280, 96, 0, 151 + m_iRank * 96, 280, 96);
		m_pAnimImages[ANIM_IMAGE::RANK_RIPPLE]->Render(940, 554, 280, 96, 0, 151 + m_iRank * 96, 280, 96);
	}




	Fade::Render();	// フェード描画

	// リザルトフリー委譲くん
	if (m_pResult)
	{
		m_pResult->Render();
	}

}
//
//=============================================================================================





int Result::Free::m_iSelectCursor = (int)SELECT::MUSIC_SELECT;
bool Result::Free::Update()
{
	if (!m_bEnable) return false;	// いつもの出ていけぇ！！

	// ガイド
	m_pKeyboardGuide->Update();

	// カーソル座標補間
	m_CurrentCursorY = m_CurrentCursorY * .8f + m_NextCursorY * .2f;
	// カーソルアニメーション
	if (++m_CursorAnim > 16 * 4) m_CursorAnim = 0;
	// 選択
	if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
	{
		g_pSE->Play("カーソル2");	// カーソル音再生
		if (--m_iSelectCursor < 0) m_iSelectCursor = (int)SELECT::MAX - 1;
		m_NextCursorY = 52 + m_iSelectCursor * 50.0f;	// 目標カーソル座標
	}
	else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
	{
		g_pSE->Play("カーソル2");	// カーソル音再生
		if (++m_iSelectCursor >= (int)SELECT::MAX) m_iSelectCursor = 0;
		m_NextCursorY = 52 + m_iSelectCursor * 50.0f;	// 目標カーソル座標
	}
	// 決定
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		g_pSE->Play("決定2");
		switch ((SELECT)m_iSelectCursor)
		{
		case SELECT::CONTINUE:
			MainFrame->ChangeScene(new scenePlaying);
			break;
		case SELECT::MUSIC_SELECT:
			MainFrame->ChangeScene(new sceneMusicSelect);
			break;
		case SELECT::EXIT:
			MainFrame->ChangeScene(new sceneModeSelect);
			break;
		}
		return true;
	}
	return false;
}

void Result::Free::Render()
{
	static const int OFFSET_X = 512, OFFSET_Y = 232;
	if (!m_bEnable) return;	// いつもの出ていけぇ！！
	m_pWindow->Render(OFFSET_X, OFFSET_Y, 256, 256, 0, 919, 256, 256);

	// カーソル
	m_pCursor->Render(OFFSET_X + 38, OFFSET_Y + (int)m_CurrentCursorY, 180, 50, 0, (m_CursorAnim / 16) * 64, 128, 64);

	// ガイド
	m_pKeyboardGuide->Render();
}

void Result::Free::Enable()
{
	// 初回ならフェードセット
	if (!m_bEnable) Fade::Set(Fade::MODE::FADE_OUT, 8, 160, Fade::m_byAlpha, 0x00000000);
	m_bEnable = true;
}





//*****************************************************************************************************************************
//
//		段位の総合リザルト
//
//*****************************************************************************************************************************
// プレイヤーデータ更新
void sceneGradeResult::RenovationRecord()
{
	bool bSave(false);	// trueになったらデータセーブフラグON

	// 達成率更新
	if (PlayerDataMgr->m_GradeRecords[m_pGradeData->iSelectGrade]->AchievementPct < m_Achievement)
	{
		PlayerDataMgr->m_GradeRecords[m_pGradeData->iSelectGrade]->AchievementPct = m_Achievement;
		bSave = true;
	}

	// 合格してたら
	if (m_bPassed)
	{
		if ((int)PlayerDataMgr->m_grade < m_pGradeData->iSelectGrade) PlayerDataMgr->m_grade = (GRADE)m_pGradeData->iSelectGrade;
		PlayerDataMgr->m_GradeRecords[m_pGradeData->iSelectGrade]->bPass = TRUE;
		bSave = true;
	}

	// セーブフラグオン
	if (bSave) PlayerDataMgr->OnSaveFlag();
}

//=============================================================================================
//		初	期	化	と	開	放
sceneGradeResult::sceneGradeResult(bool bPassed) :m_bPassed(bPassed), m_pGradeData(ModeMgr->GetGradeData()), m_pSE(new tdnSoundSE){}
bool sceneGradeResult::Initialize()
{
	// フェード初期化
	Fade::Set(Fade::MODE::FADE_IN, 10, 0x00000000);

	// 画像初期化
	char *BackImagePath;
	if (m_bPassed)BackImagePath = "DATA/UI/Grade/cleared.png";
	else BackImagePath = "DATA/UI/Grade/failed.png";
	m_pImages[IMAGE::BACK]			= std::make_unique<tdn2DObj>(BackImagePath);
	m_pImages[IMAGE::RESULT]		= std::make_unique<tdn2DObj>("DATA/UI/Grade/pass.png");
	m_pImages[IMAGE::UP_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Other/up_frame.png");
	m_pImages[IMAGE::MIDASHI]		= std::make_unique<tdn2DObj>("DATA/UI/Other/midashi.png");
	m_pImages[IMAGE::FRAME]			= std::make_unique<tdn2DObj>("DATA/UI/Grade/result_frame.png");
	m_pImages[IMAGE::NUMBER]		= std::make_unique<tdn2DObj>("DATA/Number/number.png");
	m_pImages[IMAGE::NUMBER_L]		= std::make_unique<tdn2DObj>("DATA/Number/numberL.png");
	m_pImages[IMAGE::IMG_GRADE]		= std::make_unique<tdn2DObj>("DATA/UI/Grade/text.png");

	// 最終達成率
	m_Achievement = m_pGradeData->GetAverageClearRate();

	// リザルトのガヤ
	m_pSE->Set(0, 1, (m_bPassed) ? "DATA/Sound/SE/kansei.wav" : "DATA/Sound/SE/booing.wav");
	m_pSE->Play(0);

	// 段位記録更新
	RenovationRecord();

	return true;
}

sceneGradeResult::~sceneGradeResult()
{
	delete m_pSE;
}
//=============================================================================================


//=============================================================================================
//		更			新
bool sceneGradeResult::Update()
{
	// フェード更新
	Fade::Update();

	// なんか押したらif文
	if (InputMIDIMgr->GetAnyTrigger())
	{
		MainFrame->ChangeScene(new sceneModeSelect, true);
	}

	return true;
}
//
//=============================================================================================


//=============================================================================================
//		描			画
void sceneGradeResult::Render()
{
	
	tdnView::Clear();

	// 背景
	m_pImages[IMAGE::BACK]->Render(0, 0);

	// 上フレーム
	m_pImages[IMAGE::UP_FRAME]->Render(0, 0);

	// 上の見出し
	m_pImages[IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 384, 400, 64);

	// 段位認定 *段
	m_pImages[IMAGE::IMG_GRADE]->Render(128, 200, 300, 80, 0, 880, 300, 80);
	m_pImages[IMAGE::IMG_GRADE]->Render(400, 200, 300, 80, 0, (m_pGradeData->iSelectGrade) * 80, 300, 80);

	// フレーム
	m_pImages[IMAGE::FRAME]->Render(732, 224);

	// 結果(合格 or 不合格)
	m_pImages[IMAGE::RESULT]->Render(860, 552, (int)(156 * 1.5f), (int)(64 * 1.2f), 0, m_bPassed ? 0 : 46, 156, 46);


	/* 数字たち */
	FOR(NUM_GRADE_MUSIC)
	{
		// ゲージ
		int percent = m_pGradeData->FinesseGauge[i];
		int posY = 254 + i * 48;
		m_pImages[IMAGE::NUMBER]->SetARGB(0xffb1ffd7);
		m_pImages[IMAGE::NUMBER]->Render(956, posY, 24, 32, 276, 0, 24, 32);	// %
		if (percent > 0)
		{
			NumberRender(m_pImages[IMAGE::NUMBER].get(), 956 - 24, posY, 24, 32, percent, 3, true);
		}
		else m_pImages[IMAGE::NUMBER]->Render(956 - 24, posY, 24, 32, 0, 0, 24, 32);

		// 達成率
		percent = m_pGradeData->ClearRate[i];
		m_pImages[IMAGE::NUMBER]->SetARGB(0xffb3f6ff);
		m_pImages[IMAGE::NUMBER]->Render(1124, posY, 24, 32, 276, 0, 24, 32);	// %
		if (percent > 0)
		{
			NumberRender(m_pImages[IMAGE::NUMBER].get(), 1124 - 24, posY, 24, 32, percent, 3, true);
		}
		else m_pImages[IMAGE::NUMBER]->Render(1124 - 24, posY, 24, 32, 0, 0, 24, 32);
	}

	// 最終達成率
	if (m_Achievement > 0)
	{
		NumberRender(m_pImages[IMAGE::NUMBER_L].get(), 1050, 440, 32, 42, m_Achievement, 3, true);
	}
	else m_pImages[IMAGE::NUMBER_L]->Render(1050, 440, 32, 42, 0, 0, 32, 42);

	Fade::Render();	// フェード描画
}
//
//=============================================================================================
