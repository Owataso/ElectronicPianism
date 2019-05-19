#pragma once


/********************************************/
//	�i�ʃZ���N�g�V�[��
/********************************************/
class sceneClassSelect : public BaseScene
{
public:

	//------------------------------------------------------
	//	�������E�J��
	//------------------------------------------------------
	bool Initialize();
	~sceneClassSelect();

	//------------------------------------------------------
	//	�X�V�E�`��
	//------------------------------------------------------
	bool Update();
	void Render();

private:
	//===============================================
	//	�摜ID
	//===============================================
	enum class IMAGE
	{
		BACK,
		SELECT_FRAME,
		GRADE_TEXT,
		NUMBER,
		FRAME,
		FRAME_IN_TEXT,
		MAX
	};
	enum class ANIM_IMAGE
	{
		UP_FRAME,
		MIDASHI,
		GRADE_TEXT1,
		GRADE_TEXT2,
		SELECT_FRAME,
		PASS,
		SCORE_FRAME,
		MAX
	};

	//===============================================
	//	�����o�ϐ�
	//===============================================
	int m_WaitTimer;			// ���̃V�[���ɓ����Ă���A�j���I���܂ł̂��������̑ҋ@����
	int m_iCursor;				// �i�ʂ̃J�[�\��
	std::unique_ptr<tdn2DObj>  m_pImages[(int)IMAGE::MAX];	// �摜
	std::unique_ptr<tdn2DAnim> m_pAnimImages[(int)ANIM_IMAGE::MAX];
	void GradeDataSetting();	// ���[�h�}�l�[�W���[����Ƀf�[�^�𑗐M����

	
	/********************************************/
	//	�g�N���X
	/********************************************/
	class Frame
	{
	public:
		Frame(int cursor, int grade, int rank, Texture2D *pFrame, Texture2D *pText);
		void Update();
		void Render();
		void SetCursor(int cursor);	// �J�[�\��������Ăяo���B���̍��W�̍X�V
	private:
		Vector2 pos, NextPos;
		int grade, rank;				// grade(�i�ʂ̔ԍ�) rank(�i�ʂ̐F�I�ȉ���)
		std::unique_ptr<tdn2DAnim> m_pFrame, m_pText;	// �g�Ƃ��̒��̉摜
	}**m_pFrames;	// [GRADE::MAX - 1]


};