#include "TDNLIB.h"
#include "system/Framework.h"
#include "sceneTitle.h"
#include "sceneModeSelect.h"
#include "SceneEntry.h"
#include "../Input/InputMIDI.h"
#include "../Fade/Fade.h"

// 静的メンバ変数の宣言
bool sceneTitle::m_bLoadSaveData = false;

//=============================================================================================
//		初	期	化	と	開	放
bool sceneTitle::Initialize()
{
	// フェード初期化
	Fade::Set(Fade::MODE::FADE_IN, 5, 0x00000000);

	// 画像初期化
	m_pImages[IMAGE::LOGO]	= std::make_unique<tdn2DObj>("DATA/Image/logo.png");
	m_pImages[IMAGE::BACK1] = std::make_unique<tdn2DObj>("DATA/Image/back2.png");
	m_fLoadPercentage = .5f;	// ロード割合
	m_pImages[IMAGE::BACK2] = std::make_unique<tdn2DObj>("DATA/Image/back3.png");
	m_pImages[IMAGE::BACK2]->SetARGB(0x80ffffff);

	m_fLoadPercentage = 1.0f;	// ロード割合

	// 画像移動
	memset(m_moveX, 0, sizeof(m_moveX));


	return true;
}

//=============================================================================================


//=============================================================================================
//		更			新
bool sceneTitle::Update()
{
	static bool bEnd(false);

	Fade::Update();

	// 戻るフェード中
	if (bEnd)
	{
		// フェード終わったら帰る
		if (Fade::m_eMode == Fade::MODE::FADE_NONE)
		{
			bEnd = false;

			// 初回遷移なら
			if (!m_bLoadSaveData)
			{
				// データロードシーンに移行
				m_bLoadSaveData = true;
				MainFrame->ChangeScene(new sceneEntry, true);
			}

			// そうでないならモード選択画面へ
			else MainFrame->ChangeScene(new sceneModeSelect, true);

		}
		return true;
	}

	// ループ再生用の更新
	//m_pMovie[0]->LoopUpdate();
	//m_pMovie[1]->LoopUpdate();

	// 背景画像移動更新
	m_moveX[0] += 1;
	m_moveX[1] -= 1;

	// エンターキーでスタート
	if (InputMIDIMgr->GetAnyTrigger())
	{
		// フェードアウト設定
		Fade::Set(Fade::MODE::FADE_OUT, 8);
		bEnd = true;
	}

	// スペースキーでEXIT
	else if (KeyBoardTRG(KB_SPACE)) PostQuitMessage(0);

	return true;
}
//
//=============================================================================================


//=============================================================================================
//		描			画
void sceneTitle::Render()
{
	tdnView::Clear();

	// 背景
	m_pImages[IMAGE::BACK1]->Render(0, 0, tdnSystem::GetScreenSize().right, tdnSystem::GetScreenSize().bottom, m_moveX[0], 0, 1280, 720);
	m_pImages[IMAGE::BACK2]->Render(0, 0, tdnSystem::GetScreenSize().right, tdnSystem::GetScreenSize().bottom, m_moveX[1], 0, 1280, 720);

	//m_pMovieTexture[1]->Render(0,0);
	//m_pMovieTexture[0]->Render(0, 0);

	// ロゴ
	m_pImages[IMAGE::LOGO]->Render(1280 / 2 - 720 / 2, 160);

	static int flashing = 0;
	flashing++;
	if (++flashing < 75)tdnText::Draw(560, 500, 0xffffffff, "PRESS ANY KEY");
	else if (flashing > 100) flashing = 0;

	Fade::Render();
}
//
//=============================================================================================