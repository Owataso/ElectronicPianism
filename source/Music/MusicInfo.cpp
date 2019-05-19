
#include "MusicInfo.h"

//------------------------------------------------------
//	ランクチェック
//------------------------------------------------------
SCORE_RANK ScoreRankCheck(int score){ for (int i = 0; i<(int)SCORE_RANK::MAX; i++){ if (score >= SCORE_BORDER[i])return (SCORE_RANK)i; }return SCORE_RANK::F; }

//------------------------------------------------------
//	フォルダー情報構造体初期化
//------------------------------------------------------
EditFolderInfo::EditFolderInfo(LPCSTR Filename): bAudition(false)
{
	sMusicName = Filename;

	// 配列初期化
	memset(bDifficultyExistance, false, (size_t)DIFFICULTY::MAX);
	memset(bMusicExistance, false, (size_t)DIFFICULTY::MAX);

	// フォルダーの中身を列挙する
	DirectoryInfo *di = new DirectoryInfo;
	//std::string path = "DATA/Musics/";
	//path += music_name;
	tdnFile::EnumDirectory(Filename, di, false);

	// 列挙したファイルを見て情報を設定
	for (UINT i = 0; i < di->FileNames.size(); i++)
	{
		if (di->FileNames[i] == "NORMAL") bDifficultyExistance[(int)DIFFICULTY::NORMAL] = true;
		else if (di->FileNames[i] == "HYPER") bDifficultyExistance[(int)DIFFICULTY::HYPER] = true;
		else if (di->FileNames[i] == "EXPERT") bDifficultyExistance[(int)DIFFICULTY::EXPERT] = true;
		else if (di->FileNames[i] == "VIRTUOSO") bDifficultyExistance[(int)DIFFICULTY::VIRTUOSO] = true;
		else if (di->FileNames[i] == "Audition") bAudition = true;
		else if (di->FileNames[i] == "Music1") bMusicExistance[(int)DIFFICULTY::NORMAL] = true;
		else if (di->FileNames[i] == "Music2") bMusicExistance[(int)DIFFICULTY::HYPER] = true;
		else if (di->FileNames[i] == "Music3") bMusicExistance[(int)DIFFICULTY::EXPERT] = true;
		else if (di->FileNames[i] == "Music4") bMusicExistance[(int)DIFFICULTY::VIRTUOSO] = true;
		else MyAssert(false,"エラー: フォルダーの中身列挙\nもしかして Music1がmusic1になってたり");	// 例外処理
	}
	delete di;
}