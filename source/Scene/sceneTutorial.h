#pragma once

#include "../Input/InputMIDI.h"
#include<mbstring.h>



/********************************************/
//	�`���[�g���A������
/********************************************/
class TutorialDescription
{
public:

	//------------------------------------------------------
	//	�R���X�g���N�^(�����ɐ����̕���������炤)
	//------------------------------------------------------
	TutorialDescription(LPCSTR description) :m_lpsDescription(description), m_sRenderDescription(""), m_iWaitTime(0), m_iCursor(0)
	{
		m_pAnimImages[ANIM_IMAGE::FRAME] = new tdn2DAnim("DATA/UI/Other/frame.png");
		m_pAnimImages[ANIM_IMAGE::FRAME]->OrderStretch(15, 1, .0f, 1, 4);
	}

	//------------------------------------------------------
	//	�f�X�g���N�^
	//------------------------------------------------------
	~TutorialDescription()
	{
		POINTER_ARRAY_DELETE(m_pAnimImages, ANIM_IMAGE::MAX);
	}

	//------------------------------------------------------
	//	�X�V(�S���ǂݏI����Ă���true��Ԃ�)
	//------------------------------------------------------
	bool Update()
	{
		// �Ȃ񂩃{�^�������Ă��瑁���ǂݐi�߂�	
		if (InputMIDIMgr->GetAnyTrigger()) m_iWaitTime -= 1;

		// �҂����ԏI�������
		if (--m_iWaitTime < 0)
		{
			// �I�[�łȂ����L�[����������I��
			if (m_lpsDescription[m_iCursor] == '\0')
				return InputMIDIMgr->GetAnyTrigger();

			char buf[5] = { '\0' };
			int len(_mbclen((BYTE*)&m_lpsDescription[m_iCursor]));
			FOR(len) buf[i] = m_lpsDescription[m_iCursor + i];

			// �`��pstring�^��1��������
			m_sRenderDescription += buf;

			// �ǂ݃J�[�\����i�߂�
			m_iCursor += len;

			// �����̑ҋ@��ݒ�
			switch (m_lpsDescription[m_iCursor])
			{
			case '\n':
				m_iWaitTime = c_NEWLINE_WAIT;
				break;

			case '\0':
				m_iWaitTime = c_NEWLINE_WAIT;	// �I���������s�Ɠ������Ԃɂ���
				break;

			default:
				m_iWaitTime = c_LETTER_WAIT;	// 1�����ǂݑҋ@����
				break;
			}
		}

		// �܂�������
		return false;
	}
	void Render()
	{
		// �������̕`��
		tdnFont::RenderString(m_sRenderDescription.c_str(), 32, 320, 540);
	}

private:
	static const int c_LETTER_WAIT = 3;		// 1����������̑҂�����
	static const int c_NEWLINE_WAIT = 45;		// ���s���̑҂�����

	LPCSTR m_lpsDescription;				// ������
	std::string m_sRenderDescription;		// ���ۂɕ`�悷�镶��(+=���Ă���)
	int m_iCursor;							// �����ɑ΂���J�[�\��
	int m_iWaitTime;						// �҂�����(�����؂�ɕ������o���Ɠǂ݂ɂ����̂Ŕz�����Ă�����)

	enum ANIM_IMAGE
	{
		FRAME,
		MAX
	};
	tdn2DAnim *m_pAnimImages[ANIM_IMAGE::MAX];
};



/********************************************/
//	�`���[�g���A���Ǘ�
/********************************************/
class TutorialManager
{
public:

	//------------------------------------------------------
	//	�R���X�g���N�^(������)
	//------------------------------------------------------
	TutorialManager():m_iStep(0)
	{
		m_vTutorialDescription.push_back(new TutorialDescription("�悤�����AElectronicPianism�̐��E�ցI\n���̃Q�[���̓L�[�{�[�h�̌��Ղ��g���ėV�ԉ��y�Q�[������I"));
	}

	//------------------------------------------------------
	//	�f�X�g���N�^(���X�g�̊J��)
	//------------------------------------------------------
	~TutorialManager(){for(auto it : m_vTutorialDescription) delete it;}

	//------------------------------------------------------
	//	�X�V(�S���I�������true��Ԃ�)
	//------------------------------------------------------
	bool Update()
	{
		// �`���[�g���A�������X�V&���̐������I��������ǂ���
		const bool bNextStep(m_vTutorialDescription[m_iStep]->Update());

		// �������I����Ď��̃X�e�b�v�ցA�S���I�������I��
		if(bNextStep) if(++m_iStep >= m_vTutorialDescription.size()) return true;	// �I���I

		// �܂�������
		return false;
	}

	//------------------------------------------------------
	//	�`��
	//------------------------------------------------------
	void Render()
	{
		assert(m_iStep < m_vTutorialDescription.size());

		// �`���[�g���A�������`��
		m_vTutorialDescription[m_iStep]->Render();
	}

private:

	//------------------------------------------------------
	//	�`���[�g���A���̃X�e�b�v
	//------------------------------------------------------
	UINT m_iStep;

	//------------------------------------------------------
	//	�`���[�g���A���̐������X�g(for�ł̍X�V�ł͂Ȃ��A�Y������m_iStep���g��)
	//------------------------------------------------------
	std::vector<TutorialDescription*> m_vTutorialDescription;
};



/********************************************/
//	�`���[�g���A���V�[��
/********************************************/
class sceneTutorial:public BaseScene
{
	//------------------------------------------------------
	//	�������E�J��
	//------------------------------------------------------
	bool Initialize();
	~sceneTutorial();

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
		KEYBOARD,
		MAX
	};

	//------------------------------------------------------
	//	�����o�ϐ�
	//------------------------------------------------------
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];
	TutorialManager *m_pTutorialMgr;
};