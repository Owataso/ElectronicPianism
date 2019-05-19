#include "TDNLIB.h"
#include "../system/Framework.h"
#include "SceneOption.h"
#include "sceneModeSelect.h"
#include "../Data/CustomizeData.h"
#include "../Sound/SoundManager.h"
#include "../Data/PlayerData.h"
#include "../Fade/Fade.h"
#include "../Input/InputMIDI.h"
#include "../Music/MusicInfo.h"
#include "TDN_Movie.h"

//=============================================================================================
//		�e�I��
const Vector2 SelectOption::Base::FRAME_OFFSET_XY(424, 96);

SelectOption::Base::Base(sceneOption *pOption, int NumCursor) :m_pOption(pOption), m_CursorAnimFrame(0), m_iCursor(0), m_bSettingOption(false), m_NumCursor(NumCursor)
{
	m_pText = new tdnText2(20, "�l�r �o�S�V�b�N");

	// �J�[�\�����W
	m_CurrentCursorPos.x = m_NextCursorPos.x = FRAME_OFFSET_XY.x + (m_iCursor % 6) * 128, m_CurrentCursorPos.y = m_NextCursorPos.y = FRAME_OFFSET_XY.y + (m_iCursor / 6) * 64;
}

SelectOption::Base::~Base()
{
	// �I�������J�X�^�}�C�Y�ŏ㏑��
	//m_pOption->m_iSelectOptions[m_pOption->m_iSelectOptionType] = m_iCursor;
	delete m_pText;
}

void SelectOption::Base::UpdateCursor()
{
	// �J�[�\���A�j���[�V�����X�V
	if (++m_CursorAnimFrame > 8 * 4) m_CursorAnimFrame = 0;

	// �J�[�\�����W��ԏ���
	m_CurrentCursorPos = m_CurrentCursorPos * .8f + m_NextCursorPos * .2f;

	bool bCursored(false);

	// ���E�Ńt�H���_�[�I��
	if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15))
	{
		bCursored = true;	// �J�[�\���ړ������̂�true
		g_pSE->Play("�J�[�\��2");// �J�[�\�����Đ�
		if (--m_iCursor < 0) m_iCursor = m_NumCursor - 1;
	}
	else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
	{
		bCursored = true;	// �J�[�\���ړ������̂�true
		g_pSE->Play("�J�[�\��2");// �J�[�\�����Đ�
		if (++m_iCursor >= m_NumCursor) m_iCursor = 0;
	}

	if (bCursored)
	{
		// ���̃J�[�\�����W�X�V
		m_NextCursorPos.x = FRAME_OFFSET_XY.x + (m_iCursor % 6) * 128, m_NextCursorPos.y = FRAME_OFFSET_XY.y + (m_iCursor / 6) * 64;
	}

	// �G���^�[�Ō���
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		m_pOption->m_pSE->Play(0);// ���艹�Đ�
		m_bSettingOption = true;
	}

	// �X�y�[�X�ŃL�����Z��
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
	{
		g_pSE->Play("�߂�");

		// ����
		m_pOption->Close();
	}
}

void SelectOption::Base::RenderCursor()
{
	// �J�[�\���g
	//m_pOption->m_pImages[(int)m_pOption->IMAGE::CURSOR_FRAME_S]->Render((int)m_CurrentCursorPos.x, (int)m_CurrentCursorPos.y, 128, 64, 0, (m_CursorAnimFrame / 8) * 64, 128, 64);
}

SelectOption::HiSpeed::HiSpeed(sceneOption *pOption) :Base(pOption, 2), m_iOrgHiSpeed(pOption->m_pPlayerOption->HiSpeed), m_bOrgRegulSpeed(pOption->m_pPlayerOption->bRegulSpeed){}
void SelectOption::HiSpeed::Update()
{
	if (m_bSettingOption)
	{
		if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
		{
			m_pOption->m_pSE->Play(0);// ���艹�Đ�
			m_bSettingOption = false;
		}

		switch (m_iCursor)
		{
		case 0:	// �n�C�X�s�̐ݒ�
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15))
			{
				g_pSE->Play("�J�[�\��2");// �J�[�\�����Đ�
				if (m_pOption->m_pPlayerOption->HiSpeed > 0) m_pOption->m_pPlayerOption->HiSpeed--;
			}
			else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("�J�[�\��2");// �J�[�\�����Đ�
				if (m_pOption->m_pPlayerOption->HiSpeed < 10) m_pOption->m_pPlayerOption->HiSpeed++;
			}
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("�߂�");
				m_pOption->m_pPlayerOption->bRegulSpeed = m_bOrgRegulSpeed.val;
				m_bSettingOption = false;
			}
			break;

		case 1:	// ���M�����[�X�s�[�h�̐ݒ�
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15) || InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("�J�[�\��2");// �J�[�\�����Đ�
				m_pOption->m_pPlayerOption->bRegulSpeed ^= 0x1;
			}
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("�߂�");
				m_pOption->m_pPlayerOption->bRegulSpeed = m_bOrgRegulSpeed.val;
				m_bSettingOption = false;
			}
			break;
		}

	}

	else
	{
		Base::UpdateCursor();
	}

}

void SelectOption::HiSpeed::Render()
{
	// �J�[�\���`��
	//RenderCursor();

	const DWORD l_dwCol(m_bSettingOption ? 0xffffffff : 0x80ffffff);

	switch (m_iCursor)
	{
	case 0:	// ���l�̐ݒ�
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y, l_dwCol, "�n�C�X�s�[�h�̐ݒ�");
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y + 30, l_dwCol, "%.1f", m_pOption->m_pPlayerOption->HiSpeed * HI_SPEED_WIDTH);
		break;
	case 1:	// �I���E�I�t�̐ݒ�
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y, l_dwCol, "���M�����[�X�s�[�h��ON�EOFF");
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y + 30, l_dwCol, "%s", (m_pOption->m_pPlayerOption->bRegulSpeed ? "ON" : "OFF"));
		break;
	}
}


SelectOption::Movie::Movie(sceneOption *pOption) :Base(pOption, 2), m_bOrgMovieOn(pOption->m_pPlayerOption->bMoviePlay), m_byOrgMovieAlpha(pOption->m_pPlayerOption->MovieAlpha){}
void SelectOption::Movie::Update()
{
	if (m_bSettingOption)
	{
		if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
		{
			m_pOption->m_pSE->Play(0);// ���艹�Đ�
			m_bSettingOption = false;
		}

		switch (m_iCursor)
		{
		case 0:	// �I���E�I�t�̐ݒ�
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15) || InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("�J�[�\��2");// �J�[�\�����Đ�
				m_pOption->m_pPlayerOption->bMoviePlay ^= 0x1;
			}
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("�߂�");
				m_pOption->m_pPlayerOption->bMoviePlay = m_bOrgMovieOn.val;
				m_bSettingOption = false;
			}


			break;

		case 1:	// �A���t�@�̐ݒ�
		{
					static const int ADD_WIDTH(32);	// ������
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15))
			{
				g_pSE->Play("�J�[�\��2");// �J�[�\�����Đ�
				if(m_pOption->m_pPlayerOption->MovieAlpha > ADD_WIDTH) m_pOption->m_pPlayerOption->MovieAlpha -= ADD_WIDTH;
				else m_pOption->m_pPlayerOption->MovieAlpha = 0;
			}
			else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("�J�[�\��2");// �J�[�\�����Đ�
				if (m_pOption->m_pPlayerOption->MovieAlpha + ADD_WIDTH < 255) m_pOption->m_pPlayerOption->MovieAlpha += ADD_WIDTH;
				else m_pOption->m_pPlayerOption->MovieAlpha = 255;
			}
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("�߂�");
				m_pOption->m_pPlayerOption->MovieAlpha = m_byOrgMovieAlpha;
				m_bSettingOption = false;
			}
		}
			break;
		}

	}

	else
	{
		Base::UpdateCursor();
	}

}

void SelectOption::Movie::Render()
{
	// �J�[�\���`��
	//RenderCursor();

	const DWORD l_dwCol(m_bSettingOption ? 0xffffffff : 0x80ffffff);

	switch (m_iCursor)
	{
	case 0:	// �I���E�I�t�̐ݒ�
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y, l_dwCol, "�����ON�EOFF");
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y + 30, l_dwCol, "%s", (m_pOption->m_pPlayerOption->bMoviePlay ? "ON" : "OFF"));
		break;

	case 1:	// �A���t�@�̐ݒ�
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y, l_dwCol, "�A���t�@�̐ݒ�");
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y + 30, l_dwCol, "%d", m_pOption->m_pPlayerOption->MovieAlpha);
		break;
	}
}

SelectOption::Guide::Guide(sceneOption *pOption) :Base(pOption, 1), m_bOrgGuide(pOption->m_pPlayerOption->bGuide){}
void SelectOption::Guide::Update()
{
	if (m_bSettingOption)
	{
		if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
		{
			m_pOption->m_pSE->Play(0);// ���艹�Đ�
			m_bSettingOption = false;
		}

		switch (m_iCursor)
		{
		case 0:	// �K�C�h�̃I���E�I�t�̐ݒ�
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15) || InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("�J�[�\��2");// �J�[�\�����Đ�
				m_pOption->m_pPlayerOption->bGuide ^= 0x1;
			}
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("�߂�");
				m_pOption->m_pPlayerOption->bGuide = m_bOrgGuide.val;
				m_bSettingOption = false;
			}
			break;
		}

	}

	else
	{
		Base::UpdateCursor();
	}

}
void SelectOption::Guide::Render()
{
	// �J�[�\���`��
	//RenderCursor();

	const DWORD l_dwCol(m_bSettingOption ? 0xffffffff : 0x80ffffff);

	switch (m_iCursor)
	{
	case 0:	// �K�C�h�̃I���E�I�t�̐ݒ�
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y, l_dwCol, "�K�C�h��ON�EOFF");
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y + 30, l_dwCol, "%s", (m_pOption->m_pPlayerOption->bGuide ? "ON" : "OFF"));
		break;
	}
}


//=============================================================================================
//		��	��	��	��	�J	��
bool sceneOption::Initialize()
{
	m_pImages[(int)IMAGE::BACK]				= std::make_unique<tdn2DObj>("DATA/Image/back2.png");
	m_pImages[(int)IMAGE::UP_FRAME]			= std::make_unique<tdn2DObj>("DATA/UI/Other/up_frame.png");
	m_pImages[(int)IMAGE::MIDASHI]			= std::make_unique<tdn2DObj>("DATA/UI/Other/midashi.png");
	m_fLoadPercentage = .1f;	// ���[�h����
	m_pImages[(int)IMAGE::LEFT_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Option/left_frame.png");
	m_pImages[(int)IMAGE::RIGHT_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Customize/right_frame.png");
	m_fLoadPercentage = .2f;	// ���[�h����
	m_pImages[(int)IMAGE::CURSOR_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Customize/cursor_frame.png");
	m_pImages[(int)IMAGE::RIGHT_UP_FRAME]	= std::make_unique<tdn2DObj>("DATA/UI/Customize/right_up_frame.png");
	m_pImages[(int)IMAGE::CURSOR_FRAME_S]	= std::make_unique<tdn2DObj>("DATA/UI/Other/cursor_frame2.png");
	m_fLoadPercentage = .25f;	// ���[�h����
	m_pImages[(int)IMAGE::SKIN]				= std::make_unique<tdn2DObj>(CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::SKIN).c_str());
	m_pImages[(int)IMAGE::NOTE]				= std::make_unique<tdn2DObj>(CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::NOTE).c_str());
	m_pImages[(int)IMAGE::EXPLOSION]		= std::make_unique<tdn2DObj>(CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::EXPLOSION).c_str());
	m_pImages[(int)IMAGE::KEYLINE]			= std::make_unique<tdn2DObj>("DATA/Customize/Skin/back.png");

	//	�ꎞ�����_�����O�p�T�[�t�F�C�X
	m_pPlayScreen = std::make_unique<tdn2DObj>(1280, 720, RENDERTARGET);

	//	�t���[���o�b�t�@�̃|�C���^�ۑ�
	tdnSystem::GetDevice()->GetRenderTarget(0, &m_pSurface);

	m_fLoadPercentage = .25f;	// ���[�h����

	// SE
	m_pSE = std::make_unique<tdnSoundSE>();
	m_pSE->Set(0, 3, "DATA/Sound/SE/decide1.wav");	// ���艹

	// ����
	m_pMovie = new tdnMovie("DATA/Movie/MusicSelect.wmv");
	m_pMovie->Play();
	m_pImages[(int)IMAGE::MOVIE] = std::make_unique<tdn2DObj>(m_pMovie->GetTexture());

	m_fLoadPercentage = 1.0f;	// ���[�h����

	// �I������J�X�^�}�C�Y�^�C�v�J�[�\��������
	m_iSelectOptionType = 0;

	// �v���C���[�f�[�^�̃I�v�V����
	m_pPlayerOption = new PlayerData::OptionV2;
	memcpy_s(m_pPlayerOption, sizeof(PlayerData::OptionV2), &PlayerDataMgr->m_PlayerOption, sizeof(PlayerData::OptionV2));

	// �J�X�^�}�C�Y�^�C�v�I����̈Ϗ�����
	m_pSelectOption = nullptr;

	// �J�X�^�}�C�Y�e�X�g�̈ȏ�N
	m_pOptionTest = std::make_unique<OptionTestPlayer>(this, m_pImages[(int)IMAGE::EXPLOSION].get());

	// �J�[�\�����W
	m_CurrentCursorPosY = m_NextCursorPosY = 195;

	// �t�F�[�h
	Fade::Set(Fade::MODE::FADE_IN, 8);

	return true;
}

sceneOption::~sceneOption()
{
	// �v���C���[�f�[�^�̃I�v�V�����X�V
	memcpy_s(&PlayerDataMgr->m_PlayerOption, sizeof(PlayerData::OptionV2), m_pPlayerOption, sizeof(PlayerData::OptionV2));
	delete m_pPlayerOption;

	// �S�摜���
	delete m_pMovie;
	// �J�X�^�}�C�Y�ȏキ��J��
	SAFE_DELETE(m_pSelectOption);
}
//
//=============================================================================================

//=============================================================================================
//		�X			�V
bool sceneOption::Update()
{
	static bool bEnd(false);	// �t�F�[�h�֌W�ŏI��������̂Ɏg��

	// �t�F�[�h
	Fade::Update();

	// ���惋�[�v�X�V
	m_pMovie->Update();

	// �I�v�V�����e�X�g�N�X�V
	m_pOptionTest->Update();

	// �J�[�\�����W��ԏ���
	m_CurrentCursorPosY = m_CurrentCursorPosY * .8f + m_NextCursorPosY * .2f;

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
		// �J�X�^�}�C�Y�^�C�v�I�����āA�J�X�^�}�C�Y�I��
		if (m_pSelectOption)
		{
			m_pSelectOption->Update();
		}
		else
		{
			// �㉺�Ńt�H���_�[�I��
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15))
			{
				g_pSE->Play("�J�[�\��2");	// �J�[�\�����Đ�
				if (--m_iSelectOptionType < 0) m_iSelectOptionType = (int)SELECT_OPTION::MAX - 1;

				m_NextCursorPosY = 195.0f + (m_iSelectOptionType * 73);
			}
			else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("�J�[�\��2");	// �J�[�\�����Đ�
				if (++m_iSelectOptionType >= (int)SELECT_OPTION::MAX) m_iSelectOptionType = 0;

				m_NextCursorPosY = 195.0f + (m_iSelectOptionType * 73);
			}

			// �G���^�[�ŃJ�X�^�}�C�Y�^�C�v����
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
			{
				g_pSE->Play("����2");// ���艹�Đ�

				switch ((SELECT_OPTION)m_iSelectOptionType)
				{
				case SELECT_OPTION::HI_SPEED:
					m_pSelectOption = new SelectOption::HiSpeed(this);
					break;
				case SELECT_OPTION::MOVIE:
					m_pSelectOption = new SelectOption::Movie(this);
					break;
				case SELECT_OPTION::GUIDE:
					m_pSelectOption = new SelectOption::Guide(this);
					break;
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
		}
	}

	return true;
}
//
//=============================================================================================

//=============================================================================================
//		�`			��
void sceneOption::Render()
{
	//	�ꎞ�o�b�t�@�֐؂�ւ�
	m_pPlayScreen->RenderTarget();

	
	tdnView::Clear();

	m_pImages[(int)IMAGE::SKIN]->Render(0, 0);

	m_pImages[(int)IMAGE::KEYLINE]->Render(0, 133);

	// �����]��
	if (m_pPlayerOption->bMoviePlay)
	{
		const DWORD dwAlphaColor = ((m_pPlayerOption->MovieAlpha << 24) | 0x00000000);
		tdnPolygon::Rect(1, 134, 1118, 495, RS::COPY, dwAlphaColor);
		// ����`��
		m_pImages[(int)IMAGE::MOVIE]->SetAlpha((BYTE)m_pPlayerOption->MovieAlpha);
		m_pImages[(int)IMAGE::MOVIE]->Render(120, 134, 880, 495, 0, 0, 1280, 720);
	}

	// �J�X�^�}�C�Y�e�X�g�`��
	m_pOptionTest->Render(m_pImages[(int)IMAGE::NOTE].get(), m_pImages[(int)IMAGE::EXPLOSION].get());

	//	�t���[���o�b�t�@�֐؂�ւ�
	tdnSystem::GetDevice()->SetRenderTarget(0, m_pSurface);

	// �w�i
	m_pImages[(int)IMAGE::BACK]->Render(0, 0);

	// ��̃t���[��
	m_pImages[(int)IMAGE::UP_FRAME]->Render(0, 0);
	m_pImages[(int)IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 320, 400, 64);

	// �����_�[�^�[�Q�b�g�����Q�[�����
	static int a = 0;
	if (KeyBoardTRG(KB_1)) a += 1;
	m_pPlayScreen->Render(412, 250, 802, 442, 0, 0, 1280, 720);

	// �ŏI�X�L���̃t���[��
	m_pImages[(int)IMAGE::RIGHT_FRAME]->Render(0, 0);

	// �Ϗ��`��
	if (m_pSelectOption)
	{
		// ���̃t���[���኱�Â�����
		m_pImages[(int)IMAGE::LEFT_FRAME]->SetARGB(0xff808080);
		m_pImages[(int)IMAGE::LEFT_FRAME]->Render(0, 0);

		// �E�̃t���[��
		m_pImages[(int)IMAGE::RIGHT_UP_FRAME]->SetARGB(0xffffffff);
		m_pImages[(int)IMAGE::RIGHT_UP_FRAME]->Render(0, 0);

		m_pSelectOption->Render();
	}
	else
	{
		// �E�̃t���[���኱�Â�����
		m_pImages[(int)IMAGE::RIGHT_UP_FRAME]->SetARGB(0xff808080);
		m_pImages[(int)IMAGE::RIGHT_UP_FRAME]->Render(0, 0);
		// ���̃t���[��
		m_pImages[(int)IMAGE::LEFT_FRAME]->SetARGB(0xffffffff);
		m_pImages[(int)IMAGE::LEFT_FRAME]->Render(0, 0);
	}

	// �J�[�\��
	static int AnimFrame = 0;
	if (++AnimFrame > 16 * 4) AnimFrame = 0;
	m_pImages[(int)IMAGE::CURSOR_FRAME]->Render(98, (int)m_CurrentCursorPosY, 256, 64, 0, (AnimFrame / 16) * 64, 256, 64);

	// �t�F�[�h
	Fade::Render();
}
//
//=============================================================================================

bool OptionTestPlayer::Note::Update(int NumExplosionpanel)
{
	if (++ExplosionFrame > NumExplosionpanel * 2)
		return true;

	return false;
}

void OptionTestPlayer::Note::Render(tdn2DObj *pNote, tdn2DObj *pExplosion)
{
	if (bNoteEnd)
	{
		pExplosion->Render(posX - 48, 618 - 60, 128, 128, (ExplosionFrame / 2) * 128, 0, 128, 128);
	}

	else
	{
		if (posY + JUDGE_LINE < KEYLANE_TOP)return;
		pNote->Render(posX, posY +JUDGE_LINE, 40, 16, 0, 0, 40, 16);
	}
}

void OptionTestPlayer::Clear()
{
	for (auto it = m_NoteList.begin(); it != m_NoteList.end();){ delete (*it); it = m_NoteList.erase(it); }
}

void OptionTestPlayer::Update()
{
	static const int CreatePosX[30] =
	{
		120, 180, 240, 300, 360, 420, 480, 540, 600, 660, 720, 780, 840, 900, 960,
		1020, 960, 900, 840, 780, 720, 660, 600, 540, 480, 420, 360, 300, 240, 180
	};

	// �m�[�c���X�g���󂾂����琶��
	if (m_NoteList.empty())
	{
		m_iStartClock = clock();

		m_NoteList.push_back(new Note(CreatePosX[4], 120));
		m_NoteList.push_back(new Note(CreatePosX[6], 180));
		m_NoteList.push_back(new Note(CreatePosX[8], 260));
	}

	const int iPlayCursor(clock() - m_iStartClock);

	for (auto it = m_NoteList.begin(); it != m_NoteList.end();)
	{
		if ((*it)->bNoteEnd)
		{
			if ((*it)->Update(m_NumExplosionPanel))	// true���Ԃ���������t���OON�Ƃ�������
			{
				delete (*it);
				it = m_NoteList.erase(it);
			}
			else it++;
			continue;
		}

		(*it)->iRestTime = iPlayCursor - (*it)->iEventTime;
		if ((*it)->iRestTime > 0)
		{
			(*it)->bNoteEnd = true;
			it++;
			continue;
		}

		int l_iPosY((*it)->iRestTime);

		// BPM�ɂ��e��
		if (!m_pOption->m_pPlayerOption->bRegulSpeed)
			l_iPosY = (int)(l_iPosY / (60 / (*it)->fBPM / 480 * 1000));
		// �n�C�X�s�[�h�ɂ��e��
		const float speed = m_pOption->m_pPlayerOption->HiSpeed * HI_SPEED_WIDTH;
		l_iPosY = (int)(l_iPosY * (.1f + (speed / .5f * .1f)));
		(*it)->posY = l_iPosY;

		it++;
	}
}

void OptionTestPlayer::Render(tdn2DObj *pNote, tdn2DObj *pExplosion)
{
	for (auto &it : m_NoteList)
	{
		it->Render(pNote, pExplosion);
	}
}