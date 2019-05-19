#include	"TDNLIB.h"
#include	"system/Framework.h"
#include	"sceneLoading.h"
#include	"sceneEdit.h"

//=============================================================================================
//		グ	ロ	ー	バ	ル	領	域
bool sceneLoading::isThread;


//=============================================================================================
//		初	期	化	と	開	放
bool sceneLoading::Initialize(BaseScene *newScene)
{
	m_pImage = std::make_unique<tdn2DObj>("DATA/UI/Loading/loading.png");
	m_pGauge = std::make_unique<tdn2DObj>("DATA/UI/Loading/loading_gauge.png");
	m_AnimeCount = m_AnimeFrame = 0;
	m_fLoadGaugePercent = 0;

	//	別スレッド作成
	//次のシーンのポインタは後で使うのでnewSceneに保存しておく。
	m_newScene = newScene;

	isThread = true;
	_beginthread(Thread_funk, 0, (void*)m_newScene);

	return true;
}


//=============================================================================================


//=============================================================================================
//		ス	レ	ッ	ド	処	理
void sceneLoading::Thread_funk(void *arg)
{
	BaseScene* scene = (BaseScene*)arg;

	scene->Initialize();	// ★ここで本体のsceneが必要な時間をかかる初期化を行う
	scene->m_bLoad = true;	// この時点でロードフラグをON。シーンを切り替える際の読み込みをしないようにする

	//	スレッド終了処理
	isThread = false;
	_endthread();
}


//
//=============================================================================================

//=============================================================================================
//		更			新
bool sceneLoading::Update()
{
	// 画像アニメーション処理
	static const int AnimeSpeed = 10;
	if (++m_AnimeFrame > AnimeSpeed)
	{
		m_AnimeFrame = 0;
		if (++m_AnimeCount > 5) m_AnimeFrame = 0;
	}

	// ロード割合補間処理
	m_fLoadGaugePercent = m_fLoadGaugePercent*.5f + m_newScene->m_fLoadPercentage*.5f;

	//ロードが終わったら、シーンをチェンジ
	if (!isThread)
	{
		MainFrame->ChangeScene(m_newScene);
	}

	return true;
}
//
//=============================================================================================


//=============================================================================================
//		描			画
void sceneLoading::Render()
{
	
	tdnView::Clear();

	tdnPolygon::Rect(0, 0, tdnSystem::GetScreenSize().right, tdnSystem::GetScreenSize().bottom , RS::COPY, 0xff000000);
	m_pImage->Render(720, 540, 512, 64, 0, m_AnimeCount * 64, 512, 64);

	// ゲージ中身
	m_pGauge->Render(720, 620, (int)(m_fLoadGaugePercent * 512), 64, 0, 64, (int)(m_fLoadGaugePercent * 512), 64);
	// ゲージ枠
	m_pGauge->Render(720, 620, 512, 64, 0, 0, 512, 64);
}
//
//=============================================================================================