#pragma once

class sceneCustomize;

/********************************************/
//	�J�X�^�}�C�Y�ݒ�Ϗ�
/********************************************/
namespace SelectCustomize
{
	//===============================================
	//	���
	//===============================================
	class Base
	{
	protected:
		static const Vector2 FRAME_OFFSET_XY;	// �E��g�̊J�n���W
		tdnText2 *m_pText;
		Vector2 m_CurrentCursorPos, m_NextCursorPos;	// �J�[�\���g�̕�ԂŎg��
		
		sceneCustomize *m_pCustomize;	// �Ϗ���
		int m_iCursor;
		int m_CursorAnimFrame;
		const int m_NumCustomize;	// �J�X�^�}�C�Y�̐�
		const int m_iCustomizeType;	// �J�X�^�}�C�Y�̎��
		bool UpdateCursor();		// �J�[�\��������true,�������Ȃ��t���[���Ȃ�false
		void RenderCursor();		// �J�[�\���`��
	public:
		Base(sceneCustomize *pCustomize, int iCustomizeType);
		virtual void Initialize();
		virtual ~Base();
		virtual void Release();
		virtual void Update(){ UpdateCursor(); }
		virtual void Render() = 0;
	};

	//===============================================
	//	�X�L���ύX
	//===============================================
	class Skin :public Base
	{
	public:
		Skin(sceneCustomize *pCustomize);
		void Render();
	};

	//===============================================
	//	�m�[�c�ύX
	//===============================================
	class Note :public Base
	{
	public:
		Note(sceneCustomize *pCustomize);
		void Render();
	};

	//===============================================
	//	�{���ύX
	//===============================================
	class Explosion :public Base
	{
	public:
		Explosion(sceneCustomize *pCustomize);
		void Render();
	};

	//===============================================
	//	�T�h�v���ύX
	//===============================================
	class Sudden :public Base
	{
	public:
		Sudden(sceneCustomize *pCustomize);
		void Initialize();
		//~Sudden();
		void Sudden::Release();
		void Render();
	};

	//===============================================
	//	�I��BGM�ύX
	//===============================================
	class SelectMusic :public Base
	{
	public:
		SelectMusic(sceneCustomize *pCustomize);
		void Initialize();
		//~SelectMusic(){ }
		void Release();
		void Update();
		void Render();
	private:
		tdnStreamSound *m_pStreamSound;	// �I��BGM�p�X�g���[�~���O
	};
}


/********************************************/
//	�J�X�^�}�C�Y�̃v���r���[�S��
/********************************************/
class CustomizeTestPlayer
{
public:
	//===============================================
	//	�R���X�g���N�^�E�f�X�g���N�^
	//===============================================
	CustomizeTestPlayer(sceneCustomize *pCustomize) :m_pCustomize(pCustomize){ Reset(); }
	~CustomizeTestPlayer(){ Clear(); }

	//===============================================
	//	�X�V�E�`��
	//===============================================
	void Update();
	void Render();

	//===============================================
	//	������
	//===============================================
	void Clear();
	void Reset();

	int m_NumExplosionPanel;
private:

	/********************************************/
	//	�����Ă���m�[�c
	/********************************************/
	class Note
	{
	public:
		Note(int x, bool bBlack) : posY(133), posX(x), ExplosionFrame(0), bNoteEnd(false), bBlack(bBlack) {}
		bool Update(int NumExplosionPanel);
		void Render(tdn2DObj *pNote, tdn2DObj *pExplosion);
		void Reset() { ExplosionFrame = 0; }
	private:
		int posY;
		const int posX;
		int ExplosionFrame;	// �����G�t�F�N�g�t���[��
		bool bNoteEnd;		// �m�[�c�����I����āA������
		const bool bBlack;		// �����t���N
	};

	//===============================================
	//	�����o�ϐ�
	//===============================================
	sceneCustomize *m_pCustomize;	// �Ϗ���
	std::list<Note*> m_NoteList;	// �����Ă�m�[�c�̃��X�g
};



/********************************************/
//	�J�X�^�}�C�Y�V�[��
/********************************************/
class	sceneCustomize : public BaseScene
{
public:
	//------------------------------------------------------
	//	�������E�J��
	//------------------------------------------------------
	bool Initialize();
	~sceneCustomize();

	//------------------------------------------------------
	//	�X�V�E�`��
	//------------------------------------------------------
	bool Update();
	void Render();

	//===============================================
	//	�摜ID
	//===============================================
	enum IMAGE
	{
		BACK,
		KEYLINE,
		UP_FRAME,
		MIDASHI,
		LEFT_FRAME,
		RIGHT_FRAME,
		RIGHT_UP_FRAME,
		CURSOR_FRAME,
		CURSOR_FRAME_S,
		MUSIC,
		MAX
	};

	//===============================================
	//	�����o�ϐ�
	//===============================================
	UINT *m_NumCustomizes;			// �e�J�X�^�}�C�Y�̐�(�ϔz��|�C���^�ɂ���B�Ȃ��z��Ő錾���Ȃ����́A�w�b�_�C���N���[�h��h�~���邽��)
	int m_iSelectCustomizeType;		// �I������J�X�^�}�C�Y�̃^�C�v
	int *m_iSelectCustomizes;		// �J�X�^�}�C�Y���Ƃ̑I�����Ă���
	float m_fNextSudden;			// �T�h�D������(�T�h�D���I��������1�ɂ��čő�ɂ���)
	bool m_bSelect;					// ��ޑI�񂾂�
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];			// �J�X�^�}�C�Y�V�[���̊�{�摜
	CustomizeTestPlayer *m_pCustomizeTest;
	tdn2DObj **m_pSkins;			// �w�i�̃X�L������
	tdn2DObj **m_pExplosions;		// �����G�t�F�N�g����
	tdn2DObj **m_pNotes;			// �m�[�c�摜����
	tdn2DObj **m_pSuddens;			// �T�h�D������
	tdnSoundSE *m_pSE;				// ����܂藈�邱�ƂȂ������Ȃ̂ŁA�����p��SE�𐏎��m�ۉ��
	void Close()
	{
		//SAFE_DELETE(m_pSelectCustomize); 
		m_pSelectCustomizes[m_iSelectCustomizeType]->Release();
	}

private:
	//===============================================
	//	�����o�ϐ�
	//===============================================
	float m_fSuddenPercent;							// �T�h�D������
	float m_CurrentCursorPosY, m_NextCursorPosY;	// �J�[�\���g�̕�ԂŎg��
	SelectCustomize::Base **m_pSelectCustomizes;	// [CUSTOMIZE_TYPE::MAX];
	std::unique_ptr<tdn2DObj> m_pPlayScreen;		// �v���C��ʃX�N���[��
	Surface *m_pSurface;

};
