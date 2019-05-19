#include "TDNLIB.h"
#include "SelectMusicData.h"
#include "../Music/EnumString.h"
#include "../Music/OMS_Loader.h"
#include "../Data/MusicDataBase.h"


//------------------------------------------------------
//	ノーツとソフランのバッファ解放
//------------------------------------------------------
void SelectMusicManager::Clear()
{
	// ノーツ解放
	if (m_tagSelectMusicInfo.notes)
	{
		delete[] m_tagSelectMusicInfo.notes;
		m_tagSelectMusicInfo.notes = nullptr;
	}

	// ソフラン解放
	if (m_tagSelectMusicInfo.soflans)
	{
		delete[] m_tagSelectMusicInfo.soflans;
		m_tagSelectMusicInfo.soflans = nullptr;
	}

}

//------------------------------------------------------
//	情報セット
//------------------------------------------------------
void SelectMusicManager::Set(MusicInfo *info)
{
	// 既存のバッファの開放
	Clear();

	// 情報コピー	※手動で全部コピーしてるのでMusicInfo更新したらここも更新
	memcpy_s(&m_tagSelectMusicInfo.omsInfo, sizeof(OMSInfoV2), &info->omsInfo, sizeof(OMSInfoV2));
	m_tagSelectMusicInfo.eDifficulty = info->eDifficulty;
	m_tagSelectMusicInfo.byID = info->byID;
	m_tagSelectMusicInfo.bMovieExist = info->bMovieExist;

	// ノーツとソフラン
	m_tagSelectMusicInfo.notes = new NoteInfo[info->omsInfo.NumNotes];
	m_tagSelectMusicInfo.soflans = new SoflanInfo[info->omsInfo.NumSoflans];
	FOR((int)m_tagSelectMusicInfo.omsInfo.NumNotes)
		memcpy_s(&m_tagSelectMusicInfo.notes[i], sizeof(NoteInfo), &info->notes[i], sizeof(NoteInfo));
	FOR((int)m_tagSelectMusicInfo.omsInfo.NumSoflans)
		memcpy_s(&m_tagSelectMusicInfo.soflans[i], sizeof(SoflanInfo), &info->soflans[i], sizeof(SoflanInfo));
}

bool SelectMusicManager::Set(const SelectMusicData &data)
{
	// 既存のバッファの開放
	Clear();

	// ★なぜかファイルパス作成した後で引数のデータが壊れるので、保存しとく
	int id(data.id);
	DIFFICULTY dif(data.difficulty);

	// OMSファイルから情報
	char path[256];
	sprintf_s(path, 256, "DATA/Musics/EP/No_%d/%s.oms", id, ENUM_STRING.m_DifficultyMap[dif]);
	if (!OMS_Loader::LoadOMS(path, &m_tagSelectMusicInfo, OMS_Loader::OMS_VERSION::V2)) return false;

	// 難易度
	m_tagSelectMusicInfo.eDifficulty = dif;

	// ID
	m_tagSelectMusicInfo.byID = id;

	// 動画があるかどうか
	FOR(MUSIC_DATA_BASE->GetNumEPContents())
	{
		if (MUSIC_DATA_BASE->GetEPContentsData(i)->byID == id)
		{
			m_tagSelectMusicInfo.bMovieExist = MUSIC_DATA_BASE->GetEPContentsData(i)->bMovieExist;
			break;
		}
	}
	return true;
}