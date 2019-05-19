#include "TDNLIB.h"
#include "system/Framework.h"
#include "../Data/PlayerData.h"
#include "../Data/MusicDataBase.h"
#include "sceneEntry.h"
#include "SceneModeSelect.h"
#include "../Sound/SoundManager.h"
#include "../Input/InputMIDI.h"
#include "../Alphabet/Alphabet.h"

//=============================================================================================
//		初	期	化	と	開	放
bool sceneEntry::Initialize()
{
	// プレイヤー情報読み込み&既存データがあるかどうか。
	m_bLoadSaveData = PlayerDataMgr->Initialize();

	m_fLoadPercentage = .75f;

	// プレイヤーデータがないときのステートクラスポインタの初期化
	m_pNoDataState = new NoDataState::Select(this);

	m_fLoadPercentage = 1.0f;

	return true;
}

sceneEntry::~sceneEntry()
{
	m_bLoad = false;
	delete m_pNoDataState;
}
//=============================================================================================


//=============================================================================================
//		更			新
void sceneEntry::NoDataState::Select::Update()
{
	if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
	{
		// シーン切り替え
		MainFrame->ChangeScene(new sceneModeSelect, true);
	}

	else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
	{
		// 名前入力ステートに移行
		m_parent->ChangeMode(NO_PLAYER_DATA_STATE::NAME_INPUT);
	}
}


bool sceneEntry::Update()
{
	// 既存データあり
	if (m_bLoadSaveData)
	{
		// プレイ回数カウント
		PlayerDataMgr->m_PlayerInfo.PlayCount++;

		//if (KeyBoardTRG(KB_ENTER))
			MainFrame->ChangeScene(new sceneModeSelect, true);
	}

	// 新規作成
	else
	{
		// モードポインタ実行
		m_pNoDataState->Update();
	}

	return true;
}
//
//=============================================================================================


//=============================================================================================
//		描			画
void sceneEntry::NoDataState::Select::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "セーブデータがありません！");
	tdnText::Draw(32, 60, 0xffffff00, "ENTER : データ無しで開始");
	tdnText::Draw(32, 90, 0xffffff00, "SPACE : 新規データ作成");
}

void sceneEntry::Render()
{
	
	tdnView::Clear();

	// 既存データなし
	if (!m_bLoadSaveData)
	{
		// モードポインタ実行
		m_pNoDataState->Render();
	}
}
//
//=============================================================================================




//=============================================================================================
//		名前入力
sceneEntry::NoDataState::NameInput::NameInput(sceneEntry *me) :Base(me), m_cursor(0), m_InputStep(0), m_bEnd(false), m_pSE(new tdnSoundSE), m_pAlphabetRenderer(new AlphabetRenderer)
{
	m_pSE->Set(0, 1, "DATA/Sound/SE/entry.wav");

	memset(m_InputName, '\0', sizeof(m_InputName));


}
sceneEntry::NoDataState::NameInput::~NameInput()
{
	// 解放
	delete m_pAlphabetRenderer;
	delete m_pSE;
}
void sceneEntry::NoDataState::NameInput::Update()
{
	if (m_bEnd)
	{
		// SEの再生終わったら
		if (!m_pSE->isPlay(0, 0))
		{
			// データ新規作成
			PlayerDataMgr->NewCreate(m_InputName);
			m_parent->m_bLoadSaveData = true;
		}
	}
	else
	{
		// 文字代入
		m_InputName[m_InputStep] = EP_AlphabetTable[m_cursor];

		// 上下で名前カーソル移動
		if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::LEFT))
		{
			g_pSE->Play("カーソル2");
			if (--m_cursor < 0) m_cursor = _countof(EP_AlphabetTable) - 1;
		}
		else if (InputMIDIMgr->GetEPSelectInput(EP_INPUT::RIGHT))
		{
			g_pSE->Play("カーソル2");
			if (++m_cursor >= _countof(EP_AlphabetTable)) m_cursor = 0;
		}

		// エンターで文字決定
		if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::ENTER))
		{
			// 完了を押すか、10文字全部入力してたら
			if (m_InputName[m_InputStep] == '\0' || ++m_InputStep >= 10)
			{
				m_bEnd = true;
				g_pSE->Play("カーソル2");	// エントリー音
			}
			else m_InputName[m_InputStep] = EP_AlphabetTable[m_cursor];
		}

		// スペースでバック
		else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BACK))
		{
			// 最初の文字じゃなかったら
			if (m_InputStep > 0)
			{
				// データ新規作成
				m_InputName[m_InputStep--] = '\0';

				// 前に戻る文字のカーソル取得
				m_cursor = m_pAlphabetRenderer->GetAlphabetID(m_InputName[m_InputStep]);
			}
		}
	}
}

void sceneEntry::NoDataState::NameInput::Render()
{
	static const int OFFSET_X = 320;	// 名前開始地点のX座標
	static const int OFFSET_Y = 320;	// 名前開始地点のY座標

	// 入力「した」文字描画
	for (int i = 0; i < m_InputStep; i++)
	{
		m_pAlphabetRenderer->Render(OFFSET_X + i * 64, 320, m_InputName[i]);
	}

	// 入力「する」文字描画
	{
		// 行の数
		const int MAX_COLUMN = 5;										// 行の最大数
		const int column = min(_countof(EP_AlphabetTable), MAX_COLUMN);	// 行の数
		const int ColumnCenter = column / 2;							// 行の数から真ん中を選出

		// 行の描画
		for (int i = 0; i < column; i++)
		{

			const int width = (ColumnCenter - i) * -64;	// 行の真ん中を基準点として、そこからずれる量(真ん中ならもちろん0)
			const int posY = OFFSET_Y + width;

			// 自分のカーソルでの前後を取得
			int index = m_cursor + (i - ColumnCenter);

			// 下に行きすぎたら(画面では上)
			if (index < 0)
			{
				// 全アルファベット分を足すことによって、過剰分をずらせる
				index = _countof(EP_AlphabetTable) + index;
			}

			// 上に行きすぎたら(画面では下)
			else if (index >= _countof(EP_AlphabetTable))
			{
				// 全アルファベット分を足すことによって、過剰分をずらせる
				index -= (_countof(EP_AlphabetTable));
			}

			BYTE alpha = (BYTE)(255 * (1 - (abs(width) / (float)ColumnCenter)));
			alpha = min(alpha + 64, 255);

			m_pAlphabetRenderer->Render(OFFSET_X + m_InputStep * 64, posY, EP_AlphabetTable[index]);
		}
	}
}