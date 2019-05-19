#include	"TDNLIB.h"
#include	"system/FrameworkEx.h"
#include	"LoadSceneThread.h"

//=============================================================================================
//		コンストラクタ
LoadSceneThreadManager::LoadSceneThread::LoadSceneThread(BaseScene *pNewScene) :

//次のシーンのポインタは後で使うのでnewSceneに保存しておく。
m_pNewScene(pNewScene),

// ★コンストラクタでスレッドを生成 ※メンバ関数を指定する場合は、第二引数にthisを入れること。
m_thread(&LoadSceneThreadManager::LoadSceneThread::ThreadFunction, this, pNewScene),

// スレッドフラグ
m_bThread(true)
{

}

//=============================================================================================

//=============================================================================================
//		デストラクタ
LoadSceneThreadManager::LoadSceneThread::~LoadSceneThread()
{
	// ロード中に消してしまったら、全部読み込むまで待機(落ちるの防止)
	if (m_bThread)
	{
		//MessageBoxA(0, "スレッド読み込み中にプログラムが停止しました。スレッドが終わるまで待機します。", "意図せぬ終了", MB_OK);
		m_thread.join();
		//MessageBoxA(0, "待機終了。プログラムは正常に解放されます。多分", "アイルーは神", MB_OK);
	}
	else m_thread.join();	// 読み込みスレッド自体は完了しているので、detach(普通にスレッド落とす)でもいいけど、念のためにjoin(スレッドが完全に終了するまで待機してから落とす)にする
}
//=============================================================================================


//=============================================================================================
//		スレッド処理
void LoadSceneThreadManager::LoadSceneThread::ThreadFunction(void *arg)
{
	BaseScene* scene = (BaseScene*)arg;

	// ★ここで本体のsceneが必要な時間をかかる初期化を行う
	scene->Initialize();

	// この時点でロードフラグをON。シーンを切り替える際の読み込みをしないようにする
	scene->m_bLoad = true;

	//	スレッド終了処理
	m_bThread = false;
	//_endthread();
}


//
//=============================================================================================