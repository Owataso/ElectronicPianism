#include "TDNLIB.h"
#include "system/Framework.h"
#include "../Data/PlayerData.h"
#include "../Data/MusicDataBase.h"
#include "sceneEntry.h"
#include "SceneModeSelect.h"
#include "../Sound/SoundManager.h"
#include "../Input/InputMIDI.h"
#include "../Alphabet/Alphabet.h"

//=============================================================================================
//		��	��	��	��	�J	��
bool sceneEntry::Initialize()
{
	// �v���C���[���ǂݍ���&�����f�[�^�����邩�ǂ����B
	m_bLoadSaveData = PlayerDataMgr->Initialize();

	m_fLoadPercentage = .75f;

	// �v���C���[�f�[�^���Ȃ��Ƃ��̃X�e�[�g�N���X�|�C���^�̏�����
	m_pNoDataState = new NoDataState::Select(this);

	m_fLoadPercentage = 1.0f;

	return true;
}

sceneEntry::~sceneEntry()
{
	m_bLoad = false;
	delete m_pNoDataState;
}
//=============================================================================================


//=============================================================================================
//		�X			�V
void sceneEntry::NoDataState::Select::Update()
{
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		// �V�[���؂�ւ�
		MainFrame->ChangeScene(new sceneModeSelect, true);
	}

	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
	{
		// ���O���̓X�e�[�g�Ɉڍs
		m_parent->ChangeMode(NO_PLAYER_DATA_STATE::NAME_INPUT);
	}
}


bool sceneEntry::Update()
{
	// �����f�[�^����
	if (m_bLoadSaveData)
	{
		// �v���C�񐔃J�E���g
		PlayerDataMgr->m_PlayerInfo.PlayCount++;

		//if (KeyBoardTRG(KB_ENTER))
			MainFrame->ChangeScene(new sceneModeSelect, true);
	}

	// �V�K�쐬
	else
	{
		// ���[�h�|�C���^���s
		m_pNoDataState->Update();
	}

	return true;
}
//
//=============================================================================================


//=============================================================================================
//		�`			��
void sceneEntry::NoDataState::Select::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "�Z�[�u�f�[�^������܂���I");
	tdnText::Draw(32, 60, 0xffffff00, "ENTER : �f�[�^�����ŊJ�n");
	tdnText::Draw(32, 90, 0xffffff00, "SPACE : �V�K�f�[�^�쐬");
}

void sceneEntry::Render()
{
	
	tdnView::Clear();

	// �����f�[�^�Ȃ�
	if (!m_bLoadSaveData)
	{
		// ���[�h�|�C���^���s
		m_pNoDataState->Render();
	}
}
//
//=============================================================================================




//=============================================================================================
//		���O����
sceneEntry::NoDataState::NameInput::NameInput(sceneEntry *me) :Base(me), m_cursor(0), m_InputStep(0), m_bEnd(false), m_pSE(new tdnSoundSE), m_pAlphabetRenderer(new AlphabetRenderer)
{
	m_pSE->Set(0, 1, "DATA/Sound/SE/entry.wav");

	memset(m_InputName, '\0', sizeof(m_InputName));


}
sceneEntry::NoDataState::NameInput::~NameInput()
{
	// ���
	delete m_pAlphabetRenderer;
	delete m_pSE;
}
void sceneEntry::NoDataState::NameInput::Update()
{
	if (m_bEnd)
	{
		// SE�̍Đ��I�������
		if (!m_pSE->isPlay(0, 0))
		{
			// �f�[�^�V�K�쐬
			PlayerDataMgr->NewCreate(m_InputName);
			m_parent->m_bLoadSaveData = true;
		}
	}
	else
	{
		// �������
		m_InputName[m_InputStep] = EP_AlphabetTable[m_cursor];

		// �㉺�Ŗ��O�J�[�\���ړ�
		if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
		{
			g_pSE->Play("�J�[�\��2");
			if (--m_cursor < 0) m_cursor = _countof(EP_AlphabetTable) - 1;
		}
		else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
		{
			g_pSE->Play("�J�[�\��2");
			if (++m_cursor >= _countof(EP_AlphabetTable)) m_cursor = 0;
		}

		// �G���^�[�ŕ�������
		if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
		{
			// �������������A10�����S�����͂��Ă���
			if (m_InputName[m_InputStep] == '\0' || ++m_InputStep >= 10)
			{
				m_bEnd = true;
				g_pSE->Play("�J�[�\��2");	// �G���g���[��
			}
			else m_InputName[m_InputStep] = EP_AlphabetTable[m_cursor];
		}

		// �X�y�[�X�Ńo�b�N
		else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
		{
			// �ŏ��̕�������Ȃ�������
			if (m_InputStep > 0)
			{
				// �f�[�^�V�K�쐬
				m_InputName[m_InputStep--] = '\0';

				// �O�ɖ߂镶���̃J�[�\���擾
				m_cursor = m_pAlphabetRenderer->GetAlphabetID(m_InputName[m_InputStep]);
			}
		}
	}
}

void sceneEntry::NoDataState::NameInput::Render()
{
	static const int OFFSET_X = 320;	// ���O�J�n�n�_��X���W
	static const int OFFSET_Y = 320;	// ���O�J�n�n�_��Y���W

	// ���́u�����v�����`��
	for (int i = 0; i < m_InputStep; i++)
	{
		m_pAlphabetRenderer->Render(OFFSET_X + i * 64, 320, m_InputName[i]);
	}

	// ���́u����v�����`��
	{
		// �s�̐�
		const int MAX_COLUMN = 5;										// �s�̍ő吔
		const int column = min(_countof(EP_AlphabetTable), MAX_COLUMN);	// �s�̐�
		const int ColumnCenter = column / 2;							// �s�̐�����^�񒆂�I�o

		// �s�̕`��
		for (int i = 0; i < column; i++)
		{

			const int width = (ColumnCenter - i) * -64;	// �s�̐^�񒆂���_�Ƃ��āA�������炸����(�^�񒆂Ȃ�������0)
			const int posY = OFFSET_Y + width;

			// �����̃J�[�\���ł̑O����擾
			int index = m_cursor + (i - ColumnCenter);

			// ���ɍs����������(��ʂł͏�)
			if (index < 0)
			{
				// �S�A���t�@�x�b�g���𑫂����Ƃɂ���āA�ߏ蕪�����点��
				index = _countof(EP_AlphabetTable) + index;
			}

			// ��ɍs����������(��ʂł͉�)
			else if (index >= _countof(EP_AlphabetTable))
			{
				// �S�A���t�@�x�b�g���𑫂����Ƃɂ���āA�ߏ蕪�����点��
				index -= (_countof(EP_AlphabetTable));
			}

			BYTE alpha = (BYTE)(255 * (1 - (abs(width) / (float)ColumnCenter)));
			alpha = min(alpha + 64, 255);

			m_pAlphabetRenderer->Render(OFFSET_X + m_InputStep * 64, posY, EP_AlphabetTable[index]);
		}
	}
}