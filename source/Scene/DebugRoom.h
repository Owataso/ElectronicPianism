#pragma once

class CyberCircle;
class BeatBar;

class DebugRoom : public BaseScene
{
public:
	//------------------------------------------------------
	//	初期化と開放
	//------------------------------------------------------
	bool Initialize();
	~DebugRoom();

	//------------------------------------------------------
	//	更新と描画
	//------------------------------------------------------
	bool Update();
	void Render();

private:
	CyberCircle *m_pCircle;
	BeatBar *m_pBeatBar;

	enum IKISAKI
	{
		CLASS_RESULT,
		NORMAL_RESULT,
		MODE_SELECT,
		MUSIC_SELECT,
		PLAYING,
		MAX,
	};

	int m_iSelectNumber;
	
	Surface *m_pBackUp;
	std::unique_ptr<tdn2DObj> m_pScreen;
	std::unique_ptr<tdn2DObj> m_pText;
	std::unique_ptr<tdn2DObj> m_pTextScreen;

	struct Blind
	{
		tdn2DObj *pTexture;		// ブラインド用テクスチャ
		tdn2DObj *pMaskScreen;		// ブラインド用スクリーン
		int split;
		float angle;
		float rate;
		Blind(char *TexturePath) :pTexture(new tdn2DObj(TexturePath)), pMaskScreen(new tdn2DObj(512, 512, tdn2D::RENDERTARGET)), split(24), angle(0), rate(0){}
		~Blind(){ delete pTexture; delete pMaskScreen; }
		void Render()
		{
			//m_rate = Math::Clamp(m_blindRete, 0.0f, 1.0f);
			//shader2D->SetValue("blindRate", m_blindRete);

			pMaskScreen->RenderTarget();
			tdnView::Clear();

			// 補間しない
			tdnRenderState::Filter(false);

			// ブラインド
			pTexture->Render(0, 0, 512, 512, 0, 0, 256 * split, 256 * split);
			tdnRenderState::Filter(true);
		}
	}*m_pBlind;
};
