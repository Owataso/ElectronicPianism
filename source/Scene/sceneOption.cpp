#include "TDNLIB.h"
#include "../system/Framework.h"
#include "SceneOption.h"
#include "sceneModeSelect.h"
#include "../Data/CustomizeData.h"
#include "../Sound/SoundManager.h"
#include "../Data/PlayerData.h"
#include "../Fade/Fade.h"
#include "../Input/InputMIDI.h"
#include "../Music/MusicInfo.h"
#include "TDN_Movie.h"

//=============================================================================================
//		各選択
const Vector2 SelectOption::Base::FRAME_OFFSET_XY(424, 96);

SelectOption::Base::Base(sceneOption *pOption, int NumCursor) :m_pOption(pOption), m_CursorAnimFrame(0), m_iCursor(0), m_bSettingOption(false), m_NumCursor(NumCursor)
{
	m_pText = new tdnText2(20, "ＭＳ Ｐゴシック");

	// カーソル座標
	m_CurrentCursorPos.x = m_NextCursorPos.x = FRAME_OFFSET_XY.x + (m_iCursor % 6) * 128, m_CurrentCursorPos.y = m_NextCursorPos.y = FRAME_OFFSET_XY.y + (m_iCursor / 6) * 64;
}

SelectOption::Base::~Base()
{
	// 選択したカスタマイズで上書き
	//m_pOption->m_iSelectOptions[m_pOption->m_iSelectOptionType] = m_iCursor;
	delete m_pText;
}

void SelectOption::Base::UpdateCursor()
{
	// カーソルアニメーション更新
	if (++m_CursorAnimFrame > 8 * 4) m_CursorAnimFrame = 0;

	// カーソル座標補間処理
	m_CurrentCursorPos = m_CurrentCursorPos * .8f + m_NextCursorPos * .2f;

	bool bCursored(false);

	// 左右でフォルダー選択
	if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15))
	{
		bCursored = true;	// カーソル移動したのでtrue
		g_pSE->Play("カーソル2");// カーソル音再生
		if (--m_iCursor < 0) m_iCursor = m_NumCursor - 1;
	}
	else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
	{
		bCursored = true;	// カーソル移動したのでtrue
		g_pSE->Play("カーソル2");// カーソル音再生
		if (++m_iCursor >= m_NumCursor) m_iCursor = 0;
	}

	if (bCursored)
	{
		// 次のカーソル座標更新
		m_NextCursorPos.x = FRAME_OFFSET_XY.x + (m_iCursor % 6) * 128, m_NextCursorPos.y = FRAME_OFFSET_XY.y + (m_iCursor / 6) * 64;
	}

	// エンターで決定
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		m_pOption->m_pSE->Play(0);// 決定音再生
		m_bSettingOption = true;
	}

	// スペースでキャンセル
	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
	{
		g_pSE->Play("戻る");

		// 閉じる
		m_pOption->Close();
	}
}

void SelectOption::Base::RenderCursor()
{
	// カーソル枠
	//m_pOption->m_pImages[(int)m_pOption->IMAGE::CURSOR_FRAME_S]->Render((int)m_CurrentCursorPos.x, (int)m_CurrentCursorPos.y, 128, 64, 0, (m_CursorAnimFrame / 8) * 64, 128, 64);
}

SelectOption::HiSpeed::HiSpeed(sceneOption *pOption) :Base(pOption, 2), m_iOrgHiSpeed(pOption->m_pPlayerOption->HiSpeed), m_bOrgRegulSpeed(pOption->m_pPlayerOption->bRegulSpeed){}
void SelectOption::HiSpeed::Update()
{
	if (m_bSettingOption)
	{
		if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
		{
			m_pOption->m_pSE->Play(0);// 決定音再生
			m_bSettingOption = false;
		}

		switch (m_iCursor)
		{
		case 0:	// ハイスピの設定
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15))
			{
				g_pSE->Play("カーソル2");// カーソル音再生
				if (m_pOption->m_pPlayerOption->HiSpeed > 0) m_pOption->m_pPlayerOption->HiSpeed--;
			}
			else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("カーソル2");// カーソル音再生
				if (m_pOption->m_pPlayerOption->HiSpeed < 10) m_pOption->m_pPlayerOption->HiSpeed++;
			}
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("戻る");
				m_pOption->m_pPlayerOption->bRegulSpeed = m_bOrgRegulSpeed.val;
				m_bSettingOption = false;
			}
			break;

		case 1:	// レギュラースピードの設定
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15) || InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("カーソル2");// カーソル音再生
				m_pOption->m_pPlayerOption->bRegulSpeed ^= 0x1;
			}
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("戻る");
				m_pOption->m_pPlayerOption->bRegulSpeed = m_bOrgRegulSpeed.val;
				m_bSettingOption = false;
			}
			break;
		}

	}

	else
	{
		Base::UpdateCursor();
	}

}

void SelectOption::HiSpeed::Render()
{
	// カーソル描画
	//RenderCursor();

	const DWORD l_dwCol(m_bSettingOption ? 0xffffffff : 0x80ffffff);

	switch (m_iCursor)
	{
	case 0:	// 数値の設定
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y, l_dwCol, "ハイスピードの設定");
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y + 30, l_dwCol, "%.1f", m_pOption->m_pPlayerOption->HiSpeed * HI_SPEED_WIDTH);
		break;
	case 1:	// オン・オフの設定
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y, l_dwCol, "レギュラースピードのON・OFF");
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y + 30, l_dwCol, "%s", (m_pOption->m_pPlayerOption->bRegulSpeed ? "ON" : "OFF"));
		break;
	}
}


SelectOption::Movie::Movie(sceneOption *pOption) :Base(pOption, 2), m_bOrgMovieOn(pOption->m_pPlayerOption->bMoviePlay), m_byOrgMovieAlpha(pOption->m_pPlayerOption->MovieAlpha){}
void SelectOption::Movie::Update()
{
	if (m_bSettingOption)
	{
		if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
		{
			m_pOption->m_pSE->Play(0);// 決定音再生
			m_bSettingOption = false;
		}

		switch (m_iCursor)
		{
		case 0:	// オン・オフの設定
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15) || InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("カーソル2");// カーソル音再生
				m_pOption->m_pPlayerOption->bMoviePlay ^= 0x1;
			}
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("戻る");
				m_pOption->m_pPlayerOption->bMoviePlay = m_bOrgMovieOn.val;
				m_bSettingOption = false;
			}


			break;

		case 1:	// アルファの設定
		{
					static const int ADD_WIDTH(32);	// 増減幅
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15))
			{
				g_pSE->Play("カーソル2");// カーソル音再生
				if(m_pOption->m_pPlayerOption->MovieAlpha > ADD_WIDTH) m_pOption->m_pPlayerOption->MovieAlpha -= ADD_WIDTH;
				else m_pOption->m_pPlayerOption->MovieAlpha = 0;
			}
			else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("カーソル2");// カーソル音再生
				if (m_pOption->m_pPlayerOption->MovieAlpha + ADD_WIDTH < 255) m_pOption->m_pPlayerOption->MovieAlpha += ADD_WIDTH;
				else m_pOption->m_pPlayerOption->MovieAlpha = 255;
			}
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("戻る");
				m_pOption->m_pPlayerOption->MovieAlpha = m_byOrgMovieAlpha;
				m_bSettingOption = false;
			}
		}
			break;
		}

	}

	else
	{
		Base::UpdateCursor();
	}

}

void SelectOption::Movie::Render()
{
	// カーソル描画
	//RenderCursor();

	const DWORD l_dwCol(m_bSettingOption ? 0xffffffff : 0x80ffffff);

	switch (m_iCursor)
	{
	case 0:	// オン・オフの設定
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y, l_dwCol, "動画のON・OFF");
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y + 30, l_dwCol, "%s", (m_pOption->m_pPlayerOption->bMoviePlay ? "ON" : "OFF"));
		break;

	case 1:	// アルファの設定
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y, l_dwCol, "アルファの設定");
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y + 30, l_dwCol, "%d", m_pOption->m_pPlayerOption->MovieAlpha);
		break;
	}
}

SelectOption::Guide::Guide(sceneOption *pOption) :Base(pOption, 1), m_bOrgGuide(pOption->m_pPlayerOption->bGuide){}
void SelectOption::Guide::Update()
{
	if (m_bSettingOption)
	{
		if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
		{
			m_pOption->m_pSE->Play(0);// 決定音再生
			m_bSettingOption = false;
		}

		switch (m_iCursor)
		{
		case 0:	// ガイドのオン・オフの設定
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15) || InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("カーソル2");// カーソル音再生
				m_pOption->m_pPlayerOption->bGuide ^= 0x1;
			}
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("戻る");
				m_pOption->m_pPlayerOption->bGuide = m_bOrgGuide.val;
				m_bSettingOption = false;
			}
			break;
		}

	}

	else
	{
		Base::UpdateCursor();
	}

}
void SelectOption::Guide::Render()
{
	// カーソル描画
	//RenderCursor();

	const DWORD l_dwCol(m_bSettingOption ? 0xffffffff : 0x80ffffff);

	switch (m_iCursor)
	{
	case 0:	// ガイドのオン・オフの設定
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y, l_dwCol, "ガイドのON・OFF");
		tdnText::Draw((int)FRAME_OFFSET_XY.x, (int)FRAME_OFFSET_XY.y + 30, l_dwCol, "%s", (m_pOption->m_pPlayerOption->bGuide ? "ON" : "OFF"));
		break;
	}
}


//=============================================================================================
//		初	期	化	と	開	放
bool sceneOption::Initialize()
{
	m_pImages[(int)IMAGE::BACK]				= std::make_unique<tdn2DObj>("DATA/Image/back2.png");
	m_pImages[(int)IMAGE::UP_FRAME]			= std::make_unique<tdn2DObj>("DATA/UI/Other/up_frame.png");
	m_pImages[(int)IMAGE::MIDASHI]			= std::make_unique<tdn2DObj>("DATA/UI/Other/midashi.png");
	m_fLoadPercentage = .1f;	// ロード割合
	m_pImages[(int)IMAGE::LEFT_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Option/left_frame.png");
	m_pImages[(int)IMAGE::RIGHT_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Customize/right_frame.png");
	m_fLoadPercentage = .2f;	// ロード割合
	m_pImages[(int)IMAGE::CURSOR_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Customize/cursor_frame.png");
	m_pImages[(int)IMAGE::RIGHT_UP_FRAME]	= std::make_unique<tdn2DObj>("DATA/UI/Customize/right_up_frame.png");
	m_pImages[(int)IMAGE::CURSOR_FRAME_S]	= std::make_unique<tdn2DObj>("DATA/UI/Other/cursor_frame2.png");
	m_fLoadPercentage = .25f;	// ロード割合
	m_pImages[(int)IMAGE::SKIN]				= std::make_unique<tdn2DObj>(CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::SKIN).c_str());
	m_pImages[(int)IMAGE::NOTE]				= std::make_unique<tdn2DObj>(CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::NOTE).c_str());
	m_pImages[(int)IMAGE::EXPLOSION]		= std::make_unique<tdn2DObj>(CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::EXPLOSION).c_str());
	m_pImages[(int)IMAGE::KEYLINE]			= std::make_unique<tdn2DObj>("DATA/Customize/Skin/back.png");

	//	一時レンダリング用サーフェイス
	m_pPlayScreen = std::make_unique<tdn2DObj>(1280, 720, RENDERTARGET);

	//	フレームバッファのポインタ保存
	tdnSystem::GetDevice()->GetRenderTarget(0, &m_pSurface);

	m_fLoadPercentage = .25f;	// ロード割合

	// SE
	m_pSE = std::make_unique<tdnSoundSE>();
	m_pSE->Set(0, 3, "DATA/Sound/SE/decide1.wav");	// 決定音

	// 動画
	m_pMovie = new tdnMovie("DATA/Movie/MusicSelect.wmv");
	m_pMovie->Play();
	m_pImages[(int)IMAGE::MOVIE] = std::make_unique<tdn2DObj>(m_pMovie->GetTexture());

	m_fLoadPercentage = 1.0f;	// ロード割合

	// 選択するカスタマイズタイプカーソル初期化
	m_iSelectOptionType = 0;

	// プレイヤーデータのオプション
	m_pPlayerOption = new PlayerData::OptionV2;
	memcpy_s(m_pPlayerOption, sizeof(PlayerData::OptionV2), &PlayerDataMgr->m_PlayerOption, sizeof(PlayerData::OptionV2));

	// カスタマイズタイプ選択後の委譲くん
	m_pSelectOption = nullptr;

	// カスタマイズテストの以上君
	m_pOptionTest = std::make_unique<OptionTestPlayer>(this, m_pImages[(int)IMAGE::EXPLOSION].get());

	// カーソル座標
	m_CurrentCursorPosY = m_NextCursorPosY = 195;

	// フェード
	Fade::Set(Fade::MODE::FADE_IN, 8);

	return true;
}

sceneOption::~sceneOption()
{
	// プレイヤーデータのオプション更新
	memcpy_s(&PlayerDataMgr->m_PlayerOption, sizeof(PlayerData::OptionV2), m_pPlayerOption, sizeof(PlayerData::OptionV2));
	delete m_pPlayerOption;

	// 全画像解放
	delete m_pMovie;
	// カスタマイズ以上くん開放
	SAFE_DELETE(m_pSelectOption);
}
//
//=============================================================================================

//=============================================================================================
//		更			新
bool sceneOption::Update()
{
	static bool bEnd(false);	// フェード関係で終了させるのに使う

	// フェード
	Fade::Update();

	// 動画ループ更新
	m_pMovie->Update();

	// オプションテスト君更新
	m_pOptionTest->Update();

	// カーソル座標補間処理
	m_CurrentCursorPosY = m_CurrentCursorPosY * .8f + m_NextCursorPosY * .2f;

	if (bEnd)
	{
		// フェード終わったら帰る
		if (Fade::m_eMode == Fade::MODE::FADE_NONE)
		{
			bEnd = false;
			MainFrame->ChangeScene(new sceneModeSelect, true);
		}
	}

	else
	{
		// カスタマイズタイプ選択して、カスタマイズ選択中
		if (m_pSelectOption)
		{
			m_pSelectOption->Update();
		}
		else
		{
			// 上下でフォルダー選択
			if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT, 15))
			{
				g_pSE->Play("カーソル2");	// カーソル音再生
				if (--m_iSelectOptionType < 0) m_iSelectOptionType = (int)SELECT_OPTION::MAX - 1;

				m_NextCursorPosY = 195.0f + (m_iSelectOptionType * 73);
			}
			else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT, 15))
			{
				g_pSE->Play("カーソル2");	// カーソル音再生
				if (++m_iSelectOptionType >= (int)SELECT_OPTION::MAX) m_iSelectOptionType = 0;

				m_NextCursorPosY = 195.0f + (m_iSelectOptionType * 73);
			}

			// エンターでカスタマイズタイプ決定
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
			{
				g_pSE->Play("決定2");// 決定音再生

				switch ((SELECT_OPTION)m_iSelectOptionType)
				{
				case SELECT_OPTION::HI_SPEED:
					m_pSelectOption = new SelectOption::HiSpeed(this);
					break;
				case SELECT_OPTION::MOVIE:
					m_pSelectOption = new SelectOption::Movie(this);
					break;
				case SELECT_OPTION::GUIDE:
					m_pSelectOption = new SelectOption::Guide(this);
					break;
				}
			}

			// スペースキーで抜ける
			else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
			{
				g_pSE->Play("戻る");

				// フェードアウト設定
				Fade::Set(Fade::MODE::FADE_OUT, 4);
				bEnd = true;
			}
		}
	}

	return true;
}
//
//=============================================================================================

//=============================================================================================
//		描			画
void sceneOption::Render()
{
	//	一時バッファへ切り替え
	m_pPlayScreen->RenderTarget();

	
	tdnView::Clear();

	m_pImages[(int)IMAGE::SKIN]->Render(0, 0);

	m_pImages[(int)IMAGE::KEYLINE]->Render(0, 133);

	// 黒い余白
	if (m_pPlayerOption->bMoviePlay)
	{
		const DWORD dwAlphaColor = ((m_pPlayerOption->MovieAlpha << 24) | 0x00000000);
		tdnPolygon::Rect(1, 134, 1118, 495, RS::COPY, dwAlphaColor);
		// 動画描画
		m_pImages[(int)IMAGE::MOVIE]->SetAlpha((BYTE)m_pPlayerOption->MovieAlpha);
		m_pImages[(int)IMAGE::MOVIE]->Render(120, 134, 880, 495, 0, 0, 1280, 720);
	}

	// カスタマイズテスト描画
	m_pOptionTest->Render(m_pImages[(int)IMAGE::NOTE].get(), m_pImages[(int)IMAGE::EXPLOSION].get());

	//	フレームバッファへ切り替え
	tdnSystem::GetDevice()->SetRenderTarget(0, m_pSurface);

	// 背景
	m_pImages[(int)IMAGE::BACK]->Render(0, 0);

	// 上のフレーム
	m_pImages[(int)IMAGE::UP_FRAME]->Render(0, 0);
	m_pImages[(int)IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 320, 400, 64);

	// レンダーターゲットしたゲーム画面
	static int a = 0;
	if (KeyBoardTRG(KB_1)) a += 1;
	m_pPlayScreen->Render(412, 250, 802, 442, 0, 0, 1280, 720);

	// 最終スキンのフレーム
	m_pImages[(int)IMAGE::RIGHT_FRAME]->Render(0, 0);

	// 委譲描画
	if (m_pSelectOption)
	{
		// 左のフレーム若干暗くする
		m_pImages[(int)IMAGE::LEFT_FRAME]->SetARGB(0xff808080);
		m_pImages[(int)IMAGE::LEFT_FRAME]->Render(0, 0);

		// 右のフレーム
		m_pImages[(int)IMAGE::RIGHT_UP_FRAME]->SetARGB(0xffffffff);
		m_pImages[(int)IMAGE::RIGHT_UP_FRAME]->Render(0, 0);

		m_pSelectOption->Render();
	}
	else
	{
		// 右のフレーム若干暗くする
		m_pImages[(int)IMAGE::RIGHT_UP_FRAME]->SetARGB(0xff808080);
		m_pImages[(int)IMAGE::RIGHT_UP_FRAME]->Render(0, 0);
		// 左のフレーム
		m_pImages[(int)IMAGE::LEFT_FRAME]->SetARGB(0xffffffff);
		m_pImages[(int)IMAGE::LEFT_FRAME]->Render(0, 0);
	}

	// カーソル
	static int AnimFrame = 0;
	if (++AnimFrame > 16 * 4) AnimFrame = 0;
	m_pImages[(int)IMAGE::CURSOR_FRAME]->Render(98, (int)m_CurrentCursorPosY, 256, 64, 0, (AnimFrame / 16) * 64, 256, 64);

	// フェード
	Fade::Render();
}
//
//=============================================================================================

bool OptionTestPlayer::Note::Update(int NumExplosionpanel)
{
	if (++ExplosionFrame > NumExplosionpanel * 2)
		return true;

	return false;
}

void OptionTestPlayer::Note::Render(tdn2DObj *pNote, tdn2DObj *pExplosion)
{
	if (bNoteEnd)
	{
		pExplosion->Render(posX - 48, 618 - 60, 128, 128, (ExplosionFrame / 2) * 128, 0, 128, 128);
	}

	else
	{
		if (posY + JUDGE_LINE < KEYLANE_TOP)return;
		pNote->Render(posX, posY +JUDGE_LINE, 40, 16, 0, 0, 40, 16);
	}
}

void OptionTestPlayer::Clear()
{
	for (auto it = m_NoteList.begin(); it != m_NoteList.end();){ delete (*it); it = m_NoteList.erase(it); }
}

void OptionTestPlayer::Update()
{
	static const int CreatePosX[30] =
	{
		120, 180, 240, 300, 360, 420, 480, 540, 600, 660, 720, 780, 840, 900, 960,
		1020, 960, 900, 840, 780, 720, 660, 600, 540, 480, 420, 360, 300, 240, 180
	};

	// ノーツリストが空だったら生成
	if (m_NoteList.empty())
	{
		m_iStartClock = clock();

		m_NoteList.push_back(new Note(CreatePosX[4], 120));
		m_NoteList.push_back(new Note(CreatePosX[6], 180));
		m_NoteList.push_back(new Note(CreatePosX[8], 260));
	}

	const int iPlayCursor(clock() - m_iStartClock);

	for (auto it = m_NoteList.begin(); it != m_NoteList.end();)
	{
		if ((*it)->bNoteEnd)
		{
			if ((*it)->Update(m_NumExplosionPanel))	// trueが返ったら消去フラグONということ
			{
				delete (*it);
				it = m_NoteList.erase(it);
			}
			else it++;
			continue;
		}

		(*it)->iRestTime = iPlayCursor - (*it)->iEventTime;
		if ((*it)->iRestTime > 0)
		{
			(*it)->bNoteEnd = true;
			it++;
			continue;
		}

		int l_iPosY((*it)->iRestTime);

		// BPMによる影響
		if (!m_pOption->m_pPlayerOption->bRegulSpeed)
			l_iPosY = (int)(l_iPosY / (60 / (*it)->fBPM / 480 * 1000));
		// ハイスピードによる影響
		const float speed = m_pOption->m_pPlayerOption->HiSpeed * HI_SPEED_WIDTH;
		l_iPosY = (int)(l_iPosY * (.1f + (speed / .5f * .1f)));
		(*it)->posY = l_iPosY;

		it++;
	}
}

void OptionTestPlayer::Render(tdn2DObj *pNote, tdn2DObj *pExplosion)
{
	for (auto &it : m_NoteList)
	{
		it->Render(pNote, pExplosion);
	}
}