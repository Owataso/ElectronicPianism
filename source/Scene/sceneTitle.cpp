#include "TDNLIB.h"
#include "system/Framework.h"
#include "sceneTitle.h"
#include "sceneModeSelect.h"
#include "SceneEntry.h"
#include "../Input/InputMIDI.h"
#include "../Fade/Fade.h"

// �ÓI�����o�ϐ��̐錾
bool sceneTitle::m_bLoadSaveData = false;

//=============================================================================================
//		��	��	��	��	�J	��
bool sceneTitle::Initialize()
{
	// �t�F�[�h������
	Fade::Set(Fade::MODE::FADE_IN, 5, 0x00000000);

	// �摜������
	m_pImages[IMAGE::LOGO]	= std::make_unique<tdn2DObj>("DATA/Image/logo.png");
	m_pImages[IMAGE::BACK1] = std::make_unique<tdn2DObj>("DATA/Image/back2.png");
	m_fLoadPercentage = .5f;	// ���[�h����
	m_pImages[IMAGE::BACK2] = std::make_unique<tdn2DObj>("DATA/Image/back3.png");
	m_pImages[IMAGE::BACK2]->SetARGB(0x80ffffff);

	m_fLoadPercentage = 1.0f;	// ���[�h����

	// �摜�ړ�
	memset(m_moveX, 0, sizeof(m_moveX));


	return true;
}

//=============================================================================================


//=============================================================================================
//		�X			�V
bool sceneTitle::Update()
{
	static bool bEnd(false);

	Fade::Update();

	// �߂�t�F�[�h��
	if (bEnd)
	{
		// �t�F�[�h�I�������A��
		if (Fade::m_eMode == Fade::MODE::FADE_NONE)
		{
			bEnd = false;

			// ����J�ڂȂ�
			if (!m_bLoadSaveData)
			{
				// �f�[�^���[�h�V�[���Ɉڍs
				m_bLoadSaveData = true;
				MainFrame->ChangeScene(new sceneEntry, true);
			}

			// �����łȂ��Ȃ烂�[�h�I����ʂ�
			else MainFrame->ChangeScene(new sceneModeSelect, true);

		}
		return true;
	}

	// ���[�v�Đ��p�̍X�V
	//m_pMovie[0]->LoopUpdate();
	//m_pMovie[1]->LoopUpdate();

	// �w�i�摜�ړ��X�V
	m_moveX[0] += 1;
	m_moveX[1] -= 1;

	// �G���^�[�L�[�ŃX�^�[�g
	if (InputMIDIMgr->GetAnyTrigger())
	{
		// �t�F�[�h�A�E�g�ݒ�
		Fade::Set(Fade::MODE::FADE_OUT, 8);
		bEnd = true;
	}

	// �X�y�[�X�L�[��EXIT
	else if (KeyBoardTRG(KB_SPACE)) PostQuitMessage(0);

	return true;
}
//
//=============================================================================================


//=============================================================================================
//		�`			��
void sceneTitle::Render()
{
	tdnView::Clear();

	// �w�i
	m_pImages[IMAGE::BACK1]->Render(0, 0, tdnSystem::GetScreenSize().right, tdnSystem::GetScreenSize().bottom, m_moveX[0], 0, 1280, 720);
	m_pImages[IMAGE::BACK2]->Render(0, 0, tdnSystem::GetScreenSize().right, tdnSystem::GetScreenSize().bottom, m_moveX[1], 0, 1280, 720);

	//m_pMovieTexture[1]->Render(0,0);
	//m_pMovieTexture[0]->Render(0, 0);

	// ���S
	m_pImages[IMAGE::LOGO]->Render(1280 / 2 - 720 / 2, 160);

	static int flashing = 0;
	flashing++;
	if (++flashing < 75)tdnText::Draw(560, 500, 0xffffffff, "PRESS ANY KEY");
	else if (flashing > 100) flashing = 0;

	Fade::Render();
}
//
//=============================================================================================