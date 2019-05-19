#include "TDNLIB.h"
#include "../system/Framework.h"
#include "../system/System.h"
#include "DebugRoom.h"
#include "sceneResult.h"
#include "sceneModeSelect.h"
#include "scenePlaying.h"
#include "sceneMusicSelect.h"
#include "../Data/GameScore.h"
#include "../Mode/ModeManager.h"
#include "../Data/PlayerData.h"
#include "../Data/SelectMusicData.h"
#include "../Effect/CyberCircle.h"
#include "../Music/BeatBar.h"
#include "../Music/PlayCursor.h"

//=============================================================================================
//		��	��	��	��	�J	��
bool DebugRoom::Initialize()
{
	// 2��Ă񂾂炠����H
	PlayerDataMgr->Initialize();

	m_iSelectNumber = (int)IKISAKI::MODE_SELECT;

	m_pCircle = new CyberCircle(Vector2(800, 160));
	m_pCircle->Action();

	m_pBeatBar = new BeatBar;	
	m_pBeatBar->SetBPM(96);

	m_pText			= std::make_unique<tdn2DObj>("DATA/Musics/EP/No_28/title.png");
	m_pScreen		= std::make_unique<tdn2DObj>(1280, 720, tdn2D::RENDERTARGET);
	m_pTextScreen	= std::make_unique<tdn2DObj>(1280, 720, tdn2D::USEALPHA);

	m_pBlind = new Blind("DATA/UI/Playing/blind.png");
	shader2D->SetValue("BlindTex", m_pBlind->pTexture);
	shader2D->SetValue("blindRate", m_pBlind->rate);

	tdnSystem::GetDevice()->GetRenderTarget(0, &m_pBackUp);

	play_cursor->SetMusic("DATA/Sound/BGM/Cyberspace_Breakroom.ogg");
	play_cursor->Play();

	return true;
}

DebugRoom::~DebugRoom()
{
	delete m_pBlind;
	delete m_pCircle;
	delete m_pBeatBar;
	play_cursor->Release();
}
//=============================================================================================


//=============================================================================================
//		�X			�V
bool DebugRoom::Update()
{
	if (KeyBoard(KB_UP)) m_pBlind->rate = max(m_pBlind->rate - .1f, 0);
	if (KeyBoard(KB_DOWN)) m_pBlind->rate = min(m_pBlind->rate + .1f, 1);
	shader2D->SetValue("blindRate", m_pBlind->rate);

	m_pCircle->Update();

	m_pBeatBar->Update(play_cursor->GetMSecond());

	// �s����ύX����
	if (KeyBoardTRG(KB_LEFT)) if(--m_iSelectNumber < 0) m_iSelectNumber = IKISAKI::MAX - 1;
	if (KeyBoardTRG(KB_RIGHT))if (++m_iSelectNumber >= IKISAKI::MAX) m_iSelectNumber = 0;

	// ����
	if (KeyBoardTRG(KB_ENTER))
	{
		switch ((IKISAKI)m_iSelectNumber)
		{
		case IKISAKI::CLASS_RESULT:
		{
									  ModeMgr->SetMode(PLAY_MODE::GRADE);
									  ModeMgr->SetSelectGrade((int)GRADE::BRILLIANCE);
									  //ModeMgr->SettingGradeMusic();

									  FOR(NUM_GRADE_MUSIC)
									  {
										  int JudgeCounts[(int)JUDGE_TYPE::MAX];
										  JudgeCounts[(int)JUDGE_TYPE::BAD] = 100 + rand() % 100;
										  JudgeCounts[(int)JUDGE_TYPE::GOOD] = 100 + rand() % 200;
										  JudgeCounts[(int)JUDGE_TYPE::GREAT] = 100 + rand() % 300;
										  JudgeCounts[(int)JUDGE_TYPE::SUPER] = 100 + rand() % 500;

										  ModeMgr->SetGradeClearRate(50 + rand() % 51, JudgeCounts, JudgeCounts[(int)JUDGE_TYPE::BAD] + JudgeCounts[(int)JUDGE_TYPE::GOOD] + JudgeCounts[(int)JUDGE_TYPE::GREAT] + JudgeCounts[(int)JUDGE_TYPE::SUPER]);	// ���[�h����ɐ��т�n��
										  ModeMgr->AddStep();
									  }
									  MainFrame->ChangeScene(new sceneGradeResult(true));
		}
			break;

		case IKISAKI::NORMAL_RESULT:
		{
									   SelectMusicData data(0, DIFFICULTY::VIRTUOSO);
									   if (!SelectMusicMgr->Set(data)) return true;

									   ModeMgr->SetMode(PLAY_MODE::FREE);

									   int remain(SelectMusicMgr->Get()->omsInfo.NumNotes);
									   GameScoreMgr->GetGameScore()->JudgeCount[0] = remain;
									   GameScoreMgr->GetGameScore()->JudgeCount[1] = 0;
									   GameScoreMgr->GetGameScore()->JudgeCount[2] = 0;
									   GameScoreMgr->GetGameScore()->JudgeCount[3] = 0;
									   GameScoreMgr->GetGameScore()->MaxCombo = remain;
									   GameScoreMgr->GetGameScore()->score = 100000;

									   MainFrame->ChangeScene(new sceneResult(true, true));
		}
			break;

		case IKISAKI::MODE_SELECT:
			MainFrame->ChangeScene(new sceneModeSelect, true);
			break;

		case IKISAKI::MUSIC_SELECT:
			ModeMgr->SetMode(PLAY_MODE::FREE);
			MainFrame->ChangeScene(new sceneMusicSelect);
			break;

		case IKISAKI::PLAYING:
		{
								 SelectMusicData data(22, DIFFICULTY::VIRTUOSO);
								 if (!SelectMusicMgr->Set(data)) return true;
								 PlayerDataMgr->m_PlayerOption.bAutoPlay = FALSE;
								 ModeMgr->SetMode(PLAY_MODE::FREE);

								 MainFrame->ChangeScene(new scenePlaying);
		}
			break;
		}

	}
	return true;
}
//
//=============================================================================================


//=============================================================================================
//		�`			��
void DebugRoom::Render()
{
	m_pTextScreen->RenderTarget();
	tdnView::Clear();

	m_pText->Render(160, 220, 700, 90, 0, 0, 700, 90);
	// �A�[�e�B�X�g��
	m_pText->Render(160, 500, 700, 38, 0, 90, 700, 38);

	// �u���C���h
	m_pBlind->Render();
	// �u���C���h�ŕ`�悵���G���}�X�N����	
	shader2D->SetValue("BlindTex", m_pBlind->pMaskScreen);

	m_pScreen->RenderTarget();
	tdnView::Clear();

	tdnPolygon::Rect(0, 0, 1280, 720, RS::COPY, 0xff008080);
	m_pTextScreen->Render(0, 0, shader2D, "blind");

	// �O�p�`�e�X�g
	static float angle(0);
	if (KeyBoard(KB_UP)) angle += .1f;
	if (KeyBoard(KB_DOWN)) angle -= .1f;
	tdnPolygon::Triangle(0, 0, 512, 128, 0xff00ff00, angle);

	// �����΁[�����
	m_pCircle->Render();

	// rhythm�o�[
	m_pBeatBar->Render(0, 610);

	// �s����
	LPCSTR ikisaki[IKISAKI::MAX]{"�i�ʃ��U���g", "���U���g", "���[�h�Z���N�g", "�ȃZ���N�g", "���t���"};
	tdnText::Draw(640, 360, 0xffffffff, "�s����: %s", ikisaki[m_iSelectNumber]);

	//	�t���[���o�b�t�@�֐؂�ւ�
	tdnSystem::GetDevice()->SetRenderTarget(0, m_pBackUp);
	m_pScreen->Render(0,0);
}
//
//=============================================================================================