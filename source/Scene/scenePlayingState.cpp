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
double g_NoteUpdateTime = 0;		// �m�[�c�X�V�ɂ�����������
double g_MIDIPlayerUpdateTime = 0;	// MIDI�s�A�m�֌W�ɂ�����������
double g_JudgeUpdateTime = 0;		// ����X�V�ɂ�����������
#endif

//#define NO_WAIT_INTRO

//*****************************************************************************************************************************
//
//		�Đ����
//
//*****************************************************************************************************************************
scenePlayingState::Playing::Playing(scenePlaying *pMain) :Base(pMain), m_bStreamStop(false)
{
	// �Đ��ʒu�̏�����
	//play_cursor->Initialize();

	// �Đ�
	play_cursor->Play(SelectMusicMgr->Get()->omsInfo.PlayTime);

	// ���悪����Ȃ�Đ�
	if (pMain->m_pMovie)pMain->m_pMovie->Play();
}

//------------------------------------------------------
//	�X�V
//------------------------------------------------------
void scenePlayingState::Playing::Update()
{
	// rhythm�o�[�X�V
	m_pMain->m_pBeatBar->Update(play_cursor->GetMSecond());

#ifdef _DEBUG
	g_BenchMark.Start();
#endif
	// �m�[�c�X�V
	m_pMain->m_pNoteMgr->Update(m_pMain->m_pJudgeMgr);
#ifdef _DEBUG
	g_BenchMark.End();
	g_NoteUpdateTime = g_BenchMark.GetTime();
#endif

	// �\�t�����X�V(��BPM���؂�ւ������true���Ԃ�̂ŁArhythm�o�[��؂�ւ��Ă�����)
	if(m_pMain->m_pSoflanMgr->Update()) m_pMain->m_pBeatBar->SetBPM(m_pMain->m_pSoflanMgr->GetBPM());

#ifdef _DEBUG
	g_BenchMark.Start();
#endif
	// ���t�ҍX�V
	m_pMain->m_pPlayer->Update();
#ifdef _DEBUG
	g_BenchMark.End();
	g_MIDIPlayerUpdateTime = g_BenchMark.GetTime();
#endif

	// �t���R�����o����������
	if (m_pMain->m_pJudgeMgr->isFullComboNow())
	{
		m_pMain->m_pJudgeMgr->Update();

		// �Ȏ~�߂ĂȂ�������
		if (!m_bStreamStop)
		{
			// �v��2�d�X�g�b�v�h�~
			if (play_cursor->isEnd())
			{
				// �ȃX�g�b�v
				play_cursor->Stop();
				m_bStreamStop = true;
			}
		}

		// �t���R�����o�I�������
		else if (m_pMain->m_pJudgeMgr->isFullComboEnd())
		{
			// �I���X�e�[�g��
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
		// ����X�V&�X�t���O�`�F�b�N
		if (m_pMain->m_pJudgeMgr->Update() ||
			(KeyBoard(KB_SPACE) && KeyBoardTRG(KB_ENTER)))
		{
			// �����ɓ�������X�t���O

			// �ȃX�g�b�v
			play_cursor->Stop();

			// �X�X�e�[�g��
			m_pMain->ChangeMode(scenePlaying::SCENEPLAY_STATE::FAILED);
		}

		// �Đ��I��������
		else if (play_cursor->isEnd())
		{
			// �ȃX�g�b�v
			play_cursor->Stop();

			// �I���X�e�[�g��
			m_pMain->ChangeMode(scenePlaying::SCENEPLAY_STATE::END);
		}
	}
#ifdef _DEBUG
	g_BenchMark.End();
	g_JudgeUpdateTime = g_BenchMark.GetTime();
#endif
}

//------------------------------------------------------
//	�`��
//------------------------------------------------------
void scenePlayingState::Playing::Render()
{
	// UI(�m�[�g�Ƃ���)�`��
	//m_pMain->m_pUI->Render();
	m_pMain->m_pUI->RenderMusicName((char*)m_pMain->m_pMusicName);
	//tdnText::Draw(1090, 78, 0xffffffff, "EDIT");

	// ����`��
	m_pMain->m_pJudgeMgr->Render();
	//Text::Draw(32, 64, 0xffffff00, "BPM : %.1f");
}



//*****************************************************************************************************************************
//
//		�ҋ@���(�Ȗ����o�Ă�Ԃ̎���)
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
	// �t�F�[�h�C���̃Z�b�g
	Fade::Initialize();
	Fade::Set(Fade::MODE::FADE_IN, 5, 0, 255, 0x00000000);

	// �V�F�[�_�[�ɃZ�b�g
	shader2D->SetValue("BlindTex", m_blind.pTexture);
	shader2D->SetValue("blindRate", m_blind.rate);

	m_pMain->m_pNoteMgr->GetStartNotesLane(&m_PushStart.StartLaneList);
}
void scenePlayingState::Wait::Update()
{
	// �t�F�[�h�X�V
	Fade::Update();

	// ���t�ҍX�V
	m_pMain->m_pPlayer->Update();

	// GetReady?��
	if (m_bReady)
	{
		m_blind.rate = max(0, m_blind.rate - .05f);
		shader2D->SetValue("blindRate", m_blind.rate);

		// ���f�B�[���S�ɏ�������
		if (m_Ready.Update())
		{
			// ���t�X�e�[�g�ɍs��
			m_pMain->ChangeMode(scenePlaying::SCENEPLAY_STATE::PLAYING);
		}
	}

	// ���Ղ������ăX�^�[�g�I��
	else if (m_bPushStart)
	{
		m_blind.rate = max(0, m_blind.rate - .05f);
		shader2D->SetValue("blindRate", m_blind.rate);

		// �J�n�m�[�c�̂Ƃ����S�������ƃ��f�B�[�ɍs��
		if (m_PushStart.Update()) m_bReady = true;
	}

	else
	{
		// ���܂Ńt�F�[�h���i��ł���u���C���h�𓮂���(�������������悭�����邩��)
		if (Fade::m_byAlpha < 128)
		{
			m_blind.rate = min(1, m_blind.rate + .05f);
			shader2D->SetValue("blindRate", m_blind.rate);
		}

#ifdef NO_WAIT_INTRO
		if (Fade::isFadeStop())
#else
		// ��莞�ԗ������炩�I�v�V�����{�^�������ĂȂ�������
		if (++m_iWaitTimer > 180 && KeyBoard(KB_SPACE) == 0)
#endif
		{
			// �I�[�g�v���C��������A���Ղ������ăX�^�[�g������K�v�͂Ȃ�
			if (PlayerDataMgr->m_PlayerOption.bAutoPlay)
				m_bReady = true;

			// �ʏ�v���C�Ȃ�A���Ղ��������ăX�^�[�g����
			else
				m_bPushStart = true;
		}
	}
}
void scenePlayingState::Wait::Render()
{
	m_pTextScreen->RenderTarget();
	tdnView::Clear();

	// �Ȗ��摜�̕`��
	// �Ȗ�
	m_pMusicTextImage->Render(180, 220, 700, 90, 0, 0, 700, 90);
	// �A�[�e�B�X�g��
	m_pMusicTextImage->Render(180, 500, 700, 38, 0, 90, 700, 38);

	m_blind.Render();

	// ���ŕ`�悵���G���}�X�N����	
	shader2D->SetValue("BlindTex", m_blind.pMaskScreen);

	//	�t���[���o�b�t�@�֐؂�ւ�
	tdnSystem::GetDevice()->SetRenderTarget(0, m_pMain->m_pBackUp);

	// �u���C���h�`��
	m_pTextScreen->Render(0, 0, shader2D, "blind");

	// ���f�B�[�`��
	if (m_bReady)
	{
		const bool l_bAuto(PlayerDataMgr->m_PlayerOption.bAutoPlay & 0x1);
		m_Ready.Render(l_bAuto);
		if(!l_bAuto)m_PushStart.RenderStartKeyboard();
	}

	// ���Ղ������ăX�^�[�g�I
	else if (m_bPushStart)
	{
		m_PushStart.Render();
		m_PushStart.RenderStartKeyboard();
	}

	// �t�F�[�h�̕`��
	Fade::Render();
}

bool scenePlayingState::Wait::PushStart::Update()
{
	static const float fAddCurve(.25f);
	fSinCurveRate += fAddCurve;

	// �������Ղ̃A���t�@�l��ݒ�
	pPushKeyImage->SetAlpha((BYTE)(255 * Math::Blend((sinf(fSinCurveRate) + 1) / 2, .2f, .5f)));
	
	// �w��L�[�����ׂĉ����Ă�����A�I��
	bool bAllPushed(true);

	for (auto it : StartLaneList)
	{
		// �S�������ĂȂ��I
		if (!InputMIDIMgr->GetON(it))
			bAllPushed = false;
	}

	return bAllPushed;
}

void scenePlayingState::Wait::PushStart::Render()
{
	tdnFont::RenderString("���Ղ������ăX�^�[�g�I", 30, 420, 512, 0x00ffffff | ((BYTE)(255 * Math::Blend((sinf(fSinCurveRate * .5f) + 1) / 2, .5f, .75f)) << 24));

	// 2�I�N�^�[�u�ڂ́u�h�v�̌���(�p�~)
	//tdnPolygon::Triangle(276, 565, 40, 40, 0xffffffff, PI);

	// �J�n���[������
	for (auto it : StartLaneList)
	{
		tdnPolygon::Triangle(NOTE_POS_COMPTE(it), 565 + (int)(sinf(fSinCurveRate * .5f) * 5), 40, 40, 0xffffffff, PI);
	}
}


//*****************************************************************************************************************************
//
//		�I�����(�t�F�[�h�I�������J��)
//
//*****************************************************************************************************************************
scenePlayingState::End::End(scenePlaying *pMain) : Base(pMain)
{
	// �t�F�[�h�A�E�g�Z�b�g
	Fade::Set(Fade::MODE::FADE_OUT, 4, 255, 0, 0x00000000);
}
void scenePlayingState::End::Update()
{
	Fade::Update();

	// �t�F�[�h�I�������
	if (Fade::m_eMode == Fade::MODE::FADE_NONE)
	{
		bool bClear(true);

		// �C�[�W�[���m�[�}����������
		if (m_pMain->m_GaugeOP == GAUGE_OPTION::EASY || m_pMain->m_GaugeOP == GAUGE_OPTION::NORMAL)
		{
			// 80�ȉ��Ȃ�N���A���s
			if (GameScoreMgr->GetGameScore()->FinesseGauge < 80) bClear = false;
		}

		// ���U���g�ɍs��
		MainFrame->ChangeScene(new sceneResult(bClear, m_pMain->m_bPractice));
	}
}
void scenePlayingState::End::Render()
{
	// �t�F�[�h�̕`��
	Fade::Render();
}



//*****************************************************************************************************************************
//
//		�X
//
//*****************************************************************************************************************************
scenePlayingState::Failed::Failed(scenePlaying *pMain) :Base(pMain)
{
	// �t�F�[�h�A�E�g�Z�b�g
	Fade::Set(Fade::MODE::FADE_OUT, 4, 255, 0, 0x00000000);
}

void scenePlayingState::Failed::Update()
{
	Fade::Update();

	// �t�F�[�h�I�������
	if (Fade::m_eMode == Fade::MODE::FADE_NONE)
	{
		// ���U���g�ɍs��
		MainFrame->ChangeScene(new sceneResult(false, m_pMain->m_bPractice));
	}
}

void scenePlayingState::Failed::Render()
{
	// �t�F�[�h�̕`��
	Fade::Render();
}