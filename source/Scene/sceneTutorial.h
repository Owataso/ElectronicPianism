#pragma once

#include "../Input/InputMIDI.h"
#include<mbstring.h>



/********************************************/
//	チュートリアル説明
/********************************************/
class TutorialDescription
{
public:

	//------------------------------------------------------
	//	コンストラクタ(引数に説明の文字列をもらう)
	//------------------------------------------------------
	TutorialDescription(LPCSTR description) :m_lpsDescription(description), m_sRenderDescription(""), m_iWaitTime(0), m_iCursor(0)
	{
		m_pAnimImages[ANIM_IMAGE::FRAME] = new tdn2DAnim("DATA/UI/Other/frame.png");
		m_pAnimImages[ANIM_IMAGE::FRAME]->OrderStretch(15, 1, .0f, 1, 4);
	}

	//------------------------------------------------------
	//	デストラクタ
	//------------------------------------------------------
	~TutorialDescription()
	{
		POINTER_ARRAY_DELETE(m_pAnimImages, ANIM_IMAGE::MAX);
	}

	//------------------------------------------------------
	//	更新(全部読み終わってたらtrueを返す)
	//------------------------------------------------------
	bool Update()
	{
		// なんかボタン押してたら早く読み進める	
		if (InputMIDIMgr->GetAnyTrigger()) m_iWaitTime -= 1;

		// 待ち時間終わったら
		if (--m_iWaitTime < 0)
		{
			// 終端でなおかつキーを押したら終了
			if (m_lpsDescription[m_iCursor] == '\0')
				return InputMIDIMgr->GetAnyTrigger();

			char buf[5] = { '\0' };
			int len(_mbclen((BYTE*)&m_lpsDescription[m_iCursor]));
			FOR(len) buf[i] = m_lpsDescription[m_iCursor + i];

			// 描画用string型に1文字足す
			m_sRenderDescription += buf;

			// 読みカーソルを進める
			m_iCursor += len;

			// 文字の待機を設定
			switch (m_lpsDescription[m_iCursor])
			{
			case '\n':
				m_iWaitTime = c_NEWLINE_WAIT;
				break;

			case '\0':
				m_iWaitTime = c_NEWLINE_WAIT;	// 終了時も改行と同じ時間にする
				break;

			default:
				m_iWaitTime = c_LETTER_WAIT;	// 1文字読み待機時間
				break;
			}
		}

		// まだ続くよ
		return false;
	}
	void Render()
	{
		// 説明文の描画
		tdnFont::RenderString(m_sRenderDescription.c_str(), 32, 320, 540);
	}

private:
	static const int c_LETTER_WAIT = 3;		// 1文字あたりの待ち時間
	static const int c_NEWLINE_WAIT = 45;		// 改行時の待ち時間

	LPCSTR m_lpsDescription;				// 説明文
	std::string m_sRenderDescription;		// 実際に描画する文字(+=していく)
	int m_iCursor;							// 文字に対するカーソル
	int m_iWaitTime;						// 待ち時間(いっぺんに文字を出すと読みにくいので配慮してあげる)

	enum ANIM_IMAGE
	{
		FRAME,
		MAX
	};
	tdn2DAnim *m_pAnimImages[ANIM_IMAGE::MAX];
};



/********************************************/
//	チュートリアル管理
/********************************************/
class TutorialManager
{
public:

	//------------------------------------------------------
	//	コンストラクタ(初期化)
	//------------------------------------------------------
	TutorialManager():m_iStep(0)
	{
		m_vTutorialDescription.push_back(new TutorialDescription("ようこそ、ElectronicPianismの世界へ！\nこのゲームはキーボードの鍵盤を使って遊ぶ音楽ゲームだよ！"));
	}

	//------------------------------------------------------
	//	デストラクタ(リストの開放)
	//------------------------------------------------------
	~TutorialManager(){for(auto it : m_vTutorialDescription) delete it;}

	//------------------------------------------------------
	//	更新(全部終わったらtrueを返す)
	//------------------------------------------------------
	bool Update()
	{
		// チュートリアル説明更新&その説明が終わったかどうか
		const bool bNextStep(m_vTutorialDescription[m_iStep]->Update());

		// 説明が終わって次のステップへ、全部終わったら終了
		if(bNextStep) if(++m_iStep >= m_vTutorialDescription.size()) return true;	// 終了！

		// まだ続くよ
		return false;
	}

	//------------------------------------------------------
	//	描画
	//------------------------------------------------------
	void Render()
	{
		assert(m_iStep < m_vTutorialDescription.size());

		// チュートリアル説明描画
		m_vTutorialDescription[m_iStep]->Render();
	}

private:

	//------------------------------------------------------
	//	チュートリアルのステップ
	//------------------------------------------------------
	UINT m_iStep;

	//------------------------------------------------------
	//	チュートリアルの説明リスト(forでの更新ではなく、添え字にm_iStepを使う)
	//------------------------------------------------------
	std::vector<TutorialDescription*> m_vTutorialDescription;
};



/********************************************/
//	チュートリアルシーン
/********************************************/
class sceneTutorial:public BaseScene
{
	//------------------------------------------------------
	//	初期化・開放
	//------------------------------------------------------
	bool Initialize();
	~sceneTutorial();

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	bool Update();
	void Render();

private:
	//------------------------------------------------------
	//	画像ID
	//------------------------------------------------------
	enum IMAGE
	{
		KEYBOARD,
		MAX
	};

	//------------------------------------------------------
	//	メンバ変数
	//------------------------------------------------------
	std::unique_ptr<tdn2DObj> m_pImages[IMAGE::MAX];
	TutorialManager *m_pTutorialMgr;
};