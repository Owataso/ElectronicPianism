#include "TDNLIB.h"
#include "../Music/MusicInfo.h"
#include "system/Framework.h"
#include "../Input/InputMIDI.h"
#include "sceneEdit.h"
#include "../Data/EditData.h"
#include "../Fade/Fade.h"
#include "../MIDI_Loader/MIDI_Loader.h"
#include "../Sound/SoundManager.h"
#include "../Music/OMS_Loader.h"
#include "../Music/PlayCursor.h"
#include "../Music/Note.h"
#include "../UI/ui.h"
#include "../Music/BeatBar.h"
#include "../Music/Soflan.h"
#include "../Player/Player.h"
#include "../Judge/Judge.h"
#include "../Data/PlayerData.h"
#include "../Music/EnumString.h"
#include "../Data/MusicDataBase.h"
#include "../Data/SelectMusicData.h"
#include "sceneModeSelect.h"
#include<iostream>


//*****************************************************************************************************************************
//
//		初		期		化	
//
//*****************************************************************************************************************************
bool sceneEdit::Initialize()
{
	//PlayerDataMgr->Initialize();

	tdnView::Init();

	Fade::Initialize();
	Fade::Set(Fade::MODE::FADE_IN, 5, 0, 255, 0x00000000);

	EditDataMgr->Initialize();

	m_fLoadPercentage = .25f;	// ロード割合

	m_pMode = nullptr;
	ChangeMode(MODE::FIRST);

	m_fLoadPercentage = .35f;	// ロード割合

	m_pNoteMgr = new NoteManager;
	m_pSoflanMgr = new SoflanManager;
	m_pBeatBar = new BeatBar;

	m_fLoadPercentage = .4f;	// ロード割合

	//me->m_pSoflanMgr->Initialize();
	m_pNoteMgr->Initialize(m_pSoflanMgr);
	m_pUI = new UI(MAX_OCTAVE);
	m_pUI->Initialize(m_pBeatBar, m_pNoteMgr, m_pSoflanMgr, GAUGE_OPTION::NORMAL);

	m_fLoadPercentage = .5f;	// ロード割合

	m_pJudge = new JudgeManager();
	m_pJudge->Initialize(m_pNoteMgr, GAUGE_OPTION::NORMAL, true);

	m_fLoadPercentage = .65f;	// ロード割合

	m_pPlayer = new Player();
	m_pPlayer->Initialize(MAX_OCTAVE, m_pUI, m_pNoteMgr, m_pJudge, Player::MODE::AUTO);

	m_fLoadPercentage = .7f;	// ロード割合

	// プレイカーソル
	play_cursor->Initialize();

	m_fLoadPercentage = .9f;	// ロード割合

	m_iShift = 0;

	// 曲情報参照
	m_MusicInfo = SelectMusicMgr->Get();

	m_FolderInfo = nullptr;

	m_fLoadPercentage = 1.0f;	// ロード割合

	return true;
}

sceneEdit::~sceneEdit()
{
	play_cursor->Release();
	delete m_pMode;
	EditDataMgr->Release();
	delete m_pNoteMgr;
	delete m_pUI;
	delete m_pSoflanMgr;
	delete m_pPlayer;
	delete m_pJudge;
	delete m_pBeatBar;
	SAFE_DELETE(m_FolderInfo);
}

//*****************************************************************************************************************************
//
//		主処理
//
//*****************************************************************************************************************************
bool sceneEdit::Update()
{
	m_pMode->Update();

	Fade::Update();

	return true;
}

//*****************************************************************************************************************************
//
//		描画関連
//
//*****************************************************************************************************************************
void	sceneEdit::Render( void )
{
	
	tdnView::Clear();

	m_pMode->Render();

	Fade::Render();
}

//*****************************************************************************************************************************
//
//		(process1)最初に、新規作成かロードするか選ぶ
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
void sceneEdit::Mode::First::Initialize()
{
	me->m_iSelectDifficulty = 0;
}
//------------------------------------------------------
//	更新
//------------------------------------------------------
void sceneEdit::Mode::First::Update()
{
	if (KeyBoardTRG(KB_SPACE))
	{
		//me->ChangeMode(MODE::NEW_CREATE);
		MainFrame->ChangeScene(new sceneModeSelect, true);
	}

	else if (KeyBoardTRG(KB_ENTER))
	{
		me->ChangeMode(MODE::DIRECTORY_SELECT);
	}
}

//------------------------------------------------------
//	描画
//------------------------------------------------------
void sceneEdit::Mode::First::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE :　新規作成 or ロード");
	tdnText::Draw(32, 70, 0xffffff00, "[SPACE]: エディットモード終了　　　[ENTER]: ロード");
}

//*****************************************************************************************************************************
//
//		(process2A)新規フォルダ作成(ユーザーに名前を決めさせ、ディレクトリーを作成)
//
//*****************************************************************************************************************************
bool sceneEdit::Mode::NewCreateDirectry::bThread = false;

//------------------------------------------------------
//	初期化
//------------------------------------------------------
void sceneEdit::Mode::NewCreateDirectry::Initialize()
{
	// 名前入力マルチスレッド展開
	bThread = true;
	_beginthread(NameInput, 0, (void*)&me->m_sMusicName);	// 曲名を参照渡しで書き換える

}
//------------------------------------------------------
//	更新
//------------------------------------------------------
void sceneEdit::Mode::NewCreateDirectry::Update()
{
	switch (m_step)
	{

	case 0:	/* 名前入力スレッド流れ中 */
		// 名前入力終了してたら
		if (!bThread)
		{
			m_step++;
		}
		break;

	case 1:	/* 本当にこの名前でいいですか？ */
		if (KeyBoardTRG(KB_ENTER)) m_step++;
		else if (KeyBoardTRG(KB_SPACE))
		{
			// 最初に戻る
			me->ChangeMode(MODE::FIRST);

			//m_step--;
			// もっかいスレッド作成
			//bThread = true;
			//_beginthread(NameInput, 0, (void*)&me->m_MusicName);
		}

		break;
	case 2:
		std::string path = "Edit/" + me->m_sMusicName;
		int result = tdnFile::CreateFolder((char*)path.c_str());
		if (result != 0)
		{
			// つくられなかった！
			MessageBox(0, "その曲のフォルダは既に作成されている可能性があります", "エラー", MB_OK);
			me->ChangeMode(MODE::FIRST);
		}
		else
		{
			// ちゃんとつくられた！
			me->ChangeMode(MODE::DIFFICULTY_SELECT);
		}
		break;
	}
}

//------------------------------------------------------
//	描画
//------------------------------------------------------
void sceneEdit::Mode::NewCreateDirectry::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE : ディレクトリ作成");

	// 本当にこの名前でいいですか？
	if (m_step == 1)
	{
		tdnText::Draw(320, 160, 0xffffff00, "本当にこの名前でいいですか？");
		tdnText::Draw(320, 240, 0xffffff00, "%s", me->m_sMusicName.c_str());
		tdnText::Draw(320, 320, 0xffffff00, "エンター: OK　　　スペース: やっぱやりなおす");
	}
}



//------------------------------------------------------
//	スレッド用名前入力関数
//------------------------------------------------------
void sceneEdit::Mode::NewCreateDirectry::NameInput(void *arg)
{
	// コンソールで曲名を入力させる
	tdnSystem::OpenDebugWindow();
	std::cout << "★☆曲名を入力してください☆★\n" << std::endl;
	std::cout << "※注意: スペースで空白を入れるのは厳禁です。代わりに_を使用してください！" << std::endl;
	std::cout << "_が空白に置換されます\n\n>";
	char str[128];
	//std::cin.getline(str, sizeof(str));//スペースを含む入力を、ちぎらず受け付ける
	std::cin >> str;

	// アンダーバーを空白に置換する処理
	for (int i = 0; str[i] != '\0'; i++) str[i] = (str[i] == '_') ? ' ' : str[i];

	// 引数のstring型にぶちこむ
	*((std::string*)arg) = str;

	// コンソール終了
	tdnSystem::CloseDebugWindow();

	// スレッド終了
	bThread = false;
	_endthread();
}


//*****************************************************************************************************************************
//
//		(process2B)曲フォルダの選択
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
void sceneEdit::Mode::DirectrySelect::Initialize()
{
	// ディレクトリ情報初期化
	if (!m_pDirectry)m_pDirectry = new DirectoryInfo;
	m_pDirectry->FileNames.clear();
	m_pDirectry->FolderNames.clear();

	// 譜面データフォルダのファイルパスから曲フォルダを列挙
	tdnFile::EnumDirectory("Edit", m_pDirectry);
	assert(m_pDirectry->FolderNames.size() != 0); // 何もフォルダが作られていない！
}
//------------------------------------------------------
//	更新
//------------------------------------------------------
void sceneEdit::Mode::DirectrySelect::Update()
{
	if (KeyBoardTRG(KB_UP))
	{
		// カーソルを上に
		if (--m_SelectCursor < 0) m_SelectCursor = m_pDirectry->FolderNames.size() - 1;
	}
	else if (KeyBoardTRG(KB_DOWN))
	{
		// カーソルを下に
		if (++m_SelectCursor >= (int)m_pDirectry->FolderNames.size()) m_SelectCursor = 0;
	}

	else if (KeyBoardTRG(KB_ENTER))
	{
		// フォルダー名登録して次のモード()へ
		me->m_sMusicName = m_pDirectry->FolderNames[m_SelectCursor];
		me->ChangeMode(MODE::DIFFICULTY_SELECT);
	}
}

//------------------------------------------------------
//	描画
//------------------------------------------------------
void sceneEdit::Mode::DirectrySelect::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE : フォルダ選択");
	tdnText::Draw(32, 70, 0xffffff00, "曲数 : %d", m_pDirectry->FolderNames.size());

	// 曲リストの描画
	{
		// 行の数
		const int MAX_COLUMN = 10;												// 行の最大数
		const int column = min(m_pDirectry->FolderNames.size(), MAX_COLUMN);	// 行の数
		const int ColumnCenter = column / 2;									// 行の数から真ん中を選出

		// 行の描画
		for (int i = 0; i < column; i++)
		{
			const int width = ColumnCenter - i;
			const int add = width * -30;
			const int posY = 320 + add;

			int index = m_SelectCursor + (i - ColumnCenter);
			if (index >= (int)m_pDirectry->FolderNames.size())
			{
				index -= (m_pDirectry->FolderNames.size());
			}
			else if (index < 0)
			{
				index = m_pDirectry->FolderNames.size() + index;
			}

			BYTE alpha = (BYTE)(255 * (1 - (abs(width) / (float)ColumnCenter)));
			alpha = min(alpha + 64, 255);

			const DWORD col = (i == ColumnCenter) ? 0x0000ffff : 0x00ffffff;

			tdnText::Draw(640, posY,
				(col | alpha << 24),
				"%s", tdnFile::GetFileName((char*)m_pDirectry->FolderNames[index].c_str(), false).c_str());
		}
	}
}


//*****************************************************************************************************************************
//
//		(process3)難易度の選択
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
void sceneEdit::Mode::DifficultySelect::Initialize()
{
	// エディット曲のフォルダーの情報を作成
	if (me->m_FolderInfo) delete me->m_FolderInfo;
	me->m_FolderInfo = new EditFolderInfo((char*)me->m_sMusicName.c_str());

	me->bMIDILoad = false;
	me->m_iPlayMusicNo = 0;
	me->m_iPlayTimer = 90;	// 1分30秒
}
//------------------------------------------------------
//	更新
//------------------------------------------------------
void sceneEdit::Mode::DifficultySelect::Update()
{
	if (KeyBoardTRG(KB_UP))
	{
		// カーソルを上に
		if (--me->m_iSelectDifficulty < 0) me->m_iSelectDifficulty = (int)DIFFICULTY::MAX - 1;
	}
	else if (KeyBoardTRG(KB_DOWN))
	{
		// カーソルを下に
		if (++me->m_iSelectDifficulty >= (int)DIFFICULTY::MAX) me->m_iSelectDifficulty = 0;
	}

	else if (KeyBoardTRG(KB_ENTER))
	{
		if (me->m_FolderInfo->bDifficultyExistance[me->m_iSelectDifficulty])
		{
			switch (MessageBox(0, "この難易度はすでに作られています\nこの難易度のエディットデータを新規で作らず、ロードしますか？", "ここ何て書こう", MB_YESNOCANCEL))
			{
			case IDYES:
				me->bMIDILoad = true;

				// omsをロードする
				EditDataMgr->LoadOMS_MakeMusicScore((me->m_FolderInfo->sMusicName + "/" + ENUM_STRING.m_DifficultyMap[(DIFFICULTY)me->m_iSelectDifficulty] + ".oms").c_str());

				// シフト情報
				me->m_iShift = EditDataMgr->GetMusicInfo()->omsInfo.shift;

				// 再生する曲の番号
				me->m_iPlayMusicNo = EditDataMgr->GetMusicInfo()->omsInfo.PlayMusicNo;

				// 再生時間
				me->m_iPlayTimer = EditDataMgr->GetMusicInfo()->omsInfo.PlayTime;

				// 難易度
				SelectMusicMgr->Get()->eDifficulty = (DIFFICULTY)me->m_iSelectDifficulty;

				// 再生する曲の設定
				if (me->m_iPlayMusicNo != 0)
				{
					char path[256];
					sprintf_s(path, 256, "%s/Music%d.ogg", me->m_sMusicName.c_str(), me->m_iPlayMusicNo);
					play_cursor->SetMusic(path);
				}

				break;
			
			case IDNO:
				break;
			
			case IDCANCEL:
				return;
				break;
			}
			
		}

		// いろいろ設定モードへ
		me->ChangeMode(MODE::SETTING_INFO);
	}

	else if (KeyBoardTRG(KB_SPACE))
	{
		// 戻る
		me->ChangeMode(MODE::FIRST);
	}
}

//------------------------------------------------------
//	描画
//------------------------------------------------------
void sceneEdit::Mode::DifficultySelect::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE : 難易度選択");

	// 難易度の描画
	for (int i = 0; i < (int)DIFFICULTY::MAX; i++)
	{
		const int posY = 320 + i * 30;

		tdnText::Draw(640, posY, 0xffffffff, "%s", ENUM_STRING.m_DifficultyMap[(DIFFICULTY)i]);
	}
	tdnText::Draw(600, 320 + me->m_iSelectDifficulty * 30, 0xffffffff, "→");
}


//*****************************************************************************************************************************
//
//		(process4A)いろんな情報を設定していく
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
void sceneEdit::Mode::SettingInfo::Initialize()
{

}
//------------------------------------------------------
//	更新
//------------------------------------------------------
void sceneEdit::Mode::SettingInfo::Update()
{
	if (KeyBoardTRG(KB_1))
	{
		me->ChangeMode(MODE::SETTING_WAV);
	}
	else if (KeyBoardTRG(KB_2))
	{
		//// オープンファイルダイアログを使ってファイルパス取得
		std::string path = me->m_FolderInfo->sMusicName;
			//tdnFile::OpenFileDialog("MIDIファイル(*.mid)\0 *.mid\0\0");
		//
		//// ダイアログキャンセルしたら出ていけぇ！
		//if (path == "") return;

		if (me->bMIDILoad)
		{
			if (MessageBox(0, "すでに譜面データが入っています。上書きしますか？", "ElectronicPisnism", MB_OKCANCEL) == IDCANCEL)	// キャンセルを選択
			{
				return;
			}
		}

		// 現在の難易度からパス作成
		static const char *MIDINames[(int)DIFFICULTY::MAX] = { "N", "H", "E", "V" };

		path = path + "/other/" + MIDINames[me->m_iSelectDifficulty] + ".mid";

		// ★ファイルパスからMIDIデータを読んで譜面作成する
		EditDataMgr->LoadMIDI_MakeMusicScore(path.c_str());

		// m_iShift上書き(0になってしまうの防止)
		EditDataMgr->GetMusicInfo()->omsInfo.shift = me->m_iShift;

		// 読み込みフラグON
		me->bMIDILoad = true;
	}

	else if (KeyBoardTRG(KB_SPACE))
	{
		// 難易度選択モードに移行
		me->ChangeMode(MODE::DIFFICULTY_SELECT);
	}

	else if (KeyBoardTRG(KB_ENTER))
	{
		// デバッグ用
		// ★ファイルパスからMIDIデータを読んで譜面作成する
		//EditDataMgr->LoadMIDI_MakeMusicScore("DATA/Musics/EP/No_0/collision_of_elements.mid");
		//EditDataMgr->LoadOMS_MakeMusicScore("DATA/Musics/Edit/airu/NORMAL.oms");

		// 読み込みフラグON
		//me->bMIDILoad = true;

		// MIDI読んでたら
		if (me->bMIDILoad)
		{
			// ファイルパスからMIDIデータを読んで譜面作成する
			//EditDataMgr->LoadMIDI_MakeMusicScore(me->m_MIDIName);

			// 音源データがあるなら設定する
			//if (me->m_PlayMusicNo != -1)
			//{
			//	EditDataMgr->SetWavData(me->m_WavName);
			//}
			//EditDataMgr->SetMusicName("テスト");

			// 難易度設定
			me->m_MusicInfo->eDifficulty = (DIFFICULTY)me->m_iSelectDifficulty;

			// 読み込みOKモードに移行
			me->ChangeMode(MODE::LOAD_OK);
			InputMIDIMgr->SetBaseOctave(me->m_MusicInfo->omsInfo.BaseOctave);
		}
		else
		{
			MessageBox(0, "譜面データが読み込まれていません。(MIDIデータを読み込んでください)", "エラー", MB_OK);
		}
	}
}

//------------------------------------------------------
//	描画
//------------------------------------------------------
void sceneEdit::Mode::SettingInfo::Render()
{
	tdnText::Draw(1000, 30, 0xffffff00, "難易度: %s", ENUM_STRING.m_DifficultyMap[(DIFFICULTY)me->m_iSelectDifficulty]);
	tdnText::Draw(32, 30, 0xffffff00, "MODE : いろいろ設定");
	tdnText::Draw(32, 70, 0xffffff00, "[1]: 曲情報設定");
	tdnText::Draw(32, 110, 0xffffff00, "[2]: MIDIファイルを開く");
	tdnText::Draw(32, 150, 0xffffff00, "[ENTER]: NEXT");

	tdnText::Draw(32, 320, 0xffffff00, "MIDIデータ: %s", (me->bMIDILoad) ? "読み込み済み" : "ありません");
	//tdnText::Draw(32, 350, 0xffffff00, "曲のWAVファイルパス: %s", (strcmp("", me->m_WavName) == 0) ? "まだ読み込んでないよ！" : "読み込み済み");
	//tdnText::Draw(32, 390, 0xffffff00, "曲のWAVファイルパス: %s", (strcmp("", me->m_WavName) == 0) ? "まだ読み込んでないよ！" : "読み込み済み");
	//tdnText::Draw(32, 430, 0xffffff00, "MIDIファイルパス: %s", (strcmp("", me->m_MIDIName) == 0) ? "まだ読み込んでないよ！" : "読み込み済み");
}

//*****************************************************************************************************************************
//
//		(process4B)WAVデータの読み込み)
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
void sceneEdit::Mode::SettingWAV::Initialize()
{

}
//------------------------------------------------------
//	更新
//------------------------------------------------------
void sceneEdit::Mode::SettingWAV::Update()
{
	// 再生曲の設定
	if (KeyBoardTRG(KB_Z))
	{
		if (me->m_iPlayMusicNo == 1) me->m_iPlayMusicNo = 0;
		else if (me->m_FolderInfo->bMusicExistance[0]) me->m_iPlayMusicNo = 1;
	}
	else if (KeyBoardTRG(KB_X))
	{
		if (me->m_iPlayMusicNo == 2) me->m_iPlayMusicNo = 0;
		else if (me->m_FolderInfo->bMusicExistance[1]) me->m_iPlayMusicNo = 2;
	}
	else if (KeyBoardTRG(KB_C))
	{
		if (me->m_iPlayMusicNo == 3) me->m_iPlayMusicNo = 0;
		else if (me->m_FolderInfo->bMusicExistance[2]) me->m_iPlayMusicNo = 3;
	}
	else if (KeyBoardTRG(KB_V))
	{
		if (me->m_iPlayMusicNo == 4) me->m_iPlayMusicNo = 0;
		else if (me->m_FolderInfo->bMusicExistance[3]) me->m_iPlayMusicNo = 4;
	}

	// 再生時間の増減
	if (KeyBoard(KB_RIGHT) && me->m_iPlayMusicNo == 0) me->m_iPlayTimer += 1;
	else if (KeyBoard(KB_LEFT) && me->m_iPlayMusicNo == 0 && me->m_iPlayTimer > 1) me->m_iPlayTimer -= 1;

	int SelectNo(-1);
	if (KeyBoardTRG(KB_1)) SelectNo = 1;
	else if (KeyBoardTRG(KB_2)) SelectNo = 2;
	else if (KeyBoardTRG(KB_3)) SelectNo = 3;
	else if (KeyBoardTRG(KB_4)) SelectNo = 4;
	else if (KeyBoardTRG(KB_5)) SelectNo = 5;

	// このフレーム中に何か選ばれてたら
	if (SelectNo != -1)
	{
		// オープンファイルダイアログを使ってファイルパス取得
		std::string path = tdnFile::OpenFileDialog("WAVファイル(*.wav)\0 *.wav\0\0");

		// ダイアログキャンセルしたら出ていけぇ！
		if (path == "") return;

		char str[256];

		// 試聴用
		if (SelectNo == 5)
		{
			if (me->m_FolderInfo->bAudition)
			{
				if (MessageBox(0, "すでに曲データが入っています。上書きしますか？", path.c_str(), MB_OKCANCEL) == IDCANCEL)	// キャンセルを選択
				{
					return;
				}
			}

			// フォルダー登録フラグON
			me->m_FolderInfo->bAudition = true;

			// 書き出しパス作成
			sprintf_s(str, 256, "%s/Audition.owd", me->m_FolderInfo->sMusicName.c_str());
		}
		// 曲データ
		else if (SelectNo < 4)
		{
			if (me->m_FolderInfo->bMusicExistance[SelectNo])
			{
				if (MessageBox(0, "すでに曲データが入っています。上書きしますか？", path.c_str(), MB_OKCANCEL) == IDCANCEL)	// キャンセルを選択
				{
					return;
				}
			}

			// フォルダー登録フラグON
			me->m_FolderInfo->bMusicExistance[SelectNo-1] = true;

			// 書き出しパス作成
			sprintf_s(str, 256, "%s/Music%d.owd", me->m_FolderInfo->sMusicName.c_str(), SelectNo);
		}
		else assert(0);

		// OWDデータとして音源データ書き出し
		WriteOWD((char*)path.c_str(), str);
	}

	if (KeyBoardTRG(KB_SPACE))
	{
		// ★再生くんに曲情報設定
		if (me->m_iPlayMusicNo == 0)
		{
			// 何も流さない
			play_cursor->SetMusic(nullptr);
		}
		else
		{
			// パス作成
			char path[256];
			sprintf_s(path, 256, "%s/Music%d.ogg", me->m_FolderInfo->sMusicName.c_str(), me->m_iPlayMusicNo);

			// 設定
			play_cursor->SetMusic(path);
		}

		// モードチェンジ
		me->ChangeMode(MODE::SETTING_INFO);
	}

}

//------------------------------------------------------
//	描画
//------------------------------------------------------
void sceneEdit::Mode::SettingWAV::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE : WAVデータ設定モード");
	tdnText::Draw(32, 70, 0xffffff00, "[1][2][3][4]: 曲に使用する音源データ読み込み");
	tdnText::Draw(32, 110, 0xffffff00, "[5]: 試聴用音源データ読み込み");
	tdnText::Draw(32, 150, 0xffffff00, "[Z][X][C][V]: どの曲を流すか設定");
	tdnText::Draw(32, 190, 0xffffff00, "[SPACE]: 戻る");

	for (int i = 0; i < (int)DIFFICULTY::MAX; i++)
	{
		tdnText::Draw(32, 320 + i * 30, 0xffffff00, "Music%d: %s", i+1, (me->m_FolderInfo->bMusicExistance[i]) ? "読み込み済み" : "ありません");
	}
	tdnText::Draw(32, 450, 0xffffff00, "試聴用データ: %s", (me->m_FolderInfo->bAudition) ? "読み込み済み" : "ありません");

	char str[64];

	// 流す曲が設定してある
	if (me->m_iPlayMusicNo != 0)
	{
		sprintf_s(str, 64, "Music%d", me->m_iPlayMusicNo);
	}

	// 何も流さない
	else
	{
		tdnText::Draw(640, 490, 0xffffff00, "再生時間: %d分%d秒", me->m_iPlayTimer / 60, me->m_iPlayTimer % 60);
		tdnText::Draw(32, 230, 0xffffff00, "[左右キー]: 再生時間変更");
		sprintf_s(str, 64, "流さない");
	}

	tdnText::Draw(32, 490, 0xffffff00, "この難易度での流す曲: %s", str);
}


//*****************************************************************************************************************************
//
//		(process5A)ロード完了(ここから判定の調整⇔テストプレイ、データ書き出しを行う)
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
void sceneEdit::Mode::LoadOK::Initialize()
{

}

//------------------------------------------------------
//	更新
//------------------------------------------------------
void sceneEdit::Mode::LoadOK::Update()
{
	// 上下キーで譜面と曲の差を調整
	if (KeyBoard(KB_DOWN))
	{
		me->m_iShift -= 5;
		// 書き出しくんにデータに補正する値を設定
		EditDataMgr->SetShift(me->m_iShift);
	}
	else if (KeyBoard(KB_UP))
	{
		me->m_iShift += 5;
		// 書き出しくんにデータに補正する値を設定
		EditDataMgr->SetShift(me->m_iShift);
	}

	// エンターキー
	if (KeyBoardTRG(KB_ENTER))
	{
		// 再生モードに移行
		me->ChangeMode(MODE::PLAYING);
	}

	// スペースキー
	else if (KeyBoardTRG(KB_1))
	{
		// キャンセル押したら出ていけぇ！
		if (MessageBox(0, "書き出しますか？", "ファイルの保存", MB_OKCANCEL) == IDCANCEL) return;

		// 再生番号設定
		EditDataMgr->GetMusicInfo()->omsInfo.PlayMusicNo = me->m_iPlayMusicNo;

		// 再生時間設定
		EditDataMgr->GetMusicInfo()->omsInfo.PlayTime = me->m_iPlayTimer;

		// レベル設定
		//EditDataMgr->GetMusicInfo()->omsInfo.level = me->level;

		// 譜面とか曲データをファイルとして書き出す！
		std::string path = me->m_FolderInfo->sMusicName;

		// ここマジで何とかしたい
		//if (me->m_SelectDifficulty == (int)DIFFICULTY::NORMAL) path += "Normal.oms";
		//else if (me->m_SelectDifficulty == (int)DIFFICULTY::HYPER) path += "Hyper.oms";
		//else if (me->m_SelectDifficulty == (int)DIFFICULTY::EXPERT) path += "Expert.oms";
		//else if (me->m_SelectDifficulty == (int)DIFFICULTY::VIRTUOSO) path += "Virtuoso.oms";

		path = path + "/" + ENUM_STRING.m_DifficultyMap[(DIFFICULTY)me->m_iSelectDifficulty] + ".oms";

		// ★データ書き出し！
		EditDataMgr->WriteMusicScore((char*)path.c_str());
	}

	else if (KeyBoardTRG(KB_SPACE))
	{
		// 設定モードに移行
		me->ChangeMode(MODE::SETTING_INFO);
	}
}

//------------------------------------------------------
//	描画
//------------------------------------------------------
void sceneEdit::Mode::LoadOK::Render()
{
	tdnText::Draw(32, 30, 0xffffff00, "MODE : ロード完了");
	tdnText::Draw(32, 70, 0xffffff00, "上下キー: 譜面と曲の調整 : %d", me->m_iShift);
	tdnText::Draw(32, 150, 0xffffff00, "エンター: 再生開始");
	tdnText::Draw(32, 190, 0xffffff00, "[1]: データ書き出し");
	tdnText::Draw(32, 230, 0xffffff00, "スペース: 戻る");
	tdnText::Draw(32, 270, 0xffffff00, "総ノーツ数: %d", me->m_MusicInfo->omsInfo.NumNotes);
	tdnText::Draw(32, 310, 0xffffff00, "オクターブ幅: %d", me->m_MusicInfo->omsInfo.OctaveWidth);
}



//*****************************************************************************************************************************
//
//		(process5B)再生状態
//
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
void sceneEdit::Mode::Playing::Initialize()
{
	// 再生位置の初期化
	//play_cursor->Initialize();

	me->m_pUI->SetOctaveWidth(me->m_MusicInfo->omsInfo.OctaveWidth);

	// 再生
	play_cursor->SetShift(me->m_iShift);
	play_cursor->Play(me->m_iPlayTimer * 1000);
	//play_cursor->SetSpeed(1.27f);

	// ソフランとノーツの情報をセッティング
	me->m_pSoflanMgr->Set(me->m_MusicInfo);
	me->m_pNoteMgr->Set(me->m_MusicInfo);

	// スコアとコンボ数リセット
	me->m_pJudge->Reset();
	me->m_pBeatBar->Reset();
}

//------------------------------------------------------
//	更新
//------------------------------------------------------
void sceneEdit::Mode::Playing::Update()
{
	me->m_pBeatBar->Update(play_cursor->GetMSecond());

	// ノーツ更新
	me->m_pNoteMgr->Update(me->m_pJudge);

	// ソフラン更新(★BPMが切り替わったらtrueが返るので、rhythmバーを切り替えてあげる)
	if (me->m_pSoflanMgr->Update()) me->m_pBeatBar->SetBPM(me->m_pSoflanMgr->GetBPM());

	// 演奏者更新
	me->m_pPlayer->Update();

	// 判定更新
	me->m_pJudge->Update();

	if (KeyBoardTRG(KB_ENTER))
	{
		play_cursor->Stop();
		me->ChangeMode(MODE::LOAD_OK);
		return;
	}

	if (play_cursor->isEnd())
	{
		play_cursor->Stop();
		me->ChangeMode(MODE::LOAD_OK);
	}
	else
	{
		static float speed = 1.0f;
		if (KeyBoardTRG(KB_NUMPAD8))
		{
			play_cursor->SetSpeed((speed += .08f));
		}
		else if (KeyBoardTRG(KB_NUMPAD2))
		{
			play_cursor->SetSpeed((speed -= .08f));
		}
	}
}

//------------------------------------------------------
//	描画
//------------------------------------------------------
void sceneEdit::Mode::Playing::Render()
{
	// UI(ノートとかも)描画
	me->m_pUI->RenderBack();
	me->m_pUI->Render();
	me->m_pUI->RenderMusicName((char*)tdnFile::GetFileName((char*)me->m_sMusicName.c_str(), false).c_str());
	tdnText::Draw(1090, 78, 0xffffffff, "EDIT");

	// 判定描画
	me->m_pJudge->Render();
	//Text::Draw(32, 64, 0xffffff00, "BPM : %.1f");
}