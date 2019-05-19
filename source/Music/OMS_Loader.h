#pragma once

struct MusicInfo;

/********************************************/
//	OMSファイル(おわたそミュージックスコア)読み込み<static class>
/********************************************/
class OMS_Loader
{
public:
	//===============================================
	//	定数(バージョン)
	//===============================================
	enum class OMS_VERSION
	{
		V1,
		V2
	};

	//===============================================
	//	おわたそミュージックスコア読み込み
	//===============================================
	static bool LoadOMS(LPCSTR filename, MusicInfo *out, OMS_VERSION version);
	static int GetOctaveWidth(char *filename);
};