#include "tdnlib.h"
#include "../Sound/SoundManager.h"
#include "MusicInfo.h"
#include "PlayCursor.h"
#include "Soflan.h"

//sList *sList::roots = nullptr;

//*****************************************************************************************************************************
//	初期化・解放
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
SoflanManager::SoflanManager() :m_Soflans(nullptr), m_fTicks(nullptr){}

//------------------------------------------------------
//	解放
//------------------------------------------------------
SoflanManager::~SoflanManager()
{
	if (m_Soflans)
	{
		delete[] m_Soflans;
		m_Soflans = nullptr;
	}
	if (m_fTicks)
	{
		delete[] m_fTicks;
		m_fTicks = nullptr;
	}
}

//*****************************************************************************************************************************
//		更新
//*****************************************************************************************************************************
//------------------------------------------------------
//		更新
//------------------------------------------------------
bool SoflanManager::Update()
{
	bool bChangedBPM(false);

	if (m_iSoflanStep < m_iNumSoflans - 1)
	{
		int position = (int)((play_cursor->GetMSecond() - m_Soflans[m_iSoflanStep + 1].iEventTime));
		if (position >= 0)
		{
			// カーソルを次に
			m_iSoflanStep++;

			// 例外チェック
			assert(m_iSoflanStep < m_iNumSoflans);

			// チェンジ後のBPMセット
			bChangedBPM = true;
		}
	}

	return bChangedBPM;
	//for (sList *e = sList::roots; e != nullptr;)
	//{
	//	sList* next = e->next;
	//	// 更新
	//	int position = (int)((note_mng->GetMsecond() - e->instance->event_time * note_mng->tick));
	//
	//	extern int BPMflag;
	//	if(BPMflag == 2)position -= 290;
	//
	//	if (position > 0)
	//	{
	//		bpm = e->instance->bpm;
	//		note_mng->SetBPM(bpm);
	//		delete e;
	//	}
	//	e = next;
	//}
}


//*****************************************************************************************************************************
//		セットゲット
//*****************************************************************************************************************************
float SoflanManager::GetBPM()
{
	return m_Soflans[m_iSoflanStep].fBPM;
}

void SoflanManager::Set(MusicInfo *info)
{
	// ソフラン数
	m_iNumSoflans = info->omsInfo.NumSoflans;

	if (m_Soflans) delete[] m_Soflans;
	m_Soflans = new SoflanInfo[m_iNumSoflans];
	if (m_fTicks) delete[] m_fTicks;
	m_fTicks = new float[m_iNumSoflans];

	auto ComputeTick = [](float bpm, int division){return (60 / bpm / division * 1000); };
	for (UINT i = 0; i < m_iNumSoflans; i++)
	{
		m_Soflans[i] = info->soflans[i];
		m_fTicks[i] = ComputeTick(info->soflans[i].fBPM, info->omsInfo.division);
	}

	/*
	
	BPMが変化するとノートがおもっきり下にずれてしまう。
	原因は倍率が変わったあとで、tick値が変わるのはいいとして、倍率が変わる以前の時間にもその倍率がかかってしまうのが問題。
	変わる以前の時間は変わる前の倍率、それぞれを入れてやらないとずれてしまう

	----------------------------------------------------------------------------

	例:

	「BPM60」　　「BPM130」		「BPM260」
	――――――→――――――→――――――→（BPM130の場合）
	幅100			幅100		幅100

	「BPM60」「BPM130」「BPM260」
	――――→――――→――――→（BPM260の場合）
	幅50		幅50		幅50

	こうなってしまうが大問題。



	----------------------------------------------------------------------------

			　「BPM60」　	　「BPM130」	「BPM260」
	――――――――――――→――――――→―――→
				幅200			幅100		幅50

	こうするのが正解。



	evt[0]=0		2.08
	evt[1]=240		0.96
	evt[2]=7920		0.48

	解答
	evt[1] = evt[1] * 2.08
	evt[2] = (7920 * 0.96) + evt[1]

	*/

	//// 変化後の差(このまま放っておくとBPMが変わった際ずれる。)
	//int *shift = new int[m_info->omsInfo.NumSoflans];
	//
	//// 変化前の差(BPMが変わる前の差を保存)
	//int *shift2 = new int[m_info->omsInfo.NumSoflans];
	//
	//int *val = new int[m_info->omsInfo.NumSoflans];
	//
	//// 0は使わない
	//shift[0] = shift2[0] = val[0] = 0;

	for (UINT i = 1; i < m_iNumSoflans; i++)
	{
		//// 変化後の差倍率
		//float tick2 = (60 / m_soflans[i].bpm / m_info->omsInfo.division) * 1000;
		//
		//// 変化前の差倍率
		//float tick = (60 / m_soflans[i - 1].bpm / m_info->omsInfo.division) * 1000;
		//
		//// ソフラン開始から終わりまでの差を保存
		//val[i] = m_soflans[i].event_time - m_soflans[i - 1].event_time;
		//
		//// 変わる前と変わった後の倍率それぞれを掛ける
		//shift[i] = (int)(m_soflans[i - 1].event_time);
		//shift2[i] = (int)(val[i] * tick);
		//
		//m_soflans[i].event_time = shift[i] + val[i];
		
		// これでうまく動いた
		int val = m_Soflans[i].iEventTime - info->soflans[i - 1].iEventTime;
		m_Soflans[i].iEventTime = (int)(val * m_fTicks[i - 1]) + m_Soflans[i - 1].iEventTime;

		tdnDebug::OutPutLog("%d, %d, %.1f\n", val, m_Soflans[i].iEventTime, m_fTicks[i]);

	}

	//m_soflans[2].event_time = 7700;

	//float t = (60 / m_soflans[0].bpm / m_info->omsInfo.division) * 1000;
	//m_soflans[1].event_time = (int)(m_soflans[1].event_time * t);
	//t = (60 / m_soflans[1].bpm / m_info->omsInfo.division) * 1000;
	//m_soflans[2].event_time = (int)(m_soflans[2].event_time * t) + m_soflans[1].event_time;

	//for (UINT i = 1; i < m_info->omsInfo.NumSoflans; i++)
	//{
	//	//for (int j = i - 1; j >= 1; j--)
	//	//{
	//		m_soflans[i].event_time = val[i] + shift[i];
	//	//}
	//}

	//delete[] shift;
	//delete[] shift2;
	//delete[] val;

	m_iSoflanStep = 0;
}