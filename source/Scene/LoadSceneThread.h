#pragma once

class LoadSceneThreadManager
{
public:
	//------------------------------------------------------
	//	実体の取得
	//------------------------------------------------------
	static LoadSceneThreadManager *GetInstance(){ static LoadSceneThreadManager i; return &i; }

	//------------------------------------------------------
	//	シーン作成
	//------------------------------------------------------
	void Initialize(BaseScene *NewScene){ if (m_pLoadSceneThread){ delete m_pLoadSceneThread; }m_pLoadSceneThread = new LoadSceneThread(NewScene); m_bAction = true; }

	//------------------------------------------------------
	//	解放
	//------------------------------------------------------
	void Release(){ SAFE_DELETE(m_pLoadSceneThread); m_bAction = false; }
	~LoadSceneThreadManager(){ Release(); }

	//------------------------------------------------------
	//	ゲッター
	//------------------------------------------------------
	BaseScene *GetNewScene(){ return m_pLoadSceneThread->GetNewScene(); }
	bool isInitializeEnd(){ return m_pLoadSceneThread->isInitializeEnd(); }
	bool isAction(){ return m_bAction; }

private:

	//-------------------------------------------------------------------------------
	//	スレッドを読み込む内部クラス(読み込むスレッドを持っているのはこのクラス)
	//-------------------------------------------------------------------------------
	class LoadSceneThread
	{
	public:

		//------------------------------------------------------
		//	スレッド関係
		//------------------------------------------------------
		void ThreadFunction(void *arg);
		bool m_bThread;

		//------------------------------------------------------
		//	初期化と開放
		//------------------------------------------------------
		LoadSceneThread(BaseScene *NewScene);
		~LoadSceneThread();

		//------------------------------------------------------
		//	ゲッター
		//------------------------------------------------------
		BaseScene *GetNewScene(){ return m_pNewScene; }
		bool isInitializeEnd(){ return !m_bThread; }

	private:
		//------------------------------------------------------
		//	スレッドで読み込む新しいシーン
		//------------------------------------------------------
		BaseScene*	m_pNewScene;

		//------------------------------------------------------
		//	スレッド処理くん
		//------------------------------------------------------
		std::thread m_thread;
	};

	LoadSceneThread *m_pLoadSceneThread;
	bool m_bAction;

	// シングルトンの作法
	LoadSceneThreadManager():m_pLoadSceneThread(nullptr), m_bAction(false){}
	LoadSceneThreadManager(const LoadSceneThreadManager&){}
	LoadSceneThreadManager& operator=(const LoadSceneThreadManager&){}
};


#define LoadSceneThreadMgr (LoadSceneThreadManager::GetInstance())