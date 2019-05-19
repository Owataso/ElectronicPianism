#include "TDNLIB.h"
#include "../system/Framework.h"
#include "sceneTutorial.h"
#include "sceneModeSelect.h"
#include "../Fade/Fade.h"

bool sceneTutorial::Initialize()
{	
	// �t�F�[�h�ݒ�
	Fade::Set(Fade::MODE::FADE_IN, 5, 0, 255, 0x00000000);

	// �摜������
	m_pImages[IMAGE::KEYBOARD] = std::make_unique<tdn2DObj>("DATA/Tutorial/keyboard.png");

	// �`���[�g���A���}�l�[�W���[������
	m_pTutorialMgr=new TutorialManager;

	return true;
}

sceneTutorial::~sceneTutorial()
{
	delete m_pTutorialMgr;
}

bool sceneTutorial::Update()
{
	// �t�F�[�h�X�V
	Fade::Update();

	// �`���[�g���A���X�V(Update��true���Ԃ�Ƃ������́A�I�������Ƃ�����)
	if(m_pTutorialMgr->Update()) MainFrame->ChangeScene(new sceneModeSelect, true);
 
	return true;
}

void sceneTutorial::Render()
{
	// ��ʃN���A
	tdnView::Clear();

	// �`���[�g���A���`��
	m_pTutorialMgr->Render();

	// �t�F�[�h�`��
	Fade::Render();
}

