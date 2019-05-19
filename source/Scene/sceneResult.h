#pragma once

#include "../Effect/CyberCircle.h"
#include "../Effect/KeyboardGuide.h"



/********************************************/
//	���[�h���Ƃ̃��U���g�Ϗ�
/********************************************/
namespace Result
{
	//------------------------------------------------------
	//	�萔
	//------------------------------------------------------
	enum class TYPE
	{
		FREE
	};

	//------------------------------------------------------
	//	���
	//------------------------------------------------------
	class Base
	{
	public:
		Base() :m_bEnable(false){}
		virtual ~Base(){}
		virtual bool Update() = 0;
		virtual void Render() = 0;
		virtual void Enable(){ m_bEnable = true; }
		virtual TYPE GetType() = 0;
	protected:
		bool m_bEnable;	// �ғ����(�v�͕\�����邩���Ȃ���)
	};

	//------------------------------------------------------
	//	�t���[���[�h�̎��̓���
	//------------------------------------------------------
	class Free: public Base
	{
	public:
		Free(tdn2DObj *pWindow) :Base(), m_CursorAnim(0), m_pWindow(pWindow), m_pCursor(new tdn2DObj("DATA/UI/Other/cursor_frame2.png")), m_pKeyboardGuide(new KeyboardGuide(Vector2(200, 360), 3, 0.1f, 0.5f, 1.0f, 60))
		{
			m_CurrentCursorY = m_NextCursorY = 52 + m_iSelectCursor * 50.0f;
		}
		bool Update();
		void Render();
		void Enable();
		TYPE GetType(){ return TYPE::FREE; }
	private:
		enum class SELECT
		{
			CONTINUE,		// �������
			MUSIC_SELECT,	// �ȃZ���N�g
			EXIT,			// ���[�h�I���ɖ߂�
			MAX,
		};
		static int m_iSelectCursor;
		int m_CursorAnim;
		float m_CurrentCursorY, m_NextCursorY;	// �J�[�\�����
		std::unique_ptr<tdn2DObj> m_pCursor;
		tdn2DObj *m_pWindow;	// �Q�Ƃ��邾���Ȃ̂Ő��|�C���^(�X�}�[�g�|�C���^�ɂ����痎����)

		// �K�C�h
		std::unique_ptr<KeyboardGuide> m_pKeyboardGuide;
	};

}


/********************************************/
//	���U���g�V�[��
/********************************************/
class sceneResult : public BaseScene
{
public:

	//------------------------------------------------------
	//	�������E�J��
	//------------------------------------------------------
	sceneResult(bool bClear, bool bPractice = false);
	bool Initialize();
	~sceneResult();

	//------------------------------------------------------
	//	�X�V�E�`��
	//------------------------------------------------------
	bool Update();
	void Render();

private:
	//------------------------------------------------------
	//	�摜ID
	//------------------------------------------------------
	enum IMAGE
	{
		BACK,
		UP_FRAME,
		MIDASHI,
		NUMBER,
		NUMBER_L,
		UI_MAP,
		MUSIC_NAME,	// �Ȗ�
		DIF,		// ��Փx
		MAX
	};
	enum ANIM_IMAGE
	{
		FRAME1,	// ��t���[��
		FRAME2,	// �����t���[��
		FRAME3,	// �E���t���[��
		FULLCOMBO,
		NEW_RECORD,
		RANK,	// �����N
		RANK_RIPPLE,	// �����N���b�v��
		ANIM_MAX,
	};

	//------------------------------------------------------
	//	�����o�ϐ�
	//------------------------------------------------------
	int m_iStep;			// �X�e�b�v
	const int m_iRank;		// �����N
	const bool m_bCleared;	// �N���A������
	const int m_iScore;		// �X�R�A
	const int m_iHiScore;	// �n�C�X�R�A
	const int m_combo;		// �R���{
	const int m_notes;		// �g�[�^���m�[�c
	int m_iJudgeCount[4];	// �z�񂾂���C�j�V�����C�U�őS���������ł��Ȃ��I�I�I���������������₵�������������������������h�I�I�I�I�I�I
	bool m_bPractice;		// ���K�t���O
	bool m_bNewRecord;		// �X�R�A�L�^�X�V
	bool m_bFullCombo;		// �t���R��������
	Result::Base *m_pResult;// �Ϗ��|�C���^
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];				// �摜
	std::unique_ptr<tdn2DAnim> m_pAnimImages[ANIM_IMAGE::ANIM_MAX];	// �A�j���\�V�����摜
	std::unique_ptr<CyberCircle> m_pCircle;							// �����N�̉��ɒu���~�G�t�F�N�g
	// �X�R�A�Ƃ��ĕ`�悷��悤�̐���
	int m_iRenderScore, 
		m_iRenderCombo, 
		m_iRenderJudgeCount[4];

	//------------------------------------------------------
	//	�����o�֐�
	//------------------------------------------------------
	void RenovationRecord();	// �v���C���[�f�[�^�X�V
};


struct GradeData;

// �i�ʂ̑������U���g
class sceneGradeResult : public BaseScene
{
public:
	//------------------------------------------------------
	//	�������ƊJ��
	//------------------------------------------------------
	sceneGradeResult(bool bPass);
	bool Initialize();
	~sceneGradeResult();

	//------------------------------------------------------
	//	�X�V�ƕ`��
	//------------------------------------------------------
	bool Update();
	void Render();

private:
	//------------------------------------------------------
	//	�摜ID
	//------------------------------------------------------
	enum IMAGE
	{
		BACK,
		RESULT,
		IMG_GRADE,
		UP_FRAME,
		MIDASHI,
		FRAME,
		NUMBER,
		NUMBER_L,
		MAX
	};

	//------------------------------------------------------
	//	�����o�ϐ�
	//------------------------------------------------------
	bool m_bPassed;										// �N���A�t���O
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];	// �摜
	GradeData *m_pGradeData;							// ���[�h�Ǘ�����̂��Q�Ƃ���
	UINT m_Achievement;									// �����B����
	tdnSoundSE *m_pSE;									// ���U���g�̃��A�N�V����(�����ł����g�����)


	//------------------------------------------------------
	//	�����o�֐�
	//------------------------------------------------------
	void RenovationRecord();	// �v���C���[�f�[�^�X�V
};