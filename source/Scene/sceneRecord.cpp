#include "TDNLIB.h"
#include "../Sound/SoundManager.h"
#include "system/Framework.h"
#include "sceneModeSelect.h"
#include "sceneRecord.h"
#include "../Data/PlayerData.h"
#include "../Input/InputMIDI.h"
#include "../Fade/Fade.h"
#include "../Number/Number.h"
#include "../Alphabet/Alphabet.h"

//=============================================================================================
//		��	��	��	��	�J	��
bool sceneRecord::Initialize()
{
	// �t�F�[�h������
	Fade::Set(Fade::MODE::FADE_IN, 8);

	m_fLoadPercentage = .5f;	// ���[�h����

	// �摜������
	m_pImages[IMAGE::BACK]			= std::make_unique<tdn2DObj>("DATA/Image/back2.png");
	m_pImages[IMAGE::UP_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Other/up_frame.png");
	m_pImages[IMAGE::MIDASHI]		= std::make_unique<tdn2DObj>("DATA/UI/Other/midashi.png");
	m_pImages[IMAGE::NUMBER]		= std::make_unique<tdn2DObj>("DATA/Number/Number.png");
	m_pImages[IMAGE::GRADE_TEXT]	= std::make_unique<tdn2DObj>("DATA/UI/Grade/grade.png");
	m_pImages[IMAGE::ALPHABET]		= std::make_unique<tdn2DObj>("DATA/UI/Other/alphabet.png");
	m_fLoadPercentage = 1.0f;	// ���[�h����

	m_pPlayerInfo = &PlayerDataMgr->m_PlayerInfo;
	m_grade = PlayerDataMgr->m_grade;
	m_pAlphabetRenderer = new AlphabetRenderer;

	g_pBGM->PlayStreamIn("DATA/Sound/BGM/Cyberspace_Breakroom.ogg");

	return true;
}

sceneRecord::~sceneRecord()
{
	delete m_pAlphabetRenderer;
	g_pBGM->StopStreamIn();
}
//=============================================================================================


//=============================================================================================
//		�X			�V
bool sceneRecord::Update()
{
	static bool bEnd(false);

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

	// �X�y�[�X�L�[�Ŕ�����
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
	{
		g_pSE->Play("�߂�");

		// �t�F�[�h�A�E�g�ݒ�
		Fade::Set(Fade::MODE::FADE_OUT, 4);
		bEnd = true;
	}
	return true;
}
//
//=============================================================================================


//=============================================================================================
//		�`			��
void sceneRecord::Render()
{
	
	tdnView::Clear();

	// �w�i
	m_pImages[IMAGE::BACK]->Render(0, 0);

	// �t���[��
	m_pImages[IMAGE::UP_FRAME]->Render(0, 0);

	// RECORD�̕���
	m_pImages[IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 256, 400, 64);

	tdnPolygon::Rect((int)(1280 * .25f) / 2, 96, (int)(1280 * .75f), (int)(720 * .75f), RS::COPY, 0x80808080);

	// ���O�`��
	for (int i = 0; m_pPlayerInfo->PlayerName[i] != '\0'; i++)m_pAlphabetRenderer->Render(320 + i * 64, 160, m_pPlayerInfo->PlayerName[i]);

	// �v���C��
	tdnText::Draw(400, 400, 0xffffffff, "�v���C��: %d", m_pPlayerInfo->PlayCount);

	// �v���C����
	const int minutes(m_pPlayerInfo->PlayTime % 60), hour(m_pPlayerInfo->PlayTime / 60);
	tdnText::Draw(400, 460, 0xffffffff, "�v���C����: %d���� %d��", hour, minutes);

	// �i�ʕ`��
	if (m_grade != GRADE::NONE)m_pImages[IMAGE::GRADE_TEXT]->Render(320, 240, 150, 72, 0, (int)m_grade * 72, 150, 72);



	// �t�F�[�h
	Fade::Render();
}
//
//=============================================================================================