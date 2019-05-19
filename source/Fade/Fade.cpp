#include	"TDNLIB.h"
#include	"Fade.h"

//*********************************************************************************************
//		パラメータの設定
//*********************************************************************************************
Fade::MODE Fade::m_eMode;		/*	フェードイン、フェードアウト、何もしない	*/
BYTE Fade::m_bySpeed;				/*	フェードする速度	*/
BYTE Fade::m_byLimit;				/*	どこまでフェードするか(0～255の範囲)	*/
BYTE Fade::m_byAlpha;				/*	α値(0～255)	*/
DWORD Fade::m_dwColor;			/*	カラー(0x○○000000 ～ 0x○○ffffff)ここの○○の部分を必ず00にしておくこと！	*/


//=============================================================================================
//		初	期	化
void Fade::Initialize()
{
	m_eMode = FADE_NONE;
	m_bySpeed = 0x01;
	m_byAlpha = 0xff;
	m_dwColor = 0x00000000;
}
//
//=============================================================================================


//=============================================================================================
//		フェード設定
void Fade::Set(MODE eMode, BYTE bySpeed, BYTE byLimit, BYTE byStartAlpha, DWORD dwColor)
{
	m_eMode = eMode;		// フェードモード(FADE_IN or FADE_OUT)
	m_bySpeed = bySpeed;		// フェードを増減させるスピード(5なら、毎フレーム「5」が増減される)
	m_byLimit = byLimit;		// 目標m_byAlpha値(FADE_INなら「0」、FADE_OUTなら「255」が基本)
	m_byAlpha = byStartAlpha;	// フェード開始時のm_byAlpha値(FADE_INなら「255」、DADE_OUTなら「0」が基本)
	m_dwColor = dwColor;		// フェード色

	// 0x○○ffffffの○の部分を0にする処理(m_byAlpha値の調整)
	m_dwColor &= 0x00ffffff;
}


void Fade::Set(MODE eMode, BYTE bySpeed, DWORD dwColor)
{
	m_eMode = eMode;
	m_bySpeed = bySpeed;
	m_dwColor = dwColor;

	switch (m_eMode)
	{
	case FADE_IN:
		m_byAlpha = 255;
		m_byLimit = 0;
		break;

	case FADE_OUT:
		m_byAlpha = 0;
		m_byLimit = 255;
		break;

	case FADE_NONE:
		break;
	}
}
//
//=============================================================================================



//=============================================================================================
//		更		新
void Fade::Update()
{
	switch (m_eMode)
	{
	case FADE_IN:

		// 目的αまでついたら、ストップ
		if (m_byAlpha <= m_byLimit + m_bySpeed){		// BYTEなのでオーバーフロー防止
			m_byAlpha = m_byLimit;
			m_eMode = FADE_NONE;
		}
		else
			m_byAlpha -= m_bySpeed;

		break;

	case FADE_OUT:

		// 目的αまでついたら、ストップ
		if (m_byAlpha >= m_byLimit - m_bySpeed){		// BYTEなのでオーバーフロー防止
			m_byAlpha = m_byLimit;
			m_eMode = FADE_NONE;
		}
		else
			m_byAlpha += m_bySpeed;

		break;

	case FADE_NONE:
		break;	// NothingDo
	}
}


//=============================================================================================
//		描			画
void Fade::Render(u32 dwFlags)
{
	// argbのフォーマットにする
	const DWORD l_cdwColor((m_byAlpha << 24) | m_dwColor);	

	// 画面全体に四角形を描画
	tdnPolygon::Rect(0, 0, tdnSystem::GetScreenSize().right, tdnSystem::GetScreenSize().bottom, dwFlags, l_cdwColor);
}
//
//=============================================================================================