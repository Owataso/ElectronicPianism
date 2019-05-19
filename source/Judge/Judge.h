#pragma once

//===============================================
//	前方宣言
//===============================================
class UI;
class NoteManager;
class JudgeActionManager;
struct GameScore;
class GaugeManager;
enum class GAUGE_OPTION;
enum class JUDGE_TYPE;


/********************************************/
//	ゲージの増減を管理するクラス
/********************************************/
class JudgeManager
{
public:
	//===============================================
	//	コンストラクタ・デストラクタ
	//===============================================
	JudgeManager();
	~JudgeManager();


	//===============================================
	//	初期化
	//===============================================
	void Initialize(NoteManager *mng, GAUGE_OPTION op, bool bShowTiming);
	void Reset();


	//===============================================
	//	更新・描画
	//===============================================
	bool Update();	// 閉店したらtrueが返る
	void Render();


	//===============================================
	//	トリガーに応じて発動する
	//===============================================
	void JudgeNote(char cLaneNo);	// プレイヤーがキーを押した瞬間に呼び出す
	void JudgeUpKey(char cLaneNo);	// プレイヤーがキーを離した瞬間に呼び出す(CNの終了ノーツの判定)
	void LostNote(char cLaneNo);	// ノーツが判定ラインを過ぎたら呼び出す

	//===============================================
	//	トリガーに応じて発動する
	//===============================================
	bool isFullComboNow();	// フルコン演出中かどうか
	bool isFullComboEnd();	// フルコン演出終わったか


	//===============================================
	//	アクセサ
	//===============================================
	bool isCN(char cLaneNo){return m_bCN[cLaneNo];}

private:
	//===============================================
	//	メンバ変数
	//===============================================
	int m_iCombo;						// コンボはここで管理する
	bool *m_bCN;						// CNで使う。例のごとく、ヘッダーにインクルードしたくないので、可変配列ポインタにする
	NoteManager *m_pRefNoteMgr;			// ノーツ管理くん参照するだけ
	JudgeActionManager *m_pJudgeActMgr;	// 判定管理くん
	GaugeManager *m_GaugeManager;		// ゲージ管理くん
	GameScore *m_GameScore;				// GameScoreManager<Singleton>のデータを参照して、書き換えるポインタ

	//===============================================
	//	メンバ関数
	//===============================================
	JUDGE_TYPE ComputeJudgeResult(int iPosY, BOOL bUp);	// 判定して結果を返す
};