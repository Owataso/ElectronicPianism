#include "TDNLIB.h"
#include "../Music/MusicInfo.h"
#include "../system/FrameWork.h"
#include "../system/System.h"
#include "scenePlaying.h"
#include "scenePlayingState.h"
#include "../Fade/Fade.h"
#include "../Music/PlayCursor.h"
#include "../Data/SelectMusicData.h"
#include "../Music/Note.h"
#include "../Music/Soflan.h"
#include "../Judge/Judge.h"
#include "../UI/ui.h"
#include "../Data/PlayerData.h"
#include "../Player/Player.h"
#include "sceneResult.h"
#include "../Data/MusicDataBase.h"
#include "../Mode/ModeManager.h"
#include "../Data/GameScore.h"
#include "../Music/BeatBar.h"
#include "../Input/InputMIDI.h"
#include "TDN_Movie.h"

#ifdef _DEBUG
#include "../BenchMark.h"
BenchMark g_BenchMark;
double g_NoteUpdateTime = 0;		// ノーツ更新にかかった時間
double g_MIDIPlayerUpdateTime = 0;	// MIDIピアノ関係にかかった時間
double g_JudgeUpdateTime = 0;		// 判定更新にかかった時間
#endif

//#define NO_WAIT_INTRO

//*****************************************************************************************************************************
//
//		再生状態
//
//*****************************************************************************************************************************
scenePlayingState::Playing::Playing(scenePlaying *pMain) :Base(pMain), m_bStreamStop(false)
{
	// 再生位置の初期化
	//play_cursor->Initialize();

	// 再生
	play_cursor->Play(SelectMusicMgr->Get()->omsInfo.PlayTime);

	// 動画があるなら再生
	if (pMain->m_pMovie)pMain->m_pMovie->Play();
}

//------------------------------------------------------
//	更新
//------------------------------------------------------
void scenePlayingState::Playing::Update()
{
	// rhythmバー更新
	m_pMain->m_pBeatBar->Update(play_cursor->GetMSecond());

#ifdef _DEBUG
	g_BenchMark.Start();
#endif
	// ノーツ更新
	m_pMain->m_pNoteMgr->Update(m_pMain->m_pJudgeMgr);
#ifdef _DEBUG
	g_BenchMark.End();
	g_NoteUpdateTime = g_BenchMark.GetTime();
#endif

	// ソフラン更新(★BPMが切り替わったらtrueが返るので、rhythmバーを切り替えてあげる)
	if(m_pMain->m_pSoflanMgr->Update()) m_pMain->m_pBeatBar->SetBPM(m_pMain->m_pSoflanMgr->GetBPM());

#ifdef _DEBUG
	g_BenchMark.Start();
#endif
	// 演奏者更新
	m_pMain->m_pPlayer->Update();
#ifdef _DEBUG
	g_BenchMark.End();
	g_MIDIPlayerUpdateTime = g_BenchMark.GetTime();
#endif

	// フルコン演出中だったら
	if (m_pMain->m_pJudgeMgr->isFullComboNow())
	{
		m_pMain->m_pJudgeMgr->Update();

		// 曲止めてなかったら
		if (!m_bStreamStop)
		{
			// 要は2重ストップ防止
			if (play_cursor->isEnd())
			{
				// 曲ストップ
				play_cursor->Stop();
				m_bStreamStop = true;
			}
		}

		// フルコン演出終わったら
		else if (m_pMain->m_pJudgeMgr->isFullComboEnd())
		{
			// 終了ステートへ
			m_pMain->ChangeMode(scenePlaying::SCENEPLAY_STATE::END);
		}
	}

	else
	{

		static float speed = 1.0f;
		if (KeyBoardTRG(KB_NUMPAD8))
		{
			play_cursor->SetSpeed((speed += .08f));
			m_pMain->m_bPractice = true;
		}
		else if (KeyBoardTRG(KB_NUMPAD2))
		{
			play_cursor->SetSpeed((speed -= .08f));
			m_pMain->m_bPractice = true;
		}

#ifdef _DEBUG
		g_BenchMark.Start();
#endif
		// 判定更新&閉店フラグチェック
		if (m_pMain->m_pJudgeMgr->Update() ||
			(KeyBoard(KB_SPACE) && KeyBoardTRG(KB_ENTER)))
		{
			// ここに入ったら閉店フラグ

			// 曲ストップ
			play_cursor->Stop();

			// 閉店ステートへ
			m_pMain->ChangeMode(scenePlaying::SCENEPLAY_STATE::FAILED);
		}

		// 再生終了したら
		else if (play_cursor->isEnd())
		{
			// 曲ストップ
			play_cursor->Stop();

			// 終了ステートへ
			m_pMain->ChangeMode(scenePlaying::SCENEPLAY_STATE::END);
		}
	}
#ifdef _DEBUG
	g_BenchMark.End();
	g_JudgeUpdateTime = g_BenchMark.GetTime();
#endif
}

//------------------------------------------------------
//	描画
//------------------------------------------------------
void scenePlayingState::Playing::Render()
{
	// UI(ノートとかも)描画
	//m_pMain->m_pUI->Render();
	m_pMain->m_pUI->RenderMusicName((char*)m_pMain->m_pMusicName);
	//tdnText::Draw(1090, 78, 0xffffffff, "EDIT");

	// 判定描画
	m_pMain->m_pJudgeMgr->Render();
	//Text::Draw(32, 64, 0xffffff00, "BPM : %.1f");
}



//*****************************************************************************************************************************
//
//		待機状態(曲名が出てる間の時間)
//
//*****************************************************************************************************************************
scenePlayingState::Wait::Wait(scenePlaying *pMain) : Base(pMain), 
m_iWaitTimer(0),
m_bReady(false),
m_bPushStart(false),
m_PushStart(PushStart()),
m_blind("DATA/UI/Playing/blind.png"),
m_Ready(Ready()),
m_pTextScreen(new tdn2DObj(1280, 720, tdn2D::USEALPHA)),
m_pMusicTextImage(new tdn2DObj((SelectMusicMgr->GetMusicPath() + "title.png").c_str()))
{
	// フェードインのセット
	Fade::Initialize();
	Fade::Set(Fade::MODE::FADE_IN, 5, 0, 255, 0x00000000);

	// シェーダーにセット
	shader2D->SetValue("BlindTex", m_blind.pTexture);
	shader2D->SetValue("blindRate", m_blind.rate);

	m_pMain->m_pNoteMgr->GetStartNotesLane(&m_PushStart.StartLaneList);
}
void scenePlayingState::Wait::Update()
{
	// フェード更新
	Fade::Update();

	// 演奏者更新
	m_pMain->m_pPlayer->Update();

	// GetReady?中
	if (m_bReady)
	{
		m_blind.rate = max(0, m_blind.rate - .05f);
		shader2D->SetValue("blindRate", m_blind.rate);

		// レディー完全に消えたら
		if (m_Ready.Update())
		{
			// 演奏ステートに行く
			m_pMain->ChangeMode(scenePlaying::SCENEPLAY_STATE::PLAYING);
		}
	}

	// 鍵盤を押してスタート！中
	else if (m_bPushStart)
	{
		m_blind.rate = max(0, m_blind.rate - .05f);
		shader2D->SetValue("blindRate", m_blind.rate);

		// 開始ノーツのところを全部押すとレディーに行く
		if (m_PushStart.Update()) m_bReady = true;
	}

	else
	{
		// 一定までフェードが進んでからブラインドを動かす(そうした方がよく見えるから)
		if (Fade::m_byAlpha < 128)
		{
			m_blind.rate = min(1, m_blind.rate + .05f);
			shader2D->SetValue("blindRate", m_blind.rate);
		}

#ifdef NO_WAIT_INTRO
		if (Fade::isFadeStop())
#else
		// 一定時間立ったらかつオプションボタン押してなかったら
		if (++m_iWaitTimer > 180 && KeyBoard(KB_SPACE) == 0)
#endif
		{
			// オートプレイだったら、鍵盤を押してスタートをする必要はない
			if (PlayerDataMgr->m_PlayerOption.bAutoPlay)
				m_bReady = true;

			// 通常プレイなら、鍵盤を押させてスタートする
			else
				m_bPushStart = true;
		}
	}
}
void scenePlayingState::Wait::Render()
{
	m_pTextScreen->RenderTarget();
	tdnView::Clear();

	// 曲名画像の描画
	// 曲名
	m_pMusicTextImage->Render(180, 220, 700, 90, 0, 0, 700, 90);
	// アーティスト名
	m_pMusicTextImage->Render(180, 500, 700, 38, 0, 90, 700, 38);

	m_blind.Render();

	// ↑で描画した絵をマスク情報に	
	shader2D->SetValue("BlindTex", m_blind.pMaskScreen);

	//	フレームバッファへ切り替え
	tdnSystem::GetDevice()->SetRenderTarget(0, m_pMain->m_pBackUp);

	// ブラインド描画
	m_pTextScreen->Render(0, 0, shader2D, "blind");

	// レディー描画
	if (m_bReady)
	{
		const bool l_bAuto(PlayerDataMgr->m_PlayerOption.bAutoPlay & 0x1);
		m_Ready.Render(l_bAuto);
		if(!l_bAuto)m_PushStart.RenderStartKeyboard();
	}

	// 鍵盤を押してスタート！
	else if (m_bPushStart)
	{
		m_PushStart.Render();
		m_PushStart.RenderStartKeyboard();
	}

	// フェードの描画
	Fade::Render();
}

bool scenePlayingState::Wait::PushStart::Update()
{
	static const float fAddCurve(.25f);
	fSinCurveRate += fAddCurve;

	// 押す鍵盤のアルファ値を設定
	pPushKeyImage->SetAlpha((BYTE)(255 * Math::Blend((sinf(fSinCurveRate) + 1) / 2, .2f, .5f)));
	
	// 指定キーをすべて押していたら、終了
	bool bAllPushed(true);

	for (auto it : StartLaneList)
	{
		// 全部押してない！
		if (!InputMIDIMgr->GetON(it))
			bAllPushed = false;
	}

	return bAllPushed;
}

void scenePlayingState::Wait::PushStart::Render()
{
	tdnFont::RenderString("鍵盤を押してスタート！", 30, 420, 512, 0x00ffffff | ((BYTE)(255 * Math::Blend((sinf(fSinCurveRate * .5f) + 1) / 2, .5f, .75f)) << 24));

	// 2オクターブ目の「ド」の鍵盤(廃止)
	//tdnPolygon::Triangle(276, 565, 40, 40, 0xffffffff, PI);

	// 開始レーン分回す
	for (auto it : StartLaneList)
	{
		tdnPolygon::Triangle(NOTE_POS_COMPTE(it), 565 + (int)(sinf(fSinCurveRate * .5f) * 5), 40, 40, 0xffffffff, PI);
	}
}


//*****************************************************************************************************************************
//
//		終了状態(フェード終わったら遷移)
//
//*****************************************************************************************************************************
scenePlayingState::End::End(scenePlaying *pMain) : Base(pMain)
{
	// フェードアウトセット
	Fade::Set(Fade::MODE::FADE_OUT, 4, 255, 0, 0x00000000);
}
void scenePlayingState::End::Update()
{
	Fade::Update();

	// フェード終わったら
	if (Fade::m_eMode == Fade::MODE::FADE_NONE)
	{
		bool bClear(true);

		// イージーかノーマルだったら
		if (m_pMain->m_GaugeOP == GAUGE_OPTION::EASY || m_pMain->m_GaugeOP == GAUGE_OPTION::NORMAL)
		{
			// 80以下ならクリア失敗
			if (GameScoreMgr->GetGameScore()->FinesseGauge < 80) bClear = false;
		}

		// リザルトに行く
		MainFrame->ChangeScene(new sceneResult(bClear, m_pMain->m_bPractice));
	}
}
void scenePlayingState::End::Render()
{
	// フェードの描画
	Fade::Render();
}



//*****************************************************************************************************************************
//
//		閉店
//
//*****************************************************************************************************************************
scenePlayingState::Failed::Failed(scenePlaying *pMain) :Base(pMain)
{
	// フェードアウトセット
	Fade::Set(Fade::MODE::FADE_OUT, 4, 255, 0, 0x00000000);
}

void scenePlayingState::Failed::Update()
{
	Fade::Update();

	// フェード終わったら
	if (Fade::m_eMode == Fade::MODE::FADE_NONE)
	{
		// リザルトに行く
		MainFrame->ChangeScene(new sceneResult(false, m_pMain->m_bPractice));
	}
}

void scenePlayingState::Failed::Render()
{
	// フェードの描画
	Fade::Render();
}