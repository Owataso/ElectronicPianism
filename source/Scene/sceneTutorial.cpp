#include "TDNLIB.h"
#include "../system/Framework.h"
#include "sceneTutorial.h"
#include "sceneModeSelect.h"
#include "../Fade/Fade.h"

bool sceneTutorial::Initialize()
{	
	// フェード設定
	Fade::Set(Fade::MODE::FADE_IN, 5, 0, 255, 0x00000000);

	// 画像初期化
	m_pImages[IMAGE::KEYBOARD] = std::make_unique<tdn2DObj>("DATA/Tutorial/keyboard.png");

	// チュートリアルマネージャー初期化
	m_pTutorialMgr=new TutorialManager;

	return true;
}

sceneTutorial::~sceneTutorial()
{
	delete m_pTutorialMgr;
}

bool sceneTutorial::Update()
{
	// フェード更新
	Fade::Update();

	// チュートリアル更新(Updateでtrueが返るという事は、終了したという事)
	if(m_pTutorialMgr->Update()) MainFrame->ChangeScene(new sceneModeSelect, true);
 
	return true;
}

void sceneTutorial::Render()
{
	// 画面クリア
	tdnView::Clear();

	// チュートリアル描画
	m_pTutorialMgr->Render();

	// フェード描画
	Fade::Render();
}

