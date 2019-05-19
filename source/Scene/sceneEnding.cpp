#include "TDNLIB.h"
#include "system/Framework.h"
#include "sceneTitle.h"
#include "SceneEnding.h"
#include "../Sound/SoundManager.h"
#include "../Input/InputMIDI.h"
#include "TDN_Movie.h"

//=============================================================================================
//		��	��	��	��	�J	��
bool sceneEnding::Initialize()
{
	// ����
	m_pMovie = new tdnMovie("DATA/movie/ending.wmv", true);
	m_pMovie->Play();
	m_pBackImage = std::make_unique<tdn2DObj>(m_pMovie->GetTexture());

	// �X�g���[���v���C���[�J�n
	g_pBGM->PlayStreamIn("DATA/Sound/BGM/Rising_Winter.ogg", false);

	return true;
}

sceneEnding::~sceneEnding()
{
	// �Ȏ~�߂�
	g_pBGM->StopStreamIn();

	delete m_pMovie;
}
//=============================================================================================


//=============================================================================================
//		�X			�V
bool sceneEnding::Update()
{
	// ���[�v�Đ��p�̍X�V
	m_pMovie->Update();

	// �ȏI�������܂��̓G���^�[�L�[��
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		// �^�C�g���ɍs��
		MainFrame->ChangeScene(new sceneTitle);
	}

	return true;
}
//
//=============================================================================================


//=============================================================================================
//		�`			��
void sceneEnding::Render()
{
	
	tdnView::Clear();

	// �w�i�`��
	m_pBackImage->Render(0,0);
}
//
//=============================================================================================