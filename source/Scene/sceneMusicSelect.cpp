#include "TDNLIB.h"
#include "system/Framework.h"
#include "sceneMusicSelect.h"
#include "scenePlaying.h"
#include "sceneModeSelect.h"
#include "SceneEdit.h"
#include "../Fade/Fade.h"
#include "../Data/PlayerData.h"
#include "../Data/SelectMusicData.h"
#include "../Data/MusicDataBase.h"
#include "../Music/EnumString.h"
#include "../Data/CustomizeData.h"
#include "../Sound/SoundManager.h"
#include "../Mode/ModeManager.h"
#include "../Number/Number.h"
#include "../Input/InputMIDI.h"
#include "../Sort/Sort.h"
#include "TDN_Movie.h"

#define EP_MUSIC_LOOP for (int i = 0; i < MUSIC_DATA_BASE->GetNumEPContents(); i++)

static const int NO_SELECT_MUSIC(-1);
int sceneMusicSelect::Mode::Base::m_iSelectMusic(NO_SELECT_MUSIC);

//=============================================================================================
//		��	��	��	��	�J	��
bool sceneMusicSelect::Initialize()
{
	// �t�F�[�h������
	Fade::Set(Fade::MODE::FADE_IN, 5, 0x00ffffff);

	// ���ԏ�����
	m_LimitTime = (ModeMgr->GetMode() == PLAY_MODE::STANDARD) ? 99 * 60 : NOLILMIT_TIME;

	// �V�[���ɓ����Ă���̑����t�܂ł̎���
	m_WaitTimer = 28;

	// �I���Փx������
	m_ipSelectDifficulty = SelectManager::GetInstance()->GetSelectDifAddress();
	m_DifficultyFrameX = 39.0f + *m_ipSelectDifficulty * 172;

	m_fLoadPercentage = .05f;	// ���[�h����

	// ���揉����
	m_pBackMovie = new tdnMovie("DATA/movie/MusicSelect.wmv");
	m_pBackMovie->Play();

	// �摜������
	//m_pImages[IMAGE::BACK] = std::make_unique<tdn2DObj>("DATA/Image/select_back.png");
	m_pImages[IMAGE::BACK]			= std::make_unique<tdn2DObj>(m_pBackMovie->GetTexture());
	m_fLoadPercentage = .1f;	// ���[�h����
	m_pImages[IMAGE::MUSIC_FRAME]	= std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/music_frame.png");
	m_pImages[IMAGE::NUMBER]		= std::make_unique<tdn2DObj>("DATA/Number/number.png");
	m_pImages[IMAGE::NUMBER_L]		= std::make_unique<tdn2DObj>("DATA/Number/numberL.png");
	m_fLoadPercentage = .15f;	// ���[�h����
	m_pImages[IMAGE::LAMP]			= std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/lamp.png");
	m_pImages[IMAGE::BPM_FONT]		= std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/bpm_font.png");
	m_fLoadPercentage = .2f;	// ���[�h����
	m_pImages[IMAGE::OPTION]		= std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/option.png");
	m_pImages[IMAGE::OPTION_CURSOR] = std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/option_cursor.png");
	m_pImages[IMAGE::OPTION_CURSOR]->SetARGB(0x80ffffff);
	m_pImages[IMAGE::OCTAVE_BACK]	= std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/octave.png");
	m_fLoadPercentage = .25f;	// ���[�h����

	// �A�j���[�V�����摜������
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI] = std::make_unique<tdn2DAnim>("DATA/UI/Other/midashi.png");
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->OrderMoveAppeared(16, -8, 12);
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Action(25);
	m_fLoadPercentage = .3f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::STAGE] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/stage.png");
	m_pAnimImages[(int)ANIM_IMAGE::STAGE]->OrderMoveAppeared(16, 760, 35);
	m_pAnimImages[(int)ANIM_IMAGE::STAGE]->Action(25);
	m_fLoadPercentage = .35f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/up_frame.png");
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->OrderMoveAppeared(30, 0, -64);
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Action(0);
	m_fLoadPercentage = .45f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::DIF_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/difficulty.png");
	m_pAnimImages[(int)ANIM_IMAGE::DIF_FRAME]->OrderStretch(15, 1, .0f);
	m_pAnimImages[(int)ANIM_IMAGE::DIF_FRAME]->Action(20);
	m_fLoadPercentage = .5f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_DIFFICULTY] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/difficulty_select.png");
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_DIFFICULTY]->OrderMoveAppeared(5, (int)m_DifficultyFrameX, 433 + 180);
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_DIFFICULTY]->Action(35);
	m_fLoadPercentage = .55f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/folder.png");
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->OrderMoveAppeared(12, 70 - 32, 200);
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->Action(25);
	m_fLoadPercentage = .6f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/folder_up.png");
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->OrderMoveAppeared(12, 360 + 32, 200 - 40);
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->Action(25);
	m_fLoadPercentage = .65f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::RANK] = std::make_unique<tdn2DAnim>("DATA/UI/Other/rank.png");
	m_pAnimImages[(int)ANIM_IMAGE::RANK]->OrderJump(10, 1.0f, .2f);
	m_pAnimImages[(int)ANIM_IMAGE::RANK]->Action(40);
	m_fLoadPercentage = .7f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/score.png");
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->OrderStretch(15, 1, .0f);
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->Action(25);
	m_fLoadPercentage = .75f;	// ���[�h����
	m_pAnimImages[(int)ANIM_IMAGE::CLEAR_TEXT] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/clear.png");
	m_pAnimImages[(int)ANIM_IMAGE::CLEAR_TEXT]->OrderMoveAppeared(15, 250, 603 + 10);
	m_pAnimImages[(int)ANIM_IMAGE::CLEAR_TEXT]->Action(25);
	m_pAnimImages[(int)ANIM_IMAGE::OCTAVE] = std::make_unique<tdn2DAnim>(m_pImages[IMAGE::OCTAVE_BACK]->GetTexture());
	m_pAnimImages[(int)ANIM_IMAGE::OCTAVE]->OrderAlpha(16, 0, 1);

	// �\�[�g����N���X������
	m_pSort[(int)FOLDER_TYPE::BEGINNER]				 = std::make_unique<MusicSort::Beginner>();
	m_pSort[(int)FOLDER_TYPE::ALL]					 = std::make_unique<MusicSort::All>();
	m_pSort[(int)FOLDER_TYPE::ELECTRONIC_PIANISM]	 = std::make_unique<MusicSort::ElectronicPianism>();
	m_pSort[(int)FOLDER_TYPE::LEVEL1_5]				 = std::make_unique<MusicSort::Level1_5>();
	m_pSort[(int)FOLDER_TYPE::LEVEL6_10]			 = std::make_unique<MusicSort::Level6_10>();
	m_pSort[(int)FOLDER_TYPE::LEVEL11_15]			 = std::make_unique<MusicSort::Level11_15>();
	m_pSort[(int)FOLDER_TYPE::LEVEL16_20]			 = std::make_unique<MusicSort::Level16_20>();
	m_pSort[(int)FOLDER_TYPE::LEVEL21_25]			 = std::make_unique<MusicSort::Level21_25>();
	m_pSort[(int)FOLDER_TYPE::LEVEL26_30]			 = std::make_unique<MusicSort::Level26_30>();
	m_pSort[(int)FOLDER_TYPE::LEVEL31]				 = std::make_unique<MusicSort::Level31>();
	m_pSort[(int)FOLDER_TYPE::ORIGINAL]				 = std::make_unique<MusicSort::Original>();
	m_pSort[(int)FOLDER_TYPE::GAME]					 = std::make_unique<MusicSort::Game>();
	m_pSort[(int)FOLDER_TYPE::CLASSIC]				 = std::make_unique<MusicSort::Classic>();
	m_pSort[(int)FOLDER_TYPE::VOCALOID]				 = std::make_unique<MusicSort::Vocaloid>();
	m_pSort[(int)FOLDER_TYPE::POPS]					 = std::make_unique<MusicSort::Pops>();
	m_pSort[(int)FOLDER_TYPE::OTHER]				 = std::make_unique<MusicSort::Other>();

	m_fLoadPercentage = .8f;	// ���[�h����

	// �}�b�v������
	m_FolderTypeMap[FOLDER_TYPE::BEGINNER]			 = "BEGINNER";
	m_FolderTypeMap[FOLDER_TYPE::ALL]				 = "ALL-MUSIC";
	m_FolderTypeMap[FOLDER_TYPE::ELECTRONIC_PIANISM] = "ELECTRONIC PIANISM";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL1_5]			 = "LEVEL1-5";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL6_10]			 = "LEVEL6-10";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL11_15]		 = "LEVEL11-15";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL16_20]		 = "LEVEL16-20";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL21_25]		 = "LEVEL21-25";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL26_30]		 = "LEVEL26-30";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL31]			 = "LEVEL31";
	m_FolderTypeMap[FOLDER_TYPE::ORIGINAL]			 = "ORIGINAL";
	m_FolderTypeMap[FOLDER_TYPE::GAME]				 = "GAME";
	m_FolderTypeMap[FOLDER_TYPE::CLASSIC]			 = "CLASSIC";
	m_FolderTypeMap[FOLDER_TYPE::VOCALOID]			 = "VOCALOID";
	m_FolderTypeMap[FOLDER_TYPE::POPS]				 = "POPS";
	m_FolderTypeMap[FOLDER_TYPE::OTHER]				 = "OTHER";

	m_fLoadPercentage = .9f;	// ���[�h����

	// �I���t�H���_�[�J�[�\��������
	m_ipSelectFolder = SelectManager::GetInstance()->GetSelectFolderAddress();

	// �t�H���_�[������
	m_pSelectFolderInfo = SelectManager::GetInstance()->GetSelectFolderInfoAddress();

	// ����t���O�I�t
	m_bDecision = false;

	// ���Ղ����OFF
	m_bOption = false;
	m_CurrentOptionX = m_NextOptionX = (float)tdnSystem::GetScreenSize().right;

	// �����v�A�j���[�V����
	m_LampAnimFrame = m_LampAnimPanel = 0;

	// �Ȗ��e�L�X�g�摜
	//m_pMusicTextImages = std::make_unique<tdn2DObj>*[MUSIC_DATA_BASE->GetNumEPContents()];

	//m_PathList = new std::string[MUSIC_DATA_BASE->GetNumEPContents()];
	m_fLoadPercentage = .95f;	// ���[�h����
	EP_MUSIC_LOOP
	{
		//for (int folder = 0; folder < MUSIC_DATA_BASE->GetNumEPContents(); folder++)
		{
			const int id(MUSIC_DATA_BASE->GetEPContentsData(i)->byID);
			//// �t�H���_�[������ID�擾
			//std::string path = "DATA/Musics/EP/No_" + id;
				//MUSIC_DATA_BASE->GetEPDirectory()->FolderNames[folder];

			//char *ctx;// strtok_s�̑�3�����Ɏg�p����̂����A�����I�Ɏg�p����̂Ő[���l���Ȃ��炵��
			//
			//// No_1��.�ŋ�؂��āA�E�̐�����ID�ɂ���
			//strtok_s((char*)tdnFile::GetFileName((char*)path.c_str()).c_str(), "_", &ctx);
			//int folderID = (BYTE)atoi(strtok_s(0, "\0", &ctx));

			// �t�H���_�[ID�������Ȃ�
			//if (folderID == id)
			{
				// �����f�[�^�ݒ�
				//m_pAuditionPlayer->Set(MUSIC_DATA_BASE->GetEPContentsData(i)->id, (char*)(path + "/Audition.owd").c_str());
				//m_PathList[id] = "DATA/Musics/EP/No_" + std::to_string(id);
				//
				//// �Ȗ��摜�ݒ�
				//m_pMusicTextImages[id] = std::make_unique<tdn2DObj>((char*)(m_PathList[id] + "/title.png").c_str());
				//break;
				m_MusicInfoMap[id] = new _MusicInfo(id);
			}
		}
	}

	m_fLoadPercentage = 1.0f;	// ���[�h����

	// �I��BGM����
	m_pMusicPlayer = std::make_unique<tdnSoundBGM>();
	m_pMusicPlayer->Set(MUSIC_PLAYER::SELECTION, (char*)(CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::SELECT_MUSIC) + "/music.wav").c_str());
	m_pMusicPlayer->Set(MUSIC_PLAYER::DECIDE, (char*)(CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::SELECT_MUSIC) + "/decide.wav").c_str());

	// ����g
	m_pQuestionWindow = std::make_unique<QuestionWindow>(QuestionWindow::ANSWER_TYPE::YES_NO);

	// �K�C�h
	m_pKeyboardGuide = std::make_unique<KeyboardGuide>(Vector2(26, 580), 3, 0.1f, 0.65f, 1.0f, 60);

	// ���[�h������
	if (SelectManager::GetInstance()->isEmpty())
	{
		m_pMusicPlayer->Play(MUSIC_PLAYER::SELECTION);
		SelectManager::GetInstance()->ChangeMode(MODE::SELECT_FOLDER, this);
	}
	else
	{
		SelectManager::GetInstance()->ChangeMode(MODE::SELECT_MUSIC, this);
	}

	// �����f�[�^�S���ǂݍ���
	//m_pAuditionPlayer = new tdnSoundBGM;

	return true;
}

sceneMusicSelect::~sceneMusicSelect()
{
	// ���[�h�|�C���^���
	//delete m_pMode;
	SelectManager::GetInstance()->ModeRelease();

	// �Ȃ̃p�X���
	for (auto it : m_MusicInfoMap)
	{
		// �߂��Ⴍ���Ⴞ���ǁA�Ƃ肠��������Ń��������[�N�͖h����
		//m_MusicInfoMap.erase(it.first);
		delete m_MusicInfoMap[it.first];
	}


	// ����
	delete m_pBackMovie;
}

void sceneMusicSelect::ClearFolderInfo()
{
	for (auto it : *m_pSelectFolderInfo)
		delete it;
	m_pSelectFolderInfo->clear();
}

//=============================================================================================


//=============================================================================================
//		�X			�V
bool sceneMusicSelect::Update()
{
	// �t�F�[�h�X�V
	Fade::Update();

	// �I�Ȍ�̍X�V
	if (m_bDecision)
	{
		// ���[�h���s
		SelectManager::GetInstance()->ModeUpdate();
	}

	// �I�Ȃ��Ă���Ƃ��̍X�V
	else
	{
		// �A�j���摜�X�V
		FOR((int)ANIM_IMAGE::MAX) m_pAnimImages[i]->Update();
		for (auto it : m_MusicInfoMap) m_MusicInfoMap[it.first]->UpdateAnim();

		// �������ԍX�V(�t���[����Ȃ�������)
		if (m_LimitTime != NOLILMIT_TIME)m_LimitTime = max(m_LimitTime - 1, 0);

		// �����v�A�j���X�V
		if (++m_LampAnimFrame > 2)
		{
			m_LampAnimFrame = 0;
			if (++m_LampAnimPanel > 3) m_LampAnimPanel = 0;
		}

		// �K�C�h�X�V
		m_pKeyboardGuide->Update();

		// ����X�V
		m_pBackMovie->Update();

		// BGM�X�V
		m_pMusicPlayer->Update();

		// �����t�܂ł̑ҋ@���Ԃ��c���Ă���
		if (m_WaitTimer > 0)
		{
			// ���L�̏���(��Ƀv���C���[����̑���)���X�L�b�v����
			m_WaitTimer--;
			return true;
		}

		// �I���Փx�g�̕�ԏ���
		const float NextDifficultyX(55.0f + *m_ipSelectDifficulty * 172);
		m_DifficultyFrameX = m_DifficultyFrameX * .8f + NextDifficultyX * .2f;

		// �I�v�V�������W���
		m_CurrentOptionX = m_CurrentOptionX * .75f + m_NextOptionX * .25f;

		// �I�v�V�����J���Ă�
		if (m_bOption)
		{
			// [�h]�n�C�X�s
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::C))
			{
				g_pSE->Play("��Փx�ύX");
				if ((++PlayerDataMgr->m_PlayerOption.HiSpeed) > 10) PlayerDataMgr->m_PlayerOption.HiSpeed = 0;
			}

			// [��]�Q�[�W�I�v�V����
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::D))
			{
				g_pSE->Play("��Փx�ύX");
				if (++PlayerDataMgr->m_PlayerOption.iGaugeOption > (int)GAUGE_OPTION::HAZARD) PlayerDataMgr->m_PlayerOption.iGaugeOption = 0;
			}

			// [C]�T�h�D��
			//if (KeyBoardTRG(KB_C)) PlayerDataMgr->m_PlayerOption.bSudden ^= 0x1;

			// [�~]DJ_AUTO
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::E))
			{
				g_pSE->Play("��Փx�ύX");
				PlayerDataMgr->m_PlayerOption.bAutoPlay ^= 0x1;
			}

			// ����
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::START))
			{
				m_NextOptionX = 1300;
				m_bOption = false;
				g_pSE->Play("�I�v�V��������");
				Fade::Set(Fade::MODE::FADE_IN, 8, 0, Fade::m_byAlpha, 0x00000000);
			}
		}

		// �I�v�V�����J���ĂȂ�
		else
		{
			// [2]��Փx�ύX
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::SELECT))
			{
				g_pSE->Play("��Փx�ύX");
				//m_pMode->ShiftDifficulty();
				SelectManager::GetInstance()->ShiftDifficulty();
			}

			// ���[�h���s
			//m_pMode->Update();
			SelectManager::GetInstance()->ModeUpdate();
		}
	}

	return true;
}

void sceneMusicSelect::UpdateOption()
{
	// �X�^�[�g�L�[�ŃI�v�V�������J��
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::START))
	{
		m_NextOptionX = 310;
		m_bOption = true;
		g_pSE->Play("�I�v�V�����J��");
		Fade::Set(Fade::MODE::FADE_OUT, 8, 128, Fade::m_byAlpha, 0x00000000);
	}
}
//
//=============================================================================================


//=============================================================================================
//		�`			��
void sceneMusicSelect::Render()
{
	//
	tdnView::Clear();

	// �I�Ȍ�̕`��
	if (m_bDecision)
	{
		// ���[�h�`����s
		SelectManager::GetInstance()->ModeRender();
	}

	// �I�Ȃ��Ă���Ƃ��̕`��
	else
	{
		// �w�i
		m_pImages[IMAGE::BACK]->Render(0, 0);

		// �K�C�h
		RenderGuide();

		// ��Փx�`��
		m_pAnimImages[(int)ANIM_IMAGE::DIF_FRAME]->Render(48, 400);
		//tdnText::Draw(320, 640, 0xffffffff, "%s", ENUM_STRING.m_DifficultyMap[(DIFFICULTY)m_ipSelectDifficulty]);

		/* ��Փx�g */
		m_pAnimImages[(int)ANIM_IMAGE::SELECT_DIFFICULTY]->SetARGB(DifficultyColor[*m_ipSelectDifficulty]);
		m_pAnimImages[(int)ANIM_IMAGE::SELECT_DIFFICULTY]->Render((int)m_DifficultyFrameX, 433);

		// �X�R�A�g�`��
		m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->Render(248, 540);
		if (m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->GetAction()->IsEnd())tdnText::Draw(255, 573, 0xffffffff, "SCORE");

		// ���[�h�`����s
		//m_pMode->Render();
		SelectManager::GetInstance()->ModeRender();

		// ��̃t���[��
		m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Render(0, 0);

		// MUSIC SELECT
		m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 64, 400, 64);

		// ��������
		m_pImages[IMAGE::NUMBER_L]->SetARGB(0xffffffff);
		NumberRender(m_pImages[IMAGE::NUMBER_L].get(), 1196, 52, 32, 42, (m_LimitTime != NOLILMIT_TIME) ? m_LimitTime / 60 : 99, 2, true);

		// **Stage
		m_pAnimImages[(int)ANIM_IMAGE::STAGE]->Render(760, 30, 280, 64, 0, (ModeMgr->GetMode() == PLAY_MODE::STANDARD) ? ModeMgr->GetStep() * 64 : 256, 280, 64);

		Fade::Render();

		// ����E�B���h�E�̕`��
		m_pQuestionWindow->Render();

		// �I�v�V�����J���Ă���
		if (m_CurrentOptionX < tdnSystem::GetScreenSize().right)
			RenderOption();	// �I�v�V�����`��
	}
}

void sceneMusicSelect::RenderOption()
{
	const int OFFSET_X((int)m_CurrentOptionX);
	static const int OFFSET_Y(50);

	m_pImages[IMAGE::OPTION]->Render(OFFSET_X, OFFSET_Y);

	// �n�C�X�s
	int iHiSpeed = (int)((PlayerDataMgr->m_PlayerOption.HiSpeed * HI_SPEED_WIDTH) * 100);
	m_pImages[IMAGE::NUMBER]->SetARGB(0xffffffff);
	m_pImages[IMAGE::NUMBER]->Render(OFFSET_X + 135, OFFSET_Y + 352, 22, 28, (iHiSpeed % 10) * 24, 0, 24, 32);
	iHiSpeed /= 10;
	m_pImages[IMAGE::NUMBER]->Render(OFFSET_X + 110, OFFSET_Y + 352, 22, 28, (iHiSpeed % 10) * 24, 0, 24, 32);
	iHiSpeed /= 10;
	m_pImages[IMAGE::NUMBER]->Render(OFFSET_X + 75, OFFSET_Y + 352, 22, 28, (iHiSpeed % 10) * 24, 0, 24, 32);
	m_pImages[IMAGE::NUMBER]->Render(OFFSET_X + 100, OFFSET_Y + 350, 16, 32, 240, 0, 16, 32);
	//m_pImages[IMAGE::OPTION_CURSOR]->Render(OFFSET_X + 49, OFFSET_Y + 335, RS::ADD);

	// �Q�[�W
	m_pImages[IMAGE::OPTION_CURSOR]->Render(OFFSET_X + 196, OFFSET_Y + 335 + PlayerDataMgr->m_PlayerOption.iGaugeOption * 35, RS::ADD);

	// AUTO
	m_pImages[IMAGE::OPTION_CURSOR]->Render(OFFSET_X + 343, OFFSET_Y + 335 + (PlayerDataMgr->m_PlayerOption.bAutoPlay ? 35 : 0), RS::ADD);

#ifdef MIDI_INPUT_ON
#else
	
#endif
}

void sceneMusicSelect::RenderGuide()
{
	m_pKeyboardGuide->Render();

	//static float l_fAlphaSinCurve(0);
	//static bool l_bSwitch(false);
	//static const float l_cfAlphaSpeed(0.1f);
	//static const float l_cfSwicthAngle(PI * 6);	// 3��_�ł�����؂�ւ��
	//static const int l_ciPosX(32), l_ciPosY(580);

	//m_pImages[IMAGE::GUIDE]->SetAlpha(1.0f);
	//m_pImages[IMAGE::GUIDE]->Render(l_ciPosX, l_ciPosY, 256, 128, 0, 0, 256, 128);

	//// �I��_�Œ�
	//if (l_bSwitch)
	//{
	//	// �I��
	//	m_pImages[IMAGE::GUIDE]->SetAlpha(Math::Blend((sinf(l_fAlphaSinCurve) + 1) * 0.5f, 0.6f, 1.0f));	// -1�`1��0�`1
	//	m_pImages[IMAGE::GUIDE]->Render(l_ciPosX, l_ciPosY, 256, 128, 0, 128, 256, 128);
	//	m_pImages[IMAGE::GUIDE]->SetAlpha(0.7f);

	//	// ����
	//	m_pImages[IMAGE::GUIDE]->Render(l_ciPosX, l_ciPosY, 256, 128, 0, 256, 256, 128);

	//	if ((l_fAlphaSinCurve += l_cfAlphaSpeed) > l_cfSwicthAngle)
	//	{
	//		l_fAlphaSinCurve = 0;
	//		l_bSwitch = !l_bSwitch;
	//	}
	//}

	//// ����_�Œ�
	//else
	//{
	//	// ����
	//	m_pImages[IMAGE::GUIDE]->SetAlpha(Math::Blend((sinf(l_fAlphaSinCurve) + 1) * 0.5f, 0.6f, 1.0f));	// -1�`1��0�`1
	//	m_pImages[IMAGE::GUIDE]->Render(l_ciPosX, l_ciPosY, 256, 128, 0, 256, 256, 128);
	//	m_pImages[IMAGE::GUIDE]->SetAlpha(0.7f);

	//	// �I��
	//	m_pImages[IMAGE::GUIDE]->Render(l_ciPosX, l_ciPosY, 256, 128, 0, 128, 256, 128);

	//	if ((l_fAlphaSinCurve += l_cfAlphaSpeed) > l_cfSwicthAngle)
	//	{
	//		l_fAlphaSinCurve = 0;
	//		l_bSwitch = !l_bSwitch;
	//	}
	//}
}

//
//=============================================================================================


//=============================================================================================
//		�t�H���_�[�I�����[�h
void sceneMusicSelect::Mode::FolderSelect::Update()
{
	static bool bEnd(false);

	// �߂�t�F�[�h��
	if (bEnd)
	{
		// �t�F�[�h�I�������A��
		if (Fade::m_eMode == Fade::MODE::FADE_NONE)
		{
			bEnd = false;
			MainFrame->ChangeScene(new sceneModeSelect, true);
		}
		return;
	}

	// ����E�B���h�E�o������������
	if (pMusicSelect->m_pQuestionWindow->isAction())
	{
		// ����ɓ����I����āA�͂���I�����Ă�����t�F�[�h�A�E�g���ďI��
		if (pMusicSelect->m_pQuestionWindow->Update() && pMusicSelect->m_pQuestionWindow->GetAnswer() == TRUE)
		{
			// SE
			g_pSE->Play("�߂�");

			// �t�F�[�h�A�E�g�ݒ�
			Fade::Set(Fade::MODE::FADE_OUT, 4);
			bEnd = true;
		}

		return;
	}


	// �t�H���_�[�̓����X�V
	FOR((int)FOLDER_TYPE::MAX) m_FolderPositions[i].Update();

	bool bCursored(false);
	// �㉺�Ńt�H���_�[�I��
	if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
	{
		if (--(*pMusicSelect->m_ipSelectFolder) < 0) (*pMusicSelect->m_ipSelectFolder) = (int)FOLDER_TYPE::MAX - 1;
		bCursored = true;
	}
	else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
	{
		if (++(*pMusicSelect->m_ipSelectFolder) >= (int)FOLDER_TYPE::MAX) (*pMusicSelect->m_ipSelectFolder) = 0;
		bCursored = true;
	}

	if (bCursored)
	{
		g_pSE->Play("�J�[�\��");
		SetNextFolderPos(*pMusicSelect->m_ipSelectFolder, false);	// ���̃t�H���_�[���W�̎w��

		// �t�H���_�[�����Ƃ��̃A�j���[�V�����𔭓�
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->Action();
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->Action();
	}

	// �G���^�[�L�[�Ńt�H���_�[���J��
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		// �t�H���_�[SE
		g_pSE->Play("����2");

		pMusicSelect->m_pSort[*pMusicSelect->m_ipSelectFolder]->Funk(pMusicSelect);
		pMusicSelect->Push(nullptr, DIFFICULTY::MAX, true);	// �t�H���_�[������p�̂��ǉ�

		//pMusicSelect->ChangeMode(MODE::SELECT_MUSIC);	// �ȑI���X�e�[�g��
		SelectManager::GetInstance()->ChangeMode(MODE::SELECT_MUSIC, pMusicSelect);
	}

	// �X�y�[�X�L�[�Ŕ�����
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK) && ModeMgr->GetMode() == PLAY_MODE::FREE)
	{
		pMusicSelect->m_pQuestionWindow->Action("Infomation", "���j���[��ʂɖ߂�܂����H");
	}

	else 		// �I�v�V�����X�V
		pMusicSelect->UpdateOption();
}

void sceneMusicSelect::Mode::FolderSelect::ShiftDifficulty()
{
	// ��Փx�J�[�\��
	if (++(*pMusicSelect->m_ipSelectDifficulty) >= (int)DIFFICULTY::MAX) (*pMusicSelect->m_ipSelectDifficulty) = 0;
}

void sceneMusicSelect::Mode::FolderSelect::Render()
{
	// �t�H���_�[�̃^�O�����̉摜�I��
	{
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->Render(70, 200, 560, 80, 0, 80 * *pMusicSelect->m_ipSelectFolder, 560, 80);
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->Render(360, 200 - 40, 256, 40, 0, pMusicSelect->GetFolderHosokuType() * 40, 256, 40);
	}

	// �t�H���_�[��
	{
	/*
		static const int OFFSET_X(760);	// ���O�J�n�n�_��X���W
		static const int OFFSET_Y(345);	// ���O�J�n�n�_��Y���W
	*/
		// �s�̐�
		const int MAX_COLUMN(15);										// �s�̍ő吔
		const int column(min((int)FOLDER_TYPE::MAX, MAX_COLUMN));		// �s�̐�
		const int ColumnCenter((int)(column * .45f));					// �s�̐�����^�񒆂�I�o

		// �s�̕`��
		FOR(column)
		{
			// �����̃J�[�\���ł̑O����擾
			int index(*pMusicSelect->m_ipSelectFolder + (i - ColumnCenter));

			// ���ɍs����������(��ʂł͏�)
			if (index < 0)
			{
				// �S�A���t�@�x�b�g���𑫂����Ƃɂ���āA�ߏ蕪�����点��
				index = (int)FOLDER_TYPE::MAX + index;
			}

			// ��ɍs����������(��ʂł͉�)
			else if (index >= (int)FOLDER_TYPE::MAX)
			{
				// �S�A���t�@�x�b�g���𑫂����Ƃɂ���āA�ߏ蕪�����点��
				index -= ((int)FOLDER_TYPE::MAX);
			}

			//pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetScale((index == *pMusicSelect->m_ipSelectFolder) ? 1.05f : 1.0f);
			pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->Render((int)m_FolderPositions[index].pos.x, (int)m_FolderPositions[index].pos.y, 520, 40, 0, 40, 520, 40);

			// ��Ƃ��񂩂炢���������ӌ����������B
			if (index == *pMusicSelect->m_ipSelectFolder)
			{
				static const float l_cfMinAlpha(0.0f);
				static const float l_cfMaxAlpha(0.5f);
				static float l_sfAlphaSinCurve(0);

				// �I��
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetAlpha(Math::Blend((sinf(l_sfAlphaSinCurve += 0.1f) + 1) * 0.5f, l_cfMinAlpha, l_cfMaxAlpha));	// -1�`1��0�`1
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->Render((int)m_FolderPositions[index].pos.x, (int)m_FolderPositions[index].pos.y, 520, 40, 0, 40, 520, 40, RS::ADD);
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetAlpha((BYTE)BYTE_MAX);
			}

			std::string sSpaceStr((index == (int)FOLDER_TYPE::BEGINNER) ? tdnFont::GetPictGraphStr(tdnFont::PICTOGRAPH::BEGINNER) : "�@");
			tdnFont::RenderString(std::string(sSpaceStr + pMusicSelect->m_FolderTypeMap[(FOLDER_TYPE)index]).c_str(), 32, (int)m_FolderPositions[index].pos.x + 100, (int)m_FolderPositions[index].pos.y + 4, 0xffffffff, "���C���I");
		}
	}
}
//
//=============================================================================================


//=============================================================================================
//		�t�H���_�[�J���ċȑI�����[�h
sceneMusicSelect::Mode::MusicSelect::MusicSelect(sceneMusicSelect *me) : Base(me), 
m_AuditionDelayTime(0),
m_bAuditionPlay(false),
m_pStreamAuditionPlayer(nullptr),
m_iPrevOctave(-1)
{
	// ����
	if (m_iSelectMusic == NO_SELECT_MUSIC)m_iSelectMusic = (int)pMusicSelect->m_pSelectFolderInfo->size() - 1;

	// ���łɗV��łāA�t�H���_�[�̋L�^�ۑ����Ă���
	else
	{
		const int id(pMusicSelect->GetSelectMusicID(m_iSelectMusic));

		// �Ȃ̃^�C�g���ƃA�[�e�B�X�g���̉摜�̃A�j���[�V�����𔭓�������
		pMusicSelect->m_MusicInfoMap[id]->pMusicTextAnim->Action(15);
		pMusicSelect->m_MusicInfoMap[id]->pMusicArtistAnim->Action(15);

		// �I�N�^�[�u�A���t�@
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::OCTAVE]->Action();

		// �I��BGM��~
		pMusicSelect->m_pMusicPlayer->Pause(0);

		m_pStreamAuditionPlayer = g_pBGM->PlayStream((char*)(pMusicSelect->m_MusicInfoMap[pMusicSelect->GetSelectMusicID(m_iSelectMusic)]->path + "/Audition.ogg").c_str());
		m_bAuditionPlay = true;
	}

	SetNextMusicPos(m_iSelectMusic, true);
}
sceneMusicSelect::Mode::MusicSelect::~MusicSelect()
{
	//m_iSelectMusic = NO_SELECT_MUSIC;
	if (m_bAuditionPlay)
		m_pStreamAuditionPlayer->Stop(); 
}
void sceneMusicSelect::Mode::MusicSelect::Update()
{
	// ����E�B���h�E�o������������
	if (pMusicSelect->m_pQuestionWindow->isAction())
	{
		// ����ɓ����I����āA�͂���I�����Ă�����I�Ȃ���
		if (pMusicSelect->m_pQuestionWindow->Update() && pMusicSelect->m_pQuestionWindow->GetAnswer() == TRUE)
		{
			SelectManager::GetInstance()->ChangeMode(MODE::DECIDE_MUSIC, pMusicSelect);
		}

		return;
	}


	// �Ȃ̘g�̍��W�X�V
	for (auto it : *pMusicSelect->m_pSelectFolderInfo) it->UpdatePotision();

	const bool bMusicCursor(!pMusicSelect->isExitFolder(m_iSelectMusic));
	bool bCursored(false);

	// �㉺�ŋȑI��
	if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
	{
		if (!pMusicSelect->isExitFolder(m_iSelectMusic))m_iPrevOctave = pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->iOctaveWidths[*pMusicSelect->m_ipSelectDifficulty];

		if (--m_iSelectMusic < 0) m_iSelectMusic = (int)pMusicSelect->m_pSelectFolderInfo->size() - 1;

		// ��Փx�X�V(���x���ʑI�����ƋN����₷��)
		else
		{
			*pMusicSelect->m_ipSelectDifficulty = (int)pMusicSelect->m_pSelectFolderInfo->at(m_iSelectMusic)->difficulty;
		}
		bCursored = true;
	}
	else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
	{
		if (!pMusicSelect->isExitFolder(m_iSelectMusic))m_iPrevOctave = pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->iOctaveWidths[*pMusicSelect->m_ipSelectDifficulty];

		if (++m_iSelectMusic > (int)pMusicSelect->m_pSelectFolderInfo->size() - 1)
		{
			m_iSelectMusic -= ((int)pMusicSelect->m_pSelectFolderInfo->size());
		}

		// ��Փx�X�V(���x���ʑI�����ƋN����₷��)
		if (m_iSelectMusic != (int)pMusicSelect->m_pSelectFolderInfo->size() - 1)
		{
			*pMusicSelect->m_ipSelectDifficulty = (int)pMusicSelect->m_pSelectFolderInfo->at(m_iSelectMusic)->difficulty;
		}
		bCursored = true;
	}

	// �J�[�\�����Ă���
	if (bCursored)
	{
		// �����ȗ����f�B���C���Z�b�g
		m_AuditionDelayTime = 0;

		// �ȑI��ł���
		if (bMusicCursor)
		{
			// �����Ȓ�~
			if (m_bAuditionPlay)m_pStreamAuditionPlayer->Stop();
			m_bAuditionPlay = false;
		}
		else
		{
			// �I��BGM��~
			pMusicSelect->m_pMusicPlayer->Pause(0);
		}

		// �Ȃ����̘g�Ɏ��̍��W��`���Ă�����
		SetNextMusicPos(m_iSelectMusic, false);

		// �J�[�\��SE
		g_pSE->Play("�J�[�\��");

		// �Ȃł͂Ȃ��t�H���_�[���w���Ă�����
		if (pMusicSelect->isExitFolder(m_iSelectMusic))
		{
			// �t�H���_�[�����Ƃ��̃A�j���[�V�����𔭓�
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->Action(4);
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->Action(4);
		}
		else
		{
			const int id(pMusicSelect->GetSelectMusicID(m_iSelectMusic));

			// �Ȃ̃^�C�g���ƃA�[�e�B�X�g���̉摜�̃A�j���[�V�����𔭓�������
			pMusicSelect->m_MusicInfoMap[id]->pMusicTextAnim->Action(4);
			pMusicSelect->m_MusicInfoMap[id]->pMusicArtistAnim->Action(4);

			// �X�R�A�����N�̃A�j���[�V����
			//if (PlayerDataMgr->m_MusicRecords[id][*pMusicSelect->m_ipSelectDifficulty].ClearLamp != (int)CLEAR_LAMP::NO_PLAY)
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::RANK]->Action();
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::CLEAR_TEXT]->Action();

			// �I�N�^�[�u�̃A���t�@
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::OCTAVE]->Action();
		}
	}

	// �J�[�\���ړ����Ă���f�B���C���|����BGM�֌W
	if (++m_AuditionDelayTime > 30)
	{
		// �Ȃ�I��ł���
		if (bMusicCursor)
		{
			// �����ȗ����ĂȂ�������
			if (
				!m_bAuditionPlay
				//pMusicSelect->m_pAuditionPlayer->isPlay(pMusicSelect->m_pSelectFolderInfo[m_iSelectMusic]->info->id)
				)
			{
				//pMusicSelect->m_pAuditionPlayer->Play(pMusicSelect->m_pSelectFolderInfo[m_iSelectMusic]->info->id);
				m_pStreamAuditionPlayer = g_pBGM->PlayStream((char*)(pMusicSelect->m_MusicInfoMap[pMusicSelect->GetSelectMusicID(m_iSelectMusic)]->path + "/Audition.ogg").c_str());
				m_bAuditionPlay = true;
			}
		}
		else
		{
			// ���ʂ̑I��BGM����
			if (!pMusicSelect->m_pMusicPlayer->isPlay(0))
				pMusicSelect->m_pMusicPlayer->PauseOff(0, .025f);
		}
	}

	// �X�y�[�X�L�[�Ŏ�������
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
	{
		g_pSE->Play("����2");

		m_iSelectMusic = NO_SELECT_MUSIC;

		pMusicSelect->ClearFolderInfo();

		// �I�ȉ�ʂ̋Ȃ��~�܂��ĂȂ������痬��
		if (!pMusicSelect->m_pMusicPlayer->isPlay(0))
			pMusicSelect->m_pMusicPlayer->PauseOff(0, .025f);

		//pMusicSelect->ChangeMode(MODE::SELECT_FOLDER);	// �t�H���_�[�I���X�e�[�g
		SelectManager::GetInstance()->ChangeMode(MODE::SELECT_FOLDER, pMusicSelect);
	}

	// �G���^�[�L�[�Ō���I
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		// �t�H���_�[�̃T�C�Y�Ƃ������̓t�H���_�[�𔲂��Ă�������
		if (pMusicSelect->isExitFolder(m_iSelectMusic))
		{
			g_pSE->Play("����2");

			m_iSelectMusic = NO_SELECT_MUSIC;

			pMusicSelect->ClearFolderInfo();

			// �I�ȉ�ʂ̋Ȃ��~�܂��ĂȂ������痬��
			if (!pMusicSelect->m_pMusicPlayer->isPlay(0))
				pMusicSelect->m_pMusicPlayer->PauseOff(0, .025f);

			//pMusicSelect->ChangeMode(MODE::SELECT_FOLDER);	// �t�H���_�[�I���X�e�[�g
			SelectManager::GetInstance()->ChangeMode(MODE::SELECT_FOLDER, pMusicSelect);
		}

		// �ȑI���I
		else
		{
			// �V���O���g���N���X�ɑI�������Ȃ̏���n��
			SelectMusicData data(pMusicSelect->GetSelectMusicID(m_iSelectMusic), (DIFFICULTY)pMusicSelect->m_pSelectFolderInfo->at(m_iSelectMusic)->difficulty);
			if (!SelectMusicMgr->Set(data)) return;

			// �I�N�^�[�u����Ă��������DJAUTO���񂾂�����
			if (InputMIDIMgr->GetNumOctave() >= SelectMusicMgr->Get()->omsInfo.OctaveWidth || PlayerDataMgr->m_PlayerOption.bAutoPlay)
			{
				// ���胂�[�h�ɐ؂�ւ�
				//pMusicSelect->ChangeMode(MODE::DECIDE_MUSIC);
				SelectManager::GetInstance()->ChangeMode(MODE::DECIDE_MUSIC, pMusicSelect);
			}

			// ����ĂȂ�������A�x������(�V�ׂ�̂͗V�ׂ�)
			else pMusicSelect->m_pQuestionWindow->Action("Caution", "�K�v�ȃI�N�^�[�u��������܂���B\n���t�ł��Ȃ�����������܂����A�v���C���܂����H");
		}
	}

	else 		// �I�v�V�����X�V
		pMusicSelect->UpdateOption();
}

void sceneMusicSelect::Mode::MusicSelect::ShiftDifficulty()
{
	auto Shift = [](int *p){if (++(*p) >= (int)DIFFICULTY::MAX) *p = 0; };

	//// ���x���֌W�̃t�H���_�[�́A�I�����Ă�������ύX
	//if (m_ipSelectFolder >= (int)FOLDER_TYPE::LEVEL1_5 && m_ipSelectFolder <= (int)FOLDER_TYPE::LEVEL31)
	//{
	//	// �߂�{�^���łȂ����
	//	if (m_iSelectMusic < (int)m_pSelectFolderInfo->size()) Shift(&m_pSelectFolderInfo[m_iSelectMusic]->difficulty);
	//}
	//
	//// �t�H���_�[���S���ύX
	//else
	for (auto it : *pMusicSelect->m_pSelectFolderInfo) Shift(&it->difficulty);

	// �����̂��ύX�H
	Shift(pMusicSelect->m_ipSelectDifficulty);
}

void sceneMusicSelect::Mode::MusicSelect::Render()
{
	// ������
	{
		// �Ȃ�I��ł���
		if (!pMusicSelect->isExitFolder(m_iSelectMusic))
		{
			const int id(pMusicSelect->GetSelectMusicID(m_iSelectMusic));
			const int iOctaveWidth(pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->iOctaveWidths[*pMusicSelect->m_ipSelectDifficulty]);

			//static int x(0), y(0);
			//if (KeyBoardTRG(KB_LEFT)) x -= 10;
			//if (KeyBoardTRG(KB_RIGHT)) x += 10;
			//if (KeyBoardTRG(KB_UP)) y -= 10;
			//if (KeyBoardTRG(KB_DOWN)) y += 10;

			// �I�N�^�[�u���`��
			if (iOctaveWidth > 0) tdnFont::RenderString(32, 100, 314, 0xffffffff, "���t�I�N�^�[�u: %d", iOctaveWidth);
			pMusicSelect->m_pImages[IMAGE::OCTAVE_BACK]->Render(20, 348, 320, 32, 0, m_iPrevOctave * 32, 320, 32);
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::OCTAVE]->Render(20, 348, 320, 32, 0, iOctaveWidth * 32, 320, 32);

			// �Ȗ��ƃA�[�e�B�X�g���`��
			RenderMusicTitle(id);

			// BPM�`��
			RenderBPM();

			// �X�R�A�g�̒��̕`��
			if (pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->GetAction()->IsEnd())
			{
				// �X�R�A�`��
				RenderScore(id);
			}
			else
			{
				// �t�H���_�[��
				pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->Render(70, 200, 560, 80, 0, 80 * *pMusicSelect->m_ipSelectFolder, 560, 80);
				pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->Render(360, 200 - 40, 256, 40, 0, pMusicSelect->GetFolderHosokuType() * 40, 256, 40);
			}
		}

		// �E����
		{
			// �E�����̋ȃ��X�g�̕`��
			RenderMusicList();
		}
	}
}
void sceneMusicSelect::Mode::MusicSelect::RenderMusicTitle(int id)
{
	// �Ȗ�
	pMusicSelect->m_MusicInfoMap[id]->pMusicTextAnim->Render(32, 200, 700, 90, 0, 0, 700, 90);

	// �A�[�e�B�X�g��
	pMusicSelect->m_MusicInfoMap[id]->pMusicArtistAnim->Render(150, 200 + 96, 700, 38, 0, 90, 700, 38);
}

void sceneMusicSelect::Mode::MusicSelect::RenderBPM()
{
	// ����������2�񏑂��͂߂ɂȂ����̂ŁA�����_�ňꌳ��
	auto RenderNumberBPM = [](tdn2DObj *image, int val, const Vector2 &pos)
	{
		// �����_���
		image->Render((int)pos.x, (int)pos.y, 24, 32, (val % 10) * 24, 0, 24, 32);
		val /= 10;

		// .
		image->Render((int)pos.x - 12, (int)pos.y, 16, 32, 240, 0, 16, 32);

		// �c��
		for (int i = 0; i < 3; i++)
		{
			image->Render((int)pos.x - 36 - (i * 24), (int)pos.y, 24, 32, (val % 10) * 24, 0, 24, 32);
			val /= 10;
		}
	};

	// BPM�̐�����ԗp
	static float HokanMaxBPM(150), HokanMinBPM(150);

	// �����_������x�؂�̂Ă���BPM
	const int MinBPM((int)(pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->fMinBPM * 10)), MaxBPM((int)(pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->fMaxBPM * 10));
	if (MinBPM == MaxBPM)
	{
		// BPM��ԏ���
		HokanMinBPM = (abs(HokanMinBPM - MinBPM) < 1.0f) ? MinBPM : HokanMinBPM * .65f + MinBPM * .35f;	// �v��129.9�Ŏ~�܂����肷��̂ł���̑΍�

		// BPM�`��
		pMusicSelect->m_pImages[IMAGE::BPM_FONT]->Render(500, 200 + 150, 86, 32, 24, 32, 86, 32);
		RenderNumberBPM(pMusicSelect->m_pImages[IMAGE::BPM_FONT].get(), (int)HokanMinBPM, Vector2(680, (int)200 + 150));
	}
	else
	{
		// BPM��ԏ���
		HokanMaxBPM = (abs(HokanMaxBPM - MaxBPM) < 1.0f) ? MaxBPM : HokanMaxBPM * .65f + MaxBPM * .35f;	// �v��129.9�Ŏ~�܂����肷��̂ł���̑΍�
		HokanMinBPM = (abs(HokanMinBPM - MinBPM) < 1.0f) ? MinBPM : HokanMinBPM * .65f + MinBPM * .35f;	// �v��129.9�Ŏ~�܂����肷��̂ł���̑΍�

		// BPM�`��
		pMusicSelect->m_pImages[IMAGE::BPM_FONT]->Render(360, 200 + 150, 86, 32, 24, 32, 86, 32);
		RenderNumberBPM(pMusicSelect->m_pImages[IMAGE::BPM_FONT].get(), (int)HokanMaxBPM, Vector2(540, (int)200 + 150));
		pMusicSelect->m_pImages[IMAGE::BPM_FONT]->Render(570, 200 + 150, 24, 32, 0, 32, 24, 32);
		RenderNumberBPM(pMusicSelect->m_pImages[IMAGE::BPM_FONT].get(), (int)HokanMinBPM, Vector2(680, (int)200 + 150));
	}
}

void sceneMusicSelect::Mode::MusicSelect::RenderScore(int id)
{
	// ��Փx�g�̒��̃��x��
	FOR((int)DIFFICULTY::MAX)
	{
		pMusicSelect->m_pImages[IMAGE::NUMBER_L]->SetARGB(DifficultyColor[i]);
		NumberRender(pMusicSelect->m_pImages[IMAGE::NUMBER_L].get(), 178 + i * 172, 445, 32, 42, pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->level[i], 2, true);
	}
	/* ���R�[�h�g */
	const int score(PlayerDataMgr->GetMusicRecord(id, *pMusicSelect->m_ipSelectDifficulty)->HiScore);
	const int iLamp(PlayerDataMgr->GetMusicRecord(id, *pMusicSelect->m_ipSelectDifficulty)->ClearLamp);
	int srcY(iLamp * 24);
	if ((CLEAR_LAMP)iLamp == CLEAR_LAMP::FULL_COMBO)srcY += pMusicSelect->m_LampAnimPanel * 24;
	pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::CLEAR_TEXT]->Render(266, 603, 270, 24, 0, srcY, 270, 24);

	// �X�R�A
	{
		// �̐�����ԗp
		static int HokanScore(0);
		// BPM��ԏ���
		HokanScore = (abs(HokanScore - score) < 100) ? score : (int)(HokanScore * .65f + score * .35f);	// �v��9999�Ŏ~�܂����肷��̂ł���̑΍�
		pMusicSelect->m_pImages[IMAGE::NUMBER]->SetARGB(0xffffffff);
		NumberRender(pMusicSelect->m_pImages[IMAGE::NUMBER].get(), 511, 569, 24, 32, HokanScore, 6, false);
	}

	// �����N
	if ((CLEAR_LAMP)PlayerDataMgr->GetMusicRecord(id, *pMusicSelect->m_ipSelectDifficulty)->ClearLamp != CLEAR_LAMP::NO_PLAY)
	{
		const int iRank = (int)ScoreRankCheck(score);
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::RANK]->Render(561, 560, 150, 56, 0, iRank * 56, 150, 56);
	}
}

void sceneMusicSelect::Mode::MusicSelect::RenderMusicList()
{
	/*
	static const int OFFSET_X(860);	// ���O�J�n�n�_��X���W
	static const int OFFSET_Y(345);	// ���O�J�n�n�_��Y���W
	*/

	// �s�̐�
	const UINT MAX_COLUMN(15);										// �s�̍ő吔
	const int column(min(pMusicSelect->m_pSelectFolderInfo->size(), MAX_COLUMN));	// �s�̐�
	const int ColumnCenter((int)(column * .45f));					// �s�̐�����^�񒆂�I�o

	// �s�̕`��
	FOR(column)
	{
		/*
		const int width = (ColumnCenter - i) * -40;	// �s�̐^�񒆂���_�Ƃ��āA�������炸����(�^�񒆂Ȃ�������0)
		const int posY = OFFSET_Y + width;
		*/

		// �����̃J�[�\���ł̑O����擾
		int index(m_iSelectMusic + (i - ColumnCenter));

		// ���ɍs����������(��ʂł͏�)
		if (index < 0)
		{
			// �S�A���t�@�x�b�g���𑫂����Ƃɂ���āA�ߏ蕪�����点��
			index = (int)pMusicSelect->m_pSelectFolderInfo->size() + index;
		}

		// ��ɍs����������(��ʂł͉�)
		else if (index >= (int)pMusicSelect->m_pSelectFolderInfo->size())
		{
			// �S�A���t�@�x�b�g���𑫂����Ƃɂ���āA�ߏ蕪�����点��
			index -= (int)pMusicSelect->m_pSelectFolderInfo->size();
		}

		// ���W�擾
		const Vector2 pos(pMusicSelect->m_pSelectFolderInfo->at(index)->pos);

		// �t���[���`��
		{
			const int l_ciSrcY((pMusicSelect->isExitFolder(index)) ? 40 : 0);
			pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->Render((int)pos.x, (int)pos.y, 520, 40, 0, l_ciSrcY, 520, 40);

			// ��Ƃ��񂩂炢���������ӌ����������B
			if (index == m_iSelectMusic)
			{
				static const float l_cfMinAlpha(0.0f);
				static const float l_cfMaxAlpha(0.5f);
				static float l_sfAlphaSinCurve(0);

				// �I��
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetAlpha(Math::Blend((sinf(l_sfAlphaSinCurve += 0.1f) + 1) * 0.5f, l_cfMinAlpha, l_cfMaxAlpha));	// -1�`1��0�`1
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->Render((int)pos.x, (int)pos.y, 520, 40, 0, l_ciSrcY, 520, 40, RS::ADD);
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetAlpha((BYTE)BYTE_MAX);
			}
		}

		// �߂�{�^��(�t�H���_�[�̖��O)�ł͂Ȃ�������
		if (!pMusicSelect->isExitFolder(index))
		{
			/*
			int addPosY(0);
			const bool bSelectIndex(index == m_iSelectMusic);
			if (bSelectIndex)			addPosY = 1;
			else if (i > ColumnCenter)	addPosY = 2;
			*/
			//pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetScale(bSelectIndex ? 1.05f : 1.0f);

			// ��Փx�ɂ���ĐF��ύX
			DWORD col(DifficultyColor[pMusicSelect->m_pSelectFolderInfo->at(index)->difficulty]);

			// ���x���`��
			const int level(pMusicSelect->GetSelectMusicInfo(index)->level[pMusicSelect->m_pSelectFolderInfo->at(index)->difficulty]);
			pMusicSelect->m_pImages[IMAGE::NUMBER]->SetARGB(col);
			NumberRender(pMusicSelect->m_pImages[IMAGE::NUMBER].get(), (int)pos.x + 46, (int)pos.y + 4, 24, 32, level, 2, true);
			//int iti = level % 10, juu = level / 10;
			//if(juu != 0)pMusicSelect->m_pImages[IMAGE::NUMBER]->Render(bSelectIndex ? 780 - 12 : 780, posY + 4, 24, 32, juu * 24, 0, 24, 32);
			//pMusicSelect->m_pImages[IMAGE::NUMBER]->Render(bSelectIndex ? 804 - 12 : 804, posY + 4, 24, 32, iti * 24, 0, 24, 32);

			// �N���A�����v�`��
			const int lamp(PlayerDataMgr->GetMusicRecord(pMusicSelect->GetSelectMusicID(index), pMusicSelect->m_pSelectFolderInfo->at(index)->difficulty)->ClearLamp);
			switch ((CLEAR_LAMP)lamp)
			{
				// ��Փx�ɂ��F�̕ύX�Ȃ�
			case CLEAR_LAMP::NO_PLAY:
			case CLEAR_LAMP::EASY:
			case CLEAR_LAMP::HARD:
			case CLEAR_LAMP::EX_HARD:
			case CLEAR_LAMP::FULL_COMBO:
				pMusicSelect->m_pImages[IMAGE::LAMP]->SetARGB(0xffffffff);
				break;

				// ��Փx�ɂ���ĐF���Ⴄ�����v
			case CLEAR_LAMP::FAILED:
			case CLEAR_LAMP::CLEAR:
				pMusicSelect->m_pImages[IMAGE::LAMP]->SetARGB(col);
				break;
			}
			//pMusicSelect->m_pImages[IMAGE::LAMP]->SetScale(bSelectIndex ? 1.05f : 1.0f);
			pMusicSelect->m_pImages[IMAGE::LAMP]->Render((int)pos.x - 4, (int)pos.y - 3, 24, 44, pMusicSelect->m_LampAnimPanel * 24, lamp * 44, 24, 44);

			// �Ȗ��`��
			std::string sSpaceStr((pMusicSelect->GetSelectMusicInfo(index)->iOctaveWidths[pMusicSelect->m_pSelectFolderInfo->at(index)->difficulty] == 1) ? tdnFont::GetPictGraphStr(tdnFont::PICTOGRAPH::BEGINNER) : "�@");
			tdnFont::RenderString(std::string(sSpaceStr + pMusicSelect->GetSelectMusicInfo(index)->sMusicName).c_str(), 32, (int)pos.x + 100, (int)pos.y + 4, 0xffffffff, "���C���I");
		}
		else
		{
			// �t���[���`��
			//pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetScale((m_iSelectMusic == pMusicSelect->m_pSelectFolderInfo->size()) ? 1.05f : 1.0f);

			std::string sSpaceStr((*pMusicSelect->m_ipSelectFolder == (int)FOLDER_TYPE::BEGINNER) ? tdnFont::GetPictGraphStr(tdnFont::PICTOGRAPH::BEGINNER) : "�@");
			tdnFont::RenderString(std::string(sSpaceStr + pMusicSelect->m_FolderTypeMap[(FOLDER_TYPE)*pMusicSelect->m_ipSelectFolder]).c_str(), 32, (int)pos.x + 100, (int)pos.y + 4, 0xffffffff, "���C���I");
		}
	}
}

//
//=============================================================================================



//=============================================================================================
//		�����̗]��
//
//=============================================================================================
sceneMusicSelect::Mode::DecideMusic::DecideMusic(sceneMusicSelect *me) :Base(me), m_frame(0), m_pScreen(new tdn2DObj(1280, 720, tdn2D::RENDERTARGET)), m_pMusicTitleAnim(new MusicTitleAnim(me->m_MusicInfoMap[me->GetSelectMusicInfo(m_iSelectMusic)->byID]->pMusicTextAnim->GetTexture()))
{
	// �g��G�t�F�N�g�쐬
	// �X�N���[�����b�v��
	FOR(c_NumRipple)
	{
		m_pScreenRipples[i] = std::make_unique<tdn2DAnim>(m_pScreen->GetTexture());
		m_pScreenRipples[i]->OrderRipple(25 + i*3, 1.0f, .05f - i * .001f);
	}

	// �t���b�V���t�F�[�h�Z�b�g
	Fade::Set(Fade::MODE::FADE_IN, 16, 0, 252, 0xffffff);

	// �����_�[�^�[�Q�b�g
	tdnSystem::GetDevice()->GetRenderTarget(0, &m_surface);

	// ����~�߂�
	me->m_pBackMovie->Stop();

	// ��
	//me->m_pMusicPlayer->Stop(MUSIC_PLAYER::SELECTION);
	me->m_pMusicPlayer->Play(MUSIC_PLAYER::DECIDE, false);
}
sceneMusicSelect::Mode::DecideMusic::~DecideMusic()
{
	delete m_pMusicTitleAnim;
}
void sceneMusicSelect::Mode::DecideMusic::Update()
{
	// �Ȗ��̓���
	m_pMusicTitleAnim->Update(m_frame);

	// ���b�v���g���K�[�����t�F�[�h�A�E�g��
	if (++m_frame > c_RippleTriggerFrame)
	{
		// �g��A�j���[�V����
		FOR(c_NumRipple)m_pScreenRipples[i]->Update();

		// ���Ŋg��
		m_pScreen->SetScale(1.0f + (m_frame - c_RippleTriggerFrame) * .02f);

		// �t�F�[�h�I�������
		if (Fade::m_eMode == Fade::MODE::FADE_NONE && !pMusicSelect->m_pMusicPlayer->isPlay(MUSIC_PLAYER::DECIDE))
		{
			// �V�[���؂�ւ�
			MainFrame->ChangeScene(new scenePlaying);
		}
	}

	// ���b�v�������܂ł̊�
	else
	{
		// ���o�X�L�b�v
		if (InputMIDIMgr->GetAnyTrigger())
		{
			pMusicSelect->m_pMusicPlayer->FadeOut(MUSIC_PLAYER::DECIDE, .01f);
			m_frame = c_RippleTriggerFrame;
		}

		// ���b�v�������t���[���I
		if (m_frame == c_RippleTriggerFrame)
		{
			// �t�F�[�h�A�E�g�Z�b�g
			Fade::Set(Fade::MODE::FADE_OUT, 4, 255, 0, 0x000000);

			// ���b�v�������I
			FOR(c_NumRipple)m_pScreenRipples[i]->Action();
		}

		// �w�i���g�債����Â�������
		static const float MaxScale(.5f);								// 1.0f+MaxScale���ő�l(�I�Ȃ����i�K����Ԃł���)
		const float rate((float)m_frame / c_RippleTriggerFrame);			// ���b�v��������1�Ƃ��銄��
		const BYTE rgb((BYTE)(230 * min((rate * rate) + .2f, 1.0f)));	// �w�i�̐F��(0�Ȃ�^�����A255�Ȃ�t���w�i�摜�F)
		pMusicSelect->m_pImages[IMAGE::BACK]->SetScale(1.0f + (MaxScale - rate * MaxScale));
		pMusicSelect->m_pImages[IMAGE::BACK]->SetARGB((BYTE)255, rgb, rgb, rgb);
	}
}
void sceneMusicSelect::Mode::DecideMusic::Render()
{
	// �����_�[�^�[�Q�b�g
	m_pScreen->RenderTarget();
	tdnView::Clear();

	// �w�i�`��
	pMusicSelect->m_pImages[IMAGE::BACK]->Render(0, 0);

	// �Ȗ��E�A�[�e�B�X�g��
	m_pMusicTitleAnim->Render();

	// ��̘g����
	pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Render(0, 0);
	pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 64, 400, 64);
	pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::STAGE]->Render(760, 30, 280, 64, 0, (ModeMgr->GetMode() == PLAY_MODE::STANDARD) ? ModeMgr->GetStep() * 64 : 256, 280, 64);

	//	�t���[���o�b�t�@�֐؂�ւ�
	tdnSystem::GetDevice()->SetRenderTarget(0, m_surface);

	// �����_�[�^�[�Q�b�g�����X�N���[����`��`��
	if(m_frame <= c_RippleTriggerFrame)m_pScreen->SetScale(1.0f + (m_frame / c_RippleTriggerFrame) * .08f);	// ���Ŋg��
	m_pScreen->Render(0, 0, 1280, 720, 0, 0, 1280, 720);
	FOR(c_NumRipple)m_pScreenRipples[i]->Render(0, 0, RS::ADD);

	// ���̏�Ƀt�F�[�h
	Fade::Render(RS::ADD);
}



//=============================================================================================
//		�\�[�g�֐�
void sceneMusicSelect::Push(FolderInfo *info, DIFFICULTY dif, bool bExitFolder)
{
	m_pSelectFolderInfo->push_back(new SelectMusicInfo(info, dif, bExitFolder));
}

void sceneMusicSelect::MusicSort::Beginner::Funk(sceneMusicSelect *pMusicSelect)
{
	EP_MUSIC_LOOP
	{
		// ��Փx���[�v
		for (int dif = 0; dif < (int)DIFFICULTY::MAX; dif++)
		{
			// �I�N�^�[�u����1��������˂�����
			if (MUSIC_DATA_BASE->GetEPContentsData(i)->iOctaveWidths[dif] == 1)
				pMusicSelect->Push(MUSIC_DATA_BASE->GetEPContentsData(i), (DIFFICULTY)dif);
		}
	}

	// �����_�p���[�̗͂��؂�āA�y�X�\�[�g
	auto Ascending = [](SelectMusicInfo *p1, SelectMusicInfo *p2){return (p1->info->level[p1->difficulty] < p2->info->level[p2->difficulty]); };

	std::sort(
		pMusicSelect->m_pSelectFolderInfo->begin(),
		pMusicSelect->m_pSelectFolderInfo->end(),
		Ascending
		);
}

void sceneMusicSelect::MusicSort::All::Funk(sceneMusicSelect *pMusicSelect)
{
	// �S�Ȋi�[
	EP_MUSIC_LOOP
	{
		pMusicSelect->Push(MUSIC_DATA_BASE->GetEPContentsData(i), (DIFFICULTY)*pMusicSelect->m_ipSelectDifficulty);
	}
}

void sceneMusicSelect::MusicSort::ElectronicPianism::Funk(sceneMusicSelect *pMusicSelect)
{
	// ���͂Ƃ肠�����S��
	EP_MUSIC_LOOP
	{
		pMusicSelect->Push(MUSIC_DATA_BASE->GetEPContentsData(i), (DIFFICULTY)*pMusicSelect->m_ipSelectDifficulty);
	}
}

void sceneMusicSelect::MusicSort::Base::LevelSort(int Min, int Max, sceneMusicSelect *pMusicSelect)
{
	EP_MUSIC_LOOP
	{
	// ��Փx���[�v
	for (int dif = 0; dif < (int)DIFFICULTY::MAX; dif++)
	{
		const int level = MUSIC_DATA_BASE->GetEPContentsData(i)->level[dif];

		// ���x�����͈͓��Ȃ������
		if (level >= Min && level <= Max)pMusicSelect->Push(MUSIC_DATA_BASE->GetEPContentsData(i), (DIFFICULTY)dif);
	}
}
	if (pMusicSelect->m_pSelectFolderInfo->size() < 2) return;

	//auto Swap = [](SelectMusicInfo *p1, SelectMusicInfo *p2)
	//{
	//	SelectMusicInfo info;
	//	memcpy_s(&info, sizeof(SelectMusicInfo), p1, sizeof(SelectMusicInfo));
	//	memcpy_s(p1, sizeof(SelectMusicInfo), p2, sizeof(SelectMusicInfo));
	//	memcpy_s(p2, sizeof(SelectMusicInfo), &info, sizeof(SelectMusicInfo));
	//};
	//for (UINT i = 0; i < pMusicSelect->m_pSelectFolderInfo->size() - 1; i++)
	//{
	//	for (UINT j = i + 1; j < pMusicSelect->m_pSelectFolderInfo->size(); j++)
	//	{
	//		SelectMusicInfo *p1(pMusicSelect->m_pSelectFolderInfo[j]), *p2(pMusicSelect->m_pSelectFolderInfo[i]);
	//		if (p1->info->level[p1->difficulty] < p2->info->level[p2->difficulty]) Swap(p1, p2);
	//	}
	//}

	// �����_�p���[�̗͂��؂�āA�y�X�\�[�g
	auto Ascending = [](SelectMusicInfo *p1, SelectMusicInfo *p2){return (p1->info->level[p1->difficulty] < p2->info->level[p2->difficulty]); };
	
	std::sort(
		pMusicSelect->m_pSelectFolderInfo->begin(),
		pMusicSelect->m_pSelectFolderInfo->end(),
		Ascending
	);

	//Sort::BubbleSort<SelectMusicInfo> sort;
	//sort.Function(m_pSelectFolderInfo);
}

void sceneMusicSelect::MusicSort::Level1_5::Funk(sceneMusicSelect *pMusicSelect)
{
	// ���x���i����
	LevelSort(1, 5, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level6_10::Funk(sceneMusicSelect *pMusicSelect)
{
	// ���x���i����
	LevelSort(6, 10, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level11_15::Funk(sceneMusicSelect *pMusicSelect)
{
	// ���x���i����
	LevelSort(11, 15, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level16_20::Funk(sceneMusicSelect *pMusicSelect)
{
	// ���x���i����
	LevelSort(16, 20, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level21_25::Funk(sceneMusicSelect *pMusicSelect)
{
	// ���x���i����
	LevelSort(21, 25, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level26_30::Funk(sceneMusicSelect *pMusicSelect)
{
	// ���x���i����
	LevelSort(26, 30, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level31::Funk(sceneMusicSelect *pMusicSelect)
{
	// ���x���i����
	LevelSort(31, 31, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Base::GenreSort(MUSIC_GENRE genre, sceneMusicSelect *pMusicSelect)
{
	EP_MUSIC_LOOP
	{
		// �W����������v���Ă��������
		if (MUSIC_DATA_BASE->GetEPContentsData(i)->eMusicGenre == genre)
		pMusicSelect->Push(MUSIC_DATA_BASE->GetEPContentsData(i), (DIFFICULTY)*pMusicSelect->m_ipSelectDifficulty);
	}
}

void sceneMusicSelect::MusicSort::Original::Funk(sceneMusicSelect *pMusicSelect)
{
	// �W�������i�荞��
	GenreSort(MUSIC_GENRE::ORIGINAL, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Game::Funk(sceneMusicSelect *pMusicSelect)
{
	// �W�������i�荞��
	GenreSort(MUSIC_GENRE::GAME, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Classic::Funk(sceneMusicSelect *pMusicSelect)
{
	// �W�������i�荞��
	GenreSort(MUSIC_GENRE::CLASSIC, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Vocaloid::Funk(sceneMusicSelect *pMusicSelect)
{
	// �W�������i�荞��
	GenreSort(MUSIC_GENRE::VOCALOID, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Pops::Funk(sceneMusicSelect *pMusicSelect)
{
	// �W�������i�荞��
	GenreSort(MUSIC_GENRE::POPS, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Other::Funk(sceneMusicSelect *pMusicSelect)
{
	// �W�������i�荞��
	GenreSort(MUSIC_GENRE::OTHER, pMusicSelect);
}