#include "TDNLIB.h"
#include "system/Framework.h"
#include "sceneMusicSelect.h"
#include "scenePlaying.h"
#include "sceneModeSelect.h"
#include "SceneEdit.h"
#include "../Fade/Fade.h"
#include "../Data/PlayerData.h"
#include "../Data/SelectMusicData.h"
#include "../Data/MusicDataBase.h"
#include "../Music/EnumString.h"
#include "../Data/CustomizeData.h"
#include "../Sound/SoundManager.h"
#include "../Mode/ModeManager.h"
#include "../Number/Number.h"
#include "../Input/InputMIDI.h"
#include "../Sort/Sort.h"
#include "TDN_Movie.h"

#define EP_MUSIC_LOOP for (int i = 0; i < MUSIC_DATA_BASE->GetNumEPContents(); i++)

static const int NO_SELECT_MUSIC(-1);
int sceneMusicSelect::Mode::Base::m_iSelectMusic(NO_SELECT_MUSIC);

//=============================================================================================
//		初	期	化	と	開	放
bool sceneMusicSelect::Initialize()
{
	// フェード初期化
	Fade::Set(Fade::MODE::FADE_IN, 5, 0x00ffffff);

	// 時間初期化
	m_LimitTime = (ModeMgr->GetMode() == PLAY_MODE::STANDARD) ? 99 * 60 : NOLILMIT_TIME;

	// シーンに入ってからの操作受付までの時間
	m_WaitTimer = 28;

	// 選択難易度初期化
	m_ipSelectDifficulty = SelectManager::GetInstance()->GetSelectDifAddress();
	m_DifficultyFrameX = 39.0f + *m_ipSelectDifficulty * 172;

	m_fLoadPercentage = .05f;	// ロード割合

	// 動画初期化
	m_pBackMovie = new tdnMovie("DATA/movie/MusicSelect.wmv");
	m_pBackMovie->Play();

	// 画像初期化
	//m_pImages[IMAGE::BACK] = std::make_unique<tdn2DObj>("DATA/Image/select_back.png");
	m_pImages[IMAGE::BACK]			= std::make_unique<tdn2DObj>(m_pBackMovie->GetTexture());
	m_fLoadPercentage = .1f;	// ロード割合
	m_pImages[IMAGE::MUSIC_FRAME]	= std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/music_frame.png");
	m_pImages[IMAGE::NUMBER]		= std::make_unique<tdn2DObj>("DATA/Number/number.png");
	m_pImages[IMAGE::NUMBER_L]		= std::make_unique<tdn2DObj>("DATA/Number/numberL.png");
	m_fLoadPercentage = .15f;	// ロード割合
	m_pImages[IMAGE::LAMP]			= std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/lamp.png");
	m_pImages[IMAGE::BPM_FONT]		= std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/bpm_font.png");
	m_fLoadPercentage = .2f;	// ロード割合
	m_pImages[IMAGE::OPTION]		= std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/option.png");
	m_pImages[IMAGE::OPTION_CURSOR] = std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/option_cursor.png");
	m_pImages[IMAGE::OPTION_CURSOR]->SetARGB(0x80ffffff);
	m_pImages[IMAGE::OCTAVE_BACK]	= std::make_unique<tdn2DObj>("DATA/UI/MusicSelect/octave.png");
	m_fLoadPercentage = .25f;	// ロード割合

	// アニメーション画像初期化
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI] = std::make_unique<tdn2DAnim>("DATA/UI/Other/midashi.png");
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->OrderMoveAppeared(16, -8, 12);
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Action(25);
	m_fLoadPercentage = .3f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::STAGE] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/stage.png");
	m_pAnimImages[(int)ANIM_IMAGE::STAGE]->OrderMoveAppeared(16, 760, 35);
	m_pAnimImages[(int)ANIM_IMAGE::STAGE]->Action(25);
	m_fLoadPercentage = .35f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/up_frame.png");
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->OrderMoveAppeared(30, 0, -64);
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Action(0);
	m_fLoadPercentage = .45f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::DIF_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/difficulty.png");
	m_pAnimImages[(int)ANIM_IMAGE::DIF_FRAME]->OrderStretch(15, 1, .0f);
	m_pAnimImages[(int)ANIM_IMAGE::DIF_FRAME]->Action(20);
	m_fLoadPercentage = .5f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_DIFFICULTY] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/difficulty_select.png");
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_DIFFICULTY]->OrderMoveAppeared(5, (int)m_DifficultyFrameX, 433 + 180);
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_DIFFICULTY]->Action(35);
	m_fLoadPercentage = .55f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/folder.png");
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->OrderMoveAppeared(12, 70 - 32, 200);
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->Action(25);
	m_fLoadPercentage = .6f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/folder_up.png");
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->OrderMoveAppeared(12, 360 + 32, 200 - 40);
	m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->Action(25);
	m_fLoadPercentage = .65f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::RANK] = std::make_unique<tdn2DAnim>("DATA/UI/Other/rank.png");
	m_pAnimImages[(int)ANIM_IMAGE::RANK]->OrderJump(10, 1.0f, .2f);
	m_pAnimImages[(int)ANIM_IMAGE::RANK]->Action(40);
	m_fLoadPercentage = .7f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/score.png");
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->OrderStretch(15, 1, .0f);
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->Action(25);
	m_fLoadPercentage = .75f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::CLEAR_TEXT] = std::make_unique<tdn2DAnim>("DATA/UI/MusicSelect/clear.png");
	m_pAnimImages[(int)ANIM_IMAGE::CLEAR_TEXT]->OrderMoveAppeared(15, 250, 603 + 10);
	m_pAnimImages[(int)ANIM_IMAGE::CLEAR_TEXT]->Action(25);
	m_pAnimImages[(int)ANIM_IMAGE::OCTAVE] = std::make_unique<tdn2DAnim>(m_pImages[IMAGE::OCTAVE_BACK]->GetTexture());
	m_pAnimImages[(int)ANIM_IMAGE::OCTAVE]->OrderAlpha(16, 0, 1);

	// ソートするクラス初期化
	m_pSort[(int)FOLDER_TYPE::BEGINNER]				 = std::make_unique<MusicSort::Beginner>();
	m_pSort[(int)FOLDER_TYPE::ALL]					 = std::make_unique<MusicSort::All>();
	m_pSort[(int)FOLDER_TYPE::ELECTRONIC_PIANISM]	 = std::make_unique<MusicSort::ElectronicPianism>();
	m_pSort[(int)FOLDER_TYPE::LEVEL1_5]				 = std::make_unique<MusicSort::Level1_5>();
	m_pSort[(int)FOLDER_TYPE::LEVEL6_10]			 = std::make_unique<MusicSort::Level6_10>();
	m_pSort[(int)FOLDER_TYPE::LEVEL11_15]			 = std::make_unique<MusicSort::Level11_15>();
	m_pSort[(int)FOLDER_TYPE::LEVEL16_20]			 = std::make_unique<MusicSort::Level16_20>();
	m_pSort[(int)FOLDER_TYPE::LEVEL21_25]			 = std::make_unique<MusicSort::Level21_25>();
	m_pSort[(int)FOLDER_TYPE::LEVEL26_30]			 = std::make_unique<MusicSort::Level26_30>();
	m_pSort[(int)FOLDER_TYPE::LEVEL31]				 = std::make_unique<MusicSort::Level31>();
	m_pSort[(int)FOLDER_TYPE::ORIGINAL]				 = std::make_unique<MusicSort::Original>();
	m_pSort[(int)FOLDER_TYPE::GAME]					 = std::make_unique<MusicSort::Game>();
	m_pSort[(int)FOLDER_TYPE::CLASSIC]				 = std::make_unique<MusicSort::Classic>();
	m_pSort[(int)FOLDER_TYPE::VOCALOID]				 = std::make_unique<MusicSort::Vocaloid>();
	m_pSort[(int)FOLDER_TYPE::POPS]					 = std::make_unique<MusicSort::Pops>();
	m_pSort[(int)FOLDER_TYPE::OTHER]				 = std::make_unique<MusicSort::Other>();

	m_fLoadPercentage = .8f;	// ロード割合

	// マップ初期化
	m_FolderTypeMap[FOLDER_TYPE::BEGINNER]			 = "BEGINNER";
	m_FolderTypeMap[FOLDER_TYPE::ALL]				 = "ALL-MUSIC";
	m_FolderTypeMap[FOLDER_TYPE::ELECTRONIC_PIANISM] = "ELECTRONIC PIANISM";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL1_5]			 = "LEVEL1-5";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL6_10]			 = "LEVEL6-10";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL11_15]		 = "LEVEL11-15";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL16_20]		 = "LEVEL16-20";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL21_25]		 = "LEVEL21-25";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL26_30]		 = "LEVEL26-30";
	m_FolderTypeMap[FOLDER_TYPE::LEVEL31]			 = "LEVEL31";
	m_FolderTypeMap[FOLDER_TYPE::ORIGINAL]			 = "ORIGINAL";
	m_FolderTypeMap[FOLDER_TYPE::GAME]				 = "GAME";
	m_FolderTypeMap[FOLDER_TYPE::CLASSIC]			 = "CLASSIC";
	m_FolderTypeMap[FOLDER_TYPE::VOCALOID]			 = "VOCALOID";
	m_FolderTypeMap[FOLDER_TYPE::POPS]				 = "POPS";
	m_FolderTypeMap[FOLDER_TYPE::OTHER]				 = "OTHER";

	m_fLoadPercentage = .9f;	// ロード割合

	// 選択フォルダーカーソル初期化
	m_ipSelectFolder = SelectManager::GetInstance()->GetSelectFolderAddress();

	// フォルダー初期化
	m_pSelectFolderInfo = SelectManager::GetInstance()->GetSelectFolderInfoAddress();

	// 決定フラグオフ
	m_bDecision = false;

	// おぷしょんOFF
	m_bOption = false;
	m_CurrentOptionX = m_NextOptionX = (float)tdnSystem::GetScreenSize().right;

	// ランプアニメーション
	m_LampAnimFrame = m_LampAnimPanel = 0;

	// 曲名テキスト画像
	//m_pMusicTextImages = std::make_unique<tdn2DObj>*[MUSIC_DATA_BASE->GetNumEPContents()];

	//m_PathList = new std::string[MUSIC_DATA_BASE->GetNumEPContents()];
	m_fLoadPercentage = .95f;	// ロード割合
	EP_MUSIC_LOOP
	{
		//for (int folder = 0; folder < MUSIC_DATA_BASE->GetNumEPContents(); folder++)
		{
			const int id(MUSIC_DATA_BASE->GetEPContentsData(i)->byID);
			//// フォルダー名からID取得
			//std::string path = "DATA/Musics/EP/No_" + id;
				//MUSIC_DATA_BASE->GetEPDirectory()->FolderNames[folder];

			//char *ctx;// strtok_sの第3引数に使用するのだが、内部的に使用するので深く考えないらしい
			//
			//// No_1の.で区切って、右の数字をIDにする
			//strtok_s((char*)tdnFile::GetFileName((char*)path.c_str()).c_str(), "_", &ctx);
			//int folderID = (BYTE)atoi(strtok_s(0, "\0", &ctx));

			// フォルダーIDが同じなら
			//if (folderID == id)
			{
				// 試聴データ設定
				//m_pAuditionPlayer->Set(MUSIC_DATA_BASE->GetEPContentsData(i)->id, (char*)(path + "/Audition.owd").c_str());
				//m_PathList[id] = "DATA/Musics/EP/No_" + std::to_string(id);
				//
				//// 曲名画像設定
				//m_pMusicTextImages[id] = std::make_unique<tdn2DObj>((char*)(m_PathList[id] + "/title.png").c_str());
				//break;
				m_MusicInfoMap[id] = new _MusicInfo(id);
			}
		}
	}

	m_fLoadPercentage = 1.0f;	// ロード割合

	// 選曲BGM流す
	m_pMusicPlayer = std::make_unique<tdnSoundBGM>();
	m_pMusicPlayer->Set(MUSIC_PLAYER::SELECTION, (char*)(CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::SELECT_MUSIC) + "/music.wav").c_str());
	m_pMusicPlayer->Set(MUSIC_PLAYER::DECIDE, (char*)(CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::SELECT_MUSIC) + "/decide.wav").c_str());

	// 質問枠
	m_pQuestionWindow = std::make_unique<QuestionWindow>(QuestionWindow::ANSWER_TYPE::YES_NO);

	// ガイド
	m_pKeyboardGuide = std::make_unique<KeyboardGuide>(Vector2(26, 580), 3, 0.1f, 0.65f, 1.0f, 60);

	// モード初期化
	if (SelectManager::GetInstance()->isEmpty())
	{
		m_pMusicPlayer->Play(MUSIC_PLAYER::SELECTION);
		SelectManager::GetInstance()->ChangeMode(MODE::SELECT_FOLDER, this);
	}
	else
	{
		SelectManager::GetInstance()->ChangeMode(MODE::SELECT_MUSIC, this);
	}

	// 試聴データ全部読み込み
	//m_pAuditionPlayer = new tdnSoundBGM;

	return true;
}

sceneMusicSelect::~sceneMusicSelect()
{
	// モードポインタ解放
	//delete m_pMode;
	SelectManager::GetInstance()->ModeRelease();

	// 曲のパス情報
	for (auto it : m_MusicInfoMap)
	{
		// めちゃくちゃだけど、とりあえずこれでメモリリークは防げる
		//m_MusicInfoMap.erase(it.first);
		delete m_MusicInfoMap[it.first];
	}


	// 同ｇ
	delete m_pBackMovie;
}

void sceneMusicSelect::ClearFolderInfo()
{
	for (auto it : *m_pSelectFolderInfo)
		delete it;
	m_pSelectFolderInfo->clear();
}

//=============================================================================================


//=============================================================================================
//		更			新
bool sceneMusicSelect::Update()
{
	// フェード更新
	Fade::Update();

	// 選曲後の更新
	if (m_bDecision)
	{
		// モード実行
		SelectManager::GetInstance()->ModeUpdate();
	}

	// 選曲しているときの更新
	else
	{
		// アニメ画像更新
		FOR((int)ANIM_IMAGE::MAX) m_pAnimImages[i]->Update();
		for (auto it : m_MusicInfoMap) m_MusicInfoMap[it.first]->UpdateAnim();

		// 制限時間更新(フリーじゃなかったら)
		if (m_LimitTime != NOLILMIT_TIME)m_LimitTime = max(m_LimitTime - 1, 0);

		// ランプアニメ更新
		if (++m_LampAnimFrame > 2)
		{
			m_LampAnimFrame = 0;
			if (++m_LampAnimPanel > 3) m_LampAnimPanel = 0;
		}

		// ガイド更新
		m_pKeyboardGuide->Update();

		// 動画更新
		m_pBackMovie->Update();

		// BGM更新
		m_pMusicPlayer->Update();

		// 操作受付までの待機時間が残ってたら
		if (m_WaitTimer > 0)
		{
			// 下記の処理(主にプレイヤーからの操作)をスキップする
			m_WaitTimer--;
			return true;
		}

		// 選択難易度枠の補間処理
		const float NextDifficultyX(55.0f + *m_ipSelectDifficulty * 172);
		m_DifficultyFrameX = m_DifficultyFrameX * .8f + NextDifficultyX * .2f;

		// オプション座標補間
		m_CurrentOptionX = m_CurrentOptionX * .75f + m_NextOptionX * .25f;

		// オプション開いてる
		if (m_bOption)
		{
			// [ド]ハイスピ
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::C))
			{
				g_pSE->Play("難易度変更");
				if ((++PlayerDataMgr->m_PlayerOption.HiSpeed) > 10) PlayerDataMgr->m_PlayerOption.HiSpeed = 0;
			}

			// [レ]ゲージオプション
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::D))
			{
				g_pSE->Play("難易度変更");
				if (++PlayerDataMgr->m_PlayerOption.iGaugeOption > (int)GAUGE_OPTION::HAZARD) PlayerDataMgr->m_PlayerOption.iGaugeOption = 0;
			}

			// [C]サドゥン
			//if (KeyBoardTRG(KB_C)) PlayerDataMgr->m_PlayerOption.bSudden ^= 0x1;

			// [ミ]DJ_AUTO
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::E))
			{
				g_pSE->Play("難易度変更");
				PlayerDataMgr->m_PlayerOption.bAutoPlay ^= 0x1;
			}

			// 閉じる
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::START))
			{
				m_NextOptionX = 1300;
				m_bOption = false;
				g_pSE->Play("オプション閉じる");
				Fade::Set(Fade::MODE::FADE_IN, 8, 0, Fade::m_byAlpha, 0x00000000);
			}
		}

		// オプション開いてない
		else
		{
			// [2]難易度変更
			if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::SELECT))
			{
				g_pSE->Play("難易度変更");
				//m_pMode->ShiftDifficulty();
				SelectManager::GetInstance()->ShiftDifficulty();
			}

			// モード実行
			//m_pMode->Update();
			SelectManager::GetInstance()->ModeUpdate();
		}
	}

	return true;
}

void sceneMusicSelect::UpdateOption()
{
	// スタートキーでオプションを開く
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::START))
	{
		m_NextOptionX = 310;
		m_bOption = true;
		g_pSE->Play("オプション開く");
		Fade::Set(Fade::MODE::FADE_OUT, 8, 128, Fade::m_byAlpha, 0x00000000);
	}
}
//
//=============================================================================================


//=============================================================================================
//		描			画
void sceneMusicSelect::Render()
{
	//
	tdnView::Clear();

	// 選曲後の描画
	if (m_bDecision)
	{
		// モード描画実行
		SelectManager::GetInstance()->ModeRender();
	}

	// 選曲しているときの描画
	else
	{
		// 背景
		m_pImages[IMAGE::BACK]->Render(0, 0);

		// ガイド
		RenderGuide();

		// 難易度描画
		m_pAnimImages[(int)ANIM_IMAGE::DIF_FRAME]->Render(48, 400);
		//tdnText::Draw(320, 640, 0xffffffff, "%s", ENUM_STRING.m_DifficultyMap[(DIFFICULTY)m_ipSelectDifficulty]);

		/* 難易度枠 */
		m_pAnimImages[(int)ANIM_IMAGE::SELECT_DIFFICULTY]->SetARGB(DifficultyColor[*m_ipSelectDifficulty]);
		m_pAnimImages[(int)ANIM_IMAGE::SELECT_DIFFICULTY]->Render((int)m_DifficultyFrameX, 433);

		// スコア枠描画
		m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->Render(248, 540);
		if (m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->GetAction()->IsEnd())tdnText::Draw(255, 573, 0xffffffff, "SCORE");

		// モード描画実行
		//m_pMode->Render();
		SelectManager::GetInstance()->ModeRender();

		// 上のフレーム
		m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Render(0, 0);

		// MUSIC SELECT
		m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 64, 400, 64);

		// 制限時間
		m_pImages[IMAGE::NUMBER_L]->SetARGB(0xffffffff);
		NumberRender(m_pImages[IMAGE::NUMBER_L].get(), 1196, 52, 32, 42, (m_LimitTime != NOLILMIT_TIME) ? m_LimitTime / 60 : 99, 2, true);

		// **Stage
		m_pAnimImages[(int)ANIM_IMAGE::STAGE]->Render(760, 30, 280, 64, 0, (ModeMgr->GetMode() == PLAY_MODE::STANDARD) ? ModeMgr->GetStep() * 64 : 256, 280, 64);

		Fade::Render();

		// 質問ウィンドウの描画
		m_pQuestionWindow->Render();

		// オプション開いてたら
		if (m_CurrentOptionX < tdnSystem::GetScreenSize().right)
			RenderOption();	// オプション描画
	}
}

void sceneMusicSelect::RenderOption()
{
	const int OFFSET_X((int)m_CurrentOptionX);
	static const int OFFSET_Y(50);

	m_pImages[IMAGE::OPTION]->Render(OFFSET_X, OFFSET_Y);

	// ハイスピ
	int iHiSpeed = (int)((PlayerDataMgr->m_PlayerOption.HiSpeed * HI_SPEED_WIDTH) * 100);
	m_pImages[IMAGE::NUMBER]->SetARGB(0xffffffff);
	m_pImages[IMAGE::NUMBER]->Render(OFFSET_X + 135, OFFSET_Y + 352, 22, 28, (iHiSpeed % 10) * 24, 0, 24, 32);
	iHiSpeed /= 10;
	m_pImages[IMAGE::NUMBER]->Render(OFFSET_X + 110, OFFSET_Y + 352, 22, 28, (iHiSpeed % 10) * 24, 0, 24, 32);
	iHiSpeed /= 10;
	m_pImages[IMAGE::NUMBER]->Render(OFFSET_X + 75, OFFSET_Y + 352, 22, 28, (iHiSpeed % 10) * 24, 0, 24, 32);
	m_pImages[IMAGE::NUMBER]->Render(OFFSET_X + 100, OFFSET_Y + 350, 16, 32, 240, 0, 16, 32);
	//m_pImages[IMAGE::OPTION_CURSOR]->Render(OFFSET_X + 49, OFFSET_Y + 335, RS::ADD);

	// ゲージ
	m_pImages[IMAGE::OPTION_CURSOR]->Render(OFFSET_X + 196, OFFSET_Y + 335 + PlayerDataMgr->m_PlayerOption.iGaugeOption * 35, RS::ADD);

	// AUTO
	m_pImages[IMAGE::OPTION_CURSOR]->Render(OFFSET_X + 343, OFFSET_Y + 335 + (PlayerDataMgr->m_PlayerOption.bAutoPlay ? 35 : 0), RS::ADD);

#ifdef MIDI_INPUT_ON
#else
	
#endif
}

void sceneMusicSelect::RenderGuide()
{
	m_pKeyboardGuide->Render();

	//static float l_fAlphaSinCurve(0);
	//static bool l_bSwitch(false);
	//static const float l_cfAlphaSpeed(0.1f);
	//static const float l_cfSwicthAngle(PI * 6);	// 3回点滅したら切り替わる
	//static const int l_ciPosX(32), l_ciPosY(580);

	//m_pImages[IMAGE::GUIDE]->SetAlpha(1.0f);
	//m_pImages[IMAGE::GUIDE]->Render(l_ciPosX, l_ciPosY, 256, 128, 0, 0, 256, 128);

	//// 選択点滅中
	//if (l_bSwitch)
	//{
	//	// 選択
	//	m_pImages[IMAGE::GUIDE]->SetAlpha(Math::Blend((sinf(l_fAlphaSinCurve) + 1) * 0.5f, 0.6f, 1.0f));	// -1～1→0～1
	//	m_pImages[IMAGE::GUIDE]->Render(l_ciPosX, l_ciPosY, 256, 128, 0, 128, 256, 128);
	//	m_pImages[IMAGE::GUIDE]->SetAlpha(0.7f);

	//	// 決定
	//	m_pImages[IMAGE::GUIDE]->Render(l_ciPosX, l_ciPosY, 256, 128, 0, 256, 256, 128);

	//	if ((l_fAlphaSinCurve += l_cfAlphaSpeed) > l_cfSwicthAngle)
	//	{
	//		l_fAlphaSinCurve = 0;
	//		l_bSwitch = !l_bSwitch;
	//	}
	//}

	//// 決定点滅中
	//else
	//{
	//	// 決定
	//	m_pImages[IMAGE::GUIDE]->SetAlpha(Math::Blend((sinf(l_fAlphaSinCurve) + 1) * 0.5f, 0.6f, 1.0f));	// -1～1→0～1
	//	m_pImages[IMAGE::GUIDE]->Render(l_ciPosX, l_ciPosY, 256, 128, 0, 256, 256, 128);
	//	m_pImages[IMAGE::GUIDE]->SetAlpha(0.7f);

	//	// 選択
	//	m_pImages[IMAGE::GUIDE]->Render(l_ciPosX, l_ciPosY, 256, 128, 0, 128, 256, 128);

	//	if ((l_fAlphaSinCurve += l_cfAlphaSpeed) > l_cfSwicthAngle)
	//	{
	//		l_fAlphaSinCurve = 0;
	//		l_bSwitch = !l_bSwitch;
	//	}
	//}
}

//
//=============================================================================================


//=============================================================================================
//		フォルダー選択モード
void sceneMusicSelect::Mode::FolderSelect::Update()
{
	static bool bEnd(false);

	// 戻るフェード中
	if (bEnd)
	{
		// フェード終わったら帰る
		if (Fade::m_eMode == Fade::MODE::FADE_NONE)
		{
			bEnd = false;
			MainFrame->ChangeScene(new sceneModeSelect, true);
		}
		return;
	}

	// 質問ウィンドウ出現中だったら
	if (pMusicSelect->m_pQuestionWindow->isAction())
	{
		// 質問に答え終わって、はいを選択していたらフェードアウトして終了
		if (pMusicSelect->m_pQuestionWindow->Update() && pMusicSelect->m_pQuestionWindow->GetAnswer() == TRUE)
		{
			// SE
			g_pSE->Play("戻る");

			// フェードアウト設定
			Fade::Set(Fade::MODE::FADE_OUT, 4);
			bEnd = true;
		}

		return;
	}


	// フォルダーの動き更新
	FOR((int)FOLDER_TYPE::MAX) m_FolderPositions[i].Update();

	bool bCursored(false);
	// 上下でフォルダー選択
	if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
	{
		if (--(*pMusicSelect->m_ipSelectFolder) < 0) (*pMusicSelect->m_ipSelectFolder) = (int)FOLDER_TYPE::MAX - 1;
		bCursored = true;
	}
	else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
	{
		if (++(*pMusicSelect->m_ipSelectFolder) >= (int)FOLDER_TYPE::MAX) (*pMusicSelect->m_ipSelectFolder) = 0;
		bCursored = true;
	}

	if (bCursored)
	{
		g_pSE->Play("カーソル");
		SetNextFolderPos(*pMusicSelect->m_ipSelectFolder, false);	// 次のフォルダー座標の指定

		// フォルダー説明とかのアニメーションを発動
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->Action();
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->Action();
	}

	// エンターキーでフォルダーを開く
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		// フォルダーSE
		g_pSE->Play("決定2");

		pMusicSelect->m_pSort[*pMusicSelect->m_ipSelectFolder]->Funk(pMusicSelect);
		pMusicSelect->Push(nullptr, DIFFICULTY::MAX, true);	// フォルダー抜ける用のやつを追加

		//pMusicSelect->ChangeMode(MODE::SELECT_MUSIC);	// 曲選択ステートへ
		SelectManager::GetInstance()->ChangeMode(MODE::SELECT_MUSIC, pMusicSelect);
	}

	// スペースキーで抜ける
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK) && ModeMgr->GetMode() == PLAY_MODE::FREE)
	{
		pMusicSelect->m_pQuestionWindow->Action("Infomation", "メニュー画面に戻りますか？");
	}

	else 		// オプション更新
		pMusicSelect->UpdateOption();
}

void sceneMusicSelect::Mode::FolderSelect::ShiftDifficulty()
{
	// 難易度カーソル
	if (++(*pMusicSelect->m_ipSelectDifficulty) >= (int)DIFFICULTY::MAX) (*pMusicSelect->m_ipSelectDifficulty) = 0;
}

void sceneMusicSelect::Mode::FolderSelect::Render()
{
	// フォルダーのタグ分けの画像的な
	{
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->Render(70, 200, 560, 80, 0, 80 * *pMusicSelect->m_ipSelectFolder, 560, 80);
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->Render(360, 200 - 40, 256, 40, 0, pMusicSelect->GetFolderHosokuType() * 40, 256, 40);
	}

	// フォルダー名
	{
	/*
		static const int OFFSET_X(760);	// 名前開始地点のX座標
		static const int OFFSET_Y(345);	// 名前開始地点のY座標
	*/
		// 行の数
		const int MAX_COLUMN(15);										// 行の最大数
		const int column(min((int)FOLDER_TYPE::MAX, MAX_COLUMN));		// 行の数
		const int ColumnCenter((int)(column * .45f));					// 行の数から真ん中を選出

		// 行の描画
		FOR(column)
		{
			// 自分のカーソルでの前後を取得
			int index(*pMusicSelect->m_ipSelectFolder + (i - ColumnCenter));

			// 下に行きすぎたら(画面では上)
			if (index < 0)
			{
				// 全アルファベット分を足すことによって、過剰分をずらせる
				index = (int)FOLDER_TYPE::MAX + index;
			}

			// 上に行きすぎたら(画面では下)
			else if (index >= (int)FOLDER_TYPE::MAX)
			{
				// 全アルファベット分を足すことによって、過剰分をずらせる
				index -= ((int)FOLDER_TYPE::MAX);
			}

			//pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetScale((index == *pMusicSelect->m_ipSelectFolder) ? 1.05f : 1.0f);
			pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->Render((int)m_FolderPositions[index].pos.x, (int)m_FolderPositions[index].pos.y, 520, 40, 0, 40, 520, 40);

			// 企業さんからいただいた意見を取り入れる。
			if (index == *pMusicSelect->m_ipSelectFolder)
			{
				static const float l_cfMinAlpha(0.0f);
				static const float l_cfMaxAlpha(0.5f);
				static float l_sfAlphaSinCurve(0);

				// 選択
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetAlpha(Math::Blend((sinf(l_sfAlphaSinCurve += 0.1f) + 1) * 0.5f, l_cfMinAlpha, l_cfMaxAlpha));	// -1～1→0～1
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->Render((int)m_FolderPositions[index].pos.x, (int)m_FolderPositions[index].pos.y, 520, 40, 0, 40, 520, 40, RS::ADD);
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetAlpha((BYTE)BYTE_MAX);
			}

			std::string sSpaceStr((index == (int)FOLDER_TYPE::BEGINNER) ? tdnFont::GetPictGraphStr(tdnFont::PICTOGRAPH::BEGINNER) : "　");
			tdnFont::RenderString(std::string(sSpaceStr + pMusicSelect->m_FolderTypeMap[(FOLDER_TYPE)index]).c_str(), 32, (int)m_FolderPositions[index].pos.x + 100, (int)m_FolderPositions[index].pos.y + 4, 0xffffffff, "メイリオ");
		}
	}
}
//
//=============================================================================================


//=============================================================================================
//		フォルダー開いて曲選択モード
sceneMusicSelect::Mode::MusicSelect::MusicSelect(sceneMusicSelect *me) : Base(me), 
m_AuditionDelayTime(0),
m_bAuditionPlay(false),
m_pStreamAuditionPlayer(nullptr),
m_iPrevOctave(-1)
{
	// 初回
	if (m_iSelectMusic == NO_SELECT_MUSIC)m_iSelectMusic = (int)pMusicSelect->m_pSelectFolderInfo->size() - 1;

	// すでに遊んでて、フォルダーの記録保存してたら
	else
	{
		const int id(pMusicSelect->GetSelectMusicID(m_iSelectMusic));

		// 曲のタイトルとアーティスト名の画像のアニメーションを発動させる
		pMusicSelect->m_MusicInfoMap[id]->pMusicTextAnim->Action(15);
		pMusicSelect->m_MusicInfoMap[id]->pMusicArtistAnim->Action(15);

		// オクターブアルファ
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::OCTAVE]->Action();

		// 選曲BGM停止
		pMusicSelect->m_pMusicPlayer->Pause(0);

		m_pStreamAuditionPlayer = g_pBGM->PlayStream((char*)(pMusicSelect->m_MusicInfoMap[pMusicSelect->GetSelectMusicID(m_iSelectMusic)]->path + "/Audition.ogg").c_str());
		m_bAuditionPlay = true;
	}

	SetNextMusicPos(m_iSelectMusic, true);
}
sceneMusicSelect::Mode::MusicSelect::~MusicSelect()
{
	//m_iSelectMusic = NO_SELECT_MUSIC;
	if (m_bAuditionPlay)
		m_pStreamAuditionPlayer->Stop(); 
}
void sceneMusicSelect::Mode::MusicSelect::Update()
{
	// 質問ウィンドウ出現中だったら
	if (pMusicSelect->m_pQuestionWindow->isAction())
	{
		// 質問に答え終わって、はいを選択していたら選曲する
		if (pMusicSelect->m_pQuestionWindow->Update() && pMusicSelect->m_pQuestionWindow->GetAnswer() == TRUE)
		{
			SelectManager::GetInstance()->ChangeMode(MODE::DECIDE_MUSIC, pMusicSelect);
		}

		return;
	}


	// 曲の枠の座標更新
	for (auto it : *pMusicSelect->m_pSelectFolderInfo) it->UpdatePotision();

	const bool bMusicCursor(!pMusicSelect->isExitFolder(m_iSelectMusic));
	bool bCursored(false);

	// 上下で曲選択
	if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
	{
		if (!pMusicSelect->isExitFolder(m_iSelectMusic))m_iPrevOctave = pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->iOctaveWidths[*pMusicSelect->m_ipSelectDifficulty];

		if (--m_iSelectMusic < 0) m_iSelectMusic = (int)pMusicSelect->m_pSelectFolderInfo->size() - 1;

		// 難易度更新(レベル別選択だと起こりやすい)
		else
		{
			*pMusicSelect->m_ipSelectDifficulty = (int)pMusicSelect->m_pSelectFolderInfo->at(m_iSelectMusic)->difficulty;
		}
		bCursored = true;
	}
	else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
	{
		if (!pMusicSelect->isExitFolder(m_iSelectMusic))m_iPrevOctave = pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->iOctaveWidths[*pMusicSelect->m_ipSelectDifficulty];

		if (++m_iSelectMusic > (int)pMusicSelect->m_pSelectFolderInfo->size() - 1)
		{
			m_iSelectMusic -= ((int)pMusicSelect->m_pSelectFolderInfo->size());
		}

		// 難易度更新(レベル別選択だと起こりやすい)
		if (m_iSelectMusic != (int)pMusicSelect->m_pSelectFolderInfo->size() - 1)
		{
			*pMusicSelect->m_ipSelectDifficulty = (int)pMusicSelect->m_pSelectFolderInfo->at(m_iSelectMusic)->difficulty;
		}
		bCursored = true;
	}

	// カーソルしてたら
	if (bCursored)
	{
		// 試聴曲流すディレイリセット
		m_AuditionDelayTime = 0;

		// 曲選んでたら
		if (bMusicCursor)
		{
			// 試聴曲停止
			if (m_bAuditionPlay)m_pStreamAuditionPlayer->Stop();
			m_bAuditionPlay = false;
		}
		else
		{
			// 選曲BGM停止
			pMusicSelect->m_pMusicPlayer->Pause(0);
		}

		// 曲たちの枠に次の座標を伝えてあげる
		SetNextMusicPos(m_iSelectMusic, false);

		// カーソルSE
		g_pSE->Play("カーソル");

		// 曲ではなくフォルダーを指していたら
		if (pMusicSelect->isExitFolder(m_iSelectMusic))
		{
			// フォルダー説明とかのアニメーションを発動
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->Action(4);
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->Action(4);
		}
		else
		{
			const int id(pMusicSelect->GetSelectMusicID(m_iSelectMusic));

			// 曲のタイトルとアーティスト名の画像のアニメーションを発動させる
			pMusicSelect->m_MusicInfoMap[id]->pMusicTextAnim->Action(4);
			pMusicSelect->m_MusicInfoMap[id]->pMusicArtistAnim->Action(4);

			// スコアランクのアニメーション
			//if (PlayerDataMgr->m_MusicRecords[id][*pMusicSelect->m_ipSelectDifficulty].ClearLamp != (int)CLEAR_LAMP::NO_PLAY)
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::RANK]->Action();
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::CLEAR_TEXT]->Action();

			// オクターブのアルファ
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::OCTAVE]->Action();
		}
	}

	// カーソル移動してからディレイを掛けてBGM関係
	if (++m_AuditionDelayTime > 30)
	{
		// 曲を選んでたら
		if (bMusicCursor)
		{
			// 試聴曲流してなかったら
			if (
				!m_bAuditionPlay
				//pMusicSelect->m_pAuditionPlayer->isPlay(pMusicSelect->m_pSelectFolderInfo[m_iSelectMusic]->info->id)
				)
			{
				//pMusicSelect->m_pAuditionPlayer->Play(pMusicSelect->m_pSelectFolderInfo[m_iSelectMusic]->info->id);
				m_pStreamAuditionPlayer = g_pBGM->PlayStream((char*)(pMusicSelect->m_MusicInfoMap[pMusicSelect->GetSelectMusicID(m_iSelectMusic)]->path + "/Audition.ogg").c_str());
				m_bAuditionPlay = true;
			}
		}
		else
		{
			// 普通の選曲BGM流す
			if (!pMusicSelect->m_pMusicPlayer->isPlay(0))
				pMusicSelect->m_pMusicPlayer->PauseOff(0, .025f);
		}
	}

	// スペースキーで持抜ける
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
	{
		g_pSE->Play("決定2");

		m_iSelectMusic = NO_SELECT_MUSIC;

		pMusicSelect->ClearFolderInfo();

		// 選曲画面の曲が止まってなかったら流す
		if (!pMusicSelect->m_pMusicPlayer->isPlay(0))
			pMusicSelect->m_pMusicPlayer->PauseOff(0, .025f);

		//pMusicSelect->ChangeMode(MODE::SELECT_FOLDER);	// フォルダー選択ステート
		SelectManager::GetInstance()->ChangeMode(MODE::SELECT_FOLDER, pMusicSelect);
	}

	// エンターキーで決定！
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		// フォルダーのサイズという事はフォルダーを抜けてください
		if (pMusicSelect->isExitFolder(m_iSelectMusic))
		{
			g_pSE->Play("決定2");

			m_iSelectMusic = NO_SELECT_MUSIC;

			pMusicSelect->ClearFolderInfo();

			// 選曲画面の曲が止まってなかったら流す
			if (!pMusicSelect->m_pMusicPlayer->isPlay(0))
				pMusicSelect->m_pMusicPlayer->PauseOff(0, .025f);

			//pMusicSelect->ChangeMode(MODE::SELECT_FOLDER);	// フォルダー選択ステート
			SelectManager::GetInstance()->ChangeMode(MODE::SELECT_FOLDER, pMusicSelect);
		}

		// 曲選択！
		else
		{
			// シングルトンクラスに選択した曲の情報を渡す
			SelectMusicData data(pMusicSelect->GetSelectMusicID(m_iSelectMusic), (DIFFICULTY)pMusicSelect->m_pSelectFolderInfo->at(m_iSelectMusic)->difficulty);
			if (!SelectMusicMgr->Set(data)) return;

			// オクターブ足りてるもしくはDJAUTOさんだったら
			if (InputMIDIMgr->GetNumOctave() >= SelectMusicMgr->Get()->omsInfo.OctaveWidth || PlayerDataMgr->m_PlayerOption.bAutoPlay)
			{
				// 決定モードに切り替え
				//pMusicSelect->ChangeMode(MODE::DECIDE_MUSIC);
				SelectManager::GetInstance()->ChangeMode(MODE::DECIDE_MUSIC, pMusicSelect);
			}

			// 足りてなかったら、警告する(遊べるのは遊べる)
			else pMusicSelect->m_pQuestionWindow->Action("Caution", "必要なオクターブ数が足りません。\n演奏できない部分がありますが、プレイしますか？");
		}
	}

	else 		// オプション更新
		pMusicSelect->UpdateOption();
}

void sceneMusicSelect::Mode::MusicSelect::ShiftDifficulty()
{
	auto Shift = [](int *p){if (++(*p) >= (int)DIFFICULTY::MAX) *p = 0; };

	//// レベル関係のフォルダーは、選択してるやつだけを変更
	//if (m_ipSelectFolder >= (int)FOLDER_TYPE::LEVEL1_5 && m_ipSelectFolder <= (int)FOLDER_TYPE::LEVEL31)
	//{
	//	// 戻るボタンでなければ
	//	if (m_iSelectMusic < (int)m_pSelectFolderInfo->size()) Shift(&m_pSelectFolderInfo[m_iSelectMusic]->difficulty);
	//}
	//
	//// フォルダー内全部変更
	//else
	for (auto it : *pMusicSelect->m_pSelectFolderInfo) Shift(&it->difficulty);

	// 自分のも変更？
	Shift(pMusicSelect->m_ipSelectDifficulty);
}

void sceneMusicSelect::Mode::MusicSelect::Render()
{
	// 左半分
	{
		// 曲を選んでたら
		if (!pMusicSelect->isExitFolder(m_iSelectMusic))
		{
			const int id(pMusicSelect->GetSelectMusicID(m_iSelectMusic));
			const int iOctaveWidth(pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->iOctaveWidths[*pMusicSelect->m_ipSelectDifficulty]);

			//static int x(0), y(0);
			//if (KeyBoardTRG(KB_LEFT)) x -= 10;
			//if (KeyBoardTRG(KB_RIGHT)) x += 10;
			//if (KeyBoardTRG(KB_UP)) y -= 10;
			//if (KeyBoardTRG(KB_DOWN)) y += 10;

			// オクターブ幅描画
			if (iOctaveWidth > 0) tdnFont::RenderString(32, 100, 314, 0xffffffff, "演奏オクターブ: %d", iOctaveWidth);
			pMusicSelect->m_pImages[IMAGE::OCTAVE_BACK]->Render(20, 348, 320, 32, 0, m_iPrevOctave * 32, 320, 32);
			pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::OCTAVE]->Render(20, 348, 320, 32, 0, iOctaveWidth * 32, 320, 32);

			// 曲名とアーティスト名描画
			RenderMusicTitle(id);

			// BPM描画
			RenderBPM();

			// スコア枠の中の描画
			if (pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->GetAction()->IsEnd())
			{
				// スコア描画
				RenderScore(id);
			}
			else
			{
				// フォルダー名
				pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_TEXT]->Render(70, 200, 560, 80, 0, 80 * *pMusicSelect->m_ipSelectFolder, 560, 80);
				pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::FOLDER_HOSOKU]->Render(360, 200 - 40, 256, 40, 0, pMusicSelect->GetFolderHosokuType() * 40, 256, 40);
			}
		}

		// 右半分
		{
			// 右半分の曲リストの描画
			RenderMusicList();
		}
	}
}
void sceneMusicSelect::Mode::MusicSelect::RenderMusicTitle(int id)
{
	// 曲名
	pMusicSelect->m_MusicInfoMap[id]->pMusicTextAnim->Render(32, 200, 700, 90, 0, 0, 700, 90);

	// アーティスト名
	pMusicSelect->m_MusicInfoMap[id]->pMusicArtistAnim->Render(150, 200 + 96, 700, 38, 0, 90, 700, 38);
}

void sceneMusicSelect::Mode::MusicSelect::RenderBPM()
{
	// 同じ処理を2回書くはめになったので、ラムダで一元化
	auto RenderNumberBPM = [](tdn2DObj *image, int val, const Vector2 &pos)
	{
		// 小数点第一
		image->Render((int)pos.x, (int)pos.y, 24, 32, (val % 10) * 24, 0, 24, 32);
		val /= 10;

		// .
		image->Render((int)pos.x - 12, (int)pos.y, 16, 32, 240, 0, 16, 32);

		// 残り
		for (int i = 0; i < 3; i++)
		{
			image->Render((int)pos.x - 36 - (i * 24), (int)pos.y, 24, 32, (val % 10) * 24, 0, 24, 32);
			val /= 10;
		}
	};

	// BPMの数字補間用
	static float HokanMaxBPM(150), HokanMinBPM(150);

	// 小数点ある程度切り捨てしたBPM
	const int MinBPM((int)(pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->fMinBPM * 10)), MaxBPM((int)(pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->fMaxBPM * 10));
	if (MinBPM == MaxBPM)
	{
		// BPM補間処理
		HokanMinBPM = (abs(HokanMinBPM - MinBPM) < 1.0f) ? MinBPM : HokanMinBPM * .65f + MinBPM * .35f;	// 要は129.9で止まったりするのでそれの対策

		// BPM描画
		pMusicSelect->m_pImages[IMAGE::BPM_FONT]->Render(500, 200 + 150, 86, 32, 24, 32, 86, 32);
		RenderNumberBPM(pMusicSelect->m_pImages[IMAGE::BPM_FONT].get(), (int)HokanMinBPM, Vector2(680, (int)200 + 150));
	}
	else
	{
		// BPM補間処理
		HokanMaxBPM = (abs(HokanMaxBPM - MaxBPM) < 1.0f) ? MaxBPM : HokanMaxBPM * .65f + MaxBPM * .35f;	// 要は129.9で止まったりするのでそれの対策
		HokanMinBPM = (abs(HokanMinBPM - MinBPM) < 1.0f) ? MinBPM : HokanMinBPM * .65f + MinBPM * .35f;	// 要は129.9で止まったりするのでそれの対策

		// BPM描画
		pMusicSelect->m_pImages[IMAGE::BPM_FONT]->Render(360, 200 + 150, 86, 32, 24, 32, 86, 32);
		RenderNumberBPM(pMusicSelect->m_pImages[IMAGE::BPM_FONT].get(), (int)HokanMaxBPM, Vector2(540, (int)200 + 150));
		pMusicSelect->m_pImages[IMAGE::BPM_FONT]->Render(570, 200 + 150, 24, 32, 0, 32, 24, 32);
		RenderNumberBPM(pMusicSelect->m_pImages[IMAGE::BPM_FONT].get(), (int)HokanMinBPM, Vector2(680, (int)200 + 150));
	}
}

void sceneMusicSelect::Mode::MusicSelect::RenderScore(int id)
{
	// 難易度枠の中のレベル
	FOR((int)DIFFICULTY::MAX)
	{
		pMusicSelect->m_pImages[IMAGE::NUMBER_L]->SetARGB(DifficultyColor[i]);
		NumberRender(pMusicSelect->m_pImages[IMAGE::NUMBER_L].get(), 178 + i * 172, 445, 32, 42, pMusicSelect->GetSelectMusicInfo(m_iSelectMusic)->level[i], 2, true);
	}
	/* レコード枠 */
	const int score(PlayerDataMgr->GetMusicRecord(id, *pMusicSelect->m_ipSelectDifficulty)->HiScore);
	const int iLamp(PlayerDataMgr->GetMusicRecord(id, *pMusicSelect->m_ipSelectDifficulty)->ClearLamp);
	int srcY(iLamp * 24);
	if ((CLEAR_LAMP)iLamp == CLEAR_LAMP::FULL_COMBO)srcY += pMusicSelect->m_LampAnimPanel * 24;
	pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::CLEAR_TEXT]->Render(266, 603, 270, 24, 0, srcY, 270, 24);

	// スコア
	{
		// の数字補間用
		static int HokanScore(0);
		// BPM補間処理
		HokanScore = (abs(HokanScore - score) < 100) ? score : (int)(HokanScore * .65f + score * .35f);	// 要は9999で止まったりするのでそれの対策
		pMusicSelect->m_pImages[IMAGE::NUMBER]->SetARGB(0xffffffff);
		NumberRender(pMusicSelect->m_pImages[IMAGE::NUMBER].get(), 511, 569, 24, 32, HokanScore, 6, false);
	}

	// ランク
	if ((CLEAR_LAMP)PlayerDataMgr->GetMusicRecord(id, *pMusicSelect->m_ipSelectDifficulty)->ClearLamp != CLEAR_LAMP::NO_PLAY)
	{
		const int iRank = (int)ScoreRankCheck(score);
		pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::RANK]->Render(561, 560, 150, 56, 0, iRank * 56, 150, 56);
	}
}

void sceneMusicSelect::Mode::MusicSelect::RenderMusicList()
{
	/*
	static const int OFFSET_X(860);	// 名前開始地点のX座標
	static const int OFFSET_Y(345);	// 名前開始地点のY座標
	*/

	// 行の数
	const UINT MAX_COLUMN(15);										// 行の最大数
	const int column(min(pMusicSelect->m_pSelectFolderInfo->size(), MAX_COLUMN));	// 行の数
	const int ColumnCenter((int)(column * .45f));					// 行の数から真ん中を選出

	// 行の描画
	FOR(column)
	{
		/*
		const int width = (ColumnCenter - i) * -40;	// 行の真ん中を基準点として、そこからずれる量(真ん中ならもちろん0)
		const int posY = OFFSET_Y + width;
		*/

		// 自分のカーソルでの前後を取得
		int index(m_iSelectMusic + (i - ColumnCenter));

		// 下に行きすぎたら(画面では上)
		if (index < 0)
		{
			// 全アルファベット分を足すことによって、過剰分をずらせる
			index = (int)pMusicSelect->m_pSelectFolderInfo->size() + index;
		}

		// 上に行きすぎたら(画面では下)
		else if (index >= (int)pMusicSelect->m_pSelectFolderInfo->size())
		{
			// 全アルファベット分を足すことによって、過剰分をずらせる
			index -= (int)pMusicSelect->m_pSelectFolderInfo->size();
		}

		// 座標取得
		const Vector2 pos(pMusicSelect->m_pSelectFolderInfo->at(index)->pos);

		// フレーム描画
		{
			const int l_ciSrcY((pMusicSelect->isExitFolder(index)) ? 40 : 0);
			pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->Render((int)pos.x, (int)pos.y, 520, 40, 0, l_ciSrcY, 520, 40);

			// 企業さんからいただいた意見を取り入れる。
			if (index == m_iSelectMusic)
			{
				static const float l_cfMinAlpha(0.0f);
				static const float l_cfMaxAlpha(0.5f);
				static float l_sfAlphaSinCurve(0);

				// 選択
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetAlpha(Math::Blend((sinf(l_sfAlphaSinCurve += 0.1f) + 1) * 0.5f, l_cfMinAlpha, l_cfMaxAlpha));	// -1～1→0～1
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->Render((int)pos.x, (int)pos.y, 520, 40, 0, l_ciSrcY, 520, 40, RS::ADD);
				pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetAlpha((BYTE)BYTE_MAX);
			}
		}

		// 戻るボタン(フォルダーの名前)ではなかったら
		if (!pMusicSelect->isExitFolder(index))
		{
			/*
			int addPosY(0);
			const bool bSelectIndex(index == m_iSelectMusic);
			if (bSelectIndex)			addPosY = 1;
			else if (i > ColumnCenter)	addPosY = 2;
			*/
			//pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetScale(bSelectIndex ? 1.05f : 1.0f);

			// 難易度によって色を変更
			DWORD col(DifficultyColor[pMusicSelect->m_pSelectFolderInfo->at(index)->difficulty]);

			// レベル描画
			const int level(pMusicSelect->GetSelectMusicInfo(index)->level[pMusicSelect->m_pSelectFolderInfo->at(index)->difficulty]);
			pMusicSelect->m_pImages[IMAGE::NUMBER]->SetARGB(col);
			NumberRender(pMusicSelect->m_pImages[IMAGE::NUMBER].get(), (int)pos.x + 46, (int)pos.y + 4, 24, 32, level, 2, true);
			//int iti = level % 10, juu = level / 10;
			//if(juu != 0)pMusicSelect->m_pImages[IMAGE::NUMBER]->Render(bSelectIndex ? 780 - 12 : 780, posY + 4, 24, 32, juu * 24, 0, 24, 32);
			//pMusicSelect->m_pImages[IMAGE::NUMBER]->Render(bSelectIndex ? 804 - 12 : 804, posY + 4, 24, 32, iti * 24, 0, 24, 32);

			// クリアランプ描画
			const int lamp(PlayerDataMgr->GetMusicRecord(pMusicSelect->GetSelectMusicID(index), pMusicSelect->m_pSelectFolderInfo->at(index)->difficulty)->ClearLamp);
			switch ((CLEAR_LAMP)lamp)
			{
				// 難易度による色の変更なし
			case CLEAR_LAMP::NO_PLAY:
			case CLEAR_LAMP::EASY:
			case CLEAR_LAMP::HARD:
			case CLEAR_LAMP::EX_HARD:
			case CLEAR_LAMP::FULL_COMBO:
				pMusicSelect->m_pImages[IMAGE::LAMP]->SetARGB(0xffffffff);
				break;

				// 難易度によって色が違うランプ
			case CLEAR_LAMP::FAILED:
			case CLEAR_LAMP::CLEAR:
				pMusicSelect->m_pImages[IMAGE::LAMP]->SetARGB(col);
				break;
			}
			//pMusicSelect->m_pImages[IMAGE::LAMP]->SetScale(bSelectIndex ? 1.05f : 1.0f);
			pMusicSelect->m_pImages[IMAGE::LAMP]->Render((int)pos.x - 4, (int)pos.y - 3, 24, 44, pMusicSelect->m_LampAnimPanel * 24, lamp * 44, 24, 44);

			// 曲名描画
			std::string sSpaceStr((pMusicSelect->GetSelectMusicInfo(index)->iOctaveWidths[pMusicSelect->m_pSelectFolderInfo->at(index)->difficulty] == 1) ? tdnFont::GetPictGraphStr(tdnFont::PICTOGRAPH::BEGINNER) : "　");
			tdnFont::RenderString(std::string(sSpaceStr + pMusicSelect->GetSelectMusicInfo(index)->sMusicName).c_str(), 32, (int)pos.x + 100, (int)pos.y + 4, 0xffffffff, "メイリオ");
		}
		else
		{
			// フレーム描画
			//pMusicSelect->m_pImages[IMAGE::MUSIC_FRAME]->SetScale((m_iSelectMusic == pMusicSelect->m_pSelectFolderInfo->size()) ? 1.05f : 1.0f);

			std::string sSpaceStr((*pMusicSelect->m_ipSelectFolder == (int)FOLDER_TYPE::BEGINNER) ? tdnFont::GetPictGraphStr(tdnFont::PICTOGRAPH::BEGINNER) : "　");
			tdnFont::RenderString(std::string(sSpaceStr + pMusicSelect->m_FolderTypeMap[(FOLDER_TYPE)*pMusicSelect->m_ipSelectFolder]).c_str(), 32, (int)pos.x + 100, (int)pos.y + 4, 0xffffffff, "メイリオ");
		}
	}
}

//
//=============================================================================================



//=============================================================================================
//		決定後の余興
//
//=============================================================================================
sceneMusicSelect::Mode::DecideMusic::DecideMusic(sceneMusicSelect *me) :Base(me), m_frame(0), m_pScreen(new tdn2DObj(1280, 720, tdn2D::RENDERTARGET)), m_pMusicTitleAnim(new MusicTitleAnim(me->m_MusicInfoMap[me->GetSelectMusicInfo(m_iSelectMusic)->byID]->pMusicTextAnim->GetTexture()))
{
	// 波紋エフェクト作成
	// スクリーンリップル
	FOR(c_NumRipple)
	{
		m_pScreenRipples[i] = std::make_unique<tdn2DAnim>(m_pScreen->GetTexture());
		m_pScreenRipples[i]->OrderRipple(25 + i*3, 1.0f, .05f - i * .001f);
	}

	// フラッシュフェードセット
	Fade::Set(Fade::MODE::FADE_IN, 16, 0, 252, 0xffffff);

	// レンダーターゲット
	tdnSystem::GetDevice()->GetRenderTarget(0, &m_surface);

	// 動画止める
	me->m_pBackMovie->Stop();

	// 曲
	//me->m_pMusicPlayer->Stop(MUSIC_PLAYER::SELECTION);
	me->m_pMusicPlayer->Play(MUSIC_PLAYER::DECIDE, false);
}
sceneMusicSelect::Mode::DecideMusic::~DecideMusic()
{
	delete m_pMusicTitleAnim;
}
void sceneMusicSelect::Mode::DecideMusic::Update()
{
	// 曲名の動き
	m_pMusicTitleAnim->Update(m_frame);

	// リップルトリガー発動フェードアウト中
	if (++m_frame > c_RippleTriggerFrame)
	{
		// 波紋アニメーション
		FOR(c_NumRipple)m_pScreenRipples[i]->Update();

		// 仮で拡大
		m_pScreen->SetScale(1.0f + (m_frame - c_RippleTriggerFrame) * .02f);

		// フェード終わったら
		if (Fade::m_eMode == Fade::MODE::FADE_NONE && !pMusicSelect->m_pMusicPlayer->isPlay(MUSIC_PLAYER::DECIDE))
		{
			// シーン切り替え
			MainFrame->ChangeScene(new scenePlaying);
		}
	}

	// リップル発動までの間
	else
	{
		// 演出スキップ
		if (InputMIDIMgr->GetAnyTrigger())
		{
			pMusicSelect->m_pMusicPlayer->FadeOut(MUSIC_PLAYER::DECIDE, .01f);
			m_frame = c_RippleTriggerFrame;
		}

		// リップル発動フレーム！
		if (m_frame == c_RippleTriggerFrame)
		{
			// フェードアウトセット
			Fade::Set(Fade::MODE::FADE_OUT, 4, 255, 0, 0x000000);

			// リップル発動！
			FOR(c_NumRipple)m_pScreenRipples[i]->Action();
		}

		// 背景を拡大したり暗くしたり
		static const float MaxScale(.5f);								// 1.0f+MaxScaleが最大値(選曲した段階が一番でかい)
		const float rate((float)m_frame / c_RippleTriggerFrame);			// リップル発動を1とする割合
		const BYTE rgb((BYTE)(230 * min((rate * rate) + .2f, 1.0f)));	// 背景の色率(0なら真っ黒、255ならフル背景画像色)
		pMusicSelect->m_pImages[IMAGE::BACK]->SetScale(1.0f + (MaxScale - rate * MaxScale));
		pMusicSelect->m_pImages[IMAGE::BACK]->SetARGB((BYTE)255, rgb, rgb, rgb);
	}
}
void sceneMusicSelect::Mode::DecideMusic::Render()
{
	// レンダーターゲット
	m_pScreen->RenderTarget();
	tdnView::Clear();

	// 背景描画
	pMusicSelect->m_pImages[IMAGE::BACK]->Render(0, 0);

	// 曲名・アーティスト名
	m_pMusicTitleAnim->Render();

	// 上の枠たち
	pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Render(0, 0);
	pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 64, 400, 64);
	pMusicSelect->m_pAnimImages[(int)ANIM_IMAGE::STAGE]->Render(760, 30, 280, 64, 0, (ModeMgr->GetMode() == PLAY_MODE::STANDARD) ? ModeMgr->GetStep() * 64 : 256, 280, 64);

	//	フレームバッファへ切り替え
	tdnSystem::GetDevice()->SetRenderTarget(0, m_surface);

	// レンダーターゲットしたスクリーンを描画描画
	if(m_frame <= c_RippleTriggerFrame)m_pScreen->SetScale(1.0f + (m_frame / c_RippleTriggerFrame) * .08f);	// 仮で拡大
	m_pScreen->Render(0, 0, 1280, 720, 0, 0, 1280, 720);
	FOR(c_NumRipple)m_pScreenRipples[i]->Render(0, 0, RS::ADD);

	// その上にフェード
	Fade::Render(RS::ADD);
}



//=============================================================================================
//		ソート関数
void sceneMusicSelect::Push(FolderInfo *info, DIFFICULTY dif, bool bExitFolder)
{
	m_pSelectFolderInfo->push_back(new SelectMusicInfo(info, dif, bExitFolder));
}

void sceneMusicSelect::MusicSort::Beginner::Funk(sceneMusicSelect *pMusicSelect)
{
	EP_MUSIC_LOOP
	{
		// 難易度ループ
		for (int dif = 0; dif < (int)DIFFICULTY::MAX; dif++)
		{
			// オクターブ数が1だったら突っ込む
			if (MUSIC_DATA_BASE->GetEPContentsData(i)->iOctaveWidths[dif] == 1)
				pMusicSelect->Push(MUSIC_DATA_BASE->GetEPContentsData(i), (DIFFICULTY)dif);
		}
	}

	// ラムダパワーの力を借りて、楽々ソート
	auto Ascending = [](SelectMusicInfo *p1, SelectMusicInfo *p2){return (p1->info->level[p1->difficulty] < p2->info->level[p2->difficulty]); };

	std::sort(
		pMusicSelect->m_pSelectFolderInfo->begin(),
		pMusicSelect->m_pSelectFolderInfo->end(),
		Ascending
		);
}

void sceneMusicSelect::MusicSort::All::Funk(sceneMusicSelect *pMusicSelect)
{
	// 全曲格納
	EP_MUSIC_LOOP
	{
		pMusicSelect->Push(MUSIC_DATA_BASE->GetEPContentsData(i), (DIFFICULTY)*pMusicSelect->m_ipSelectDifficulty);
	}
}

void sceneMusicSelect::MusicSort::ElectronicPianism::Funk(sceneMusicSelect *pMusicSelect)
{
	// 今はとりあえず全曲
	EP_MUSIC_LOOP
	{
		pMusicSelect->Push(MUSIC_DATA_BASE->GetEPContentsData(i), (DIFFICULTY)*pMusicSelect->m_ipSelectDifficulty);
	}
}

void sceneMusicSelect::MusicSort::Base::LevelSort(int Min, int Max, sceneMusicSelect *pMusicSelect)
{
	EP_MUSIC_LOOP
	{
	// 難易度ループ
	for (int dif = 0; dif < (int)DIFFICULTY::MAX; dif++)
	{
		const int level = MUSIC_DATA_BASE->GetEPContentsData(i)->level[dif];

		// レベルが範囲内ならつっこむ
		if (level >= Min && level <= Max)pMusicSelect->Push(MUSIC_DATA_BASE->GetEPContentsData(i), (DIFFICULTY)dif);
	}
}
	if (pMusicSelect->m_pSelectFolderInfo->size() < 2) return;

	//auto Swap = [](SelectMusicInfo *p1, SelectMusicInfo *p2)
	//{
	//	SelectMusicInfo info;
	//	memcpy_s(&info, sizeof(SelectMusicInfo), p1, sizeof(SelectMusicInfo));
	//	memcpy_s(p1, sizeof(SelectMusicInfo), p2, sizeof(SelectMusicInfo));
	//	memcpy_s(p2, sizeof(SelectMusicInfo), &info, sizeof(SelectMusicInfo));
	//};
	//for (UINT i = 0; i < pMusicSelect->m_pSelectFolderInfo->size() - 1; i++)
	//{
	//	for (UINT j = i + 1; j < pMusicSelect->m_pSelectFolderInfo->size(); j++)
	//	{
	//		SelectMusicInfo *p1(pMusicSelect->m_pSelectFolderInfo[j]), *p2(pMusicSelect->m_pSelectFolderInfo[i]);
	//		if (p1->info->level[p1->difficulty] < p2->info->level[p2->difficulty]) Swap(p1, p2);
	//	}
	//}

	// ラムダパワーの力を借りて、楽々ソート
	auto Ascending = [](SelectMusicInfo *p1, SelectMusicInfo *p2){return (p1->info->level[p1->difficulty] < p2->info->level[p2->difficulty]); };
	
	std::sort(
		pMusicSelect->m_pSelectFolderInfo->begin(),
		pMusicSelect->m_pSelectFolderInfo->end(),
		Ascending
	);

	//Sort::BubbleSort<SelectMusicInfo> sort;
	//sort.Function(m_pSelectFolderInfo);
}

void sceneMusicSelect::MusicSort::Level1_5::Funk(sceneMusicSelect *pMusicSelect)
{
	// レベル絞込み
	LevelSort(1, 5, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level6_10::Funk(sceneMusicSelect *pMusicSelect)
{
	// レベル絞込み
	LevelSort(6, 10, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level11_15::Funk(sceneMusicSelect *pMusicSelect)
{
	// レベル絞込み
	LevelSort(11, 15, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level16_20::Funk(sceneMusicSelect *pMusicSelect)
{
	// レベル絞込み
	LevelSort(16, 20, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level21_25::Funk(sceneMusicSelect *pMusicSelect)
{
	// レベル絞込み
	LevelSort(21, 25, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level26_30::Funk(sceneMusicSelect *pMusicSelect)
{
	// レベル絞込み
	LevelSort(26, 30, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Level31::Funk(sceneMusicSelect *pMusicSelect)
{
	// レベル絞込み
	LevelSort(31, 31, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Base::GenreSort(MUSIC_GENRE genre, sceneMusicSelect *pMusicSelect)
{
	EP_MUSIC_LOOP
	{
		// ジャンルが一致してたらつっこむ
		if (MUSIC_DATA_BASE->GetEPContentsData(i)->eMusicGenre == genre)
		pMusicSelect->Push(MUSIC_DATA_BASE->GetEPContentsData(i), (DIFFICULTY)*pMusicSelect->m_ipSelectDifficulty);
	}
}

void sceneMusicSelect::MusicSort::Original::Funk(sceneMusicSelect *pMusicSelect)
{
	// ジャンル絞り込み
	GenreSort(MUSIC_GENRE::ORIGINAL, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Game::Funk(sceneMusicSelect *pMusicSelect)
{
	// ジャンル絞り込み
	GenreSort(MUSIC_GENRE::GAME, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Classic::Funk(sceneMusicSelect *pMusicSelect)
{
	// ジャンル絞り込み
	GenreSort(MUSIC_GENRE::CLASSIC, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Vocaloid::Funk(sceneMusicSelect *pMusicSelect)
{
	// ジャンル絞り込み
	GenreSort(MUSIC_GENRE::VOCALOID, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Pops::Funk(sceneMusicSelect *pMusicSelect)
{
	// ジャンル絞り込み
	GenreSort(MUSIC_GENRE::POPS, pMusicSelect);
}

void sceneMusicSelect::MusicSort::Other::Funk(sceneMusicSelect *pMusicSelect)
{
	// ジャンル絞り込み
	GenreSort(MUSIC_GENRE::OTHER, pMusicSelect);
}