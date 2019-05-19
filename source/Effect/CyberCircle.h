#pragma once
#include "TDNLIB.h"

/********************************************/
//	サイバーチックな円のアニメーション
/********************************************/
class CyberCircle
{
public:
	//===============================================
	//	コンストラクタ
	//===============================================
	CyberCircle(const Vector2 &vPos, float fScale = 1, float fAlpha = 1) :
		m_iDelayFrame(-1)
	{
		m_ptagCircle[0] = std::make_unique<Circle>("DATA/UI/Other/cyber1.png", vPos, -.01f, fScale, fAlpha);
		m_ptagCircle[1] = std::make_unique<Circle>("DATA/UI/Other/cyber2.png", vPos, .008f, fScale, fAlpha);
	}

	//===============================================
	//	更新
	//===============================================
	void Update()
	{
		// まだアクションしていないので、更新しない
		if (m_iDelayFrame == -1) return;

		// 遅延発動フレーム更新
		else if (m_iDelayFrame > 0)
		{
			m_iDelayFrame--;
			return;
		}

		// 円の更新
		FOR(NUM_CIRCLE)m_ptagCircle[i]->Update();
	}


	//===============================================
	//	描画
	//===============================================
	void Render()
	{
		// 何かしらの遅延を掛けている、もしくはアクションしていないならスルー
		if (m_iDelayFrame)return;

		// 円の描画
		FOR(NUM_CIRCLE)m_ptagCircle[i]->Render();
	}


	//===============================================
	//	発動トリガー
	//===============================================
	void Action(int delay = 0) { m_iDelayFrame = delay; }

private:

	//===============================================
	//	円の単体のクラス
	//===============================================
	struct Circle
	{
	public:
		Circle(LPCSTR path, const Vector2 &vPos, float fAddAngle, float fScale, float fAlpha) :pImage(new tdn2DObj(path)), vPos(vPos), fAngle(0), fAddAngle(fAddAngle)
		{
			pImage->SetScale(fScale);
			pImage->SetAlpha(fAlpha);
		}
		void Update()
		{
			pImage->SetAngle((fAngle += fAddAngle));
		}
		void Render()
		{
			pImage->Render((int)vPos.x, (int)vPos.y);
		}

	private:
		std::unique_ptr<tdn2DObj> pImage;
		Vector2 vPos;
		float fAngle;
		const float fAddAngle;
	};

	//===============================================
	//	メンバ変数
	//===============================================
	int m_iDelayFrame;									// -1ならNO_ACTION
	static const int NUM_CIRCLE = 2;					// 重ねる円の数
	std::unique_ptr<Circle> m_ptagCircle[NUM_CIRCLE];	// 円のクラスの実体
};
