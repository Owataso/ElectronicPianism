#include "TDNLIB.h"
#include "system/Framework.h"
#include "sceneModeSelect.h"
#include "SceneEdit.h"
#include "sceneMusicSelect.h"
#include "sceneEnding.h"
#include "sceneCustomize.h"
#include "sceneOption.h"
#include "sceneTitle.h"
#include "sceneRecord.h"
#include "sceneClassSelect.h"
#include "sceneTutorial.h"
#include "../Sound/SoundManager.h"
#include "../Mode/ModeManager.h"
#include "../Fade/Fade.h"
#include "../Input/InputMIDI.h"
#include "../Data/PlayerData.h"
#include "TDN_Movie.h"

#ifdef _DEBUG
#include "DebugRoom.h"
#endif

//*****************************************************************************************************************************
//
//		���[�h�֌W
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	�e���[�h�̏�����
//------------------------------------------------------
SelectMode::Standard::Standard()
{
	//m_ModeDescription = "��{�I�ȃ��[�h�ł��B3�ȉ��t���܂��B";
}
BaseScene *SelectMode::Standard::GetNextScene()
{
	// ���[�h�Ǘ�����Ƀ��[�h�ݒ肷��
	//ModeMgr->SetMode(PLAY_MODE::STANDARD);
	ModeMgr->SetMode(PLAY_MODE::FREE);
	return new sceneMusicSelect; 
}

SelectMode::Grade::Grade()
{
	//m_ModeDescription = "���킷��i�ʂ�I�����A�R�Ȋ�������΍��i�ł��B";
}
BaseScene *SelectMode::Grade::GetNextScene()
{
	// ���[�h�Ǘ�����Ƀ��[�h�ݒ肷��
	ModeMgr->SetMode(PLAY_MODE::GRADE);

	return new sceneClassSelect;
}

SelectMode::Free::Free()
{
	//m_ModeDescription = "���R�ɋȂ�I��Ńv���C���郂�[�h�ł��B";
}
BaseScene *SelectMode::Free::GetNextScene()
{
	// ���[�h�Ǘ�����Ƀ��[�h�ݒ肷��
	ModeMgr->SetMode(PLAY_MODE::FREE);

	return new sceneMusicSelect;
}

SelectMode::Tutorial::Tutorial()
{
	//m_ModeDescription = "�`���[�g���A���ł��B";
}
BaseScene *SelectMode::Tutorial::GetNextScene()
{
	// ���[�h�Ǘ�����Ƀ��[�h�ݒ肷��
	ModeMgr->SetMode(PLAY_MODE::MAX);

	return 
		//new sceneTutorial;
		nullptr;
}

SelectMode::Customize::Customize()
{
	//m_ModeDescription = "�v���C���[�X�L�����J�X�^�}�C�Y���郂�[�h�ł�";
}
BaseScene *SelectMode::Customize::GetNextScene()
{
	return new sceneCustomize;
}

SelectMode::Option::Option()
{
	//m_ModeDescription = "�I�v�V�����ł��B�L�[�{�[�h�̉����̐ݒ�Ƃ��B";
}
BaseScene *SelectMode::Option::GetNextScene()
{
	return new sceneOption;
}

SelectMode::Record::Record()
{
	//m_ModeDescription = "�v���C���[�̋L�^���{�����܂��B";
}
BaseScene *SelectMode::Record::GetNextScene()
{
	return new sceneRecord;
}

//------------------------------------------------------
//	���[�h���ʂ̃f�X�g���N�^
//------------------------------------------------------
SelectMode::Base::~Base()
{

}

//------------------------------------------------------
//	���[�h���ʂ̃Q�b�^
//------------------------------------------------------
//const char *SelectMode::Base::GetDescription(){ return m_ModeDescription.c_str(); }


int sceneModeSelect::m_ModeCursor = 1;

//=============================================================================================
//		��	��	��	��	�J	��
bool sceneModeSelect::Initialize()
{
	// �I�����[�h�|�C���^������
	m_pSelectMode[(int)SELECT_MODE::STANDARD]	= std::make_unique<SelectMode::Standard>();
	m_pSelectMode[(int)SELECT_MODE::GRADE]		= std::make_unique<SelectMode::Grade>();
	m_pSelectMode[(int)SELECT_MODE::FREE]		= std::make_unique<SelectMode::Free>();
	m_pSelectMode[(int)SELECT_MODE::RECORD]		= std::make_unique<SelectMode::Record>();
	m_pSelectMode[(int)SELECT_MODE::CUSTMIZE]	= std::make_unique<SelectMode::Customize>();
	m_pSelectMode[(int)SELECT_MODE::OPTION]		= std::make_unique<SelectMode::Option>();
	m_pSelectMode[(int)SELECT_MODE::TUTORIAL]	= std::make_unique<SelectMode::Tutorial>();
	//m_ModeCursor = 0;

	m_fLoadPercentage = .25f;	// ���[�h����

	// ���揉����
	m_pMovie = new tdnMovie("DATA/movie/ModeSelect.wmv");
	m_pMovie->Play();

	m_fLoadPercentage = .4f;

	// �摜�Ǘ����񏉊���
	//m_pImageMgr = new ImageManager("DATA/Text/ImageSet/ModeSelect.txt");
	m_pImages[(int)IMAGE::BACK] = std::make_unique<tdn2DObj>(m_pMovie->GetTexture());
	m_pImages[(int)IMAGE::MODE] = std::make_unique<tdn2DObj>("DATA/UI/ModeSelect/mode.png");
	m_fLoadPercentage = .6f;	// ���[�h����
	FOR((int)SELECT_MODE::MAX)
	{
		m_pAnimImages[i] = std::make_unique<tdn2DAnim>(m_pImages[(int)IMAGE::MODE]->GetTexture());
		m_pAnimImages[i]->OrderMoveAppeared(3, 40 + i * 168, 120 + 360);
		m_pAnimImages[i]->Action(15 + i * 2);
	}
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI] = std::make_unique<tdn2DAnim>("DATA/UI/Other/midashi.png");
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->OrderMoveAppeared(16, -8, 12);
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Action(25);
	m_fLoadPercentage = .7f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/frame.png");
	m_pAnimImages[(int)ANIM_IMAGE::FRAME]->OrderStretch(15, 1, .0f);
	m_pAnimImages[(int)ANIM_IMAGE::FRAME]->Action(20);
	m_pAnimImages[(int)ANIM_IMAGE::MODE_STR] = std::make_unique<tdn2DAnim>("DATA/UI/ModeSelect/mode_str.png");
	m_pAnimImages[(int)ANIM_IMAGE::MODE_STR]->OrderMoveAppeared(10, 1280 + 320, 470);
	m_pAnimImages[(int)ANIM_IMAGE::MODE_STR]->Action(30);
	m_fLoadPercentage = .85f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/up_frame.png");
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->OrderMoveAppeared(30, 0, -64);
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Action(0);
	m_fLoadPercentage = .95f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::DESCRIPTION] = std::make_unique<tdn2DAnim>("DATA/UI/ModeSelect/description.png");
	m_pAnimImages[(int)ANIM_IMAGE::DESCRIPTION]->OrderMoveAppeared(20, 160, 572);
	m_pAnimImages[(int)ANIM_IMAGE::DESCRIPTION]->Action(30);

	m_fLoadPercentage = 1.0f;	// ���[�h����

	// �L�[�{�[�h�̃K�C�h
	m_pKeyboardGuide = std::make_unique<KeyboardGuide>(Vector2(32, 430), 3, 0.1f, 0.5f, 1.0f, 60);

	// �V�[���ɓ����Ă���̑����t�܂ł̎���
	m_WaitTimer = 28;

	// �T�E���h����
	//g_pBGM->Fade_in("���[�h�Z���N�g", 0.25f);
	g_pBGM->PlayStreamIn("DATA/Sound/BGM/Cyberspace_Players.ogg");

	// �t�F�[�h
	Fade::Set(Fade::MODE::FADE_IN, 10, 0x00ffffff);

	return true;
}

sceneModeSelect::~sceneModeSelect()
{
	g_pBGM->StopStreamIn();
	//delete m_pImageMgr;
	delete m_pMovie;
}
//=============================================================================================


//=============================================================================================
//		�X			�V
bool sceneModeSelect::Update()
{
	// ����X�V
	m_pMovie->Update();

	// �t�F�[�h�X�V
	Fade::Update();

	// �摜�A�j���X�V
	FOR((int)ANIM_IMAGE::MAX) m_pAnimImages[i]->Update();

	// �L�[�{�[�h�̃K�C�h�X�V
	m_pKeyboardGuide->Update();

	// �A�j���I���܂őҋ@
	if (m_WaitTimer > 0){ m_WaitTimer--; return true; }

	// ���[�h�I��SE
	static const char *selectSE[(int)SELECT_MODE::MAX] = { "C", "D", "E", "F", "G", "A", "B" };

	bool bCursored(false);
	if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
	{
		if (--m_ModeCursor < 1) m_ModeCursor = (int)SELECT_MODE::MAX - 1;
		bCursored = true;
	}
	else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
	{
		if (++m_ModeCursor >= (int)SELECT_MODE::MAX) m_ModeCursor = 1;
		bCursored = true;
	}

	if (bCursored)
	{
		// SE��炷
		g_pSE->Play((char*)selectSE[m_ModeCursor]);

		// ���[�h�̕����̃A�N�V����
		m_pAnimImages[(int)ANIM_IMAGE::MODE_STR]->Action();
		m_pAnimImages[(int)ANIM_IMAGE::DESCRIPTION]->Action();
	}

	// �����L�[�Ń��[�h�I��
	static const BYTE key[(int)SELECT_MODE::MAX] = { KB_1, KB_2, KB_3, KB_4, KB_5, KB_6, KB_7 };
	for (int i = 1; i < (int)SELECT_MODE::MAX; i++)
	{
		if (KeyBoardTRG(key[i]))
		{
			g_pSE->Play((char*)selectSE[i]);
			m_ModeCursor = i;
			break;
		}
	}


	// �G���^�[�L�[�Ń��[�h����I
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		//g_pBGM->Fade_out("���[�h�Z���N�g", 0.25f);
		// ���[�h�ɂ���Đ؂�ւ���V�[���𕪊�
		BaseScene *l_pNewScene(m_pSelectMode[m_ModeCursor]->GetNextScene());
		if (l_pNewScene)MainFrame->ChangeScene(l_pNewScene, true);
	}
	// �X�y�[�X�L�[�Ŗ߂�
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
	{
		// �^�C�g����
		MainFrame->ChangeScene(new sceneTitle, true);
	}
	else if (KeyBoardTRG(KB_S))
	{
		// �Z�[�u�͂��܂߂�
		PlayerDataMgr->SavePlayerData();

		Fade::Set(Fade::MODE::FADE_IN, 8, 0, 128, 0x00ffffff);
	}

#ifdef _DEBUG
	// �f�o�b�O
	else if (KeyBoardTRG(KB_E))
	{
		//MainFrame->ChangeScene(new sceneEnding, true);
		MainFrame->ChangeScene(new sceneEdit, true);
	}

	else if (KeyBoardTRG(KB_D))
	{
		MainFrame->ChangeScene(new DebugRoom, true);
	}
#endif

	return true;
}
//
//=============================================================================================


//=============================================================================================
//		�`			��
void sceneModeSelect::Render()
{
	tdnView::Clear();

	// �w�i
	m_pImages[(int)IMAGE::BACK]->Render(0, 0);

	// ��̃t���[��
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Render(0, 0);

	// ���[�h�Z���N�g�̕���
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 0, 400, 64);

	// �L�[�{�[�h�̃K�C�h
	m_pKeyboardGuide->Render();

	// 7�̃��[�h�̉摜
	for (int i = 1; i < (int)SELECT_MODE::MAX; i++)
	{
		// �I�����[�h��������g��
		m_pAnimImages[i]->SetScale((i == m_ModeCursor) ? 1.0f : .8f);
		m_pAnimImages[i]->Render(50+ i * 160, 110, 167, 355, i * 256, 0, 167, 355);
	}

	// ���[�h�̌��o��
	m_pAnimImages[(int)ANIM_IMAGE::MODE_STR]->Render(250, 460, 810, 100, 0, m_ModeCursor * 100, 810, 100);

	// ���[�h�����̘g
	m_pAnimImages[(int)ANIM_IMAGE::FRAME]->Render(160, 580);

	// ���[�h������
	m_pAnimImages[(int)ANIM_IMAGE::DESCRIPTION]->Render(160, 580, 992, 96, 0, m_ModeCursor * 96, 992, 96);

	// �t�F�[�h
	Fade::Render();
}
//
//=============================================================================================