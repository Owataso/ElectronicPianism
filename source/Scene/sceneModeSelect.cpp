#include "TDNLIB.h"
#include "system/Framework.h"
#include "sceneModeSelect.h"
#include "SceneEdit.h"
#include "sceneMusicSelect.h"
#include "sceneEnding.h"
#include "sceneCustomize.h"
#include "sceneOption.h"
#include "sceneTitle.h"
#include "sceneRecord.h"
#include "sceneClassSelect.h"
#include "sceneTutorial.h"
#include "../Sound/SoundManager.h"
#include "../Mode/ModeManager.h"
#include "../Fade/Fade.h"
#include "../Input/InputMIDI.h"
#include "../Data/PlayerData.h"
#include "TDN_Movie.h"

#ifdef _DEBUG
#include "DebugRoom.h"
#endif

//*****************************************************************************************************************************
//
//		モード関係
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	各モードの初期化
//------------------------------------------------------
SelectMode::Standard::Standard()
{
	//m_ModeDescription = "基本的なモードです。3曲演奏します。";
}
BaseScene *SelectMode::Standard::GetNextScene()
{
	// モード管理さんにモード設定する
	//ModeMgr->SetMode(PLAY_MODE::STANDARD);
	ModeMgr->SetMode(PLAY_MODE::FREE);
	return new sceneMusicSelect; 
}

SelectMode::Grade::Grade()
{
	//m_ModeDescription = "挑戦する段位を選択し、３曲完走すれば合格です。";
}
BaseScene *SelectMode::Grade::GetNextScene()
{
	// モード管理さんにモード設定する
	ModeMgr->SetMode(PLAY_MODE::GRADE);

	return new sceneClassSelect;
}

SelectMode::Free::Free()
{
	//m_ModeDescription = "自由に曲を選んでプレイするモードです。";
}
BaseScene *SelectMode::Free::GetNextScene()
{
	// モード管理さんにモード設定する
	ModeMgr->SetMode(PLAY_MODE::FREE);

	return new sceneMusicSelect;
}

SelectMode::Tutorial::Tutorial()
{
	//m_ModeDescription = "チュートリアルです。";
}
BaseScene *SelectMode::Tutorial::GetNextScene()
{
	// モード管理さんにモード設定する
	ModeMgr->SetMode(PLAY_MODE::MAX);

	return 
		//new sceneTutorial;
		nullptr;
}

SelectMode::Customize::Customize()
{
	//m_ModeDescription = "プレイヤースキンをカスタマイズするモードです";
}
BaseScene *SelectMode::Customize::GetNextScene()
{
	return new sceneCustomize;
}

SelectMode::Option::Option()
{
	//m_ModeDescription = "オプションです。キーボードの音程の設定とか。";
}
BaseScene *SelectMode::Option::GetNextScene()
{
	return new sceneOption;
}

SelectMode::Record::Record()
{
	//m_ModeDescription = "プレイヤーの記録を閲覧します。";
}
BaseScene *SelectMode::Record::GetNextScene()
{
	return new sceneRecord;
}

//------------------------------------------------------
//	モード共通のデストラクタ
//------------------------------------------------------
SelectMode::Base::~Base()
{

}

//------------------------------------------------------
//	モード共通のゲッタ
//------------------------------------------------------
//const char *SelectMode::Base::GetDescription(){ return m_ModeDescription.c_str(); }


int sceneModeSelect::m_ModeCursor = 1;

//=============================================================================================
//		初	期	化	と	開	放
bool sceneModeSelect::Initialize()
{
	// 選択モードポインタ初期化
	m_pSelectMode[(int)SELECT_MODE::STANDARD]	= std::make_unique<SelectMode::Standard>();
	m_pSelectMode[(int)SELECT_MODE::GRADE]		= std::make_unique<SelectMode::Grade>();
	m_pSelectMode[(int)SELECT_MODE::FREE]		= std::make_unique<SelectMode::Free>();
	m_pSelectMode[(int)SELECT_MODE::RECORD]		= std::make_unique<SelectMode::Record>();
	m_pSelectMode[(int)SELECT_MODE::CUSTMIZE]	= std::make_unique<SelectMode::Customize>();
	m_pSelectMode[(int)SELECT_MODE::OPTION]		= std::make_unique<SelectMode::Option>();
	m_pSelectMode[(int)SELECT_MODE::TUTORIAL]	= std::make_unique<SelectMode::Tutorial>();
	//m_ModeCursor = 0;

	m_fLoadPercentage = .25f;	// ロード割合

	// 動画初期化
	m_pMovie = new tdnMovie("DATA/movie/ModeSelect.wmv");
	m_pMovie->Play();

	m_fLoadPercentage = .4f;

	// 画像管理くん初期化
	//m_pImageMgr = new ImageManager("DATA/Text/ImageSet/ModeSelect.txt");
	m_pImages[(int)IMAGE::BACK] = std::make_unique<tdn2DObj>(m_pMovie->GetTexture());
	m_pImages[(int)IMAGE::MODE] = std::make_unique<tdn2DObj>("DATA/UI/ModeSelect/mode.png");
	m_fLoadPercentage = .6f;	// ロード割合
	FOR((int)SELECT_MODE::MAX)
	{
		m_pAnimImages[i] = std::make_unique<tdn2DAnim>(m_pImages[(int)IMAGE::MODE]->GetTexture());
		m_pAnimImages[i]->OrderMoveAppeared(3, 40 + i * 168, 120 + 360);
		m_pAnimImages[i]->Action(15 + i * 2);
	}
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI] = std::make_unique<tdn2DAnim>("DATA/UI/Other/midashi.png");
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->OrderMoveAppeared(16, -8, 12);
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Action(25);
	m_fLoadPercentage = .7f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/frame.png");
	m_pAnimImages[(int)ANIM_IMAGE::FRAME]->OrderStretch(15, 1, .0f);
	m_pAnimImages[(int)ANIM_IMAGE::FRAME]->Action(20);
	m_pAnimImages[(int)ANIM_IMAGE::MODE_STR] = std::make_unique<tdn2DAnim>("DATA/UI/ModeSelect/mode_str.png");
	m_pAnimImages[(int)ANIM_IMAGE::MODE_STR]->OrderMoveAppeared(10, 1280 + 320, 470);
	m_pAnimImages[(int)ANIM_IMAGE::MODE_STR]->Action(30);
	m_fLoadPercentage = .85f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/up_frame.png");
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->OrderMoveAppeared(30, 0, -64);
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Action(0);
	m_fLoadPercentage = .95f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::DESCRIPTION] = std::make_unique<tdn2DAnim>("DATA/UI/ModeSelect/description.png");
	m_pAnimImages[(int)ANIM_IMAGE::DESCRIPTION]->OrderMoveAppeared(20, 160, 572);
	m_pAnimImages[(int)ANIM_IMAGE::DESCRIPTION]->Action(30);

	m_fLoadPercentage = 1.0f;	// ロード割合

	// キーボードのガイド
	m_pKeyboardGuide = std::make_unique<KeyboardGuide>(Vector2(32, 430), 3, 0.1f, 0.5f, 1.0f, 60);

	// シーンに入ってからの操作受付までの時間
	m_WaitTimer = 28;

	// サウンド流す
	//g_pBGM->Fade_in("モードセレクト", 0.25f);
	g_pBGM->PlayStreamIn("DATA/Sound/BGM/Cyberspace_Players.ogg");

	// フェード
	Fade::Set(Fade::MODE::FADE_IN, 10, 0x00ffffff);

	return true;
}

sceneModeSelect::~sceneModeSelect()
{
	g_pBGM->StopStreamIn();
	//delete m_pImageMgr;
	delete m_pMovie;
}
//=============================================================================================


//=============================================================================================
//		更			新
bool sceneModeSelect::Update()
{
	// 動画更新
	m_pMovie->Update();

	// フェード更新
	Fade::Update();

	// 画像アニメ更新
	FOR((int)ANIM_IMAGE::MAX) m_pAnimImages[i]->Update();

	// キーボードのガイド更新
	m_pKeyboardGuide->Update();

	// アニメ終わるまで待機
	if (m_WaitTimer > 0){ m_WaitTimer--; return true; }

	// モード選択SE
	static const char *selectSE[(int)SELECT_MODE::MAX] = { "C", "D", "E", "F", "G", "A", "B" };

	bool bCursored(false);
	if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
	{
		if (--m_ModeCursor < 1) m_ModeCursor = (int)SELECT_MODE::MAX - 1;
		bCursored = true;
	}
	else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
	{
		if (++m_ModeCursor >= (int)SELECT_MODE::MAX) m_ModeCursor = 1;
		bCursored = true;
	}

	if (bCursored)
	{
		// SEを鳴らす
		g_pSE->Play((char*)selectSE[m_ModeCursor]);

		// モードの文字のアクション
		m_pAnimImages[(int)ANIM_IMAGE::MODE_STR]->Action();
		m_pAnimImages[(int)ANIM_IMAGE::DESCRIPTION]->Action();
	}

	// 数字キーでモード選択
	static const BYTE key[(int)SELECT_MODE::MAX] = { KB_1, KB_2, KB_3, KB_4, KB_5, KB_6, KB_7 };
	for (int i = 1; i < (int)SELECT_MODE::MAX; i++)
	{
		if (KeyBoardTRG(key[i]))
		{
			g_pSE->Play((char*)selectSE[i]);
			m_ModeCursor = i;
			break;
		}
	}


	// エンターキーでモード決定！
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		//g_pBGM->Fade_out("モードセレクト", 0.25f);
		// モードによって切り替えるシーンを分岐
		BaseScene *l_pNewScene(m_pSelectMode[m_ModeCursor]->GetNextScene());
		if (l_pNewScene)MainFrame->ChangeScene(l_pNewScene, true);
	}
	// スペースキーで戻る
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
	{
		// タイトルへ
		MainFrame->ChangeScene(new sceneTitle, true);
	}
	else if (KeyBoardTRG(KB_S))
	{
		// セーブはこまめに
		PlayerDataMgr->SavePlayerData();

		Fade::Set(Fade::MODE::FADE_IN, 8, 0, 128, 0x00ffffff);
	}

#ifdef _DEBUG
	// デバッグ
	else if (KeyBoardTRG(KB_E))
	{
		//MainFrame->ChangeScene(new sceneEnding, true);
		MainFrame->ChangeScene(new sceneEdit, true);
	}

	else if (KeyBoardTRG(KB_D))
	{
		MainFrame->ChangeScene(new DebugRoom, true);
	}
#endif

	return true;
}
//
//=============================================================================================


//=============================================================================================
//		描			画
void sceneModeSelect::Render()
{
	tdnView::Clear();

	// 背景
	m_pImages[(int)IMAGE::BACK]->Render(0, 0);

	// 上のフレーム
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Render(0, 0);

	// モードセレクトの文字
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 0, 400, 64);

	// キーボードのガイド
	m_pKeyboardGuide->Render();

	// 7つのモードの画像
	for (int i = 1; i < (int)SELECT_MODE::MAX; i++)
	{
		// 選択モードだったら拡大
		m_pAnimImages[i]->SetScale((i == m_ModeCursor) ? 1.0f : .8f);
		m_pAnimImages[i]->Render(50+ i * 160, 110, 167, 355, i * 256, 0, 167, 355);
	}

	// モードの見出し
	m_pAnimImages[(int)ANIM_IMAGE::MODE_STR]->Render(250, 460, 810, 100, 0, m_ModeCursor * 100, 810, 100);

	// モード説明の枠
	m_pAnimImages[(int)ANIM_IMAGE::FRAME]->Render(160, 580);

	// モード説明文
	m_pAnimImages[(int)ANIM_IMAGE::DESCRIPTION]->Render(160, 580, 992, 96, 0, m_ModeCursor * 96, 992, 96);

	// フェード
	Fade::Render();
}
//
//=============================================================================================