#pragma once

class AlphabetRenderer;

/********************************************/
//	初回エントリーシーン
/********************************************/
class sceneEntry : public BaseScene
{
public:
	//------------------------------------------------------
	//	初期化・開放
	//------------------------------------------------------
	bool Initialize();
	~sceneEntry();

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	bool Update();
	void Render();

private:

	//===============================================
	//	定数
	//===============================================
	enum class NO_PLAYER_DATA_STATE
	{
		SELECT,			// 名前入力を選択するか、そのままデータなしで始める
		NAME_INPUT,		// 名前入力のとき
		MAX
	};


	/********************************************/
	//	プレイヤーデータがないときのステート
	/********************************************/
	class NoDataState
	{
	public:
		//===============================================
		//	基底
		//===============================================
		class Base
		{
		protected:
			sceneEntry *m_parent;
		public:
			Base(sceneEntry *me) :m_parent(me){}
			virtual ~Base(){}
			virtual void Update()=0;
			virtual void Render()=0;
		};

		//===============================================
		//	データなしで始めるか名前入力して新規作成か選ぶ
		//===============================================
		class Select : public Base
		{
		public:
			Select(sceneEntry *me) :Base(me){}
			void Update();
			void Render();
		};

		//===============================================
		//	名前入力
		//===============================================
		class NameInput : public Base
		{
		private:
			int m_cursor;		// 名前入力のカーソル
			char m_InputName[11];	// 入力バッファ
			int m_InputStep;		// 入力バッファの添え字になる
			AlphabetRenderer *m_pAlphabetRenderer;	// アルファベット描画
			bool m_bEnd;			// 終わってSE流れるまで待つ用

			tdnSoundSE *m_pSE;	// ほぼ来ることないので、ここだけのSEは自分で持つ
		public:
			NameInput(sceneEntry *me);
			~NameInput();
			void Update();
			void Render();
		};
	};

	//===============================================
	//	メンバ変数
	//===============================================
	bool m_bLoadSaveData;				// プレイヤーデータが存在するか
	NoDataState::Base *m_pNoDataState;	// 委譲


	//===============================================
	//	ステート変更
	//===============================================
	void ChangeMode(NO_PLAYER_DATA_STATE mode)
	{
		if (m_pNoDataState) delete m_pNoDataState;
		switch (mode)
		{
		case NO_PLAYER_DATA_STATE::SELECT:
			m_pNoDataState = new NoDataState::Select(this);
			break;

		case NO_PLAYER_DATA_STATE::NAME_INPUT:
			m_pNoDataState = new NoDataState::NameInput(this);
			break;
		}
	}
};