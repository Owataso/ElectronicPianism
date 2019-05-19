#include "TDNLIB.h"
#include "../Sound/SoundManager.h"
#include "../system/Framework.h"
#include "SceneCustomize.h"
#include "sceneModeSelect.h"
#include "../Data/CustomizeData.h"
#include "../Sound/SoundManager.h"
#include "../Data/PlayerData.h"
#include "../Fade/Fade.h"
#include "../Input/InputMIDI.h"


//=============================================================================================
//		�e�I��
const Vector2 SelectCustomize::Base::FRAME_OFFSET_XY(424, 96);

SelectCustomize::Base::Base(sceneCustomize *pCustomize, int iCustomizeType) :m_NumCustomize((int)pCustomize->m_NumCustomizes[iCustomizeType]), m_pCustomize(pCustomize), m_iCustomizeType(iCustomizeType), m_CursorAnimFrame(0),
m_iCursor(pCustomize->m_iSelectCustomizes[iCustomizeType])	// �ŏ��̃J�[�\��
{
	m_pText = new tdnText2(20, "�l�r �o�S�V�b�N");

	// �J�[�\�����W
	m_CurrentCursorPos.x = m_NextCursorPos.x = FRAME_OFFSET_XY.x + (m_iCursor % 6) * 128, m_CurrentCursorPos.y = m_NextCursorPos.y = FRAME_OFFSET_XY.y + (m_iCursor / 6) * 64;
}
void SelectCustomize::Base::Initialize()
{
	m_iCursor = m_pCustomize->m_iSelectCustomizes[m_iCustomizeType];
}
void SelectCustomize::Base::Release()
{
	m_pCustomize->m_bSelect = false;	// �I���t���O�I�t
}

SelectCustomize::Base::~Base()
{
	// �I�������J�X�^�}�C�Y�ŏ㏑��
	//m_pCustomize->m_iSelectCustomizes[m_pCustomize->m_iSelectCustomizeType] = m_iCursor;
	delete m_pText;
}

bool SelectCustomize::Base::UpdateCursor()
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
		g_pSE->Play("�J�[�\��2");	// �J�[�\�����Đ�
		if (--m_iCursor < 0) m_iCursor = m_NumCustomize - 1;
	}
	else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT,15))
	{
		bCursored = true;		// �J�[�\���ړ������̂�true
		g_pSE->Play("�J�[�\��2");	// �J�[�\�����Đ�
		if (++m_iCursor >= m_NumCustomize) m_iCursor = 0;
	}

	if (bCursored)
	{
		// �I�������J�X�^�}�C�Y�ŏ㏑��
		m_pCustomize->m_iSelectCustomizes[m_iCustomizeType] = m_iCursor;

		// �e�X�g�N���Z�b�g
		m_pCustomize->m_pCustomizeTest->Reset();

		// ���̃J�[�\�����W�X�V
		m_NextCursorPos.x = FRAME_OFFSET_XY.x + (m_iCursor % 6) * 128, m_NextCursorPos.y = FRAME_OFFSET_XY.y + (m_iCursor / 6) * 64;
	}

	// �G���^�[�Ō���
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		m_pCustomize->m_pSE->Play(0);// ���艹�Đ�

		// �J�X�^�}�C�Y�ݒ�����
		m_pCustomize->Close();
	}

	// �X�y�[�X�ŃL�����Z��
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
	{
		g_pSE->Play("�߂�");

		// �O��̃J�X�^�}�C�Y�ɖ߂���
		m_pCustomize->m_iSelectCustomizes[m_iCustomizeType] = PlayerDataMgr->m_CustomizeRecords[m_iCustomizeType]->no;

		// ����
		m_pCustomize->Close();
	}

	return bCursored;
}

void SelectCustomize::Base::RenderCursor()
{
	// �J�[�\���g
	m_pCustomize->m_pImages[m_pCustomize->IMAGE::CURSOR_FRAME_S]->Render((int)m_CurrentCursorPos.x, (int)m_CurrentCursorPos.y, 128, 64, 0, (m_CursorAnimFrame / 8) * 64, 128, 64);
}

SelectCustomize::Skin::Skin(sceneCustomize *pCustomize) :Base(pCustomize, (int)CUSTOMIZE_TYPE::SKIN){}

void SelectCustomize::Skin::Render()
{
	// �X�L���ꗗ�̕`��
	FOR((int)m_pCustomize->m_NumCustomizes[m_iCustomizeType])
	{
		if (!m_pCustomize->m_bSelect)m_pCustomize->m_pSkins[i]->SetARGB(0xff808080);	// �Â�
		m_pCustomize->m_pSkins[i]->Render((int)FRAME_OFFSET_XY.x + (i % 6) * 128 + 24, (int)FRAME_OFFSET_XY.y + (i / 6) * 64 + 8, 64, 36,  0, 0, 1280, 720);
		m_pCustomize->m_pSkins[i]->SetARGB(0xffffffff);	// �߂�
		// �X�L����
		m_pText->Draw((int)FRAME_OFFSET_XY.x + ((i % 6) * 128) + 16, (int)FRAME_OFFSET_XY.y + ((i / 6) * 64) + 40, (m_pCustomize->m_bSelect) ? 0xffffffff : 0xff808080, "%s", CustomizeDataMgr->GetSkinName((CUSTOMIZE_TYPE)m_iCustomizeType, i).c_str());
	}

	// �J�[�\���`��
	RenderCursor();
}


SelectCustomize::Note::Note(sceneCustomize *pCustomize) :Base(pCustomize, (int)CUSTOMIZE_TYPE::NOTE){}
void SelectCustomize::Note::Render()
{
	// �m�[�c�ꗗ�̕`��
	FOR((int)m_pCustomize->m_NumCustomizes[m_iCustomizeType])
	{
		if (!m_pCustomize->m_bSelect)m_pCustomize->m_pNotes[i]->SetARGB(0xff808080);	// �Â�
		m_pCustomize->m_pNotes[i]->Render((int)FRAME_OFFSET_XY.x + (i % 6) * 128 + 16, (int)FRAME_OFFSET_XY.y + (i / 6) * 64 + 16, 40, 16, 0, 0, 40, 16);
		m_pCustomize->m_pNotes[i]->Render((int)FRAME_OFFSET_XY.x + (i % 6) * 128 + 64, (int)FRAME_OFFSET_XY.y + (i / 6) * 64 + 16, 40, 16, 40, 0, 40, 16);
		m_pCustomize->m_pNotes[i]->SetARGB(0xffffffff);	// �߂�
		// �m�[�c��
		m_pText->Draw((int)FRAME_OFFSET_XY.x + ((i % 6) * 128) + 16, (int)FRAME_OFFSET_XY.y + ((i / 6) * 64) + 40, (m_pCustomize->m_bSelect) ? 0xffffffff : 0xff808080, "%s", CustomizeDataMgr->GetSkinName((CUSTOMIZE_TYPE)m_iCustomizeType, i).c_str());
	}

	// �J�[�\���`��
	RenderCursor();
}

SelectCustomize::Explosion::Explosion(sceneCustomize *pCustomize) :Base(pCustomize, (int)CUSTOMIZE_TYPE::EXPLOSION){}

void SelectCustomize::Explosion::Render()
{
	// �����ꗗ�̕`��
	FOR((int)m_pCustomize->m_NumCustomizes[m_iCustomizeType])
	{
		const int NumAnim = m_pCustomize->m_pExplosions[i]->GetWidth() / 128;
		if (!m_pCustomize->m_bSelect)m_pCustomize->m_pExplosions[i]->SetARGB(0xff808080);	// �Â�
		m_pCustomize->m_pExplosions[i]->Render((int)FRAME_OFFSET_XY.x + (i % 6) * 128, (int)FRAME_OFFSET_XY.y + (i / 6) * 64, 48, 48, (NumAnim / 2) * 128, 0, 128, 128);
		m_pCustomize->m_pExplosions[i]->SetARGB(0xffffffff);	// �߂�
		// ������
		m_pText->Draw((int)FRAME_OFFSET_XY.x + ((i % 6) * 128) + 16, (int)FRAME_OFFSET_XY.y + ((i / 6) * 64) + 40, (m_pCustomize->m_bSelect) ? 0xffffffff : 0xff808080, "%s", CustomizeDataMgr->GetSkinName((CUSTOMIZE_TYPE)m_iCustomizeType, i).c_str());
	}

	// �J�[�\���`��
	RenderCursor();
}

SelectCustomize::Sudden::Sudden(sceneCustomize *pCustomize) :Base(pCustomize, (int)CUSTOMIZE_TYPE::SUDDEN){}
void SelectCustomize::Sudden::Initialize()
{
	Base::Initialize();
	m_pCustomize->m_fNextSudden = 1;
}
void SelectCustomize::Sudden::Release()
{
	Base::Release();
	m_pCustomize->m_fNextSudden = .25f; 
}
void SelectCustomize::Sudden::Render()
{
	// SUDDEN�ꗗ�̕`��
	FOR((int)m_pCustomize->m_NumCustomizes[m_iCustomizeType])
	{
		if (!m_pCustomize->m_bSelect)m_pCustomize->m_pSuddens[i]->SetARGB(0xff808080);	// �Â�
		m_pCustomize->m_pSuddens[i]->Render((int)FRAME_OFFSET_XY.x + (i % 6) * 128 + 24, (int)FRAME_OFFSET_XY.y + (i / 6) * 64 + 8, 64, 36, 0, 0, 1120, 500);
		m_pCustomize->m_pSuddens[i]->SetARGB(0xffffffff);	// �߂�
		// �X�L����
		m_pText->Draw((int)FRAME_OFFSET_XY.x + ((i % 6) * 128) + 16, (int)FRAME_OFFSET_XY.y + ((i / 6) * 64) + 40, (m_pCustomize->m_bSelect) ? 0xffffffff : 0xff808080, "%s", CustomizeDataMgr->GetSkinName((CUSTOMIZE_TYPE)m_iCustomizeType, i).c_str());
	}

	// �J�[�\���`��
	RenderCursor();
}

SelectCustomize::SelectMusic::SelectMusic(sceneCustomize *pCustomize) :Base(pCustomize, (int)CUSTOMIZE_TYPE::SELECT_MUSIC){}
void SelectCustomize::SelectMusic::Initialize()
{
	Base::Initialize();
	m_pStreamSound = g_pBGM->PlayStream((char*)(CustomizeDataMgr->GetSkin((CUSTOMIZE_TYPE)m_iCustomizeType, m_iCursor) + "/music.ogg").c_str());
}
void SelectCustomize::SelectMusic::Release()
{
	Base::Release();
	m_pStreamSound->Stop();
}
void SelectCustomize::SelectMusic::Update()
{
	// �J�[�\���ړ�������
	if (UpdateCursor())
	{
		// �X�g���[���~�߂�
		m_pStreamSound->Stop();

		// ���̃X�g���[��
		m_pStreamSound = g_pBGM->PlayStream((char*)(CustomizeDataMgr->GetSkin((CUSTOMIZE_TYPE)m_iCustomizeType, m_iCursor) + "/music.ogg").c_str());
	}
}
void SelectCustomize::SelectMusic::Render()
{
	// �Ȉꗗ�̕`��
	FOR((int)m_pCustomize->m_NumCustomizes[m_iCustomizeType])
	{
		// �\������x�̃��R�[�h
		if (!m_pCustomize->m_bSelect)m_pCustomize->m_pImages[m_pCustomize->IMAGE::MUSIC]->SetARGB(0xff808080);	// �Â�
		m_pCustomize->m_pImages[m_pCustomize->IMAGE::MUSIC]->Render((int)FRAME_OFFSET_XY.x + (i % 6) * 128 + 16, (int)FRAME_OFFSET_XY.y + (i / 6) * 64 + 8, 32, 32, (i % 8) * 32, (i / 8) * 32, 32, 32);
		m_pCustomize->m_pImages[m_pCustomize->IMAGE::MUSIC]->SetARGB(0xffffffff);	// �߂�

		// �Ȗ�
		m_pText->Draw((int)FRAME_OFFSET_XY.x + ((i % 6) * 128) + 16, (int)FRAME_OFFSET_XY.y + ((i / 6) * 64) + 40, (m_pCustomize->m_bSelect) ? 0xffffffff : 0xff808080, "%s", CustomizeDataMgr->GetSkinName((CUSTOMIZE_TYPE)m_iCustomizeType, i).c_str());
	}

	// �J�[�\���`��
	RenderCursor();
}


//=============================================================================================
//		��	��	��	��	�J	��
bool sceneCustomize::Initialize()
{
	// �t�F�[�h������
	Fade::Set(Fade::MODE::FADE_IN, 8);

	m_pImages[IMAGE::BACK]				= std::make_unique<tdn2DObj>("DATA/Image/back2.png");
	m_pImages[IMAGE::KEYLINE]			= std::make_unique<tdn2DObj>("DATA/Customize/Skin/back.png");
	m_pImages[IMAGE::UP_FRAME]			= std::make_unique<tdn2DObj>("DATA/UI/Other/up_frame.png");
	m_pImages[IMAGE::MIDASHI]			= std::make_unique<tdn2DObj>("DATA/UI/Other/midashi.png");
	m_fLoadPercentage = .1f;	// ���[�h����
	m_pImages[IMAGE::LEFT_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Customize/left_frame.png");
	m_pImages[IMAGE::RIGHT_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Customize/right_frame.png");
	m_fLoadPercentage = .2f;	// ���[�h����
	m_pImages[IMAGE::CURSOR_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Customize/cursor_frame.png");
	m_pImages[IMAGE::RIGHT_UP_FRAME]	= std::make_unique<tdn2DObj>("DATA/UI/Customize/right_up_frame.png");
	m_pImages[IMAGE::CURSOR_FRAME_S]	= std::make_unique<tdn2DObj>("DATA/UI/Other/cursor_frame2.png");
	m_pImages[IMAGE::MUSIC]				= std::make_unique<tdn2DObj>("DATA/UI/Customize/music.png");

	//	�ꎞ�����_�����O�p�T�[�t�F�C�X
	m_pPlayScreen = std::make_unique<tdn2DObj>(1280, 720, RENDERTARGET);

	//	�t���[���o�b�t�@�̃|�C���^�ۑ�
	tdnSystem::GetDevice()->GetRenderTarget(0, &m_pSurface);

	m_fLoadPercentage = .25f;	// ���[�h����

	// SE
	m_pSE = new tdnSoundSE;
	m_pSE->Set(0, 3, "DATA/Sound/SE/decide1.wav");	// ���艹

	// ���ꂼ��̃J�X�^�}�C�Y�̐�
	m_NumCustomizes = new UINT[(int)CUSTOMIZE_TYPE::MAX];

	// �J�X�^�}�C�Y�ǂݍ���
	FOR((UINT)CUSTOMIZE_TYPE::MAX)
	{
		// �܂��J�X�^�}�C�Y��
		m_NumCustomizes[i] = CustomizeDataMgr->GetNumSkin((CUSTOMIZE_TYPE)i);

		// ���̃J�X�^�}�C�Y�̐��őS�ǂݍ���
		switch ((CUSTOMIZE_TYPE)i)
		{
		case CUSTOMIZE_TYPE::SKIN:
			m_pSkins = new tdn2DObj*[m_NumCustomizes[i]];
		{for (UINT j = 0; j < m_NumCustomizes[i]; j++)
		{
			// �X�L���摜�̏�����
			m_pSkins[j] = new tdn2DObj(CustomizeDataMgr->GetSkin(CUSTOMIZE_TYPE::SKIN, j).c_str());
		}
		m_fLoadPercentage = .45f;	// ���[�h����
		}
			break;
		case CUSTOMIZE_TYPE::NOTE:
			m_pNotes = new tdn2DObj*[m_NumCustomizes[i]];
			{for (UINT j = 0; j < m_NumCustomizes[i]; j++)
			{
				// �m�[�c�摜�̏�����
				m_pNotes[j] = new tdn2DObj(CustomizeDataMgr->GetSkin(CUSTOMIZE_TYPE::NOTE, j).c_str());
			}
			}
			m_fLoadPercentage = .65f;	// ���[�h����
			break;
		case CUSTOMIZE_TYPE::EXPLOSION:
			m_pExplosions = new tdn2DObj*[m_NumCustomizes[i]];
			{for (UINT j = 0; j < m_NumCustomizes[i]; j++)
			{
				// �����摜�̏�����
				m_pExplosions[j] = new tdn2DObj(CustomizeDataMgr->GetSkin(CUSTOMIZE_TYPE::EXPLOSION, j).c_str());
			}
			}
			m_fLoadPercentage = .85f;	// ���[�h����
			break;
		case CUSTOMIZE_TYPE::SUDDEN:
			m_pSuddens = new tdn2DObj*[m_NumCustomizes[i]];
			{for (UINT j = 0; j < m_NumCustomizes[i]; j++)
			{
				// �T�h�D���摜�̏�����
				m_pSuddens[j] = new tdn2DObj(CustomizeDataMgr->GetSkin(CUSTOMIZE_TYPE::SUDDEN, j).c_str());
			}
			}
			m_fLoadPercentage = .95f;	// ���[�h����
			break;
			// �{�C�X�ƋȂ͖���
		case CUSTOMIZE_TYPE::VOICE:
		case CUSTOMIZE_TYPE::SELECT_MUSIC:
			break;
		}
	}
	m_fLoadPercentage = 1.0f;	// ���[�h����

	// �I������J�X�^�}�C�Y�^�C�v�J�[�\��������
	m_iSelectCustomizeType = 0;

	// �v���C���[�f�[�^�̃J�X�^�}�C�Y
	m_iSelectCustomizes = new int[(int)CUSTOMIZE_TYPE::MAX];
	FOR((int)CUSTOMIZE_TYPE::MAX) m_iSelectCustomizes[i] = PlayerDataMgr->m_CustomizeRecords[i]->no;

	// �J�X�^�}�C�Y�^�C�v�I����̈Ϗ�����
	m_pSelectCustomizes = new SelectCustomize::Base*[(int)CUSTOMIZE_TYPE::MAX];
	m_pSelectCustomizes[(int)CUSTOMIZE_TYPE::SKIN] = new SelectCustomize::Skin(this);
	m_pSelectCustomizes[(int)CUSTOMIZE_TYPE::NOTE] = new SelectCustomize::Note(this);
	m_pSelectCustomizes[(int)CUSTOMIZE_TYPE::EXPLOSION] = new SelectCustomize::Explosion(this);
	m_pSelectCustomizes[(int)CUSTOMIZE_TYPE::SUDDEN] = new SelectCustomize::Sudden(this);
	m_pSelectCustomizes[(int)CUSTOMIZE_TYPE::VOICE] = nullptr;
	m_pSelectCustomizes[(int)CUSTOMIZE_TYPE::SELECT_MUSIC] = new SelectCustomize::SelectMusic(this);
	m_bSelect = false;

	// �J�X�^�}�C�Y�e�X�g�̈ȏ�N
	m_pCustomizeTest = new CustomizeTestPlayer(this);

	// �T�h�D������
	m_fSuddenPercent = 0;
	m_fNextSudden = .25f;

	// �J�[�\�����W
	m_CurrentCursorPosY = m_NextCursorPosY = 195;

	return true;
}

sceneCustomize::~sceneCustomize()
{
	// �v���C���[�L�^�X�V
	FOR((int)CUSTOMIZE_TYPE::MAX) PlayerDataMgr->m_CustomizeRecords[i]->no = m_iSelectCustomizes[i];

	// �S�摜���
	DOUBLE_POINTER_DELETE(m_pSkins, (int)m_NumCustomizes[(int)CUSTOMIZE_TYPE::SKIN]);
	DOUBLE_POINTER_DELETE(m_pNotes, (int)m_NumCustomizes[(int)CUSTOMIZE_TYPE::NOTE]);
	DOUBLE_POINTER_DELETE(m_pExplosions, (int)m_NumCustomizes[(int)CUSTOMIZE_TYPE::EXPLOSION]);
	DOUBLE_POINTER_DELETE(m_pSuddens, (int)m_NumCustomizes[(int)CUSTOMIZE_TYPE::SUDDEN]);
	delete[] m_NumCustomizes;
	delete[] m_iSelectCustomizes;
	delete m_pSE;
	// �J�X�^�}�C�Y�ȏキ��J��
	if(m_bSelect)m_pSelectCustomizes[m_iSelectCustomizeType]->Release();
	DOUBLE_POINTER_DELETE(m_pSelectCustomizes, (int)CUSTOMIZE_TYPE::MAX);
	delete m_pCustomizeTest;
}
//
//=============================================================================================

//=============================================================================================
//		�X			�V
bool sceneCustomize::Update()
{
	static bool bEnd(false);	// �t�F�[�h�֌W�ŏI��������̂Ɏg��

	// �t�F�[�h
	Fade::Update();

	// �J�X�^�}�C�Y�e�X�g�N�X�V
	m_pCustomizeTest->Update();

	// �T�h�D�����
	m_fSuddenPercent = m_fSuddenPercent*.75f + m_fNextSudden*.25f;

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
		if (m_bSelect)
		{
			// ���̃t���[���኱�Â�����
			m_pImages[IMAGE::LEFT_FRAME]->SetARGB(0xff808080);
			// �E�̃t���[��
			m_pImages[IMAGE::RIGHT_UP_FRAME]->SetARGB(0xffffffff);

			// �J�[�\���g���邭
			m_pImages[IMAGE::CURSOR_FRAME_S]->SetARGB(0xffffffff);

			// �Ϗ�����X�V
			m_pSelectCustomizes[m_iSelectCustomizeType]->Update();
		}
		else
		{
			// �E�̃t���[���኱�Â�����
			m_pImages[IMAGE::RIGHT_UP_FRAME]->SetARGB(0xff808080);
			// ���̃t���[��
			m_pImages[IMAGE::LEFT_FRAME]->SetARGB(0xffffffff);

			// �J�[�\���g�Â�����
			m_pImages[IMAGE::CURSOR_FRAME_S]->SetARGB(0xff808080);

			// �㉺�Ńt�H���_�[�I��
			bool bCursored(false);
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT,15))
			{
				if (--m_iSelectCustomizeType < 0) m_iSelectCustomizeType = (int)CUSTOMIZE_TYPE::MAX - 1;
				bCursored = true;
			}
			else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT,15))
			{
				if (++m_iSelectCustomizeType >= (int)CUSTOMIZE_TYPE::MAX) m_iSelectCustomizeType = 0;
				bCursored = true;
			}
			if (bCursored)
			{
				g_pSE->Play("�J�[�\��2");										// �J�[�\�����Đ�
				m_NextCursorPosY = 195.0f + (m_iSelectCustomizeType * 73);	// �J�[�\���ʒu�쐬
			}

			// �G���^�[�ŃJ�X�^�}�C�Y�^�C�v����
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
			{
				g_pSE->Play("����2");// ���艹�Đ�

				if (m_pSelectCustomizes[m_iSelectCustomizeType])
				{
					m_pSelectCustomizes[m_iSelectCustomizeType]->Initialize();
					m_bSelect = true;
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
void sceneCustomize::Render()
{
	//	�ꎞ�o�b�t�@�֐؂�ւ�
	m_pPlayScreen->RenderTarget();

	
	tdnView::Clear();

	m_pImages[IMAGE::KEYLINE]->Render(0, 133);
	m_pSkins[m_iSelectCustomizes[(int)CUSTOMIZE_TYPE::SKIN]]->Render(0, 0);

	// �J�X�^�}�C�Y�e�X�g�`��
	m_pCustomizeTest->Render();

	m_pSuddens[m_iSelectCustomizes[(int)CUSTOMIZE_TYPE::SUDDEN]]->Render(0, 133, 1120, 60 + (int)(440 * m_fSuddenPercent), 0, 500 - (60 + (int)(440 * m_fSuddenPercent)), 1120, 60 + (int)(440 * m_fSuddenPercent));

	//	�t���[���o�b�t�@�֐؂�ւ�
	tdnSystem::GetDevice()->SetRenderTarget(0, m_pSurface);



	// �w�i
	m_pImages[IMAGE::BACK]->Render(0, 0);

	// ��̃t���[��
	m_pImages[IMAGE::UP_FRAME]->Render(0, 0);
	m_pImages[IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 192, 400, 64);

	// �����_�[�^�[�Q�b�g�����Q�[�����
	static int a = 0;
	if (KeyBoardTRG(KB_1)) a += 1;
	m_pPlayScreen->Render(412, 250, 802, 442, 0, 0, 1280, 720);

	// �ŏI�X�L���̃t���[��
	m_pImages[IMAGE::RIGHT_FRAME]->Render(0, 0);

	// �E�̃t���[��
	m_pImages[IMAGE::RIGHT_UP_FRAME]->Render(0, 0);

	// ���̃t���[��
	m_pImages[IMAGE::LEFT_FRAME]->Render(0, 0);

	// �Ϗ��`��
	if(m_pSelectCustomizes[m_iSelectCustomizeType])m_pSelectCustomizes[m_iSelectCustomizeType]->Render();

	// �J�[�\��
	static int AnimFrame = 0;
	if (++AnimFrame > 16 * 4) AnimFrame = 0;
	m_pImages[IMAGE::CURSOR_FRAME]->Render(98, (int)m_CurrentCursorPosY, 256, 64, 0, (AnimFrame / 16) * 64, 256, 64);

	// �t�F�[�h
	Fade::Render();
}
//
//=============================================================================================

bool CustomizeTestPlayer::Note::Update(int NumExplosionpanel)
{
	if (bNoteEnd)
	{
		if (++ExplosionFrame >= NumExplosionpanel * 2)
			return true;
	}
	else
	{
		if ((posY += 4) > 618) bNoteEnd = true;
	}

	return false;
}

void CustomizeTestPlayer::Note::Render(tdn2DObj *pNote, tdn2DObj *pExplosion)
{
	if (bNoteEnd)
	{
		pExplosion->Render(posX - 48, 618 - 60, 128, 128, (ExplosionFrame / 2) * 128, 0, 128, 128);
	}
	else
	{
		pNote->Render(posX, posY, 40, 16, (bBlack) ? 40 : 0, 0, 40, 16);
	}
}

void CustomizeTestPlayer::Clear()
{
	for (auto it = m_NoteList.begin(); it != m_NoteList.end();){ delete (*it); it = m_NoteList.erase(it); }
}

void CustomizeTestPlayer::Reset()
{
	m_NumExplosionPanel = m_pCustomize->m_pExplosions[m_pCustomize->m_iSelectCustomizes[(int)CUSTOMIZE_TYPE::EXPLOSION]]->GetWidth() / 128;
	//for (auto &it : m_NoteList) it->Reset();
}

void CustomizeTestPlayer::Update()
{
	static int CreateFrame = 0;
	static int CreateCursor = 0;
	static const int CreatePosX[30] =
	{
		120, 180, 240, 300, 360, 420, 480, 540, 600, 660, 720, 780, 840, 900, 960,
		1020, 960, 900, 840, 780, 720, 660, 600, 540, 480, 420, 360, 300, 240, 180
	};

	static int CreateInterval = 24;
	if (KeyBoardTRG(KB_NUMPAD8)) CreateInterval = max(CreateInterval - 1, 0);
	else if (KeyBoardTRG(KB_NUMPAD2)) CreateInterval = min(CreateInterval + 1, 24);
	if (++CreateFrame > CreateInterval)
	{
		CreateFrame = 0;
		m_NoteList.push_back(new Note(CreatePosX[CreateCursor], (CreateCursor % 2 == 1)));
		if (++CreateCursor >= 30) CreateCursor = 0;
	}

	for (auto it = m_NoteList.begin(); it != m_NoteList.end();)
	{
		if ((*it)->Update(m_NumExplosionPanel))	// true���Ԃ���������t���OON�Ƃ�������
		{
			delete (*it);
			it = m_NoteList.erase(it);
		}
		else it++;
	}
}

void CustomizeTestPlayer::Render()
{
	for (auto &it : m_NoteList)
		it->Render(
		m_pCustomize->m_pNotes[m_pCustomize->m_iSelectCustomizes[(int)CUSTOMIZE_TYPE::NOTE]],
		m_pCustomize->m_pExplosions[m_pCustomize->m_iSelectCustomizes[(int)CUSTOMIZE_TYPE::EXPLOSION]]);
}