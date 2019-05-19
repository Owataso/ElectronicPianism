
#include "MusicInfo.h"

//------------------------------------------------------
//	�����N�`�F�b�N
//------------------------------------------------------
SCORE_RANK ScoreRankCheck(int score){ for (int i = 0; i<(int)SCORE_RANK::MAX; i++){ if (score >= SCORE_BORDER[i])return (SCORE_RANK)i; }return SCORE_RANK::F; }

//------------------------------------------------------
//	�t�H���_�[���\���̏�����
//------------------------------------------------------
EditFolderInfo::EditFolderInfo(LPCSTR Filename): bAudition(false)
{
	sMusicName = Filename;

	// �z�񏉊���
	memset(bDifficultyExistance, false, (size_t)DIFFICULTY::MAX);
	memset(bMusicExistance, false, (size_t)DIFFICULTY::MAX);

	// �t�H���_�[�̒��g��񋓂���
	DirectoryInfo *di = new DirectoryInfo;
	//std::string path = "DATA/Musics/";
	//path += music_name;
	tdnFile::EnumDirectory(Filename, di, false);

	// �񋓂����t�@�C�������ď���ݒ�
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
		else MyAssert(false,"�G���[: �t�H���_�[�̒��g��\n���������� Music1��music1�ɂȂ��Ă���");	// ��O����
	}
	delete di;
}