#pragma once

//===============================================
//	前方宣言
//===============================================
struct NoteInfo;
struct SoflanInfo;
//struct MusicInfo;
class SoflanManager;
class JudgeManager;


/********************************************/
//	流れてくるノーツの情報
/********************************************/
struct PlayNoteInfo
{
	//UINT *EventTimeArray;	// ★ソフランするとずれるので、ソフラン分のイベントタイムをもつ
	NoteInfo note;				// ノーツ実体
	int position;			// 位置
	bool erase;				// 役目を終えたか
	int SoflanNo;			// 自分がどのソフランの位置か

	PlayNoteInfo(){}
	~PlayNoteInfo(){}
};


/********************************************/
//	ノーツ管理クラス
/********************************************/
class NoteManager
{
public:
	//===============================================
	//	コンストラクタ・デストラクタ
	//===============================================
	NoteManager();
	~NoteManager();

	//===============================================
	//	初期化・解放
	//===============================================
	void Initialize(SoflanManager *mng);
	void Release();

	//===============================================
	//	更新
	//===============================================
	void Update(JudgeManager *judge);


	//===============================================
	//	アクセサ
	//===============================================
	std::list<PlayNoteInfo*> *GetList(int lane);	// リスト取得
	UINT GetNumNorts(){ return m_iNumNorts; }
	void GetStartNotesLane(std::vector<int> *pOutLanes);
	void GetNoteAbsolutePos(const PlayNoteInfo &note, int *pos_y);	// 絶対的なノーツ座標(主に判定で使用)
	void GetNoteHiSpeedPos(const PlayNoteInfo &note, int *pos_y);	// BPMとか、ハイスピに依存したノーツの座標(主にUIで使用)
	void Set(MusicInfo *info);
	
private:
	//===============================================
	//	メンバ変数
	//===============================================
	SoflanManager *m_pSoflanMgr;		// ソフラン管理を指し示す
	UINT m_iNumNorts;					// ノーツ数
	char m_cBaseOctave;					// 基本オクターブ
	std::list<PlayNoteInfo*> *m_list;	// ノーツリスト

	//===============================================
	//	メンバ関数
	//===============================================
	void Clear();	// ノーツクリア
	void SetTick(int second, float bpm);
	// 最適化前と最適化後で分けたノーツ更新
	void BeforeOptimisationUpdate(JudgeManager *pJudge);
	void OptimisationedUpdate(JudgeManager *pJudge);
};