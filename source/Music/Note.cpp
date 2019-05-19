#include "tdnlib.h"
#include "../Sound/SoundManager.h"
#include "MusicInfo.h"
#include "Note.h"
#include "PlayCursor.h"
#include "Soflan.h"
#include "../Data/PlayerData.h"
#include "../Judge/Judge.h"

//*****************************************************************************************************************************
//	初期化・解放
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
NoteManager::NoteManager()
{
	// リスト配列の動的確保
	m_list = new std::list<PlayNoteInfo*>[NUM_KEYBOARD];
}
void NoteManager::Initialize(SoflanManager *pSoflanManager)
{
	m_pSoflanMgr = pSoflanManager;

	/*
	分能値は、４分音符が何ティックかを設定します。例えば、タイムベースが120なら８分音符は60ティック、三連８分音符は40ティック、全音符は480ティックになります。
	分能値は、通常120、196、240、480などが使われます。他の値を設定しても構いませんが、シーケンサによってはサポートしていない場合があります。
	１ティックの時間(秒) = 60 / テンポ / タイムベース

	例えば、テンポ120でタイムベースが480ならば1ティックは1 / 960秒、約１ミリ秒になります。
	*/
	FOR(NUM_KEYBOARD)
	{
		m_list[i].clear();
	}
}
//------------------------------------------------------
//	解放
//------------------------------------------------------
NoteManager::~NoteManager()
{
	Clear();

	// リスト配列の開放
	delete[] m_list;
}

void NoteManager::Clear()
{
	FOR(NUM_KEYBOARD)
	{
		for (auto it = m_list[i].begin(); it != m_list[i].end();)
		{
			delete (*it);
			it = m_list[i].erase(it);
		}
		m_list[i].clear();
	}
}

//*****************************************************************************************************************************gwq
//		更新
//*****************************************************************************************************************************
//------------------------------------------------------
//		更新
//------------------------------------------------------
void NoteManager::Update(JudgeManager *judge)
{
	// 最適化比較用
	static bool bOptimisation = true;

	// [O]キーで最適化切り替え
	if (KeyBoardTRG(KB_O) && KeyBoard(KB_SPACE)) bOptimisation = !bOptimisation;

	// 更新分岐
	bOptimisation ? OptimisationedUpdate(judge) : BeforeOptimisationUpdate(judge);
}

void NoteManager::BeforeOptimisationUpdate(JudgeManager *pJudge)
{
	FOR(NUM_KEYBOARD)
	{
		for (auto it = m_list[i].begin(); it != m_list[i].end();)
		{
			(*it)->position = (int)(
				play_cursor->GetMSecond() -
				(*it)->note.iEventTime
				);		// 現在の再生時間 - 自分のイベントタイム

			// 消去チェック
			if ((*it)->erase)
			{
				//InputMIDIMgr->SetTone((*it)->note.tone);
				delete (*it);
				it = m_list[i].erase(it);
			}

			// 見逃しpoor
			else if ((*it)->position > 200)
			{
				// 判定強制BAD
				pJudge->LostNote((*it)->note.cLaneNo - m_cBaseOctave);
				delete (*it);
				it = m_list[i].erase(it);
			}
			else
			{
				// ノーツ座標計算
				it++;
			}
		}
	}
}

void NoteManager::OptimisationedUpdate(JudgeManager *pJudge)
{
	std::list<PlayNoteInfo*>::iterator it;
	FOR(NUM_KEYBOARD)
	{
		it = m_list[i].begin();
		while (it != m_list[i].end())
		{
			// ノーツ座標計算(現在の再生時間 - 自分のイベントタイム)
			if (((*it)->position = (int)(play_cursor->GetMSecond() - (*it)->note.iEventTime))
				//_/_/_/_/_/_/_/_超スーパーVIRTUOSO級の最適化/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
				< -5000)
				break;// これより後計算する必要なし
				//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

			// 消去チェック
			else if ((*it)->erase)
			{
				//InputMIDIMgr->SetTone((*it)->note.tone);
				delete (*it);
				it = m_list[i].erase(it);
			}

			// 見逃しpoor
			else if ((*it)->position > 200)
			{
				// 判定強制BAD
				pJudge->LostNote((*it)->note.cLaneNo - m_cBaseOctave);

				// CN開始を見逃すと、CN終了も消す
				if ((*it)->note.cNoteType == (int)NOTE_TYPE::CN_START)
				{
					// CN開始ノーツ消去
					delete (*it);
					it = m_list[i].erase(it);

					// CN終了ノーツ消去
					// 判定強制BAD
					pJudge->LostNote((*it)->note.cLaneNo - m_cBaseOctave);
					delete (*it);
					it = m_list[i].erase(it);
				}
				else
				{
					// ノーツ消去
					delete (*it);
					it = m_list[i].erase(it);
				}
			}

			else it++;
		}
	}
}

//*****************************************************************************************************************************
//		セットゲット
//*****************************************************************************************************************************
std::list<PlayNoteInfo*> *NoteManager::GetList(int lane)
{
	return &m_list[lane];
}
void NoteManager::GetNoteAbsolutePos(const PlayNoteInfo &note, int *pos_y)
{
	*pos_y = note.position;
}

void NoteManager::GetNoteHiSpeedPos(const PlayNoteInfo &note, int *pos_y)
{
	*pos_y = note.position;

	// BPMによる影響
	if (!PlayerDataMgr->m_PlayerOption.bRegulSpeed)
		*pos_y = (int)(*pos_y / m_pSoflanMgr->GetTicks()[note.SoflanNo]);
	// ハイスピードによる影響
	const float speed = PlayerDataMgr->m_PlayerOption.HiSpeed * HI_SPEED_WIDTH;

	//static int type(0);
	//if (KeyBoardTRG(KB_1)) type = 0;
	//if (KeyBoardTRG(KB_2)) type = 1;
	//if (KeyBoardTRG(KB_3)) type = 2;
	//if (KeyBoardTRG(KB_4)) type = 3;

	//switch (type)
	//{
	//case 0:
		*pos_y = (int)(*pos_y * (.1f + (speed / .1f * .01f)));
	//	break;
	//case 1:
	//	*pos_y = (int)(*pos_y * (.1f + (speed / .75f * .1f)));
	//	break;
	//case 2:
	//	*pos_y = (int)(*pos_y * (.1f + (speed / .5f * .05f)));
	//	break;
	//case 3:
	//	*pos_y = (int)(*pos_y * (.1f + (speed / .75f * .1f)));
	//	break;
	//}
}

void NoteManager::Set(MusicInfo *info)
{
	// 例外処理(SoflanManagerのSetを先に呼んでください)
	assert(m_pSoflanMgr->GetSoflans());

	// ベースオクターブ保存
	m_cBaseOctave = info->omsInfo.BaseOctave;

	// 既存のバッファ開放
 	Clear();

	m_iNumNorts = info->omsInfo.NumNotes;

	// ノーツ情報をリストに入れていく
	for (UINT i = 0; i < m_iNumNorts; i++)
	{
		PlayNoteInfo *set = new PlayNoteInfo;
		set->erase = false;
		memcpy_s(&set->note, sizeof(NoteInfo), &info->notes[i], sizeof(NoteInfo));
		set->position = -114514;

		// ソフラン対応用のイベントタイム配列作成
		//set->EventTimeArray = new UINT[m_info->omsInfo.NumSoflans];
		//memcpy_s(set->EventTimeArray, sizeof(UINT)*m_info->omsInfo.NumSoflans, &set->note->iEventTime, sizeof(UINT)*m_info->omsInfo.NumSoflans);

		//float tick = (60 / m_SoflanMgr->m_soflans[0].fBPM / m_info->omsInfo.division) * 1000;
		//set->EventTimeArray[0] = (int)(note->iEventTime * tick);
		//UINT debug[300];

		for (UINT j = info->omsInfo.NumSoflans - 1; j >= 0; j--)
		{
			if (info->notes[i].iEventTime >= info->soflans[j].iEventTime)	// >ではなく、>=にしないとソフランがずれる
			{
				const float tick((60 / m_pSoflanMgr->GetSoflans()[j].fBPM / info->omsInfo.division) * 1000);
				const int val(info->notes[i].iEventTime - info->soflans[j].iEventTime);
				set->note.iEventTime = (int)(val * tick) + m_pSoflanMgr->GetSoflans()[j].iEventTime;	// ソフランのイベント時間+そっからの自分の時間的な何か
				set->SoflanNo = j;
				break;
			}
		}

		int lane = info->notes[i].cLaneNo - m_cBaseOctave;	// ノーツのレーン

		MyAssert(lane >= 0 && lane < NUM_KEYBOARD, "セーブデータのノーツ情報がおかしい");
		m_list[lane].push_back(set);
	}


	// 変化後の差(このまま放っておくとBPMが変わった際ずれる。)
	//int *shift = new int[m_info->omsInfo.NumSoflans];
	//
	//// 変化前の差(BPMが変わる前の差を保存)
	//int *shift2 = new int[m_info->omsInfo.NumSoflans];
	//
	//// 0は使わない
	//shift[0] = shift2[0] = 0;
	//
	//for (UINT i = 1; i < m_info->omsInfo.NumSoflans; i++)
	//{
	//	// 変化後の差倍率
	//	float tick2 = (60 / m_SoflanMgr->m_soflans[i].fBPM / m_info->omsInfo.division) * 1000;
	//
	//	// 変化前の差倍率
	//	float tick = (60 / m_SoflanMgr->m_soflans[i - 1].fBPM / m_info->omsInfo.division) * 1000;
	//
	//	// ソフラン開始から終わりまでの差を保存
	//	int val = m_SoflanMgr->m_soflans[i].iEventTime - m_SoflanMgr->m_soflans[i - 1].iEventTime;
	//
	//	// 変わる前と変わった後の倍率それぞれを掛ける
	//	shift[i] = (int)(val * tick2);
	//	shift2[i] = (int)(val * tick);
	//}
	//for (UINT i = 1; i < m_info->omsInfo.NumSoflans; i++)
	//{
	//	for (auto it : nList)
	//	{
	//		if (it->note->iEventTime <= m_SoflanMgr->m_soflans[i].iEventTime)
	//			continue;
	//
	//		//it->EventTimeArray[i] = it->note->iEventTime;
	//		for (int j = i; j >= 0; j--)
	//		{
	//			it->EventTimeArray[i] = it->EventTimeArray[i] - shift[j] + shift2[j];
	//			if (j == 2)
	//			{
	//				int val = it->EventTimeArray[i] - m_SoflanMgr->m_soflans[j].iEventTime;
	//				float tick2 = (60 / m_SoflanMgr->m_soflans[j].fBPM / m_info->omsInfo.division) * 1000;
	//				it->EventTimeArray[i] = it->EventTimeArray[i] - val + (int)(val * tick2);
	//			}
	//		}
	//	}
	//}
	//
	//delete shift;
	//delete shift2;
}

void NoteManager::GetStartNotesLane(std::vector<int> *pOutLanes)
{
	UINT iMinEventTime(INT_MAX);

	// 全ノーツから、最初に押されるノーツを探す
	FOR(NUM_KEYBOARD)
	{
		// このレーンには何も降ってこない場合の処理
		if (m_list[i].empty()) continue;

		// レーンの一番最初のノーツ(最初のイベント時間の計測だけでいいので、2個目以降のレーンのノーツはいらない)
		auto LaneFirstNote = m_list[i].begin();

		const UINT iNoteEventTime((*LaneFirstNote)->note.iEventTime);
		// 最短時間より短かったら
		if (iNoteEventTime < iMinEventTime)
		{
			// ノーツ開始最短時間更新
			iMinEventTime = iNoteEventTime;

			pOutLanes->clear();
			pOutLanes->push_back(i);
		}

		// 同時押しにも対応
		else if (iNoteEventTime == iMinEventTime) pOutLanes->push_back(i);
	}
}