#pragma once

struct MusicInfo;

/********************************************/
//	OMS�t�@�C��(���킽���~���[�W�b�N�X�R�A)�ǂݍ���<static class>
/********************************************/
class OMS_Loader
{
public:
	//===============================================
	//	�萔(�o�[�W����)
	//===============================================
	enum class OMS_VERSION
	{
		V1,
		V2
	};

	//===============================================
	//	���킽���~���[�W�b�N�X�R�A�ǂݍ���
	//===============================================
	static bool LoadOMS(LPCSTR filename, MusicInfo *out, OMS_VERSION version);
	static int GetOctaveWidth(char *filename);
};