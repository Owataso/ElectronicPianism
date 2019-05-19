#pragma once

class tdnMovie;

/********************************************/
//	演奏シーンのステート
/********************************************/
namespace scenePlayingState
{
	//------------------------------------------------------
	//	ステート基底クラス
	//------------------------------------------------------
	class Base
	{
	protected:
		scenePlaying *m_pMain;
	public:
		Base(scenePlaying *pMain) :m_pMain(pMain){}
		virtual ~Base(){}
		virtual void Update(){}
		virtual void Render(){}
	};

	//------------------------------------------------------
	//	曲名が出る→押してね→READY？までの流れ(なぜ分けないかというと、スクリーンを複数使ってのマスク処理の関係)
	//------------------------------------------------------
	class Wait :public Base
	{
	private:
		int m_iWaitTimer;
		bool m_bReady;
		bool m_bPushStart;				// 鍵盤を押してスタート
		std::unique_ptr<tdn2DObj> m_pMusicTextImage;	// 曲名とアーティスト名の画像
		std::unique_ptr<tdn2DObj> m_pTextScreen;		// 曲名とアーティスト名のスクリーン

		//------------------------------------------------------
		//	Ready?の委譲
		//------------------------------------------------------
		struct Ready
		{
			float fSinCurveRate;
			float fReadyAlpha;
			Ready() :fSinCurveRate(0), fReadyAlpha(1){}
			bool Update()
			{
				static const float fAddCurve(.25f);
				fSinCurveRate += fAddCurve;

				static const float SubAlphaSpeed(.01f);
				return ((fReadyAlpha = (fReadyAlpha - SubAlphaSpeed < SubAlphaSpeed) ? 0 : fReadyAlpha - SubAlphaSpeed) == 0);
			}
			void Render(bool bAuto)
			{
				const DWORD dwReadyCol((fReadyAlpha <= .5f) ? 0x00ffffff | ((BYTE)(255 * (fReadyAlpha * 2)) << 24) : 0xffffffff);
				tdnFont::RenderStringCentering("GetReady？", 42, 540, 320, dwReadyCol);
				if(!bAuto)tdnFont::RenderString("ここからスタート！", 30, 420, 512, 0x00ffffff | ((BYTE)(255 * Math::Blend((sinf(fSinCurveRate * .5f) + 1) / 2, .5f, 1.0f)) << 24));
			}
		}m_Ready;

		//------------------------------------------------------
		//	この鍵盤を押してスタートしようの委譲
		//------------------------------------------------------
		struct PushStart
		{
			std::unique_ptr<tdn2DObj> pPushKeyImage;			// 押した鍵盤の光
			std::vector<int> StartLaneList;	// 開始レーン保存

			float fSinCurveRate;

			PushStart() :fSinCurveRate(0), pPushKeyImage(new tdn2DObj("DATA/UI/Playing/push.png")), StartLaneList(){}
			bool Update();

			//------------------------------------------------------
			//	鍵盤を押してスタートするよ描画
			//------------------------------------------------------
			void Render();

			//------------------------------------------------------
			//	開始のノーツの鍵盤を光らせる描画
			//------------------------------------------------------
			void RenderStartKeyboard()
			{
				// 開始レーン分回す
				for (auto it : StartLaneList)
				{
					pPushKeyImage->Render(it / 12 * 280, 630, 280, 90, 0, (it % 12) * 90, 280, 90);
				}
			}
		}m_PushStart;

		//------------------------------------------------------
		//	曲名とアーティスト名にマスクをかけてブランド処理する用
		//------------------------------------------------------
		struct Blind
		{
			tdn2DObj *pTexture;		// ブラインド用テクスチャ
			tdn2DObj *pMaskScreen;		// ブラインド用スクリーン
			int split;
			float angle;
			float rate;
			Blind(char *TexturePath) :pTexture(new tdn2DObj(TexturePath)), pMaskScreen(new tdn2DObj(512, 512, tdn2D::RENDERTARGET)), split(48), angle(0), rate(0){}
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
		}m_blind;
	public:
		Wait(scenePlaying *pMain);
		void Update();
		void Render();
	};

	//------------------------------------------------------
	//	実際にプレイヤーが演奏している状態
	//------------------------------------------------------
	class Playing :public Base
	{
	public:
		Playing(scenePlaying *pMain);
		void Update();
		void Render();
	private:
		bool m_bStreamStop;	// 応急処置
	};


	//------------------------------------------------------
	//	演奏が終わって終了
	//------------------------------------------------------
	class End :public Base
	{
	public:
		End(scenePlaying *pMain);
		void Update();
		void Render();
	};


	//------------------------------------------------------
	//	閉店
	//------------------------------------------------------
	class Failed :public Base
	{
	public:
		Failed(scenePlaying *pMain);
		void Update();
		void Render();
	};
};