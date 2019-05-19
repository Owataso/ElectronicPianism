#pragma once

//===============================================
//	�O���錾
//===============================================
class NoteManager;
class UI;
class SoflanManager;
class Player;
class JudgeManager;
struct MusicInfo;
class EditFolderInfo;
class BeatBar;


/********************************************/
//	�f�[�^��ǂݍ��݁A���ʂ�������肷��̂����V�[��
/********************************************/
class	sceneEdit : public BaseScene
{
public:
	//------------------------------------------------------
	//	�������E�J��
	//------------------------------------------------------
	bool Initialize();
	~sceneEdit();

	//------------------------------------------------------
	//	�X�V�E�`��
	//------------------------------------------------------
	bool Update();
	void Render();


private:
	//===============================================
	//	�萔
	//===============================================
	enum class MODE
	{
		FIRST,				// �V�K�쐬 or ���[�h
		NEW_CREATE,			// �t�H���_�V�K�쐬
		DIRECTORY_SELECT,	// �ǂ̃t�@�C�������[�h���邩
		DIFFICULTY_SELECT,	// ��Փx�I��
		SETTING_INFO,		// ���ʂƂ�����ݒ肵�Ă���
		SETTING_WAV,		// ���WAV�f�[�^�̐ݒ�
		LOAD_OK,			// MIDI��WAV�̗����ǂݍ��߂�
		PLAYING				// �Ȃƕ��ʍĐ��Ȃ�
	};

	/********************************************/
	//	�X�e�[�g�Ϗ��N���X
	/********************************************/
	class Mode
	{
	public:
		//===============================================
		//	���
		//===============================================
		class Base
		{
		protected:
			int m_step;						// �ėp�I�ȃX�e�b�v�ϐ�

		public:
			sceneEdit *me;

			Base(sceneEdit *me) :me(me), m_step(0){}
			virtual ~Base(){}
			virtual void Initialize(){}
			virtual void Update(){}
			virtual void Render(){}
		};

		//===============================================
		//	�ŏ��ɐV�K�t�H���_�������t�H���_����I�΂���
		//===============================================
		class First : public Base
		{
		public:
			First(sceneEdit *me) : Base(me){}
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	(�V�K�t�H���_��I�񂾏ꍇ�̕����)�ȃt�H���_�쐬
		//===============================================
		class NewCreateDirectry :public Base
		{
		public:
			NewCreateDirectry(sceneEdit *me) :Base(me){}
			void Initialize();
			void Update();
			void Render();

			static bool bThread;
			static void NameInput(void *arg);
		};

		//===============================================
		//	�ȃt�H���_�I��
		//===============================================
		class DirectrySelect : public Base
		{
		private:
			int m_SelectCursor;			// �t�H���_�[�̑I���J�[�\��
			DirectoryInfo *m_pDirectry;	// �f�B���N�g���Ɋւ����񂪓������\����
		public:
			DirectrySelect(sceneEdit *me) : Base(me), m_SelectCursor(0), m_pDirectry(nullptr){}
			~DirectrySelect(){ SAFE_DELETE(m_pDirectry); }
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	��Փx�I��
		//===============================================
		class DifficultySelect : public Base
		{
		private:

		public:
			DifficultySelect(sceneEdit *me) : Base(me){}
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	���ʂƂ��̏����Z�b�g���Ă���
		//===============================================
		class SettingInfo : public Base
		{
		private:
			// �ȃf�[�^�Ƃ����ʃf�[�^�̓ǂݍ��݂��Ăяo��
			void UpdateLoadFile();
		public:
			SettingInfo(sceneEdit *me) : Base(me){}
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	�������̐ݒ�
		//===============================================
		class SettingWAV : public Base
		{
		public:
			SettingWAV(sceneEdit *me) :Base(me){}
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	�v���C�҂��E�����o��
		//===============================================
		class LoadOK :public Base
		{
		public:
			LoadOK(sceneEdit *me) :Base(me){}
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	�e�X�g�v���C
		//===============================================
		class Playing :public Base
		{
		private:
			NoteManager *note_mng;
		public:
			Playing(sceneEdit *me) :Base(me){}
			void Initialize();
			void Update();
			void Render();
		};
	};


	//===============================================
	//	�����o�ϐ�
	//===============================================
	int m_iShift;					// �Ȃƕ��ʂ̍��̒����p
	int m_iSelectDifficulty;		// �I���Փx
	int m_iPlayMusicNo;				// �t�H���_���ł̗����Ȃ̔ԍ�(�����Ȃ��Ȃ�-1)
	int m_iPlayTimer;				// ������ݒ肵�Ȃ��ꍇ�A�Ȃ̍Đ�����
	bool bMIDILoad;					// MIDI�̃��[�h��
	Mode::Base *m_pMode;			// ���[�h�Ϗ��|�C���^
	MusicInfo *m_MusicInfo;			// �I�ȏ��
	EditFolderInfo *m_FolderInfo;	// �I���t�H���_�[���
	std::string m_sMusicName;		// �Ȗ�


	//===============================================
	//	���y�Q�[���������\������e����
	//===============================================
	NoteManager *m_pNoteMgr;
	UI *m_pUI;
	SoflanManager *m_pSoflanMgr;
	Player *m_pPlayer;
	JudgeManager *m_pJudge;
	BeatBar *m_pBeatBar;



	//===============================================
	//	���[�h�ύX
	//===============================================
	void ChangeMode(MODE m)
	{
		if (m_pMode) delete m_pMode;
		switch (m)
		{
		case MODE::SETTING_WAV:
			m_pMode = new Mode::SettingWAV(this);
			break;
		case MODE::LOAD_OK:
			m_pMode = new Mode::LoadOK(this);
			break;
		case MODE::PLAYING:
			m_pMode = new Mode::Playing(this);
			break;
		case MODE::NEW_CREATE:
			m_pMode = new Mode::NewCreateDirectry(this);
			break;
		case MODE::FIRST:
			m_pMode = new Mode::First(this);
			break;
		case MODE::DIRECTORY_SELECT:
			m_pMode = new Mode::DirectrySelect(this);
			break;
		case MODE::SETTING_INFO:
			m_pMode = new Mode::SettingInfo(this);
			break;
		case MODE::DIFFICULTY_SELECT:
			m_pMode = new Mode::DifficultySelect(this);
			break;
		default:
			assert(0);	// ��O����
			break;
		}
		m_pMode->Initialize();
	}
};
