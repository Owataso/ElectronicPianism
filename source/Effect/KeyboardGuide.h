#pragma once
#include "TDNLIB.h"

/********************************************/
//	キーボードのガイド(選択・決定)アニメーション
/********************************************/
class KeyboardGuide
{
public:
	//===============================================
	//	コンストラクタ
	//===============================================
	KeyboardGuide(const Vector2 &vPos, int iFlashCount, float fAlphaSpeed, float fMaxAlpha, float fMinAlpha, int iDelay=0) :
		m_ciDelay(iDelay),
		m_iDelayFrame(0),
		m_vPos(vPos),
		m_cfAlphaSpeed(fAlphaSpeed),
		m_cfSwicthAngle(2 * iFlashCount * PI),
		m_cfMinAlpha(fMinAlpha),
		m_cfMaxAlpha(fMaxAlpha),
		m_fAlphaSinCurve(0),
		m_bSwitch(true),
		m_pImage(new tdn2DObj(
#ifdef MIDI_INPUT_ON
			"DATA/UI/Other/guide.png"
#else
			"DATA/UI/Other/info_window.png"
#endif
			))
	{}

	//===============================================
	//	更新
	//===============================================
	void Update()
	{
		// 発動まで若干の猶予を持たせる
		if (m_iDelayFrame < m_ciDelay) m_iDelayFrame++;

		// 片方の点滅が既定回数まで来たら
		if ((m_fAlphaSinCurve += m_cfAlphaSpeed) > m_cfSwicthAngle)
		{
			// 点滅チェンジ
			m_fAlphaSinCurve = 0;
			m_bSwitch = !m_bSwitch;
		}
	}


	//===============================================
	//	描画
	//===============================================
	void Render();

private:
	//===============================================
	//	メンバ変数
	//===============================================
	int m_iDelayFrame;					// 開始時はフェードインする漢字で出るのでそれ用
	const int m_ciDelay;				// 開始時はフェードインする感じで出るのでそれ用
	float m_fAlphaSinCurve;				// 点滅をサインカーブで行っているのでそれ用の変数
	bool m_bSwitch;						// 切り替えフラグ(現在は2種類なのでbool型にしている)
	const Vector2 m_vPos;				// 座標
	const float m_cfAlphaSpeed;			// 点滅速度
	const float m_cfSwicthAngle;		// 何回点滅したら切り替わるか
	const float m_cfMinAlpha;			// 点滅の最少アルファ値
	const float m_cfMaxAlpha;			// 点滅の最大アルファ値
	std::unique_ptr<tdn2DObj> m_pImage;	// 画像
};
