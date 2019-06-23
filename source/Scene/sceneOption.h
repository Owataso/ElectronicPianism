#pragma once

//------------------------------------------------------
//	�O���錾
//------------------------------------------------------
class sceneOption;
class tdnMovie;
namespace PlayerData
{
	struct OptionV2;
}

/********************************************/
//	�I�v�V�����ݒ�Ϗ�
/********************************************/
namespace SelectOption
{
	//------------------------------------------------------
	//	���
	//------------------------------------------------------
	class Base
	{
	protected:
		static const Vector2 FRAME_OFFSET_XY;	// �E��g�̊J�n���W
		tdnText2 *m_pText;
		Vector2 m_CurrentCursorPos, m_NextCursorPos;	// �J�[�\���g�̕�ԂŎg��

		sceneOption *m_pOption;	// �Ϗ���
		int m_iCursor;
		int m_CursorAnimFrame;
		void RenderCursor();		// �J�[�\���`��
		const int m_NumCursor;
		bool m_bSettingOption;	// �I�v�V������I�����āA�ݒ���������Ă�����
		void UpdateCursor();
	public:
		Base(sceneOption *pOption, int NumCursor);
		virtual ~Base();
		virtual void Update() = 0;
		virtual void Render() = 0;
	};

	//------------------------------------------------------
	//	�n�C�X�s�[�h�֘A
	//------------------------------------------------------
	class HiSpeed :public Base
	{
	public:
		HiSpeed(sceneOption *pOption);
		void Update();
		void Render();
	private:
		const Mybool m_bOrgRegulSpeed;
		const int m_iOrgHiSpeed;
	};

	//------------------------------------------------------
	//	����֘A
	//------------------------------------------------------
	class Movie :public Base
	{
	public:
		Movie(sceneOption *pOption);
		void Update();
		void Render();
	private:
		const BYTE m_byOrgMovieAlpha;
		const Mybool m_bOrgMovieOn;

	};

	//------------------------------------------------------
	//	���֘A
	//------------------------------------------------------
	class Sound :public Base
	{
	public:
		Sound(sceneOption *pOption);
		void Update();
		void Render();
	private:
		const Mybool m_bOrgMIDISoundOn;
	};

	//------------------------------------------------------
	//	�K�C�h�֘A
	//------------------------------------------------------
	class Guide :public Base
	{
	public:
		Guide(sceneOption *pOption);
		void Update();
		void Render();
	private:
		const Mybool m_bOrgGuide;
	};
}



/********************************************/
//	�I�v�V�����v���r���[
/********************************************/
class OptionTestPlayer
{
public:
	//------------------------------------------------------
	//	�R���X�g���N�^�E�f�X�g���N�^
	//------------------------------------------------------
	OptionTestPlayer(sceneOption *pOption, tdn2DObj *pExplosion) :m_pOption(pOption), m_NumExplosionPanel(pExplosion->GetWidth() / 128), m_iStartClock(clock()){}
	~OptionTestPlayer(){ Clear(); }

	//------------------------------------------------------
	//	�X�V�E�`��
	//------------------------------------------------------
	void Update();
	void Render(tdn2DObj *pNote, tdn2DObj *pExplosion);

	//------------------------------------------------------
	//	������
	//------------------------------------------------------
	void Clear();

	int m_NumExplosionPanel;
private:

	/********************************************/
	//	�����Ă���m�[�c
	/********************************************/
	class Note
	{
	public:
		Note(int x, float fBPM) : fBPM(fBPM), iEventTime(3000), iRestTime(3000), posX(x), ExplosionFrame(0), bNoteEnd(false) {}
		bool Update(int NumExplosionPanel);
		void Render(tdn2DObj *pNote, tdn2DObj *pExplosion);
		void Reset() { ExplosionFrame = 0; }
		int iEventTime;	// �m�[�c�̃C�x���g����
		int iRestTime;
		int posY;			// �m�[�c��Y���W
		const int posX;		// �m�[�c��X���W
		const float fBPM;	// �m�[�c��BPM
		int ExplosionFrame;	// �����G�t�F�N�g�t���[��
		bool bNoteEnd;		// �m�[�c�����I����āA������
	};
	//------------------------------------------------------
	//	�����o�ϐ�
	//------------------------------------------------------
	clock_t m_iStartClock;			// �m�[�c�ʒu�v�Z�p
	sceneOption *m_pOption;			// �Ϗ���
	std::list<Note*> m_NoteList;	// �m�[�c���X�g
};



/********************************************/
//	�I�v�V�����V�[��
/********************************************/
class	sceneOption : public BaseScene
{
public:
	//------------------------------------------------------
	//	�������E�J��
	//------------------------------------------------------
	bool Initialize();
	~sceneOption();

	//------------------------------------------------------
	//	�X�V�E�`��
	//------------------------------------------------------
	bool Update();
	void Render();


	//------------------------------------------------------
	//	�萔
	//------------------------------------------------------
	enum class SELECT_OPTION
	{
		HI_SPEED,
		MOVIE,
		GUIDE,
		SOUND,
		MAX
	};

	//------------------------------------------------------
	//	�摜ID
	//------------------------------------------------------
	enum class IMAGE
	{
		BACK,
		UP_FRAME,
		MIDASHI,
		LEFT_FRAME,
		RIGHT_FRAME,
		RIGHT_UP_FRAME,
		CURSOR_FRAME,
		CURSOR_FRAME_S,
		MOVIE,

		// �e�X�g�`��ɕK�v�Ȃ���
		SKIN,
		NOTE,
		EXPLOSION,
		KEYLINE,
		MAX
	};

	void Close() { SAFE_DELETE(m_pSelectOption); }

	//------------------------------------------------------
	//	�����o�ϐ�
	//------------------------------------------------------
	int m_iSelectOptionType;								// �I������I�v�V�����̃^�C�v
	tdnMovie *m_pMovie;										// �e�X�g����
	PlayerData::OptionV2 *m_pPlayerOption;					// �v���C���[���Z�b�g����I�v�V����(�v���C���[�f�[�^�Ƃ͕ʂŎ���)
	std::unique_ptr<tdn2DObj> m_pImages[(int)IMAGE::MAX];	// �I�v�V�����V�[���̊�{�摜
	std::unique_ptr<OptionTestPlayer> m_pOptionTest;		// �v���r���[�N
	std::unique_ptr<tdnSoundSE> m_pSE;						// ����܂藈�邱�ƂȂ������Ȃ̂ŁA�����p��SE�𐏎��m�ۉ��
private:
	std::unique_ptr<tdn2DObj> m_pPlayScreen;				// �v���C��ʃX�N���[��
	SelectOption::Base *m_pSelectOption;					// �ݒ�Ϗ�
	Surface *m_pSurface;
	float m_CurrentCursorPosY, m_NextCursorPosY;			// �J�[�\���g�̕�ԂŎg��
};
