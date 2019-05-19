#pragma once

#include "../Effect/KeyboardGuide.h"

class tdnMovie;



/********************************************/
//	�I���������[�h��
/********************************************/
namespace SelectMode
{
	//------------------------------------------------------
	//	���
	//------------------------------------------------------
	class Base
	{
	public:
		~Base();
		//virtual void Initialize() = 0;	// �S�������ŁAm_pNextScene�̎��̂�new����B�����ă��[�h�̃R�����g��ݒ肷��	�����^�[����new��Ԃ��悤�ɂ����̂ŕK�v�Ȃ��Ȃ���

		// �Q�b�^�[
		virtual BaseScene *GetNextScene() = 0;	// �e���ꂼ��new�̃V�[����Ԃ�
		//const char *GetDescription();
	protected:
		//std::string m_ModeDescription;	// ���[�h�̐�����
	};

	//------------------------------------------------------
	//	�X�^���_�[�h
	//------------------------------------------------------
	class Standard : public Base
	{
	public:
		Standard();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// �i�ʔF��
	//------------------------------------------------------
	class Grade : public Base
	{
	public:
		Grade();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// �t���[
	//------------------------------------------------------
	class Free : public Base
	{
	public:
		Free();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// �`���[�g���A��
	//------------------------------------------------------
	class Tutorial : public Base
	{
	public:
		Tutorial();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// �X�L���J�X�^�}�C�Y
	//------------------------------------------------------
	class Customize: public Base
	{
	public:
		Customize();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// �I�v�V����
	//------------------------------------------------------
	class Option : public Base
	{
	public:
		Option();
		BaseScene *GetNextScene();
	};

	//------------------------------------------------------
	// �v���C���[�̋L�^
	//------------------------------------------------------
	class Record : public Base
	{
	public:
		Record();
		BaseScene *GetNextScene();
	};
}



/********************************************/
//	���[�h�Z���N�g�V�[��
/********************************************/
class sceneModeSelect : public BaseScene
{
public:
	//------------------------------------------------------
	//	�������E�J��
	//------------------------------------------------------
	bool Initialize();
	~sceneModeSelect();

	//------------------------------------------------------
	//	�X�V�E�`��
	//------------------------------------------------------
	bool Update();
	void Render();

private:
	//------------------------------------------------------
	//	�萔
	//------------------------------------------------------
	enum class SELECT_MODE
	{
		TUTORIAL,	// �`���[�g���A��
		STANDARD,	// �A�[�P�[�h
		GRADE,		// �i�ʔF��
		FREE,		// �t���[���[�h
		CUSTMIZE,	// �X�L���J�X�^�}�C�Y
		RECORD,		// �v���C�L�^
		OPTION,		// �I�v�V����
		MAX
	};
	
	//------------------------------------------------------
	//	�摜ID
	//------------------------------------------------------
	enum class IMAGE
	{
		BACK,	// �w�i	
		MODE,	// 7�̃��[�h�̉摜
		MAX
	};
	enum class ANIM_IMAGE
	{
		MODE1,
		MODE2, 
		MODE3,
		MODE4,
		MODE5,
		MODE6,
		MODE7,
		MIDASHI,
		FRAME, 
		MODE_STR,
		UP_FRAME,
		DESCRIPTION,
		MAX
	};

	//------------------------------------------------------
	//	�����o�ϐ�
	//------------------------------------------------------
	static int m_ModeCursor;												// ���[�h�I���J�[�\��(++�Ƃ�����̂ŁAint�^�ɂ���)
	int m_WaitTimer;														// ���̃V�[���ɓ����Ă���A�j���I���܂ł̂��������̑ҋ@����
	std::unique_ptr<SelectMode::Base> m_pSelectMode[(int)SELECT_MODE::MAX];	// �I�ԃ��[�h�̃|�C���^
	std::unique_ptr<tdn2DObj> m_pImages[(int)IMAGE::MAX];					// tdn�摜����
	std::unique_ptr<tdn2DAnim> m_pAnimImages[(int)ANIM_IMAGE::MAX];			// �A�j���[�V�����摜����
	std::unique_ptr<KeyboardGuide> m_pKeyboardGuide;						// �K�C�h
	tdnMovie *m_pMovie;														// ����
};