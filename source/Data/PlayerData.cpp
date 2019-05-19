#include "TDNLIB.h"
#include "../Music/MusicInfo.h"
#include "CustomizeData.h"
#include "PlayerData.h"
#include "../Input/InputMIDI.h"

//------------------------------------------------------
//	初期化
//------------------------------------------------------
bool PlayerDataManager::Initialize()
{
	//ClearData();
	//SavePlayerData();
	//return true;
	
	// この関数でプレイ情報を読み込み。ついでにデータがない(新規作成させる)かのboolを返す
	return LoadPlayerData();
}

//------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------
PlayerDataManager::PlayerDataManager() :m_bSaveFlag(true),
m_StartClock(clock()),	// 現在時刻保存
m_grade(GRADE::NONE)
{
	// 段位記録
	m_GradeRecords = new PlayerData::GradeRecord*[(int)GRADE::MAX];
	for (int i = 0; i < (int)GRADE::MAX; i++) m_GradeRecords[i] = new PlayerData::GradeRecord;

	// 曲解禁フラグ
	m_bUnlockMusics = new Mybool[MAX_MUSIC];

	// 曲記録
	m_MusicRecords = new PlayerData::MusicRecord*[MAX_MUSIC];
	for (int i = 0; i < MAX_MUSIC; i++) m_MusicRecords[i] = new PlayerData::MusicRecord[(int)DIFFICULTY::MAX];

	// スキン記録
	m_CustomizeRecords = new PlayerData::CustomizeRecord*[(int)CUSTOMIZE_TYPE::MAX];
	for (int i = 0; i < (int)CUSTOMIZE_TYPE::MAX; i++) m_CustomizeRecords[i] = new PlayerData::CustomizeRecord;
}

//------------------------------------------------------
//	デストラクタ
//------------------------------------------------------
PlayerDataManager::~PlayerDataManager()
{
	// セーブフラグあったらデータ記録
	if (m_bSaveFlag)
		SavePlayerData();

	// ポインタの開放

	// 段位記録解放
	DOUBLE_POINTER_DELETE(m_GradeRecords, (int)GRADE::MAX);

	// 曲の解禁フラグ解放
	delete[] m_bUnlockMusics;

	// 曲の記録解放
	FOR(MAX_MUSIC)delete[] m_MusicRecords[i];
	delete[] m_MusicRecords;

	// スキン記録解放
	DOUBLE_POINTER_DELETE(m_CustomizeRecords, (int)CUSTOMIZE_TYPE::MAX);
}

//------------------------------------------------------
//	セーブデータ読み込み
//------------------------------------------------------
bool PlayerDataManager::LoadPlayerData()
{
	// バイナリ読み込みくん
	std::ifstream ifs("DATA/Save/data.bin", std::ios::binary);

	//assert(ifs);	// セーブデータない！
	if (!ifs) return false;

	// プレイヤー基本データ読み込み
	ifs.read((char*)&m_PlayerInfo, sizeof(PlayerData::Infomation));

	// プレイヤーのオプションデータ読み込み
	ifs.read((char*)&m_PlayerOption, sizeof(PlayerData::OptionV2));
	/*
	PlayerData::Option proto;
	ifs.read((char*)&proto, sizeof(PlayerData::Option));
	m_PlayerOption.bAutoPlay = proto.bAutoPlay;
	m_PlayerOption.bGuide = proto.bGuide;
	m_PlayerOption.bMoviePlay = proto.bMoviePlay;
	m_PlayerOption.bRegulSpeed = 1;
	m_PlayerOption.bSudden = proto.bSudden;
	m_PlayerOption.HiSpeed = proto.HiSpeed;
	m_PlayerOption.iGaugeOption = proto.iGaugeOption;
	m_PlayerOption.MovieAlpha = proto.MovieAlpha;
	m_PlayerOption.Other1 = 0;
	m_PlayerOption.Other2 = 0;
	m_PlayerOption.Other3 = 0;
	m_PlayerOption.SuddenPos = proto.SuddenPos;
	*/

	// 段位の空き枠読み込み
	ifs.read((char*)&m_OtherData, sizeof(PlayerData::Other));

	// 段位情報読み込み
	for (int i = 0; i < (int)GRADE::MAX; i++)
	{
		ifs.read((char*)m_GradeRecords[i], sizeof(PlayerData::GradeRecord));
	}

	// 今自分が合格している最大の段位を探す
	for (int i = (int)GRADE::MAX - 1; i >= 0; i--)
	{
		if (m_GradeRecords[i]->bPass)
		{
			m_grade = (GRADE)i;
			break;
		}
	}

	// 曲セーブデータ読み込み
	for (int i = 0; i < MAX_MUSIC; i++)
	{
		// 解禁フラグ
		ifs.read((char*)&m_bUnlockMusics[i], sizeof(Mybool));

		// 難易度ぶんの曲の記録(スコアとかクリアランプとか)
		for (int j = 0; j < (int)DIFFICULTY::MAX; j++)
		{
			ifs.read((char*)&m_MusicRecords[i][j], sizeof(PlayerData::MusicRecord));
		}

		//if (i >= 10 && i <= 17)
		//{
		//	//memcpy_s(&m_MusicRecords[i][(int)DIFFICULTY::HYPER], sizeof(PlayerData::MusicRecord), &m_MusicRecords[i][(int)DIFFICULTY::NORMAL], sizeof(PlayerData::MusicRecord));
		//	//ZeroMemory(&m_MusicRecords[i][(int)DIFFICULTY::NORMAL],sizeof(PlayerData::MusicRecord));
		//}
	}

	// スキンセーブデータ読み込み
	for (int i = 0; i < (int)CUSTOMIZE_TYPE::MAX; i++)
	{
		ifs.read((char*)m_CustomizeRecords[i], sizeof(PlayerData::CustomizeRecord));
	}

	return true;		// 読み込み成功
}

//------------------------------------------------------
//	セーブデータ書き込み
//------------------------------------------------------
void PlayerDataManager::SavePlayerData()
{
	m_bSaveFlag = false;

	// プレイ時間加算(ミリ秒→秒→分)
	const DWORD delta(((clock() - m_StartClock) / 1000) / 60);
	m_PlayerInfo.PlayTime += delta;

	// バイナリ書き出しくん
	std::ofstream ofs("DATA/Save/data.bin", std::ios::binary);
	MyAssert(ofs,"デデドン(絶望)\nセーブデータ書き出しに失敗した！");	// まず止まることはないと思うが…

	// プレイヤー基本データ書き出し
	ofs.write((char*)&m_PlayerInfo, sizeof(PlayerData::Infomation));

	// プレイヤーのオプション情報書き出し
	ofs.write((char*)&m_PlayerOption, sizeof(PlayerData::OptionV2));

	// 段位の空き枠書き出し
	ofs.write((char*)&m_OtherData, sizeof(PlayerData::Other));

	// 段位データ書き出し
	FOR((int)GRADE::MAX)
	{
		ofs.write((char*)m_GradeRecords[i], sizeof(PlayerData::GradeRecord));
	}

	// 曲セーブデータ書き出し
	FOR(MAX_MUSIC)
	{
		// 解禁フラグ
		ofs.write((char*)&m_bUnlockMusics[i], sizeof(Mybool));

		// 難易度ぶんの曲の記録(スコアとかクリアランプとか)
		for (int j = 0; j < (int)DIFFICULTY::MAX; j++)
		{
			ofs.write((char*)&m_MusicRecords[i][j], sizeof(PlayerData::MusicRecord));
		}
	}

	// スキンセーブデータ書き出し
	FOR((int)CUSTOMIZE_TYPE::MAX)
	{
		ofs.write((char*)m_CustomizeRecords[i], sizeof(PlayerData::CustomizeRecord));
	}
}

//------------------------------------------------------
//	新規データ作成
//------------------------------------------------------
void PlayerDataManager::NewCreate(char *PlayerName)
{
	// 一旦全消去
	ClearData();

	// 名前登録
	strcpy_s(m_PlayerInfo.PlayerName, 11, PlayerName);
	//sprintf(m_PlayerInfo->PlayerName, "%s", PlayerName);

	// セーブデータを書き込む
	SavePlayerData();
}

//------------------------------------------------------
//	データ全消去
//------------------------------------------------------
void PlayerDataManager::ClearData()
{
	// プレイヤー基本データクリア
	memset(m_PlayerInfo.PlayerName, '\0', sizeof(m_PlayerInfo.PlayerName));
	m_PlayerInfo.PlayCount = 0;
	m_PlayerInfo.PlayTime = 0;

	// オプションデータクリア
	ZeroMemory(&m_PlayerOption, sizeof(PlayerData::OptionV2));
	m_PlayerOption.MovieAlpha = 255;
	m_PlayerOption.bRegulSpeed = TRUE;

	// 段位の空き枠クリア
	ZeroMemory(&m_OtherData, sizeof(PlayerData::Other));

	// 段位情報クリア
	FOR((int)GRADE::MAX) ZeroMemory(m_GradeRecords[i], sizeof(PlayerData::GradeRecord));

	// 曲セーブデータオール0
	FOR(MAX_MUSIC) for (int j = 0; j < (int)DIFFICULTY::MAX; j++)ZeroMemory(&m_MusicRecords[i][j], sizeof(PlayerData::MusicRecord));

	// カスタマイズデータオール0
	FOR((int)CUSTOMIZE_TYPE::MAX)
	{
		m_CustomizeRecords[i]->no = 0;
		m_CustomizeRecords[i]->bUnlock.val = 1;	// フル解禁状態
		memset(m_CustomizeRecords[i], 0, sizeof(PlayerData::CustomizeRecord));
	}
}