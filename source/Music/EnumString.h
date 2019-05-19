#pragma once

#include<map>
#include "MusicInfo.h"

/********************************************/
//	enumのやつを文字列にする (enum::NORMAL == "NORMAL") みたいな感じ
/********************************************/
class EnumString
{
public:
	//------------------------------------------------------
	//	実体取得
	//------------------------------------------------------
	static EnumString &GetInstance() { static EnumString instance; return instance; }

	//------------------------------------------------------
	//	各コンテナ
	//------------------------------------------------------
	std::map<DIFFICULTY, char*> m_DifficultyMap;	// 難易度
	std::map<CLEAR_LAMP, char*> m_ClearLampMap;		// クリアランプ
	std::map<MUSIC_GENRE, char*> m_MusicGenreMap;	// ジャンル名
	std::map<GAUGE_OPTION, char*> m_GaugeOption;	// ゲージオプション

private:
	//------------------------------------------------------
	//	コンストラクタ
	//------------------------------------------------------
	EnumString()
	{
		// 難易度コンテナ初期化
		m_DifficultyMap[DIFFICULTY::NORMAL] = "NORMAL";
		m_DifficultyMap[DIFFICULTY::HYPER] = "HYPER";
		m_DifficultyMap[DIFFICULTY::EXPERT] = "EXPERT";
		m_DifficultyMap[DIFFICULTY::VIRTUOSO] = "VIRTUOSO";

		// クリアランプコンテナ初期化
		m_ClearLampMap[CLEAR_LAMP::NO_PLAY] = "NO_PLAY";
		m_ClearLampMap[CLEAR_LAMP::FAILED] = "FAILED";
		m_ClearLampMap[CLEAR_LAMP::EASY] = "EASY CLEAR";
		m_ClearLampMap[CLEAR_LAMP::CLEAR] = "CLEAR";
		m_ClearLampMap[CLEAR_LAMP::HARD] = "HARD CLEAR";
		m_ClearLampMap[CLEAR_LAMP::EX_HARD] = "EX-HARD CLEAR";
		m_ClearLampMap[CLEAR_LAMP::FULL_COMBO] = "FULL-COMBO";

		// ジャンル名コンテナ初期化
		m_MusicGenreMap[MUSIC_GENRE::ORIGINAL] = "ORIGINAL";
		m_MusicGenreMap[MUSIC_GENRE::GAME] = "GAME";
		m_MusicGenreMap[MUSIC_GENRE::CLASSIC] = "CLASSIC";
		m_MusicGenreMap[MUSIC_GENRE::VOCALOID] = "VOCALOID";
		m_MusicGenreMap[MUSIC_GENRE::POPS] = "CLASSIC";
		m_MusicGenreMap[MUSIC_GENRE::OTHER] = "OTHER";

		// ゲージオプション初期化
		m_GaugeOption[GAUGE_OPTION::NORMAL] = "NORMAL";
		m_GaugeOption[GAUGE_OPTION::EASY] = "EASY";
		m_GaugeOption[GAUGE_OPTION::HARD] = "HARD";
		m_GaugeOption[GAUGE_OPTION::EX_HARD] = "EX-HARD";
		m_GaugeOption[GAUGE_OPTION::HAZARD] = "HAZARD";
	}
};

#define ENUM_STRING (EnumString::GetInstance())