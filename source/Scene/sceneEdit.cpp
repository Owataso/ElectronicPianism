#include "TDNLIB.h"
#include "../Music/MusicInfo.h"
#include "system/Framework.h"
#include "../Input/InputMIDI.h"
#include "sceneEdit.h"
#include "../Data/EditData.h"
#include "../Fade/Fade.h"
#include "../MIDI_Loader/MIDI_Loader.h"
#include "../Sound/SoundManager.h"
#include "../Music/OMS_Loader.h"
#include "../Music/PlayCursor.h"
#include "../Music/Note.h"
#include "../UI/ui.h"
#include "../Music/BeatBar.h"
#include "../Music/Soflan.h"
#include "../Player/Player.h"
#include "../Judge/Judge.h"
#include "../Data/PlayerData.h"
#include "../Music/EnumString.h"
#include "../Data/MusicDataBase.h"
#include "../Data/SelectMusicData.h"
#include "sceneModeSelect.h"
#include<iostream>


//*****************************************************************************************************************************
//
//		��		��		��	
//
//*****************************************************************************************************************************
bool sceneEdit::Initialize()
{
	//PlayerDataMgr->Initialize();

	tdnView::Init();

	Fade::Initialize();
	Fade::Set(Fade::MODE::FADE_IN, 5, 0, 255, 0x00000000);

	EditDataMgr->Initialize();

	m_fLoadPercentage = .25f;	// ���[�h����

	m_pMode = nullptr;
	ChangeMode(MODE::FIRST);

	m_fLoadPercentage = .35f;	// ���[�h����

	m_pNoteMgr = new NoteManager;
	m_pSoflanMgr = new SoflanManager;
	m_pBeatBar = new BeatBar;

	m_fLoadPercentage = .4f;	// ���[�h����

	//me->m_pSoflanMgr->Initialize();
	m_pNoteMgr->Initialize(m_pSoflanMgr);
	m_pUI = new UI(MAX_OCTAVE);
	m_pUI->Initialize(m_pBeatBar, m_pNoteMgr, m_pSoflanMgr, GAUGE_OPTION::NORMAL);

	m_fLoadPercentage = .5f;	// ���[�h����

	m_pJudge = new JudgeManager();
	m_pJudge->Initialize(m_pNoteMgr, GAUGE_OPTION::NORMAL, true);

	m_fLoadPercentage = .65f;	// ���[�h����

	m_pPlayer = new Player();
	m_pPlayer->Initialize(MAX_OCTAVE, m_pUI, m_pNoteMgr, m_pJudge, Player::MODE::AUTO);

	m_fLoadPercentage = .7f;	// ���[�h����

	// �v���C�J�[�\��
	play_cursor->Initialize();

	m_fLoadPercentage = .9f;	// ���[�h����

	m_iShift = 0;

	// �ȏ��Q��
	m_MusicInfo = SelectMusicMgr->Get();

	m_FolderInfo = nullptr;

	m_fLoadPercentage = 1.0f;	// ���[�h����

	return true;
}

sceneEdit::~sceneEdit()
{
	play_cursor->Release();
	delete m_pMode;
	EditDataMgr->Release();
	delete m_pNoteMgr;
	delete m_pUI;
	delete m_pSoflanMgr;
	delete m_pPlayer;
	delete m_pJudge;
	delete m_pBeatBar;
	SAFE_DELETE(m_FolderInfo);
}

//*****************************************************************************************************************************
//
//		�又��
//
//*****************************************************************************************************************************
bool sceneEdit::Update()
{
	m_pMode->Update();

	Fade::Update();

	return true;
}

//*****************************************************************************************************************************
//
//		�`��֘A
//
//*****************************************************************************************************************************
void	sceneEdit::Render( void )
{
	
	tdnView::Clear();

	m_pMode->Render();

	Fade::Render();
}

//*****************************************************************************************************************************
//
//		(process1)�ŏ��ɁA�V�K�쐬�����[�h���邩�I��
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	������
//------------------------------------------------------
void sceneEdit::Mode::First::Initialize()
{
	me->m_iSelectDifficulty = 0;
}
//------------------------------------------------------
//	�X�V
//------------------------------------------------------
void sceneEdit::Mode::First::Update()
{
	if (KeyBoardTRG(KB_SPACE))
	{
		//me->ChangeMode(MODE::NEW_CREATE);
		MainFrame->ChangeScene(new sceneModeSelect, true);
	}

	else if (KeyBoardTRG(KB_ENTER))
	{
		me->ChangeMode(MODE::DIRECTORY_SELECT);
	}
}

//------------------------------------------------------
//	�`��
//------------------------------------------------------
void sceneEdit::Mode::First::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE :�@�V�K�쐬 or ���[�h");
	tdnText::Draw(32, 70, 0xffffff00, "[SPACE]: �G�f�B�b�g���[�h�I���@�@�@[ENTER]: ���[�h");
}

//*****************************************************************************************************************************
//
//		(process2A)�V�K�t�H���_�쐬(���[�U�[�ɖ��O�����߂����A�f�B���N�g���[���쐬)
//
//*****************************************************************************************************************************
bool sceneEdit::Mode::NewCreateDirectry::bThread = false;

//------------------------------------------------------
//	������
//------------------------------------------------------
void sceneEdit::Mode::NewCreateDirectry::Initialize()
{
	// ���O���̓}���`�X���b�h�W�J
	bThread = true;
	_beginthread(NameInput, 0, (void*)&me->m_sMusicName);	// �Ȗ����Q�Ɠn���ŏ���������

}
//------------------------------------------------------
//	�X�V
//------------------------------------------------------
void sceneEdit::Mode::NewCreateDirectry::Update()
{
	switch (m_step)
	{

	case 0:	/* ���O���̓X���b�h���ꒆ */
		// ���O���͏I�����Ă���
		if (!bThread)
		{
			m_step++;
		}
		break;

	case 1:	/* �{���ɂ��̖��O�ł����ł����H */
		if (KeyBoardTRG(KB_ENTER)) m_step++;
		else if (KeyBoardTRG(KB_SPACE))
		{
			// �ŏ��ɖ߂�
			me->ChangeMode(MODE::FIRST);

			//m_step--;
			// ���������X���b�h�쐬
			//bThread = true;
			//_beginthread(NameInput, 0, (void*)&me->m_MusicName);
		}

		break;
	case 2:
		std::string path = "Edit/" + me->m_sMusicName;
		int result = tdnFile::CreateFolder((char*)path.c_str());
		if (result != 0)
		{
			// �����Ȃ������I
			MessageBox(0, "���̋Ȃ̃t�H���_�͊��ɍ쐬����Ă���\��������܂�", "�G���[", MB_OK);
			me->ChangeMode(MODE::FIRST);
		}
		else
		{
			// �����Ƃ���ꂽ�I
			me->ChangeMode(MODE::DIFFICULTY_SELECT);
		}
		break;
	}
}

//------------------------------------------------------
//	�`��
//------------------------------------------------------
void sceneEdit::Mode::NewCreateDirectry::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE : �f�B���N�g���쐬");

	// �{���ɂ��̖��O�ł����ł����H
	if (m_step == 1)
	{
		tdnText::Draw(320, 160, 0xffffff00, "�{���ɂ��̖��O�ł����ł����H");
		tdnText::Draw(320, 240, 0xffffff00, "%s", me->m_sMusicName.c_str());
		tdnText::Draw(320, 320, 0xffffff00, "�G���^�[: OK�@�@�@�X�y�[�X: ����ς��Ȃ���");
	}
}



//------------------------------------------------------
//	�X���b�h�p���O���͊֐�
//------------------------------------------------------
void sceneEdit::Mode::NewCreateDirectry::NameInput(void *arg)
{
	// �R���\�[���ŋȖ�����͂�����
	tdnSystem::OpenDebugWindow();
	std::cout << "�����Ȗ�����͂��Ă�����������\n" << std::endl;
	std::cout << "������: �X�y�[�X�ŋ󔒂�����̂͌��ւł��B�����_���g�p���Ă��������I" << std::endl;
	std::cout << "_���󔒂ɒu������܂�\n\n>";
	char str[128];
	//std::cin.getline(str, sizeof(str));//�X�y�[�X���܂ޓ��͂��A�����炸�󂯕t����
	std::cin >> str;

	// �A���_�[�o�[���󔒂ɒu�����鏈��
	for (int i = 0; str[i] != '\0'; i++) str[i] = (str[i] == '_') ? ' ' : str[i];

	// ������string�^�ɂԂ�����
	*((std::string*)arg) = str;

	// �R���\�[���I��
	tdnSystem::CloseDebugWindow();

	// �X���b�h�I��
	bThread = false;
	_endthread();
}


//*****************************************************************************************************************************
//
//		(process2B)�ȃt�H���_�̑I��
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	������
//------------------------------------------------------
void sceneEdit::Mode::DirectrySelect::Initialize()
{
	// �f�B���N�g����񏉊���
	if (!m_pDirectry)m_pDirectry = new DirectoryInfo;
	m_pDirectry->FileNames.clear();
	m_pDirectry->FolderNames.clear();

	// ���ʃf�[�^�t�H���_�̃t�@�C���p�X����ȃt�H���_���
	tdnFile::EnumDirectory("Edit", m_pDirectry);
	assert(m_pDirectry->FolderNames.size() != 0); // �����t�H���_������Ă��Ȃ��I
}
//------------------------------------------------------
//	�X�V
//------------------------------------------------------
void sceneEdit::Mode::DirectrySelect::Update()
{
	if (KeyBoardTRG(KB_UP))
	{
		// �J�[�\�������
		if (--m_SelectCursor < 0) m_SelectCursor = m_pDirectry->FolderNames.size() - 1;
	}
	else if (KeyBoardTRG(KB_DOWN))
	{
		// �J�[�\��������
		if (++m_SelectCursor >= (int)m_pDirectry->FolderNames.size()) m_SelectCursor = 0;
	}

	else if (KeyBoardTRG(KB_ENTER))
	{
		// �t�H���_�[���o�^���Ď��̃��[�h()��
		me->m_sMusicName = m_pDirectry->FolderNames[m_SelectCursor];
		me->ChangeMode(MODE::DIFFICULTY_SELECT);
	}
}

//------------------------------------------------------
//	�`��
//------------------------------------------------------
void sceneEdit::Mode::DirectrySelect::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE : �t�H���_�I��");
	tdnText::Draw(32, 70, 0xffffff00, "�Ȑ� : %d", m_pDirectry->FolderNames.size());

	// �ȃ��X�g�̕`��
	{
		// �s�̐�
		const int MAX_COLUMN = 10;												// �s�̍ő吔
		const int column = min(m_pDirectry->FolderNames.size(), MAX_COLUMN);	// �s�̐�
		const int ColumnCenter = column / 2;									// �s�̐�����^�񒆂�I�o

		// �s�̕`��
		for (int i = 0; i < column; i++)
		{
			const int width = ColumnCenter - i;
			const int add = width * -30;
			const int posY = 320 + add;

			int index = m_SelectCursor + (i - ColumnCenter);
			if (index >= (int)m_pDirectry->FolderNames.size())
			{
				index -= (m_pDirectry->FolderNames.size());
			}
			else if (index < 0)
			{
				index = m_pDirectry->FolderNames.size() + index;
			}

			BYTE alpha = (BYTE)(255 * (1 - (abs(width) / (float)ColumnCenter)));
			alpha = min(alpha + 64, 255);

			const DWORD col = (i == ColumnCenter) ? 0x0000ffff : 0x00ffffff;

			tdnText::Draw(640, posY,
				(col | alpha << 24),
				"%s", tdnFile::GetFileName((char*)m_pDirectry->FolderNames[index].c_str(), false).c_str());
		}
	}
}


//*****************************************************************************************************************************
//
//		(process3)��Փx�̑I��
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	������
//------------------------------------------------------
void sceneEdit::Mode::DifficultySelect::Initialize()
{
	// �G�f�B�b�g�Ȃ̃t�H���_�[�̏����쐬
	if (me->m_FolderInfo) delete me->m_FolderInfo;
	me->m_FolderInfo = new EditFolderInfo((char*)me->m_sMusicName.c_str());

	me->bMIDILoad = false;
	me->m_iPlayMusicNo = 0;
	me->m_iPlayTimer = 90;	// 1��30�b
}
//------------------------------------------------------
//	�X�V
//------------------------------------------------------
void sceneEdit::Mode::DifficultySelect::Update()
{
	if (KeyBoardTRG(KB_UP))
	{
		// �J�[�\�������
		if (--me->m_iSelectDifficulty < 0) me->m_iSelectDifficulty = (int)DIFFICULTY::MAX - 1;
	}
	else if (KeyBoardTRG(KB_DOWN))
	{
		// �J�[�\��������
		if (++me->m_iSelectDifficulty >= (int)DIFFICULTY::MAX) me->m_iSelectDifficulty = 0;
	}

	else if (KeyBoardTRG(KB_ENTER))
	{
		if (me->m_FolderInfo->bDifficultyExistance[me->m_iSelectDifficulty])
		{
			switch (MessageBox(0, "���̓�Փx�͂��łɍ���Ă��܂�\n���̓�Փx�̃G�f�B�b�g�f�[�^��V�K�ō�炸�A���[�h���܂����H", "�������ď�����", MB_YESNOCANCEL))
			{
			case IDYES:
				me->bMIDILoad = true;

				// oms�����[�h����
				EditDataMgr->LoadOMS_MakeMusicScore((me->m_FolderInfo->sMusicName + "/" + ENUM_STRING.m_DifficultyMap[(DIFFICULTY)me->m_iSelectDifficulty] + ".oms").c_str());

				// �V�t�g���
				me->m_iShift = EditDataMgr->GetMusicInfo()->omsInfo.shift;

				// �Đ�����Ȃ̔ԍ�
				me->m_iPlayMusicNo = EditDataMgr->GetMusicInfo()->omsInfo.PlayMusicNo;

				// �Đ�����
				me->m_iPlayTimer = EditDataMgr->GetMusicInfo()->omsInfo.PlayTime;

				// ��Փx
				SelectMusicMgr->Get()->eDifficulty = (DIFFICULTY)me->m_iSelectDifficulty;

				// �Đ�����Ȃ̐ݒ�
				if (me->m_iPlayMusicNo != 0)
				{
					char path[256];
					sprintf_s(path, 256, "%s/Music%d.ogg", me->m_sMusicName.c_str(), me->m_iPlayMusicNo);
					play_cursor->SetMusic(path);
				}

				break;
			
			case IDNO:
				break;
			
			case IDCANCEL:
				return;
				break;
			}
			
		}

		// ���낢��ݒ胂�[�h��
		me->ChangeMode(MODE::SETTING_INFO);
	}

	else if (KeyBoardTRG(KB_SPACE))
	{
		// �߂�
		me->ChangeMode(MODE::FIRST);
	}
}

//------------------------------------------------------
//	�`��
//------------------------------------------------------
void sceneEdit::Mode::DifficultySelect::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE : ��Փx�I��");

	// ��Փx�̕`��
	for (int i = 0; i < (int)DIFFICULTY::MAX; i++)
	{
		const int posY = 320 + i * 30;

		tdnText::Draw(640, posY, 0xffffffff, "%s", ENUM_STRING.m_DifficultyMap[(DIFFICULTY)i]);
	}
	tdnText::Draw(600, 320 + me->m_iSelectDifficulty * 30, 0xffffffff, "��");
}


//*****************************************************************************************************************************
//
//		(process4A)�����ȏ���ݒ肵�Ă���
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	������
//------------------------------------------------------
void sceneEdit::Mode::SettingInfo::Initialize()
{

}
//------------------------------------------------------
//	�X�V
//------------------------------------------------------
void sceneEdit::Mode::SettingInfo::Update()
{
	if (KeyBoardTRG(KB_1))
	{
		me->ChangeMode(MODE::SETTING_WAV);
	}
	else if (KeyBoardTRG(KB_2))
	{
		//// �I�[�v���t�@�C���_�C�A���O���g���ăt�@�C���p�X�擾
		std::string path = me->m_FolderInfo->sMusicName;
			//tdnFile::OpenFileDialog("MIDI�t�@�C��(*.mid)\0 *.mid\0\0");
		//
		//// �_�C�A���O�L�����Z��������o�Ă������I
		//if (path == "") return;

		if (me->bMIDILoad)
		{
			if (MessageBox(0, "���łɕ��ʃf�[�^�������Ă��܂��B�㏑�����܂����H", "ElectronicPisnism", MB_OKCANCEL) == IDCANCEL)	// �L�����Z����I��
			{
				return;
			}
		}

		// ���݂̓�Փx����p�X�쐬
		static const char *MIDINames[(int)DIFFICULTY::MAX] = { "N", "H", "E", "V" };

		path = path + "/other/" + MIDINames[me->m_iSelectDifficulty] + ".mid";

		// ���t�@�C���p�X����MIDI�f�[�^��ǂ�ŕ��ʍ쐬����
		EditDataMgr->LoadMIDI_MakeMusicScore(path.c_str());

		// m_iShift�㏑��(0�ɂȂ��Ă��܂��̖h�~)
		EditDataMgr->GetMusicInfo()->omsInfo.shift = me->m_iShift;

		// �ǂݍ��݃t���OON
		me->bMIDILoad = true;
	}

	else if (KeyBoardTRG(KB_SPACE))
	{
		// ��Փx�I�����[�h�Ɉڍs
		me->ChangeMode(MODE::DIFFICULTY_SELECT);
	}

	else if (KeyBoardTRG(KB_ENTER))
	{
		// �f�o�b�O�p
		// ���t�@�C���p�X����MIDI�f�[�^��ǂ�ŕ��ʍ쐬����
		//EditDataMgr->LoadMIDI_MakeMusicScore("DATA/Musics/EP/No_0/collision_of_elements.mid");
		//EditDataMgr->LoadOMS_MakeMusicScore("DATA/Musics/Edit/airu/NORMAL.oms");

		// �ǂݍ��݃t���OON
		//me->bMIDILoad = true;

		// MIDI�ǂ�ł���
		if (me->bMIDILoad)
		{
			// �t�@�C���p�X����MIDI�f�[�^��ǂ�ŕ��ʍ쐬����
			//EditDataMgr->LoadMIDI_MakeMusicScore(me->m_MIDIName);

			// �����f�[�^������Ȃ�ݒ肷��
			//if (me->m_PlayMusicNo != -1)
			//{
			//	EditDataMgr->SetWavData(me->m_WavName);
			//}
			//EditDataMgr->SetMusicName("�e�X�g");

			// ��Փx�ݒ�
			me->m_MusicInfo->eDifficulty = (DIFFICULTY)me->m_iSelectDifficulty;

			// �ǂݍ���OK���[�h�Ɉڍs
			me->ChangeMode(MODE::LOAD_OK);
			InputMIDIMgr->SetBaseOctave(me->m_MusicInfo->omsInfo.BaseOctave);
		}
		else
		{
			MessageBox(0, "���ʃf�[�^���ǂݍ��܂�Ă��܂���B(MIDI�f�[�^��ǂݍ���ł�������)", "�G���[", MB_OK);
		}
	}
}

//------------------------------------------------------
//	�`��
//------------------------------------------------------
void sceneEdit::Mode::SettingInfo::Render()
{
	tdnText::Draw(1000, 30, 0xffffff00, "��Փx: %s", ENUM_STRING.m_DifficultyMap[(DIFFICULTY)me->m_iSelectDifficulty]);
	tdnText::Draw(32, 30, 0xffffff00, "MODE : ���낢��ݒ�");
	tdnText::Draw(32, 70, 0xffffff00, "[1]: �ȏ��ݒ�");
	tdnText::Draw(32, 110, 0xffffff00, "[2]: MIDI�t�@�C�����J��");
	tdnText::Draw(32, 150, 0xffffff00, "[ENTER]: NEXT");

	tdnText::Draw(32, 320, 0xffffff00, "MIDI�f�[�^: %s", (me->bMIDILoad) ? "�ǂݍ��ݍς�" : "����܂���");
	//tdnText::Draw(32, 350, 0xffffff00, "�Ȃ�WAV�t�@�C���p�X: %s", (strcmp("", me->m_WavName) == 0) ? "�܂��ǂݍ���łȂ���I" : "�ǂݍ��ݍς�");
	//tdnText::Draw(32, 390, 0xffffff00, "�Ȃ�WAV�t�@�C���p�X: %s", (strcmp("", me->m_WavName) == 0) ? "�܂��ǂݍ���łȂ���I" : "�ǂݍ��ݍς�");
	//tdnText::Draw(32, 430, 0xffffff00, "MIDI�t�@�C���p�X: %s", (strcmp("", me->m_MIDIName) == 0) ? "�܂��ǂݍ���łȂ���I" : "�ǂݍ��ݍς�");
}

//*****************************************************************************************************************************
//
//		(process4B)WAV�f�[�^�̓ǂݍ���)
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	������
//------------------------------------------------------
void sceneEdit::Mode::SettingWAV::Initialize()
{

}
//------------------------------------------------------
//	�X�V
//------------------------------------------------------
void sceneEdit::Mode::SettingWAV::Update()
{
	// �Đ��Ȃ̐ݒ�
	if (KeyBoardTRG(KB_Z))
	{
		if (me->m_iPlayMusicNo == 1) me->m_iPlayMusicNo = 0;
		else if (me->m_FolderInfo->bMusicExistance[0]) me->m_iPlayMusicNo = 1;
	}
	else if (KeyBoardTRG(KB_X))
	{
		if (me->m_iPlayMusicNo == 2) me->m_iPlayMusicNo = 0;
		else if (me->m_FolderInfo->bMusicExistance[1]) me->m_iPlayMusicNo = 2;
	}
	else if (KeyBoardTRG(KB_C))
	{
		if (me->m_iPlayMusicNo == 3) me->m_iPlayMusicNo = 0;
		else if (me->m_FolderInfo->bMusicExistance[2]) me->m_iPlayMusicNo = 3;
	}
	else if (KeyBoardTRG(KB_V))
	{
		if (me->m_iPlayMusicNo == 4) me->m_iPlayMusicNo = 0;
		else if (me->m_FolderInfo->bMusicExistance[3]) me->m_iPlayMusicNo = 4;
	}

	// �Đ����Ԃ̑���
	if (KeyBoard(KB_RIGHT) && me->m_iPlayMusicNo == 0) me->m_iPlayTimer += 1;
	else if (KeyBoard(KB_LEFT) && me->m_iPlayMusicNo == 0 && me->m_iPlayTimer > 1) me->m_iPlayTimer -= 1;

	int SelectNo(-1);
	if (KeyBoardTRG(KB_1)) SelectNo = 1;
	else if (KeyBoardTRG(KB_2)) SelectNo = 2;
	else if (KeyBoardTRG(KB_3)) SelectNo = 3;
	else if (KeyBoardTRG(KB_4)) SelectNo = 4;
	else if (KeyBoardTRG(KB_5)) SelectNo = 5;

	// ���̃t���[�����ɉ����I�΂�Ă���
	if (SelectNo != -1)
	{
		// �I�[�v���t�@�C���_�C�A���O���g���ăt�@�C���p�X�擾
		std::string path = tdnFile::OpenFileDialog("WAV�t�@�C��(*.wav)\0 *.wav\0\0");

		// �_�C�A���O�L�����Z��������o�Ă������I
		if (path == "") return;

		char str[256];

		// �����p
		if (SelectNo == 5)
		{
			if (me->m_FolderInfo->bAudition)
			{
				if (MessageBox(0, "���łɋȃf�[�^�������Ă��܂��B�㏑�����܂����H", path.c_str(), MB_OKCANCEL) == IDCANCEL)	// �L�����Z����I��
				{
					return;
				}
			}

			// �t�H���_�[�o�^�t���OON
			me->m_FolderInfo->bAudition = true;

			// �����o���p�X�쐬
			sprintf_s(str, 256, "%s/Audition.owd", me->m_FolderInfo->sMusicName.c_str());
		}
		// �ȃf�[�^
		else if (SelectNo < 4)
		{
			if (me->m_FolderInfo->bMusicExistance[SelectNo])
			{
				if (MessageBox(0, "���łɋȃf�[�^�������Ă��܂��B�㏑�����܂����H", path.c_str(), MB_OKCANCEL) == IDCANCEL)	// �L�����Z����I��
				{
					return;
				}
			}

			// �t�H���_�[�o�^�t���OON
			me->m_FolderInfo->bMusicExistance[SelectNo-1] = true;

			// �����o���p�X�쐬
			sprintf_s(str, 256, "%s/Music%d.owd", me->m_FolderInfo->sMusicName.c_str(), SelectNo);
		}
		else assert(0);

		// OWD�f�[�^�Ƃ��ĉ����f�[�^�����o��
		WriteOWD((char*)path.c_str(), str);
	}

	if (KeyBoardTRG(KB_SPACE))
	{
		// ���Đ�����ɋȏ��ݒ�
		if (me->m_iPlayMusicNo == 0)
		{
			// ���������Ȃ�
			play_cursor->SetMusic(nullptr);
		}
		else
		{
			// �p�X�쐬
			char path[256];
			sprintf_s(path, 256, "%s/Music%d.ogg", me->m_FolderInfo->sMusicName.c_str(), me->m_iPlayMusicNo);

			// �ݒ�
			play_cursor->SetMusic(path);
		}

		// ���[�h�`�F���W
		me->ChangeMode(MODE::SETTING_INFO);
	}

}

//------------------------------------------------------
//	�`��
//------------------------------------------------------
void sceneEdit::Mode::SettingWAV::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE : WAV�f�[�^�ݒ胂�[�h");
	tdnText::Draw(32, 70, 0xffffff00, "[1][2][3][4]: �ȂɎg�p���鉹���f�[�^�ǂݍ���");
	tdnText::Draw(32, 110, 0xffffff00, "[5]: �����p�����f�[�^�ǂݍ���");
	tdnText::Draw(32, 150, 0xffffff00, "[Z][X][C][V]: �ǂ̋Ȃ𗬂����ݒ�");
	tdnText::Draw(32, 190, 0xffffff00, "[SPACE]: �߂�");

	for (int i = 0; i < (int)DIFFICULTY::MAX; i++)
	{
		tdnText::Draw(32, 320 + i * 30, 0xffffff00, "Music%d: %s", i+1, (me->m_FolderInfo->bMusicExistance[i]) ? "�ǂݍ��ݍς�" : "����܂���");
	}
	tdnText::Draw(32, 450, 0xffffff00, "�����p�f�[�^: %s", (me->m_FolderInfo->bAudition) ? "�ǂݍ��ݍς�" : "����܂���");

	char str[64];

	// �����Ȃ��ݒ肵�Ă���
	if (me->m_iPlayMusicNo != 0)
	{
		sprintf_s(str, 64, "Music%d", me->m_iPlayMusicNo);
	}

	// ���������Ȃ�
	else
	{
		tdnText::Draw(640, 490, 0xffffff00, "�Đ�����: %d��%d�b", me->m_iPlayTimer / 60, me->m_iPlayTimer % 60);
		tdnText::Draw(32, 230, 0xffffff00, "[���E�L�[]: �Đ����ԕύX");
		sprintf_s(str, 64, "�����Ȃ�");
	}

	tdnText::Draw(32, 490, 0xffffff00, "���̓�Փx�ł̗�����: %s", str);
}


//*****************************************************************************************************************************
//
//		(process5A)���[�h����(�������画��̒����̃e�X�g�v���C�A�f�[�^�����o�����s��)
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	������
//------------------------------------------------------
void sceneEdit::Mode::LoadOK::Initialize()
{

}

//------------------------------------------------------
//	�X�V
//------------------------------------------------------
void sceneEdit::Mode::LoadOK::Update()
{
	// �㉺�L�[�ŕ��ʂƋȂ̍��𒲐�
	if (KeyBoard(KB_DOWN))
	{
		me->m_iShift -= 5;
		// �����o������Ƀf�[�^�ɕ␳����l��ݒ�
		EditDataMgr->SetShift(me->m_iShift);
	}
	else if (KeyBoard(KB_UP))
	{
		me->m_iShift += 5;
		// �����o������Ƀf�[�^�ɕ␳����l��ݒ�
		EditDataMgr->SetShift(me->m_iShift);
	}

	// �G���^�[�L�[
	if (KeyBoardTRG(KB_ENTER))
	{
		// �Đ����[�h�Ɉڍs
		me->ChangeMode(MODE::PLAYING);
	}

	// �X�y�[�X�L�[
	else if (KeyBoardTRG(KB_1))
	{
		// �L�����Z����������o�Ă������I
		if (MessageBox(0, "�����o���܂����H", "�t�@�C���̕ۑ�", MB_OKCANCEL) == IDCANCEL) return;

		// �Đ��ԍ��ݒ�
		EditDataMgr->GetMusicInfo()->omsInfo.PlayMusicNo = me->m_iPlayMusicNo;

		// �Đ����Ԑݒ�
		EditDataMgr->GetMusicInfo()->omsInfo.PlayTime = me->m_iPlayTimer;

		// ���x���ݒ�
		//EditDataMgr->GetMusicInfo()->omsInfo.level = me->level;

		// ���ʂƂ��ȃf�[�^���t�@�C���Ƃ��ď����o���I
		std::string path = me->m_FolderInfo->sMusicName;

		// �����}�W�ŉ��Ƃ�������
		//if (me->m_SelectDifficulty == (int)DIFFICULTY::NORMAL) path += "Normal.oms";
		//else if (me->m_SelectDifficulty == (int)DIFFICULTY::HYPER) path += "Hyper.oms";
		//else if (me->m_SelectDifficulty == (int)DIFFICULTY::EXPERT) path += "Expert.oms";
		//else if (me->m_SelectDifficulty == (int)DIFFICULTY::VIRTUOSO) path += "Virtuoso.oms";

		path = path + "/" + ENUM_STRING.m_DifficultyMap[(DIFFICULTY)me->m_iSelectDifficulty] + ".oms";

		// ���f�[�^�����o���I
		EditDataMgr->WriteMusicScore((char*)path.c_str());
	}

	else if (KeyBoardTRG(KB_SPACE))
	{
		// �ݒ胂�[�h�Ɉڍs
		me->ChangeMode(MODE::SETTING_INFO);
	}
}

//------------------------------------------------------
//	�`��
//------------------------------------------------------
void sceneEdit::Mode::LoadOK::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE : ���[�h����");
	tdnText::Draw(32, 70, 0xffffff00, "�㉺�L�[: ���ʂƋȂ̒��� : %d", me->m_iShift);
	tdnText::Draw(32, 150, 0xffffff00, "�G���^�[: �Đ��J�n");
	tdnText::Draw(32, 190, 0xffffff00, "[1]: �f�[�^�����o��");
	tdnText::Draw(32, 230, 0xffffff00, "�X�y�[�X: �߂�");
	tdnText::Draw(32, 270, 0xffffff00, "���m�[�c��: %d", me->m_MusicInfo->omsInfo.NumNotes);
	tdnText::Draw(32, 310, 0xffffff00, "�I�N�^�[�u��: %d", me->m_MusicInfo->omsInfo.OctaveWidth);
}



//*****************************************************************************************************************************
//
//		(process5B)�Đ����
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	������
//------------------------------------------------------
void sceneEdit::Mode::Playing::Initialize()
{
	// �Đ��ʒu�̏�����
	//play_cursor->Initialize();

	me->m_pUI->SetOctaveWidth(me->m_MusicInfo->omsInfo.OctaveWidth);

	// �Đ�
	play_cursor->SetShift(me->m_iShift);
	play_cursor->Play(me->m_iPlayTimer * 1000);
	//play_cursor->SetSpeed(1.27f);

	// �\�t�����ƃm�[�c�̏����Z�b�e�B���O
	me->m_pSoflanMgr->Set(me->m_MusicInfo);
	me->m_pNoteMgr->Set(me->m_MusicInfo);

	// �X�R�A�ƃR���{�����Z�b�g
	me->m_pJudge->Reset();
	me->m_pBeatBar->Reset();
}

//------------------------------------------------------
//	�X�V
//------------------------------------------------------
void sceneEdit::Mode::Playing::Update()
{
	me->m_pBeatBar->Update(play_cursor->GetMSecond());

	// �m�[�c�X�V
	me->m_pNoteMgr->Update(me->m_pJudge);

	// �\�t�����X�V(��BPM���؂�ւ������true���Ԃ�̂ŁArhythm�o�[��؂�ւ��Ă�����)
	if (me->m_pSoflanMgr->Update()) me->m_pBeatBar->SetBPM(me->m_pSoflanMgr->GetBPM());

	// ���t�ҍX�V
	me->m_pPlayer->Update();

	// ����X�V
	me->m_pJudge->Update();

	if (KeyBoardTRG(KB_ENTER))
	{
		play_cursor->Stop();
		me->ChangeMode(MODE::LOAD_OK);
		return;
	}

	if (play_cursor->isEnd())
	{
		play_cursor->Stop();
		me->ChangeMode(MODE::LOAD_OK);
	}
	else
	{
		static float speed = 1.0f;
		if (KeyBoardTRG(KB_NUMPAD8))
		{
			play_cursor->SetSpeed((speed += .08f));
		}
		else if (KeyBoardTRG(KB_NUMPAD2))
		{
			play_cursor->SetSpeed((speed -= .08f));
		}
	}
}

//------------------------------------------------------
//	�`��
//------------------------------------------------------
void sceneEdit::Mode::Playing::Render()
{
	// UI(�m�[�g�Ƃ���)�`��
	me->m_pUI->RenderBack();
	me->m_pUI->Render();
	me->m_pUI->RenderMusicName((char*)tdnFile::GetFileName((char*)me->m_sMusicName.c_str(), false).c_str());
	tdnText::Draw(1090, 78, 0xffffffff, "EDIT");

	// ����`��
	me->m_pJudge->Render();
	//Text::Draw(32, 64, 0xffffff00, "BPM : %.1f");
}