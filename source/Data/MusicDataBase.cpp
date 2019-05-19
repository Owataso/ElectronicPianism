#include "tdnlib.h"
#include "../Music/MusicInfo.h"
#include "MusicDataBase.h"
#include "../Music/OMS_Loader.h"
#include "../Music/EnumString.h"

//******************************************************
//	ElectronicPianism収録曲データ読み込み
//******************************************************
void MusicDataBase::LoadEPContents()
{
	// 曲フォルダ内列挙
	tdnFile::EnumDirectory("DATA/Musics/EP", m_EPDirectory);

	// 曲フォルダが空(絶対ないと思うが…)
	assert(!m_EPDirectory->FolderNames.empty());

	const UINT DirectorySize(m_EPDirectory->FolderNames.size());

	// 中にあるフォルダ分確保する
	m_EPContentsData = new FolderInfo*[DirectorySize];

	// 説明
	// フォルダー列挙だと、名前順で列挙するので、2番より先に11番が来てしまう、ので順番通りにするためにfor文いっぱい回さないといけないのだが、その負荷軽減のために、
	// もうすでにリストに突っ込んだ後の情報のフォルダーはスキップしたい。そのためのbool型配列
	bool *bSearchList = new bool[DirectorySize];
	memset(bSearchList, false, sizeof(bool)* DirectorySize);

	for (UINT folder = 0; folder < DirectorySize; folder++)
	{
		// 実体確保
		m_EPContentsData[folder] = new FolderInfo;

		// IDの順番通りに格納する用のfor文
		int MinID(255);
		int MinSearchID(255);
		for (UINT i = 0; i < DirectorySize; i++)
		{
			if (bSearchList[i]) continue;	// もうすでに見つけてたらスキップ(このbool型配列で処理減るかな…？)

			// フォルダー名からID取得
			std::string path(m_EPDirectory->FolderNames[i]);
			char *ctx;// strtok_sの第3引数に使用するのだが、内部的に使用するので深く考えないらしい

			// No_1の.で区切って、右の数字をIDにする
			strtok_s((char*)tdnFile::GetFileName((char*)path.c_str()).c_str(), "_", &ctx);
			const int id(atoi(strtok_s(0, "\0", &ctx)));

			if (MinID >= id)
			{
				MinID = id;
				MinSearchID = i;
				//bSearchList[folder] = true;	// 見つけたフラグオン
				//m_EPContentsData[folder]->id = id;
				//break;
			}
		}
		m_EPContentsData[folder]->byID = MinID;
		bSearchList[MinSearchID] = true;	// 見つけたフラグオン

		// オクターブ情報読み込み
		for (int difficulty = 0; difficulty < (int)DIFFICULTY::MAX; difficulty++)
		{
			const int OctaveWidth(OMS_Loader::GetOctaveWidth((char*)std::string("DATA/Musics/EP/No_" + std::to_string(MinID) + "/" + ENUM_STRING.m_DifficultyMap[(DIFFICULTY)difficulty] + ".oms").c_str()));
			m_EPContentsData[folder]->iOctaveWidths[difficulty] = OctaveWidth;
		}

		// 曲名とBPM情報はそのフォルダのテキストに入ってる
		std::ifstream ifs(std::string("DATA/Musics/EP/No_" + std::to_string(MinID) + "/info.txt"));
		assert(ifs);

		char c;

		// 改行まで文字を読み取る(>>とやると空白で区切りやがるので)
		int count(0);
		while (true)
		{
			assert(++count < 65535);	// 多分無限ループしてる
			ifs.read(&c, 1);
			if (c == '\n') break;
			m_EPContentsData[folder]->sMusicName += c;
		}

		// 曲名が英語(半角)かどうかの情報を取得
		m_EPContentsData[folder]->bNameEnglish = (IsDBCSLeadByte(m_EPContentsData[folder]->sMusicName[0]) == 0);

		// ソート用文字列取得
		char work[128];
		ifs >> work;
		m_EPContentsData[folder]->sSortMusicName = work;
		ifs.read(&c, 1);	// 改行読み取り

		// アーティスト名読み取り
		while (true)
		{
			assert(++count < 65535);	// 多分無限ループしてる
			ifs.read(&c, 1);
			if (c == '\n') break;
			m_EPContentsData[folder]->sArtistName += c;
		}

		// レベル情報読み取り
		for (int j = 0; j < (int)DIFFICULTY::MAX; j++)
		{
			ifs >> m_EPContentsData[folder]->level[j];
		}

		// ジャンル情報読み取り
		int iMusicGenre(0);
		ifs >> iMusicGenre;
		m_EPContentsData[folder]->eMusicGenre = (MUSIC_GENRE)iMusicGenre;

		// BPM情報読み取り
		ifs >> m_EPContentsData[folder]->fMaxBPM;
		ifs >> c;
		ifs >> m_EPContentsData[folder]->fMinBPM;

		// 動画があるかどうか読み取り
		ifs >> work;	// MOVIE文字列読み飛ばし
		ifs >> work;
		if (strcmp(work, "あり") == 0) m_EPContentsData[folder]->bMovieExist = true;
		else if (strcmp(work, "なし") == 0)  m_EPContentsData[folder]->bMovieExist = false;
		else MyAssert(false, "エラー: Info.txtのずれがある");
	}

	delete[] bSearchList;	// 配列解放
}

//******************************************************
//	エディット収録曲データ読み込み
//******************************************************
void MusicDataBase::LoadEditContents()
{
	// エディットフォルダ内列挙
	tdnFile::EnumDirectory("DATA/Musics/Edit", m_EditDirectry);

	// エディットフォルダーが空
	if (m_EditDirectry->FolderNames.empty())return;

	// 中にあるフォルダ分確保する
	m_EditContentsData = new EditFolderInfo*[m_EditDirectry->FolderNames.size()];

	// 総当たり
	for (UINT i = 0; i < m_EditDirectry->FolderNames.size(); i++)
	{
		// ダブルポインタ2段目
		m_EditContentsData[i] = new EditFolderInfo((char*)m_EditDirectry->FolderNames[i].c_str());
	}
}


//******************************************************
//	コンストラクタ(初期化)
//******************************************************
MusicDataBase::MusicDataBase() :m_EditContentsData(nullptr)
{
	m_EPDirectory = new DirectoryInfo;
	m_EditDirectry = new DirectoryInfo;

	// EP収録曲読み込み
	LoadEPContents();

	// エディット譜面データ読み込み
	LoadEditContents();
}


//******************************************************
//	デストラクタ(解放)
//******************************************************
MusicDataBase::~MusicDataBase()
{
	if (m_EditDirectry)DOUBLE_POINTER_DELETE(m_EditContentsData, (int)m_EditDirectry->FolderNames.size());
	if (m_EPDirectory)DOUBLE_POINTER_DELETE(m_EPContentsData, (int)m_EPDirectory->FolderNames.size());

	delete m_EPDirectory;
	delete m_EditDirectry;
}


//******************************************************
//	ゲッター
//******************************************************

// 曲数
int MusicDataBase::GetNumEPContents(){ return m_EPDirectory->FolderNames.size(); }
int MusicDataBase::GetNumEditContents(){ return m_EditDirectry->FolderNames.size(); }

// 曲データ
FolderInfo *MusicDataBase::GetEPContentsData(int no){ return m_EPContentsData[no]; }
EditFolderInfo *MusicDataBase::GetEditContentsData(int no){ return m_EditContentsData[no]; }