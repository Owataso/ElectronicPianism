#pragma once

//===============================================
//	前方宣言
//===============================================
class tdnSoundBase;
class tdnSoundBGM;


/********************************************/
//	サウンドの再生位置管理
/********************************************/
class PlayCursor
{
public:
	//===============================================
	//	実体取得
	//===============================================
	static PlayCursor* GetInstance() { static PlayCursor instance; return &instance; }

	//===============================================
	//	初期化・解放
	//===============================================
	void Initialize();
	void Release();

	//===============================================
	//	サウンド制御
	//===============================================
	void SetMusic(LPSTR filename);
	void Play(int Mlimit = 0);	// ミリ秒単位での再生時間を設定
	void Stop();
	void Pause();
	void Resume();

	//===============================================
	//	アクセサ
	//===============================================
	int GetMSecond();	// ミリ秒
	void SetShift(int val){ m_iShift = val; }
	void SetSpeed(float speed){ m_pSoundPlayer->SetSpeed(0, speed); }
	bool isEnd();

private:
	//===============================================
	//	メンバ変数
	//===============================================
	tdnSoundBGM *m_pSoundPlayer;	// 曲流す用
	int m_iDelta;					// デルタタイム
	int m_iStart;					// 計測開始時間
	int m_iMSecond;					// 経過時間<秒/1000>
	int m_iLimit;					// 設定時間
	int m_iShift;					// 補正時間
	bool m_bSoundPlay;				// サウンド再生フラグ

	//===============================================
	//	シングルトンの作法
	//===============================================
	PlayCursor();
	PlayCursor(const PlayCursor&){}
	PlayCursor operator=(const PlayCursor&){}
};

/********************************************/
//	インスタンス簡略化
/********************************************/
#define play_cursor (PlayCursor::GetInstance())