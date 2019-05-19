#include "TDNLIB.h"
#include "../Sound/SoundManager.h"
#include "system/Framework.h"
#include "sceneModeSelect.h"
#include "sceneClassSelect.h"
#include "scenePlaying.h"
#include "../Data/PlayerData.h"
#include "../Input/InputMIDI.h"
#include "../Fade/Fade.h"
#include "../Mode/ModeManager.h"
#include "../Data/SelectMusicData.h"
#include "../Number/Number.h"

//=============================================================================================
//		フレーム委譲くん
sceneClassSelect::Frame::Frame(int cursor, int grade, int rank, Texture2D *pFrame, Texture2D *pText) :
	grade(grade), 
	rank(rank), 
	m_pFrame(std::make_unique<tdn2DAnim>(pFrame)), 
	m_pText(std::make_unique<tdn2DAnim>(pText))
{
	SetCursor(cursor);
	pos = NextPos;

	// 枠たちにアクションする
	m_pFrame->OrderMoveAppeared(8, (int)pos.x - 600, (int)pos.y);
	m_pText->OrderMoveAppeared(8, (int)pos.x - 600, (int)pos.y);

	// 真ん中に近ければ近いほど、早く発動する
	const int r(18 + abs(cursor - grade) * 5);
	m_pFrame->Action(r);
	m_pText->Action(r);
}

void sceneClassSelect::Frame::Update()
{
	pos = pos*.75f + NextPos*.25f;

	// アニメーションの更新
	m_pFrame->Update();
	m_pText->Update();
}

void sceneClassSelect::Frame::Render()
{
	if (pos.y > 720 || pos.y < -128) return;	// 枠から出てたら、でていけぇ！！

	// 枠描画
	m_pFrame->Render((int)pos.x, (int)pos.y, 420, 128, 0, rank * 128, 420, 128);

	// 枠の中のテキスト描画
	m_pText->Render((int)pos.x + 135, (int)pos.y + 29, 150, 72, 0, grade * 72, 150, 72);
}

void sceneClassSelect::Frame::SetCursor(int cursor)
{
	// カーソルと段位の値に応じて座標を設定

	// 自分の番号と選択してるやつの差分
	int sabun = cursor - grade;

	NextPos.x = abs(sabun) * 32.0f + 800;
	NextPos.y = (sabun * 150.0f) + 256;
}
//=============================================================================================


//=============================================================================================
//		初	期	化	と	開	放
bool sceneClassSelect::Initialize()
{
	// フェード初期化
	Fade::Set(Fade::MODE::FADE_IN, 5, 0x00ffffff);

	// シーンに入ってからの操作受付までの時間
	m_WaitTimer = 28;

	m_fLoadPercentage = .05f;	// ロード割合

	// 画像初期化
	m_pImages[(int)IMAGE::BACK]				= std::make_unique<tdn2DObj>("DATA/Image/back2.png");
	m_pImages[(int)IMAGE::GRADE_TEXT]		= std::make_unique<tdn2DObj>("DATA/UI/Grade/text.png");
	m_fLoadPercentage = .1f;	// ロード割合
	m_pImages[(int)IMAGE::NUMBER]			= std::make_unique<tdn2DObj>("DATA/Number/numberL.png");
	m_pImages[(int)IMAGE::SELECT_FRAME]		= std::make_unique<tdn2DObj>("DATA/UI/Grade/select_frame.png");
	m_pImages[(int)IMAGE::FRAME]			= std::make_unique<tdn2DObj>("DATA/UI/Grade/frame.png");
	m_pImages[(int)IMAGE::FRAME_IN_TEXT]	= std::make_unique<tdn2DObj>("DATA/UI/Grade/grade.png");
	m_fLoadPercentage = .15f;	// ロード割合

	// アニメーション画像初期化
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI] = std::make_unique<tdn2DAnim>("DATA/UI/Other/midashi.png");
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->OrderMoveAppeared(16, -8, 12);
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Action(15);
	m_fLoadPercentage = .2f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/up_frame.png");
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->OrderMoveAppeared(30, 0, -64);
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Action(0);
	m_fLoadPercentage = .25f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT1] = std::make_unique<tdn2DAnim>(m_pImages[(int)IMAGE::GRADE_TEXT]->GetTexture());
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT1]->OrderMoveAppeared(15, 128 - 64, 200);
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT1]->Action(10);
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT2] = std::make_unique<tdn2DAnim>(m_pImages[(int)IMAGE::GRADE_TEXT]->GetTexture());
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT2]->OrderMoveAppeared(15, 400, 200 - 10);
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT2]->Action(10);
	m_fLoadPercentage = .3f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_FRAME] = std::make_unique<tdn2DAnim>(m_pImages[(int)IMAGE::SELECT_FRAME]->GetTexture());
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_FRAME]->OrderRipple(20, 1.0, .01f);
	m_fLoadPercentage = .35f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::PASS] = std::make_unique<tdn2DAnim>("DATA/UI/Grade/pass.png");
	m_pAnimImages[(int)ANIM_IMAGE::PASS]->OrderJump(5, 1.0f, .2f);
	m_pAnimImages[(int)ANIM_IMAGE::PASS]->Action(5);
	m_fLoadPercentage = .4f;	// ロード割合
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME] = std::make_unique<tdn2DAnim>("DATA/UI/Other/score.png");
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->OrderStretch(15, 1, .0f);
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->Action(10);
	m_fLoadPercentage = .45f;	// ロード割合
	m_fLoadPercentage = .5f;	// ロード割合
	m_fLoadPercentage = 1.0f;	// ロード割合

	// カーソル初期化
	m_iCursor = (int)PlayerDataMgr->m_grade;	// 今自分が合格している最大の段位にカーソルを合わせる
	if (m_iCursor == (int)GRADE::NONE) m_iCursor = (int)GRADE::FIRST;

	// 枠委譲くん
	m_pFrames = new Frame*[(int)GRADE::MAX];
	FOR((int)GRADE::MAX)
	{
		int rank;
		switch ((GRADE)(i))
		{
		case GRADE::FIRST:
		case GRADE::SECOND:
		case GRADE::THERD:
		case GRADE::FOURTH:
			rank = 0;	// 銅
			break;
		case GRADE::FIFTH:
		case GRADE::SIXTH:
		case GRADE::SEVENTH:
		case GRADE::EIGHTH:
			rank = 1;	// 銀
			break;
		case GRADE::NINTH:
		case GRADE::TENTH:
			rank = 2;	// 金
			break;
		case GRADE::BRILLIANCE:
			rank = 3;	// プラチナ
			break;
		}
		m_pFrames[i] = new Frame(m_iCursor, i, rank, m_pImages[(int)IMAGE::FRAME]->GetTexture(), m_pImages[(int)IMAGE::FRAME_IN_TEXT]->GetTexture());
	}

	g_pBGM->PlayStreamIn("DATA/Customize/SelectMusic/Cyberspace_Walkers/music.ogg");

	return true;
}

sceneClassSelect::~sceneClassSelect()
{
	g_pBGM->StopStreamIn();
	DOUBLE_POINTER_DELETE(m_pFrames, (int)GRADE::MAX);
}
//=============================================================================================


//=============================================================================================
//		更			新
bool sceneClassSelect::Update()
{
	static bool bEnd(false);

	// アニメーション画像更新
	FOR((int)ANIM_IMAGE::MAX) m_pAnimImages[i]->Update();

	// 枠たち
	FOR((int)GRADE::MAX) m_pFrames[i]->Update();

	// フェード
	Fade::Update();

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
		if (m_WaitTimer > 0){ m_WaitTimer--; return true; }

		bool bCursored(false);
		// 上下で選択
		if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
		{
			if (m_iCursor > (int)GRADE::FIRST)
			{
				m_iCursor--;
				bCursored = true;
			}
		}
		else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
		{
			if (m_iCursor < (int)GRADE::MAX - 1)
			{
				m_iCursor++;
				bCursored = true;
			}
		}
		if (bCursored)
		{
			g_pSE->Play("カーソル");

			// 枠たちに今のカーソルを伝える
			FOR((int)GRADE::MAX) m_pFrames[i]->SetCursor(m_iCursor);

			// アニメーションを発動させる
			m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT2]->Action(4);
			m_pAnimImages[(int)ANIM_IMAGE::SELECT_FRAME]->Action();
			m_pAnimImages[(int)ANIM_IMAGE::PASS]->Action();
		}

		// エンターキーで決定
		if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
		{
			// オートプレイ強制解除
			PlayerDataMgr->m_PlayerOption.bAutoPlay = FALSE;

			// モード管理に選択した段位の番号を渡すと勝手に曲のデータをとってきてくれる
			ModeMgr->SetSelectGrade(m_iCursor);
			ModeMgr->SettingGradeMusic();		// 選曲管理さんに曲を設定させる

			// プレイシーンへ
			MainFrame->ChangeScene(new scenePlaying);
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


	return true;
}
//
//=============================================================================================


//=============================================================================================
//		描			画
void sceneClassSelect::Render()
{
	
	tdnView::Clear();

	// 背景
	m_pImages[(int)IMAGE::BACK]->Render(0, 0);

	// 枠たち
	FOR((int)GRADE::MAX) m_pFrames[i]->Render();

	// フレーム
	m_pAnimImages[(int)ANIM_IMAGE::UP_FRAME]->Render(0, 0);

	// CLASS SELECT
	m_pAnimImages[(int)ANIM_IMAGE::MIDASHI]->Render(16, 12, 400, 64, 0, 128, 400, 64);

	// スコア枠描画
	m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->Render(232, 540);

	// スコア枠の中の描画
	if (m_pAnimImages[(int)ANIM_IMAGE::SCORE_FRAME]->GetAction()->IsEnd())
	{
		// 合格・不合格
		int srcY;
		int percent(PlayerDataMgr->m_GradeRecords[m_iCursor]->AchievementPct);
		if (PlayerDataMgr->m_GradeRecords[m_iCursor]->bPass) srcY = 0;
		else srcY = (percent > 0) ? 46 : 46 * 2;
		m_pAnimImages[(int)ANIM_IMAGE::PASS]->Render(278, 578, 156, 46, 0, srcY, 156, 46);

		// 達成率
		m_pAnimImages[(int)ANIM_IMAGE::PASS]->Render(420, 600, 156, 23, 0, 46 * 3, 156, 23);
		m_pImages[(int)IMAGE::NUMBER]->Render(652, 580, 32, 42, 320, 0, 32, 42);	// %
		if (percent > 0)
		{
			// の数字補間用
			static int HokanPercent(0);
			HokanPercent = (abs(HokanPercent - percent) < 10) ? percent : (int)(HokanPercent * .75f + percent * .25f);	// 要は99で止まったりするのでそれの対策
			NumberRender(m_pImages[(int)IMAGE::NUMBER].get(), 652 - 32, 580, 32, 42, HokanPercent, 3, true);
		}
		else m_pImages[(int)IMAGE::NUMBER]->Render(652 - 32, 580, 32, 42, 0, 0, 32, 42);
	}

	// 段位認定
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT1]->Render(128, 200, 300, 80, 0, 880, 300, 80);
	m_pAnimImages[(int)ANIM_IMAGE::GRADE_TEXT2]->Render(400, 200, 300, 80, 0, (m_iCursor)* 80, 300, 80);

	static int y(0);

	if (KeyBoardTRG(KB_Z)) y += 1;

	// 選択枠
	m_pImages[(int)IMAGE::SELECT_FRAME]->Render(800, 246);
	m_pAnimImages[(int)ANIM_IMAGE::SELECT_FRAME]->Render(800, 246);

	// フェード
	Fade::Render();
}
//
//=============================================================================================