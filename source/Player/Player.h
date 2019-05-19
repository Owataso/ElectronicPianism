#pragma once

//===============================================
//	前方宣言
//===============================================
class UI;
class NoteManager;
class JudgeManager;


/********************************************/
//	演奏するプレイヤー
/********************************************/
class Player
{
public:
	//===============================================
	//	定数
	//===============================================
	enum class MODE { YOU, AUTO };

	//===============================================
	//	初期化・解放
	//===============================================
	Player();
	~Player();
	void Initialize(int iOctaveWidth, UI *pUI, NoteManager *pNoteMgr, JudgeManager *pJudgeMgr, MODE eMode);

	//===============================================
	//	更新
	//===============================================
	void Update();

private:
	//===============================================
	//	操作委譲クラス
	//===============================================
	class Input
	{
	public:
		class Base
		{
		protected:
			Player *me;
		public:
			Base(Player *me) :me(me) {}
			virtual void Update() = 0;
		};
		/*実際にやってもらう人*/
		class You : public Base
		{
		public:
			You(Player *me) :Base(me) {}
			void Update()override;
		};
		/*DJ AUTOさん*/
		class DJ_AUTO : public Base
		{
		public:
			DJ_AUTO(Player *me) :Base(me) {}
			void Update()override;
		};
	};


	//===============================================
	//	メンバ変数
	//===============================================
	int m_iNumKey;				// キーの数
	BYTE *m_byKeyList;			// キーボードのキーコードリスト
	bool *m_bON;				// 押してる顔してないかの可変配列
	Input::Base *m_pInput;		// 委譲
	UI *m_pUI;					// 参照するだけUI
	NoteManager *m_pNoteMgr;	// 参照するだけNote
	JudgeManager *m_pJudgeMgr;	// 参照するだけJudge


	//===============================================
	//	メンバ関数
	//===============================================
	void ControlSudden();				// サドゥンオプション処理
	void ControlHiSpeed();				// ハイスピオプション処理
	void LoadKeyCode(int iOctaveWidth);	// キーボード入力のキーコード読み込み
};