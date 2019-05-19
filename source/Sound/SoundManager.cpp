#include	"TDNLIB.h"
#include	"SoundManager.h"


//☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★
//
//								サウンド呼び出しID一覧														　☆★
//
//☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★☆★

//==============================================================================================
//				S	E
//==============================================================================================
const SE_Manager::DATA all_dataS[]
{
	//-------------------------------------------------------------------
	// 効果音関係
	//-------------------------------------------------------------------
	{ "C", "DATA/Sound/SE/C.wav", 20, false },
	{ "D", "DATA/Sound/SE/D.wav", 20, false },
	{ "E", "DATA/Sound/SE/E.wav", 20, false },
	{ "F", "DATA/Sound/SE/F.wav", 20, false },
	{ "G", "DATA/Sound/SE/G.wav", 20, false },
	{ "A", "DATA/Sound/SE/A.wav", 20, false },
	{ "B", "DATA/Sound/SE/B.wav", 20, false },
	{ "カーソル", "DATA/Sound/SE/cursor1.wav", 6, false },
	{ "カーソル2", "DATA/Sound/SE/cursor6.wav", 6, false },
	{ "決定2", "DATA/Sound/SE/decide2.wav", 6, false },
	{ "オプション開く", "DATA/Sound/SE/decide2.wav", 3, false },
	{ "オプション閉じる", "DATA/Sound/SE/decide2.wav", 3, false },
	{ "難易度変更", "DATA/Sound/SE/Cursor2.wav", 5, false },
	{ "戻る", "DATA/Sound/SE/back.wav", 2, false },
	{ "ウィンドウ1", "DATA/Sound/SE/menu1.wav", 1, false },
	{ "ウィンドウ2", "DATA/Sound/SE/menu2.wav", 1, false },
	{ "ランク", "DATA/Sound/SE/rank.wav", 1, false },
	//{ "END", nullptr }
};


//==============================================================================================
//				B	G	M
//==============================================================================================
const BGM_Manager::DATA all_dataB[]
{
	//{ "モードセレクト", "DATA/Sound/BGM/ModeSelect.wav", false },

	{ "END", nullptr }
};





//**************************************************************************************************************
//
//		サウンド管理クラス(winmainとframeworkで使うだけ)
//
//**************************************************************************************************************
//=============================================================================================
//		初	期	化
void SoundManager::Initialize()
{
	if (g_pSE)delete g_pSE;
	g_pSE = new SE_Manager;
	g_pSE->Initialize();
	if (g_pBGM) delete g_pBGM;
	g_pBGM = new BGM_Manager;
	g_pBGM->Initialize();
}

//=============================================================================================
//		解		放
void SoundManager::Release()
{
	SAFE_DELETE(g_pSE);
	SAFE_DELETE(g_pBGM);
}

//=============================================================================================
//		更新
void SoundManager::Update()
{
	g_pSE->Update();
	g_pBGM->Update();
}


///********************************************************************************************
//		リスナー情報設定！
void SoundManager::SetListenerPos(const Vector2 &pos)
{
	g_pSE->SetListener(pos);
}


//**************************************************************************************************************
//
//		SE管理クラス
//
//**************************************************************************************************************


//=============================================================================================
//		初	期	化
void SE_Manager::Initialize()
{
	play_manager = new tdnSoundSE;

	for (int i = 0; i < _countof(all_dataS); i++)
	{
		//if (strcmp(all_dataS[i].id, "END") == 0) break;	// 終端

		ID[all_dataS[i].id] = i;
		play_manager->Set(i, all_dataS[i].play_simultaneously, all_dataS[i].file_name, all_dataS[i].b3D);
	}

	// リスナーの初期設定
	m_ListenerPos = Vector2(tdnSystem::GetScreenSize().right * .5f, tdnSystem::GetScreenSize().bottom * .5f);
	//play_manager->SetListenerAll(
	//	Vector3(tdnSystem::GetScreenSize().right * .5f, tdnSystem::GetScreenSize().bottom * .5f, 0),	// リスナー座標(画面の真ん中と仮定する)
	//	Vector3(1, 0, 0),	// 正面ベクトル
	//	Vector3(0, 1, 0),	// 上方ベクトル
	//	Vector3(0, 0, 0)	// 移動値
	//	);
}
//
//=============================================================================================



//=============================================================================================
//		解		放
SE_Manager::~SE_Manager()
{
	delete play_manager;
}
//
//=============================================================================================



//=============================================================================================
//		更		新
void SE_Manager::Update()
{
	//play_manager->UpdateListener();
}
//
//=============================================================================================



//=============================================================================================
//		処		理
int SE_Manager::Play_in(int data_num, bool loop)
{
	if (data_num != TDNSOUND_PLAY_NONE)
	{
		play_manager->Play(data_num, loop);
	}
	return TDNSOUND_PLAY_NONE;
}

int SE_Manager::Play_in(int data_num, const Vector2 &pos, const Vector2 &move, bool loop)
{
	if (data_num != TDNSOUND_PLAY_NONE)
	{
		// ステレオ手動で設定してみる
		static const int DSBPAN_WIDTH = 10000;
		const int pan = (int)((DSBPAN_WIDTH / 8)*((pos.x - m_ListenerPos.x) / (tdnSystem::GetScreenSize().right / 2)));
		const int vol = DSBVOLUME_MAX + (int)((DSBVOLUME_MIN / 32) * (sqrtf((m_ListenerPos.x - pos.x)*(m_ListenerPos.x - pos.x)) / m_ListenerPos.x) + (DSBVOLUME_MIN / 16) * (sqrtf((m_ListenerPos.y - pos.y)*(m_ListenerPos.y - pos.y)) / m_ListenerPos.y));
		play_manager->SetPan(data_num, pan);
		play_manager->SetVolume(data_num, vol);

		return play_manager->Play(data_num, loop);
	}
	return TDNSOUND_PLAY_NONE;
}

int SE_Manager::Play(LPSTR _ID, bool loop)
{
	return Play_in(ID[_ID], loop);
}

int SE_Manager::Play(LPSTR _ID, const Vector2 &pos, const Vector2 &move, bool loop)
{
	return Play_in(ID[_ID], pos, move, loop);
}

void SE_Manager::Stop(LPSTR _ID, int no)
{
	play_manager->Stop(ID[_ID], no);
}

void SE_Manager::Stop_all()
{
	play_manager->AllStop();
}

void SE_Manager::SetTone(LPSTR _ID, int tone)
{
	play_manager->SetSpeed(ID[_ID], 1.0f + (float)tone*.048f);
}

void SE_Manager::SetListener(const Vector2 &pos)
{
	m_ListenerPos = pos;
	//play_manager->SetListenerPos(Vector3(pos.x, pos.y, 0));
	//play_manager->SetListenerMove(Vector3(move.x, move.y, 0));
}
//
//=============================================================================================






//**************************************************************************************************************
//
//		BGM管理クラス
//
//**************************************************************************************************************


//=============================================================================================
//		初	期	化
void BGM_Manager::Initialize()
{
	play_manager = new tdnSoundBGM;
	m_bPlayStream = false;

	for (int i = 0;; i++)
	{
		if (strcmp(all_dataB[i].id, "END") == 0) break;	// 終端

		ID[all_dataB[i].id] = i;
		play_manager->Set(i, all_dataB[i].file_name, all_dataB[i].b3D);
	}

	// リスナーの初期設定
	play_manager->SetListenerAll(
		Vector3(tdnSystem::GetScreenSize().right * .5f, tdnSystem::GetScreenSize().bottom * .5f, 0),	// リスナー座標(画面の真ん中と仮定する)
		Vector3(1, 0, 0),	// 正面ベクトル
		Vector3(0, 1, 0),	// 上方ベクトル
		Vector3(0, 0, 0)	// 移動値
		);
}
//
//=============================================================================================



//=============================================================================================
//		解		放
BGM_Manager::~BGM_Manager()
{
	delete play_manager;
}
//
//=============================================================================================

//=============================================================================================
//		更		新
void BGM_Manager::Update()
{
	play_manager->Update();
	//play_manager->UpdateListener();
}
//
//=============================================================================================



//=============================================================================================
//		処		理
void BGM_Manager::Play_in(int data_num, bool loop)
{
	if (data_num != TDNSOUND_PLAY_NONE)
	{
		play_manager->Play(data_num, loop);
	}
}

void BGM_Manager::Play_in(int data_num, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop)
{
	if (data_num != TDNSOUND_PLAY_NONE)
	{
		play_manager->Play(data_num, pos, front, move, loop);
	}
}

void BGM_Manager::Play(LPSTR _ID, bool loop)
{
	Play_in(ID[_ID], loop);
}

void BGM_Manager::Play(LPSTR _ID, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop)
{
	return Play_in(ID[_ID], pos, front, move, loop);
}

void BGM_Manager::Stop(LPSTR _ID)
{
	play_manager->Stop(ID[_ID]);
}

void BGM_Manager::Stop_all()
{
	play_manager->AllStop();
}

void BGM_Manager::Fade_in(LPSTR _ID, float fade_speed, bool loop)
{
	play_manager->FadeIn(ID[_ID], fade_speed, loop);
}

void BGM_Manager::Fade_out(LPSTR _ID, float fade_speed)
{
	play_manager->FadeOut(ID[_ID], fade_speed);
}

void BGM_Manager::Cross_fade(LPSTR inID, LPSTR outID, float fade_speed, bool loop)
{
	play_manager->CrossFade(ID[inID], ID[outID], fade_speed, tdnSoundBGM::CROSS_FADE_TYPE::END_OF_ETERNITY, loop);
}

void BGM_Manager::Cross_fade(LPSTR inID, LPSTR outID, float in_speed, float out_speed, bool loop)
{
	play_manager->CrossFade(ID[inID], ID[outID], in_speed, out_speed, tdnSoundBGM::CROSS_FADE_TYPE::END_OF_ETERNITY, loop);
}

void BGM_Manager::SetListener(const Vector2 &pos, const Vector2 &move)
{
	play_manager->SetListenerPos(Vector3(pos.x, pos.y, 0));
	play_manager->SetListenerMove(Vector3(move.x, move.y, 0));
}
//
//=============================================================================================



//=============================================================================================
//		実		体
SE_Manager *g_pSE(nullptr);
BGM_Manager *g_pBGM(nullptr);
//
//=============================================================================================