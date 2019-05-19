#include "../Music/MusicInfo.h"
#include "Gauge.h"

//*****************************************************************************************************************************
//	初期化・解放
//*****************************************************************************************************************************
GaugeManager::GaugeManager(GAUGE_OPTION eOption, int *ipGauge) :
	m_ipGauge(ipGauge), 
	m_eOption(eOption), 
	m_fGauge(22.0f)
{

}

//*****************************************************************************************************************************
//	ゲージリセット
//*****************************************************************************************************************************
void GaugeManager::GaugeSet(int iGauge)
{
	*m_ipGauge = iGauge;
	m_fGauge = (float)iGauge;
}

//*****************************************************************************************************************************
//		ノーツ数に応じて、ノマゲと易のゲージ増加率計算
//*****************************************************************************************************************************
void GaugeManager::SetNumNorts(int iNumNorts)
{
	// トータル値算出
	const float l_cfTotalValue(max(7.605f * iNumNorts / (0.01f * iNumNorts + 6.5f), 260));

	// トータル値をノーツ数で割ると1こ当たりの増加量が求められる
	m_fNormalAdd = l_cfTotalValue / iNumNorts;
}


//*****************************************************************************************************************************
//		判定に応じてゲージを増減させる
//*****************************************************************************************************************************
void GaugeManager::Compute(JUDGE_TYPE type)
{
	switch (type)
	{
	case JUDGE_TYPE::SUPER:
	case JUDGE_TYPE::GREAT:
	{
		const float l_cfAddGauge[(int)GAUGE_OPTION::MAX]{ m_fNormalAdd, m_fNormalAdd, .16f, .16f, .16f, .16f };
		m_fGauge = min(m_fGauge + l_cfAddGauge[(int)m_eOption], 100);
	}
		break;
	case JUDGE_TYPE::GOOD:
	{
		const float l_cfAddGauge[(int)GAUGE_OPTION::MAX]{ m_fNormalAdd * .5f, m_fNormalAdd * .5f, .0f, .0f, .0f, .04f };
		m_fGauge = min(m_fGauge + l_cfAddGauge[(int)m_eOption], 100);
	}
		break;
	case JUDGE_TYPE::BAD:
	{
		static const float l_cfSubGauge[(int)GAUGE_OPTION::MAX]{ -2, -1.6f, -5, -10, -100, -1.5f };
		m_fGauge = max(m_fGauge + l_cfSubGauge[(int)m_eOption], (m_eOption == GAUGE_OPTION::EASY || m_eOption == GAUGE_OPTION::NORMAL) ? 2 : 0);	// オプションに応じてゲージを減らす
	}
		break;
	}

	// 小数点切り捨ての方の計算+2の倍数に丸める
	*m_ipGauge = ((int)m_fGauge / 2) * 2;
}