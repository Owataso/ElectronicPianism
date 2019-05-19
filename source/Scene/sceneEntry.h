#pragma once

class AlphabetRenderer;

/********************************************/
//	����G���g���[�V�[��
/********************************************/
class sceneEntry : public BaseScene
{
public:
	//------------------------------------------------------
	//	�������E�J��
	//------------------------------------------------------
	bool Initialize();
	~sceneEntry();

	//------------------------------------------------------
	//	�X�V�E�`��
	//------------------------------------------------------
	bool Update();
	void Render();

private:

	//===============================================
	//	�萔
	//===============================================
	enum class NO_PLAYER_DATA_STATE
	{
		SELECT,			// ���O���͂�I�����邩�A���̂܂܃f�[�^�Ȃ��Ŏn�߂�
		NAME_INPUT,		// ���O���͂̂Ƃ�
		MAX
	};


	/********************************************/
	//	�v���C���[�f�[�^���Ȃ��Ƃ��̃X�e�[�g
	/********************************************/
	class NoDataState
	{
	public:
		//===============================================
		//	���
		//===============================================
		class Base
		{
		protected:
			sceneEntry *m_parent;
		public:
			Base(sceneEntry *me) :m_parent(me){}
			virtual ~Base(){}
			virtual void Update()=0;
			virtual void Render()=0;
		};

		//===============================================
		//	�f�[�^�Ȃ��Ŏn�߂邩���O���͂��ĐV�K�쐬���I��
		//===============================================
		class Select : public Base
		{
		public:
			Select(sceneEntry *me) :Base(me){}
			void Update();
			void Render();
		};

		//===============================================
		//	���O����
		//===============================================
		class NameInput : public Base
		{
		private:
			int m_cursor;		// ���O���͂̃J�[�\��
			char m_InputName[11];	// ���̓o�b�t�@
			int m_InputStep;		// ���̓o�b�t�@�̓Y�����ɂȂ�
			AlphabetRenderer *m_pAlphabetRenderer;	// �A���t�@�x�b�g�`��
			bool m_bEnd;			// �I�����SE�����܂ő҂p

			tdnSoundSE *m_pSE;	// �قڗ��邱�ƂȂ��̂ŁA����������SE�͎����Ŏ���
		public:
			NameInput(sceneEntry *me);
			~NameInput();
			void Update();
			void Render();
		};
	};

	//===============================================
	//	�����o�ϐ�
	//===============================================
	bool m_bLoadSaveData;				// �v���C���[�f�[�^�����݂��邩
	NoDataState::Base *m_pNoDataState;	// �Ϗ�


	//===============================================
	//	�X�e�[�g�ύX
	//===============================================
	void ChangeMode(NO_PLAYER_DATA_STATE mode)
	{
		if (m_pNoDataState) delete m_pNoDataState;
		switch (mode)
		{
		case NO_PLAYER_DATA_STATE::SELECT:
			m_pNoDataState = new NoDataState::Select(this);
			break;

		case NO_PLAYER_DATA_STATE::NAME_INPUT:
			m_pNoDataState = new NoDataState::NameInput(this);
			break;
		}
	}
};