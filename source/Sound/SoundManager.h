#pragma once

//********************************************************************************************************************
//
//		クラスの宣言
//
//********************************************************************************************************************
//		サウンドマネージャー(グローバル実体で関数呼び出し)
//****************************************************************************************************************

namespace SoundManager
{
	void Initialize();
	void Release();
	void Update();
	void SetListenerPos(const Vector2 &pos);
}

class iexSoundBase;
class tdnSoundSE;
class tdnSoundBGM;

class SE_Manager
{
private:
	std::map<LPSTR, int> ID;	// char*型で番号を振り分ける

	tdnSoundSE *play_manager;	// iexSound
	Vector2 m_ListenerPos;		// 聞こえる人の座標


	int Play_in(int data_num, bool loop);
	int Play_in(int data_num, const Vector2 &pos, const Vector2 &move, bool loop);

public:

	//===============================================
	//	音データ
	//===============================================
	struct	DATA{
		LPSTR id;					// 呼び出す際に使用する識別コード
		LPSTR file_name;			// ファイルパス
		int play_simultaneously;	// 同一の音を複数再生する最大値
		bool b3D;					// 3Dサウンドフラグ
	};


	//===============================================
	//	初期化と解放
	//===============================================
	SE_Manager(){}
	~SE_Manager();
	void Initialize();

	//===============================================
	//	更		新
	//===============================================
	void Update();


	//===============================================
	//	処		理
	//===============================================
	bool isPlay(LPSTR _ID, int no);
	int Play(LPSTR _ID, bool loop = false);																			// 簡易版																								// ボリューム設定版
	int Play(LPSTR _ID, const Vector2 &pos, const Vector2 &move = Vector2(0, 0), bool loop = false);				// 3D設定版
	void SetTone(LPSTR _ID, int tone);
	void Stop(LPSTR _ID, int no);																					// Playで返ってきた数値を入れる
	void Stop_all();																								// 全部止める
	void SetFX(LPSTR _ID, int no, DXA_FX flag);

	void SetListener(const Vector2 &pos);			// リスナー情報
};

class BGM_Manager
{
private:
	std::map<LPSTR, int> ID;	// char*型で番号を振り分ける

	tdnSoundBGM *play_manager;	// iexSound

	// ストリーミング用
	tdnStreamSound *m_pStreamSound;
	bool m_bPlayStream;

	void Play_in(int data_num, bool loop);
	void Play_in(int data_num, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop);

public:

	//===============================================
	//	音データ
	//===============================================
	struct	DATA{
		LPSTR id;					// 呼び出す際に使用する識別コード
		LPSTR file_name;			// ファイルパス
		bool b3D;
	};


	//===============================================
	//	初期化と解放
	//===============================================
	void Initialize();
	~BGM_Manager();

	//===============================================
	//	更		新
	//===============================================
	void Update();


	//===============================================
	//	処		理
	//===============================================
	void Play(LPSTR _ID, bool loop = true);																											// 簡易版																								// ボリューム設定版
	void Play(LPSTR _ID, const Vector3 &pos, const Vector3 &front = Vector3(0, 0, -1), const Vector3 &move = Vector3(0, 0, 0), bool loop = true);	// 3D設定版
	void Stop(LPSTR _ID);																															// Playで返ってきた数値を入れる
	void Stop_all();																																// 全部止める

	void Fade_in(LPSTR _ID, float fade_speed, bool loop = true);
	void Fade_out(LPSTR _ID, float fade_speed);
	void Cross_fade(LPSTR inID, LPSTR outID, float fade_speed = .0075f, bool loop = true);
	void Cross_fade(LPSTR inID, LPSTR outID, float in_speed, float out_speed, bool loop);
	bool isPlay(LPSTR _ID);
	void SetSpeed(LPSTR _ID, float speed);

	void SetListener(const Vector2 &pos, const Vector2 &move);											// リスナー情報

	tdnStreamSound *PlayStream(LPSTR filename, bool bLoop = true){ return play_manager->PlayStream(filename, bLoop); }
	void PlayStreamIn(LPSTR filename, bool bLoop = true)
	{
		if (m_bPlayStream) m_pStreamSound->Stop();
		m_pStreamSound = play_manager->PlayStream(filename, bLoop);
		m_bPlayStream = true;
	}
	void StopStreamIn()
	{
		if (m_bPlayStream)
		{
			m_pStreamSound->Stop();
			m_bPlayStream = false;
		}
	}
};

//===============================================
//	実体(関数呼び出しはこれらを使う)
//===============================================
extern SE_Manager *g_pSE;
extern BGM_Manager *g_pBGM;