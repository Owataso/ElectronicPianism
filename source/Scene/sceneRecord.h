#pragma once

#include "../Data/PlayerData.h"

// �O���錾
class AlphabetRenderer;



/********************************************/
//	�L�^�{���V�[��
/********************************************/
class sceneRecord : public BaseScene
{
public:
	//------------------------------------------------------
	//	�������E�J��
	//------------------------------------------------------
	bool Initialize();
	~sceneRecord();

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
		GRADE_TEXT,
		ALPHABET,
		MAX
	};

	//------------------------------------------------------
	//	�����o�ϐ�
	//------------------------------------------------------
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];	// �摜
	AlphabetRenderer *m_pAlphabetRenderer;				// �v���C���[�̖��O�`��p
	PlayerData::Infomation *m_pPlayerInfo;				// �v���C���[�̃v���C���
	GRADE m_grade;										// �v���C���[���������Ă���i��
};