#include "TDNLIB.h"
#include "../Music/MusicInfo.h"
#include "../system/FrameWork.h"
#include "scenePlaying.h"
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
#include "scenePlayingState.h"
#include "TDN_Movie.h"

//*****************************************************************************************************************************
//
//		初		期		化	
//
//*****************************************************************************************************************************
bool scenePlaying::Initialize()
{
	// ビュー
	tdnView::Init();

	// 動画
	if (SelectMusicMgr->Get()->bMovieExist)
	{
		char path[256];
		sprintf_s(path, 256, "DATA/Musics/EP/No_%d/movie.wmv", SelectMusicMgr->Get()->byID);
		m_pMovie = new tdnMovie(path, false, false);
		m_pMovieImage = new tdn2DObj(m_pMovie->GetTexture());
		m_pMovieImage->SetARGB((PlayerDataMgr->m_PlayerOption.MovieAlpha << 24) | 0x00ffffff);	// 動画透明度
	}
	else
	{
		// 動画無し
		m_pMovie = nullptr, m_pMovieImage = nullptr;
	}

	// ソフラン
	m_pSoflanMgr = new SoflanManager;

	// ノーツ
	m_pNoteMgr = new NoteManager;
	m_pNoteMgr->Initialize(m_pSoflanMgr);

	// ソフランとノーツの情報をセッティング
	m_pSoflanMgr->Set(SelectMusicMgr->Get());
	m_pNoteMgr->Set(SelectMusicMgr->Get());

	// rhythmバー
	m_pBeatBar = new BeatBar;

	// ゲージ設定、段位ゲージは別枠(プレイヤーデータには保存したくないので。段位ゲージは段位認定専用ゲージであって、普通に保存されると困る)
	m_GaugeOP = (ModeMgr->GetMode() == PLAY_MODE::GRADE) ? GAUGE_OPTION::GRADE : (GAUGE_OPTION)PlayerDataMgr->m_PlayerOption.iGaugeOption;

	// UI
	const int l_cOctaveWidth(SelectMusicMgr->Get()->omsInfo.OctaveWidth);
	m_pUI = new UI(l_cOctaveWidth);
	m_pUI->Initialize(m_pBeatBar, m_pNoteMgr, m_pSoflanMgr, m_GaugeOP);

	// 判定
	m_pJudgeMgr = new JudgeManager;
	m_pJudgeMgr->Initialize(m_pNoteMgr, m_GaugeOP, (PlayerDataMgr->m_PlayerOption.bShowTiming == TRUE));

	// プレイヤー
	m_pPlayer = new Player;
	m_pPlayer->Initialize(l_cOctaveWidth, m_pUI, m_pNoteMgr, m_pJudgeMgr, PlayerDataMgr->m_PlayerOption.bAutoPlay ? Player::MODE::AUTO : Player::MODE::YOU);	// DJ AUTOさん分岐

	// 再生する曲の番号
	MusicInfo *pInfo = SelectMusicMgr->Get();
	if (pInfo->omsInfo.PlayMusicNo != 0)
	{
		// 再生する曲の設定
		char path[256];
		sprintf_s(path, 256, "DATA/Musics/EP/No_%d/Music%d.ogg", pInfo->byID, pInfo->omsInfo.PlayMusicNo);

		// 再生君に設定
		play_cursor->SetMusic(path);
	}
	play_cursor->SetShift(pInfo->omsInfo.shift);

	// ベースオクターブ設定
	InputMIDIMgr->SetBaseOctave(pInfo->omsInfo.BaseOctave);

	// 曲名
	FOR(MUSIC_DATA_BASE->GetNumEPContents())
	{
		if (MUSIC_DATA_BASE->GetEPContentsData(i)->byID == pInfo->byID)
		{
			m_pMusicName = MUSIC_DATA_BASE->GetEPContentsData(i)->sMusicName.c_str();
		}
	}


	// ステート
	m_pState = new scenePlayingState::Wait(this);

	m_bPractice = false;

	tdnSystem::GetDevice()->GetRenderTarget(0, &m_pBackUp);

	return true;
}

scenePlaying::~scenePlaying()
{
	play_cursor->Release();
	delete m_pNoteMgr;
	delete m_pUI;
	delete m_pJudgeMgr;
	delete m_pSoflanMgr;
	delete m_pState;
	delete m_pPlayer;
	delete m_pBeatBar;
	SAFE_DELETE(m_pMovieImage);
	SAFE_DELETE(m_pMovie);
}

//*****************************************************************************************************************************
//
//		主処理
//
//*****************************************************************************************************************************
bool scenePlaying::Update()
{
	// モード更新
	m_pState->Update();

	return true;
}

//*****************************************************************************************************************************
//
//		描画関連
//
//*****************************************************************************************************************************
void scenePlaying::Render()
{
	tdnView::Clear();

	// UIの背景描画
	m_pUI->RenderBack();

	// 動画画像あるなら
	if (m_pMovieImage)
	{
		// 黒い余白
		const DWORD AlphaColor = ((PlayerDataMgr->m_PlayerOption.MovieAlpha << 24) | 0x00000000);
		tdnPolygon::Rect(0, 133, 1120, 497, RS::COPY, AlphaColor);

		// 動画描画
		m_pMovieImage->Render(120, 133, 880, 497, 0, 0, 1280, 720);
	}

	// UIの判定とかノーツとか動画より前に出したいやつ
	m_pUI->Render();

	// モードの描画
	m_pState->Render();
}



void scenePlaying::ChangeMode(SCENEPLAY_STATE state)
{
	if (m_pState) delete m_pState;

	switch (state)
	{
	case SCENEPLAY_STATE::WAIT:
		m_pState = new scenePlayingState::Wait(this);
		break;
	case SCENEPLAY_STATE::PLAYING:
		m_pState = new scenePlayingState::Playing(this);
		break;
	case SCENEPLAY_STATE::END:
		m_pState = new scenePlayingState::End(this);
		break;
	case scenePlaying::SCENEPLAY_STATE::FAILED:
		m_pState = new scenePlayingState::Failed(this);
		break;
	}
}