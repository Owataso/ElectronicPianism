#pragma once

class tdnMovie;

/********************************************/
//	���t�V�[���̃X�e�[�g
/********************************************/
namespace scenePlayingState
{
	//------------------------------------------------------
	//	�X�e�[�g���N���X
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
	//	�Ȗ����o�遨�����Ăˁ�READY�H�܂ł̗���(�Ȃ������Ȃ����Ƃ����ƁA�X�N���[���𕡐��g���Ẵ}�X�N�����̊֌W)
	//------------------------------------------------------
	class Wait :public Base
	{
	private:
		int m_iWaitTimer;
		bool m_bReady;
		bool m_bPushStart;				// ���Ղ������ăX�^�[�g
		std::unique_ptr<tdn2DObj> m_pMusicTextImage;	// �Ȗ��ƃA�[�e�B�X�g���̉摜
		std::unique_ptr<tdn2DObj> m_pTextScreen;		// �Ȗ��ƃA�[�e�B�X�g���̃X�N���[��

		//------------------------------------------------------
		//	Ready?�̈Ϗ�
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
				tdnFont::RenderStringCentering("GetReady�H", 42, 540, 320, dwReadyCol);
				if(!bAuto)tdnFont::RenderString("��������X�^�[�g�I", 30, 420, 512, 0x00ffffff | ((BYTE)(255 * Math::Blend((sinf(fSinCurveRate * .5f) + 1) / 2, .5f, 1.0f)) << 24));
			}
		}m_Ready;

		//------------------------------------------------------
		//	���̌��Ղ������ăX�^�[�g���悤�̈Ϗ�
		//------------------------------------------------------
		struct PushStart
		{
			std::unique_ptr<tdn2DObj> pPushKeyImage;			// ���������Ղ̌�
			std::vector<int> StartLaneList;	// �J�n���[���ۑ�

			float fSinCurveRate;

			PushStart() :fSinCurveRate(0), pPushKeyImage(new tdn2DObj("DATA/UI/Playing/push.png")), StartLaneList(){}
			bool Update();

			//------------------------------------------------------
			//	���Ղ������ăX�^�[�g�����`��
			//------------------------------------------------------
			void Render();

			//------------------------------------------------------
			//	�J�n�̃m�[�c�̌��Ղ����点��`��
			//------------------------------------------------------
			void RenderStartKeyboard()
			{
				// �J�n���[������
				for (auto it : StartLaneList)
				{
					pPushKeyImage->Render(it / 12 * 280, 630, 280, 90, 0, (it % 12) * 90, 280, 90);
				}
			}
		}m_PushStart;

		//------------------------------------------------------
		//	�Ȗ��ƃA�[�e�B�X�g���Ƀ}�X�N�������ău�����h��������p
		//------------------------------------------------------
		struct Blind
		{
			tdn2DObj *pTexture;		// �u���C���h�p�e�N�X�`��
			tdn2DObj *pMaskScreen;		// �u���C���h�p�X�N���[��
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

				// ��Ԃ��Ȃ�
				tdnRenderState::Filter(false);

				// �u���C���h
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
	//	���ۂɃv���C���[�����t���Ă�����
	//------------------------------------------------------
	class Playing :public Base
	{
	public:
		Playing(scenePlaying *pMain);
		void Update();
		void Render();
	private:
		bool m_bStreamStop;	// ���}���u
	};


	//------------------------------------------------------
	//	���t���I����ďI��
	//------------------------------------------------------
	class End :public Base
	{
	public:
		End(scenePlaying *pMain);
		void Update();
		void Render();
	};


	//------------------------------------------------------
	//	�X
	//------------------------------------------------------
	class Failed :public Base
	{
	public:
		Failed(scenePlaying *pMain);
		void Update();
		void Render();
	};
};