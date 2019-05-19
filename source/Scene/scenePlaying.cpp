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
//		��		��		��	
//
//*****************************************************************************************************************************
bool scenePlaying::Initialize()
{
	// �r���[
	tdnView::Init();

	// ����
	if (SelectMusicMgr->Get()->bMovieExist)
	{
		char path[256];
		sprintf_s(path, 256, "DATA/Musics/EP/No_%d/movie.wmv", SelectMusicMgr->Get()->byID);
		m_pMovie = new tdnMovie(path, false, false);
		m_pMovieImage = new tdn2DObj(m_pMovie->GetTexture());
		m_pMovieImage->SetARGB((PlayerDataMgr->m_PlayerOption.MovieAlpha << 24) | 0x00ffffff);	// ���擧���x
	}
	else
	{
		// ���斳��
		m_pMovie = nullptr, m_pMovieImage = nullptr;
	}

	// �\�t����
	m_pSoflanMgr = new SoflanManager;

	// �m�[�c
	m_pNoteMgr = new NoteManager;
	m_pNoteMgr->Initialize(m_pSoflanMgr);

	// �\�t�����ƃm�[�c�̏����Z�b�e�B���O
	m_pSoflanMgr->Set(SelectMusicMgr->Get());
	m_pNoteMgr->Set(SelectMusicMgr->Get());

	// rhythm�o�[
	m_pBeatBar = new BeatBar;

	// �Q�[�W�ݒ�A�i�ʃQ�[�W�͕ʘg(�v���C���[�f�[�^�ɂ͕ۑ��������Ȃ��̂ŁB�i�ʃQ�[�W�͒i�ʔF���p�Q�[�W�ł����āA���ʂɕۑ������ƍ���)
	m_GaugeOP = (ModeMgr->GetMode() == PLAY_MODE::GRADE) ? GAUGE_OPTION::GRADE : (GAUGE_OPTION)PlayerDataMgr->m_PlayerOption.iGaugeOption;

	// UI
	const int l_cOctaveWidth(SelectMusicMgr->Get()->omsInfo.OctaveWidth);
	m_pUI = new UI(l_cOctaveWidth);
	m_pUI->Initialize(m_pBeatBar, m_pNoteMgr, m_pSoflanMgr, m_GaugeOP);

	// ����
	m_pJudgeMgr = new JudgeManager;
	m_pJudgeMgr->Initialize(m_pNoteMgr, m_GaugeOP, (PlayerDataMgr->m_PlayerOption.bShowTiming == TRUE));

	// �v���C���[
	m_pPlayer = new Player;
	m_pPlayer->Initialize(l_cOctaveWidth, m_pUI, m_pNoteMgr, m_pJudgeMgr, PlayerDataMgr->m_PlayerOption.bAutoPlay ? Player::MODE::AUTO : Player::MODE::YOU);	// DJ AUTO���񕪊�

	// �Đ�����Ȃ̔ԍ�
	MusicInfo *pInfo = SelectMusicMgr->Get();
	if (pInfo->omsInfo.PlayMusicNo != 0)
	{
		// �Đ�����Ȃ̐ݒ�
		char path[256];
		sprintf_s(path, 256, "DATA/Musics/EP/No_%d/Music%d.ogg", pInfo->byID, pInfo->omsInfo.PlayMusicNo);

		// �Đ��N�ɐݒ�
		play_cursor->SetMusic(path);
	}
	play_cursor->SetShift(pInfo->omsInfo.shift);

	// �x�[�X�I�N�^�[�u�ݒ�
	InputMIDIMgr->SetBaseOctave(pInfo->omsInfo.BaseOctave);

	// �Ȗ�
	FOR(MUSIC_DATA_BASE->GetNumEPContents())
	{
		if (MUSIC_DATA_BASE->GetEPContentsData(i)->byID == pInfo->byID)
		{
			m_pMusicName = MUSIC_DATA_BASE->GetEPContentsData(i)->sMusicName.c_str();
		}
	}


	// �X�e�[�g
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
//		�又��
//
//*****************************************************************************************************************************
bool scenePlaying::Update()
{
	// ���[�h�X�V
	m_pState->Update();

	return true;
}

//*****************************************************************************************************************************
//
//		�`��֘A
//
//*****************************************************************************************************************************
void scenePlaying::Render()
{
	tdnView::Clear();

	// UI�̔w�i�`��
	m_pUI->RenderBack();

	// ����摜����Ȃ�
	if (m_pMovieImage)
	{
		// �����]��
		const DWORD AlphaColor = ((PlayerDataMgr->m_PlayerOption.MovieAlpha << 24) | 0x00000000);
		tdnPolygon::Rect(0, 133, 1120, 497, RS::COPY, AlphaColor);

		// ����`��
		m_pMovieImage->Render(120, 133, 880, 497, 0, 0, 1280, 720);
	}

	// UI�̔���Ƃ��m�[�c�Ƃ�������O�ɏo���������
	m_pUI->Render();

	// ���[�h�̕`��
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