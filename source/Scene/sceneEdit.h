#pragma once

//===============================================
//	前方宣言
//===============================================
class NoteManager;
class UI;
class SoflanManager;
class Player;
class JudgeManager;
struct MusicInfo;
class EditFolderInfo;
class BeatBar;


/********************************************/
//	データを読み込み、譜面を作ったりするのをやるシーン
/********************************************/
class	sceneEdit : public BaseScene
{
public:
	//------------------------------------------------------
	//	初期化・開放
	//------------------------------------------------------
	bool Initialize();
	~sceneEdit();

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	bool Update();
	void Render();


private:
	//===============================================
	//	定数
	//===============================================
	enum class MODE
	{
		FIRST,				// 新規作成 or ロード
		NEW_CREATE,			// フォルダ新規作成
		DIRECTORY_SELECT,	// どのファイルをロードするか
		DIFFICULTY_SELECT,	// 難易度選択
		SETTING_INFO,		// 譜面とか情報を設定していく
		SETTING_WAV,		// 主にWAVデータの設定
		LOAD_OK,			// MIDIとWAVの両方読み込めた
		PLAYING				// 曲と譜面再生なう
	};

	/********************************************/
	//	ステート委譲クラス
	/********************************************/
	class Mode
	{
	public:
		//===============================================
		//	基底
		//===============================================
		class Base
		{
		protected:
			int m_step;						// 汎用的なステップ変数

		public:
			sceneEdit *me;

			Base(sceneEdit *me) :me(me), m_step(0){}
			virtual ~Base(){}
			virtual void Initialize(){}
			virtual void Update(){}
			virtual void Render(){}
		};

		//===============================================
		//	最初に新規フォルダか既存フォルダかを選ばせる
		//===============================================
		class First : public Base
		{
		public:
			First(sceneEdit *me) : Base(me){}
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	(新規フォルダを選んだ場合の分岐先)曲フォルダ作成
		//===============================================
		class NewCreateDirectry :public Base
		{
		public:
			NewCreateDirectry(sceneEdit *me) :Base(me){}
			void Initialize();
			void Update();
			void Render();

			static bool bThread;
			static void NameInput(void *arg);
		};

		//===============================================
		//	曲フォルダ選択
		//===============================================
		class DirectrySelect : public Base
		{
		private:
			int m_SelectCursor;			// フォルダーの選択カーソル
			DirectoryInfo *m_pDirectry;	// ディレクトリに関する情報が入った構造体
		public:
			DirectrySelect(sceneEdit *me) : Base(me), m_SelectCursor(0), m_pDirectry(nullptr){}
			~DirectrySelect(){ SAFE_DELETE(m_pDirectry); }
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	難易度選択
		//===============================================
		class DifficultySelect : public Base
		{
		private:

		public:
			DifficultySelect(sceneEdit *me) : Base(me){}
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	譜面とかの情報をセットしていく
		//===============================================
		class SettingInfo : public Base
		{
		private:
			// 曲データとか譜面データの読み込みを呼び出す
			void UpdateLoadFile();
		public:
			SettingInfo(sceneEdit *me) : Base(me){}
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	音源情報の設定
		//===============================================
		class SettingWAV : public Base
		{
		public:
			SettingWAV(sceneEdit *me) :Base(me){}
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	プレイ待ち・書き出し
		//===============================================
		class LoadOK :public Base
		{
		public:
			LoadOK(sceneEdit *me) :Base(me){}
			void Initialize();
			void Update();
			void Render();
		};

		//===============================================
		//	テストプレイ
		//===============================================
		class Playing :public Base
		{
		private:
			NoteManager *note_mng;
		public:
			Playing(sceneEdit *me) :Base(me){}
			void Initialize();
			void Update();
			void Render();
		};
	};


	//===============================================
	//	メンバ変数
	//===============================================
	int m_iShift;					// 曲と譜面の差の調整用
	int m_iSelectDifficulty;		// 選択難易度
	int m_iPlayMusicNo;				// フォルダ内での流す曲の番号(流さないなら-1)
	int m_iPlayTimer;				// 音源を設定しない場合、曲の再生時間
	bool bMIDILoad;					// MIDIのロード状況
	Mode::Base *m_pMode;			// モード委譲ポインタ
	MusicInfo *m_MusicInfo;			// 選曲情報
	EditFolderInfo *m_FolderInfo;	// 選択フォルダー情報
	std::string m_sMusicName;		// 曲名


	//===============================================
	//	音楽ゲーム部分を構成する各実体
	//===============================================
	NoteManager *m_pNoteMgr;
	UI *m_pUI;
	SoflanManager *m_pSoflanMgr;
	Player *m_pPlayer;
	JudgeManager *m_pJudge;
	BeatBar *m_pBeatBar;



	//===============================================
	//	モード変更
	//===============================================
	void ChangeMode(MODE m)
	{
		if (m_pMode) delete m_pMode;
		switch (m)
		{
		case MODE::SETTING_WAV:
			m_pMode = new Mode::SettingWAV(this);
			break;
		case MODE::LOAD_OK:
			m_pMode = new Mode::LoadOK(this);
			break;
		case MODE::PLAYING:
			m_pMode = new Mode::Playing(this);
			break;
		case MODE::NEW_CREATE:
			m_pMode = new Mode::NewCreateDirectry(this);
			break;
		case MODE::FIRST:
			m_pMode = new Mode::First(this);
			break;
		case MODE::DIRECTORY_SELECT:
			m_pMode = new Mode::DirectrySelect(this);
			break;
		case MODE::SETTING_INFO:
			m_pMode = new Mode::SettingInfo(this);
			break;
		case MODE::DIFFICULTY_SELECT:
			m_pMode = new Mode::DifficultySelect(this);
			break;
		default:
			assert(0);	// 例外処理
			break;
		}
		m_pMode->Initialize();
	}
};
