#include "TDNLIB.h"
#include "system/Framework.h"
#include "sceneTitle.h"
#include "SceneEnding.h"
#include "../Sound/SoundManager.h"
#include "../Input/InputMIDI.h"
#include "TDN_Movie.h"

//=============================================================================================
//		初	期	化	と	開	放
bool sceneEnding::Initialize()
{
	// 動画
	m_pMovie = new tdnMovie("DATA/movie/ending.wmv", true);
	m_pMovie->Play();
	m_pBackImage = std::make_unique<tdn2DObj>(m_pMovie->GetTexture());

	// ストリームプレイヤー開始
	g_pBGM->PlayStreamIn("DATA/Sound/BGM/Rising_Winter.ogg", false);

	return true;
}

sceneEnding::~sceneEnding()
{
	// 曲止める
	g_pBGM->StopStreamIn();

	delete m_pMovie;
}
//=============================================================================================


//=============================================================================================
//		更			新
bool sceneEnding::Update()
{
	// ループ再生用の更新
	m_pMovie->Update();

	// 曲終わったらまたはエンターキーで
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		// タイトルに行く
		MainFrame->ChangeScene(new sceneTitle);
	}

	return true;
}
//
//=============================================================================================


//=============================================================================================
//		描			画
void sceneEnding::Render()
{
	
	tdnView::Clear();

	// 背景描画
	m_pBackImage->Render(0,0);
}
//
//=============================================================================================