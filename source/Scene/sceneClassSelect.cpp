#include "TDNLIB.h"
#include "../Sound/SoundManager.h"
#include "system/Framework.h"
#include "sceneModeSelect.h"
#include "sceneClassSelect.h"
#include "scenePlaying.h"
#include "../Data/PlayerData.h"
#include "../Input/InputMIDI.h"
#include "../Fade/Fade.h"
#include "../Mode/ModeManager.h"
#include "../Data/SelectMusicData.h"
#include "../Number/Number.h"

//=============================================================================================
//		�t���[���Ϗ�����
sceneClassSelect::Frame::Frame(int cursor, int grade, int rank, Texture2D *pFrame, Texture2D *pText) :
	grade(grade), 
	rank(rank), 
	m_pFrame(std::make_unique<tdn2DAnim>(pFrame)), 
	m_pText(std::make_unique<tdn2DAnim>(pText))
{
	SetCursor(cursor);
	pos = NextPos;

	// �g�����ɃA�N�V��������
	m_pFrame->OrderMoveAppeared(8, (int)pos.x - 600, (int)pos.y);
	m_pText->OrderMoveAppeared(8, (int)pos.x - 600, (int)pos.y);

	// �^�񒆂ɋ߂���΋߂��قǁA������������
	const int r(18 + abs(cursor - grade) * 5);
	m_pFrame->Action(r);
	m_pText->Action(r);
}

void sceneClassSelect::Frame::Update()
{
	pos = pos*.75f + NextPos*.25f;

	// �A�j���[�V�����̍X�V
	m_pFrame->Update();
	m_pText->Update();
}

void sceneClassSelect::Frame::Render()
{
	if (pos.y > 720 || pos.y < -128) return;	// �g����o�Ă���A�łĂ������I�I

	// �g�`��
	m_pFrame->Render((int)pos.x, (int)pos.y, 420, 128, 0, rank * 128, 420, 128);

	// �g�̒��̃e�L�X�g�`��
	m_pText->Render((int)pos.x + 135, (int)pos.y + 29, 150, 72, 0, grade * 72, 150, 72);
}

void sceneClassSelect::Frame::SetCursor(int cursor)
{
	// �J�[�\���ƒi�ʂ̒l�ɉ����č��W��ݒ�

	// �����̔ԍ��ƑI�����Ă��̍���
	int sabun = cursor - grade;

	NextPos.x = abs(sabun) * 32.0f + 800;
	NextPos.y = (sabun * 150.0f) + 256;
}
//=============================================================================================


//=============================================================================================
//		��	��	��	��	�J	��
bool sceneClassSelect::Initialize()
{
	// �t�F�[�h������
	Fade::Set(Fade::MODE::FADE_IN, 5, 0x00ffffff);

	// �V�[���ɓ����Ă���̑����t�܂ł̎���
	m_WaitTimer = 28;

	m_fLoadPercentage = .05f;	// ���[�h����

	// �摜������
	m_pImages[(int)IMAGE::BACK]				= std::make_unique<tdn2DObj>("DATA/Image/back2.png");
	m_pImages[(int)IMAGE::GRADE_TEXT]		= std::make_unique<tdn2DObj>("DATA/UI/Grade/text.png");
	m_fLoadPercentage = .1f;	// ���[�h����
	m_pImages[(int)IMAGE::NUMBER]			= std::make_unique<tdn2DObj>("DATA/Number/numberL.png");
	m_pImages[(int)IMAGE::SELECT_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Grade/select_frame.png");
	m_pImages[(int)IMAGE::FRAME]			= std::make_unique<tdn2DObj>("DATA/UI/Grade/frame.png");
	m_pImages[(int)IMAGE::FRAME_IN_TEXT]	= std::make_unique<tdn2DObj>("DATA/UI/Grade/grade.png");
	m_fLoadPercentage = .15f;	// ���[�h����

	// �A�j���[�V�����摜������
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI] = std::make_unique<tdn2DAnim>("DATA/UI/Other/midashi.png");
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->OrderMoveAppeared(16, -8, 12);
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Action(15);
	m_fLoadPercentage = .2f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/up_frame.png");
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->OrderMoveAppeared(30, 0, -64);
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Action(0);
	m_fLoadPercentage = .25f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT1] = std::make_unique<tdn2DAnim>(m_pImages[(int)IMAGE::GRADE_TEXT]->GetTexture());
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT1]->OrderMoveAppeared(15, 128 - 64, 200);
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT1]->Action(10);
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT2] = std::make_unique<tdn2DAnim>(m_pImages[(int)IMAGE::GRADE_TEXT]->GetTexture());
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT2]->OrderMoveAppeared(15, 400, 200 - 10);
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT2]->Action(10);
	m_fLoadPercentage = .3f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_FRAME] = std::make_unique<tdn2DAnim>(m_pImages[(int)IMAGE::SELECT_FRAME]->GetTexture());
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_FRAME]->OrderRipple(20, 1.0, .01f);
	m_fLoadPercentage = .35f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::PASS] = std::make_unique<tdn2DAnim>("DATA/UI/Grade/pass.png");
	m_pAnimImages[(int)ANIM_IMAGE::PASS]->OrderJump(5, 1.0f, .2f);
	m_pAnimImages[(int)ANIM_IMAGE::PASS]->Action(5);
	m_fLoadPercentage = .4f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/score.png");
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->OrderStretch(15, 1, .0f);
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->Action(10);
	m_fLoadPercentage = .45f;	// ���[�h����
	m_fLoadPercentage = .5f;	// ���[�h����
	m_fLoadPercentage = 1.0f;	// ���[�h����

	// �J�[�\��������
	m_iCursor = (int)PlayerDataMgr->m_grade;	// �����������i���Ă���ő�̒i�ʂɃJ�[�\�������킹��
	if (m_iCursor == (int)GRADE::NONE) m_iCursor = (int)GRADE::FIRST;

	// �g�Ϗ�����
	m_pFrames = new Frame*[(int)GRADE::MAX];
	FOR((int)GRADE::MAX)
	{
		int rank;
		switch ((GRADE)(i))
		{
		case GRADE::FIRST:
		case GRADE::SECOND:
		case GRADE::THERD:
		case GRADE::FOURTH:
			rank = 0;	// ��
			break;
		case GRADE::FIFTH:
		case GRADE::SIXTH:
		case GRADE::SEVENTH:
		case GRADE::EIGHTH:
			rank = 1;	// ��
			break;
		case GRADE::NINTH:
		case GRADE::TENTH:
			rank = 2;	// ��
			break;
		case GRADE::BRILLIANCE:
			rank = 3;	// �v���`�i
			break;
		}
		m_pFrames[i] = new Frame(m_iCursor, i, rank, m_pImages[(int)IMAGE::FRAME]->GetTexture(), m_pImages[(int)IMAGE::FRAME_IN_TEXT]->GetTexture());
	}

	g_pBGM->PlayStreamIn("DATA/Customize/SelectMusic/Cyberspace_Walkers/music.ogg");

	return true;
}

sceneClassSelect::~sceneClassSelect()
{
	g_pBGM->StopStreamIn();
	DOUBLE_POINTER_DELETE(m_pFrames, (int)GRADE::MAX);
}
//=============================================================================================


//=============================================================================================
//		�X			�V
bool sceneClassSelect::Update()
{
	static bool bEnd(false);

	// �A�j���[�V�����摜�X�V
	FOR((int)ANIM_IMAGE::MAX) m_pAnimImages[i]->Update();

	// �g����
	FOR((int)GRADE::MAX) m_pFrames[i]->Update();

	// �t�F�[�h
	Fade::Update();

	if (bEnd)
	{
		// �t�F�[�h�I�������A��
		if (Fade::m_eMode == Fade::MODE::FADE_NONE)
		{
			bEnd = false;
			MainFrame->ChangeScene(new sceneModeSelect, true);
		}
	}
	else
	{
		if (m_WaitTimer > 0){ m_WaitTimer--; return true; }

		bool bCursored(false);
		// �㉺�őI��
		if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
		{
			if (m_iCursor > (int)GRADE::FIRST)
			{
				m_iCursor--;
				bCursored = true;
			}
		}
		else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
		{
			if (m_iCursor < (int)GRADE::MAX - 1)
			{
				m_iCursor++;
				bCursored = true;
			}
		}
		if (bCursored)
		{
			g_pSE->Play("�J�[�\��");

			// �g�����ɍ��̃J�[�\����`����
			FOR((int)GRADE::MAX) m_pFrames[i]->SetCursor(m_iCursor);

			// �A�j���[�V�����𔭓�������
			m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT2]->Action(4);
			m_pAnimImages[(int)ANIM_IMAGE::SELECT_FRAME]->Action();
			m_pAnimImages[(int)ANIM_IMAGE::PASS]->Action();
		}

		// �G���^�[�L�[�Ō���
		if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
		{
			// �I�[�g�v���C��������
			PlayerDataMgr->m_PlayerOption.bAutoPlay = FALSE;

			// ���[�h�Ǘ��ɑI�������i�ʂ̔ԍ���n���Ə���ɋȂ̃f�[�^���Ƃ��Ă��Ă����
			ModeMgr->SetSelectGrade(m_iCursor);
			ModeMgr->SettingGradeMusic();		// �I�ȊǗ�����ɋȂ�ݒ肳����

			// �v���C�V�[����
			MainFrame->ChangeScene(new scenePlaying);
		}

		// �X�y�[�X�L�[�Ŕ�����
		else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
		{
			g_pSE->Play("�߂�");

			// �t�F�[�h�A�E�g�ݒ�
			Fade::Set(Fade::MODE::FADE_OUT, 4);
			bEnd = true;
		}
	}


	return true;
}
//
//=============================================================================================


//=============================================================================================
//		�`			��
void sceneClassSelect::Render()
{
	
	tdnView::Clear();

	// �w�i
	m_pImages[(int)IMAGE::BACK]->Render(0, 0);

	// �g����
	FOR((int)GRADE::MAX) m_pFrames[i]->Render();

	// �t���[��
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Render(0, 0);

	// CLASS SELECT
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 128, 400, 64);

	// �X�R�A�g�`��
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->Render(232, 540);

	// �X�R�A�g�̒��̕`��
	if (m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->GetAction()->IsEnd())
	{
		// ���i�E�s���i
		int srcY;
		int percent(PlayerDataMgr->m_GradeRecords[m_iCursor]->AchievementPct);
		if (PlayerDataMgr->m_GradeRecords[m_iCursor]->bPass) srcY = 0;
		else srcY = (percent > 0) ? 46 : 46 * 2;
		m_pAnimImages[(int)ANIM_IMAGE::PASS]->Render(278, 578, 156, 46, 0, srcY, 156, 46);

		// �B����
		m_pAnimImages[(int)ANIM_IMAGE::PASS]->Render(420, 600, 156, 23, 0, 46 * 3, 156, 23);
		m_pImages[(int)IMAGE::NUMBER]->Render(652, 580, 32, 42, 320, 0, 32, 42);	// %
		if (percent > 0)
		{
			// �̐�����ԗp
			static int HokanPercent(0);
			HokanPercent = (abs(HokanPercent - percent) < 10) ? percent : (int)(HokanPercent * .75f + percent * .25f);	// �v��99�Ŏ~�܂����肷��̂ł���̑΍�
			NumberRender(m_pImages[(int)IMAGE::NUMBER].get(), 652 - 32, 580, 32, 42, HokanPercent, 3, true);
		}
		else m_pImages[(int)IMAGE::NUMBER]->Render(652 - 32, 580, 32, 42, 0, 0, 32, 42);
	}

	// �i�ʔF��
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT1]->Render(128, 200, 300, 80, 0, 880, 300, 80);
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT2]->Render(400, 200, 300, 80, 0, (m_iCursor)* 80, 300, 80);

	static int y(0);

	if (KeyBoardTRG(KB_Z)) y += 1;

	// �I��g
	m_pImages[(int)IMAGE::SELECT_FRAME]->Render(800, 246);
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_FRAME]->Render(800, 246);

	// �t�F�[�h
	Fade::Render();
}
//
//=============================================================================================