#pragma once

#include "TDNLIB.h"
#include "../Fade/Fade.h"
#include "../Sound/SoundManager.h"
#include "../Input/InputMIDI.h"

/********************************************/
//	質問ウィンドウ
/********************************************/
class QuestionWindow
{
public:

	//------------------------------------------------------
	//	質問に答えるタイプ
	//------------------------------------------------------
	enum ANSWER_TYPE
	{
		YES_NO	// はいかいいえで答える(白鍵と黒鍵でいいかな)
	};

	//------------------------------------------------------
	//	コンストラクタ
	//------------------------------------------------------
	QuestionWindow(ANSWER_TYPE eAnswerType) :
		m_iStep(NO_ACTION),
		m_iAnswer(NO_ANSWER),
		m_pWindowImage(new tdn2DAnim("DATA/UI/Other/info_window.png")),
		m_lpcHeading(""),
		m_lpcQuestion("")
	{
		// 引数のタイプによって、アクションのクラスを変える
		switch (eAnswerType)
		{
		case QuestionWindow::YES_NO:
			m_pAnswerAction = std::make_unique<AnswerAction::YesNo>(this);
			break;
		default:
			assert(0);
			break;
		}
	}

	//------------------------------------------------------
	//	更新
	//------------------------------------------------------
	bool Update()
	{
		switch (m_iStep)
		{
		case 0:	/* ウィンドウが開くまでの間 */

			// 画像アニメ更新
			m_pWindowImage->Update();

			// 画像アニメーションが終わったら
			if (m_pWindowImage->GetAction()->IsEnd())
			{
				// 次のステップへ
				m_iStep++;
			}
			break;

		case 1:	/* 実際にプレイヤーが選択する間 */

			// 選択が終わったら
			if (m_pAnswerAction->Update())
			{
				// 画像の縮小アニメをセットする
				m_pWindowImage->OrderStretch(15, 1, 1, 1, 0);
				m_pWindowImage->Action();

				// フェードを解除
				Fade::Set(Fade::MODE::FADE_IN, 8, 0, Fade::m_byAlpha, 0x00000000);

				// 次のステップへ
				m_iStep++;
			}
			break;

		case 2:	/* ウィンドウが閉じるまでの間 */

			// 画像アニメ更新
			m_pWindowImage->Update();

			// 画像アニメーションが終わったら
			if (m_pWindowImage->GetAction()->IsEnd())
			{
				// 初期状態にする
				m_iStep = NO_ACTION;

				// 終了！
				return true;
			}
			break;
		}

		// ここに来たらまだ終わっていない
		return false;
	}

	//------------------------------------------------------
	//	描画
	//------------------------------------------------------
	void Render()
	{
		// 発動してなかったら出ていけぇ！！
		if (m_iStep == NO_ACTION)return;

		// 画像アニメ描画
		m_pWindowImage->Render(310, 232);

		//static int x(0), y(0);
		//
		//if (KeyBoardTRG(KB_RIGHT))x += 10;
		//if (KeyBoardTRG(KB_LEFT))x -= 10;
		//if (KeyBoardTRG(KB_DOWN))y += 10;
		//if (KeyBoardTRG(KB_UP))y -= 10;

		// 選択中のみ描画
		if (m_iStep == 1)
		{
			// 見出しと質問の文字列の描画
			tdnFont::RenderStringCentering(m_lpcHeading, 20, 482, 236);
			tdnFont::RenderStringCentering(m_lpcQuestion, 24, 660, 300);

			// 委譲描画
			m_pAnswerAction->Render();
		}
	}

	//------------------------------------------------------
	//	発動命令
	//------------------------------------------------------
	void Action(LPCSTR lpcHeading, LPCSTR lpcQuestion)
	{
		// 各文字列セット
		m_lpcHeading = lpcHeading;
		m_lpcQuestion = lpcQuestion;

		// アクションするなフラグ(-1)解除
		m_iStep = 0;

		// 解答を初期化
		m_iAnswer = NO_ANSWER;

		// 画像の引き伸ばしアニメをセットする
		m_pWindowImage->OrderStretch(15, 1, .0f);
		m_pWindowImage->Action();

		// フェードを若干薄暗くする
		Fade::Set(Fade::MODE::FADE_OUT, 8, 200, Fade::m_byAlpha, 0x00000000);

		// ウィンドウ開くSE
		g_pSE->Play("ウィンドウ1");
	}

	//------------------------------------------------------
	//	ゲッター
	//------------------------------------------------------
	int GetAnswer(){ return m_iAnswer; }
	bool isAction(){ return (m_iStep != NO_ACTION); }

private:

	//------------------------------------------------------
	//	質問のタイプ毎にクラスで持つ
	//------------------------------------------------------
	class AnswerAction
	{
	public:

		//------------------------------------------------------
		//	質問のタイプ基底
		//------------------------------------------------------
		class Base
		{
		public:
			Base(QuestionWindow *pParent) :m_pParent(pParent){}
			virtual ~Base(){}
			virtual bool Update() = 0;
			virtual void Render() = 0;

		protected:
			QuestionWindow  *m_pParent;	// このクラスを持つ親の実体
		};


		//------------------------------------------------------
		//	YESかNOで答えるタイプ
		//------------------------------------------------------
		class YesNo :public Base
		{
		public:
			YesNo(QuestionWindow *pParent) :Base(pParent){}
			bool Update()
			{
				// 白鍵キー(YESチェック)
				if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::WHITE))
				{
					// 1回目の選択なら
					if (m_pParent->m_iAnswer != TRUE)
					{
						// SEの再生
						g_pSE->Play("カーソル2");

						// 解答をYESにする
						m_pParent->m_iAnswer = TRUE;
					}

					// 2回同じのを選んでるので終了！
					else
					{
						// SEの再生
						g_pSE->Play("決定2");

						return true;
					}
				}

				// 黒鍵キー(NOチェック)
				else if (InputMIDIMgr->GetEPInputTRG(EP_INPUT::BLACK))
				{
					// 1回目の選択なら
					if (m_pParent->m_iAnswer != FALSE)
					{
						// SEの再生
						g_pSE->Play("カーソル2");

						// 解答をNOにする
						m_pParent->m_iAnswer = FALSE;
					}

					// 2回同じのを選んでるので終了！
					else
					{
						// SEの再生
						g_pSE->Play("決定2");

						return true;
					}
				}

				// まだ続くよ
				return false;
			}
			void Render()
			{
				tdnFont::RenderString("白鍵 :", 30, 680, 380);
				tdnFont::RenderString("はい　", 30, 780, 380, (m_pParent->m_iAnswer == TRUE) ? 0xffffff00 : 0x80ffffff);
				tdnFont::RenderString("黒鍵 :", 30, 680, 420);
				tdnFont::RenderString("いいえ", 30, 780, 420, (m_pParent->m_iAnswer == FALSE) ? 0xffffff00 : 0x80ffffff);
			}
		};
	};
	std::unique_ptr<AnswerAction::Base> m_pAnswerAction;


	//------------------------------------------------------
	//	順序
	//------------------------------------------------------
	int m_iStep;
	static const int NO_ACTION = -1;

	//------------------------------------------------------
	//	プレイヤーの選択した答え
	//------------------------------------------------------
	int m_iAnswer;
	static const int NO_ANSWER = -1;

	//------------------------------------------------------
	//	文字列
	//------------------------------------------------------
	LPCSTR m_lpcHeading;		// 見出しの文字列
	LPCSTR m_lpcQuestion;		// 質問の文字列

	//------------------------------------------------------
	//	画像
	//------------------------------------------------------
	std::unique_ptr<tdn2DAnim> m_pWindowImage;	// 枠の画像
};