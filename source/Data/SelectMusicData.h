#pragma once

#include "../Music/MusicInfo.h"
enum class DIFFICULTY;

//===============================================
//	選曲データ
//===============================================
struct SelectMusicData
{
	int id;					// 曲のID
	DIFFICULTY difficulty;	// 曲の難易度
	SelectMusicData() :id(0){}
	SelectMusicData(int ID, DIFFICULTY difficulty) :id(ID), difficulty(difficulty){}
};


/********************************************/
//	scene跨ぎでの選曲データ保持用<Singleton>
/********************************************/
class SelectMusicManager
{
public:
	//===============================================
	//	実体取得
	//===============================================
	static SelectMusicManager *GetInstance(){ static SelectMusicManager instance; return &instance; }

	//===============================================
	//	デストラクタ
	//===============================================
	~SelectMusicManager(){ Clear(); }


	//===============================================
	//	選曲情報の設定
	//===============================================
	void Set(MusicInfo *info);		// エディットモード用
	bool Set(const SelectMusicData &data);	// それ以外


	//===============================================
	//	選曲情報の取得
	//===============================================
	MusicInfo *Get(){ return &m_tagSelectMusicInfo; }
	std::string GetMusicPath()
	{
		char path[MAX_PATH];
		sprintf_s(path, MAX_PATH, "DATA/Musics/EP/No_%d/", m_tagSelectMusicInfo.byID);
		return std::string(path);
	}

private:
	//===============================================
	//	メンバ関数
	//===============================================
	void Clear();	// バッファの開放

	//===============================================
	//	メンバ関数
	//===============================================
	MusicInfo m_tagSelectMusicInfo;	// スコアデータの実体

	//===============================================
	//	シングルトンの作法
	//===============================================
	SelectMusicManager() : m_tagSelectMusicInfo(MusicInfo{}){}
	SelectMusicManager(const SelectMusicManager&){}
	SelectMusicManager &operator =(const SelectMusicManager&){}
};


/********************************************/
//	インスタンス簡略化
/********************************************/
#define SelectMusicMgr (SelectMusicManager::GetInstance())
