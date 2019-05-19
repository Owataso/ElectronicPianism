#pragma once

//===============================================
//	前方宣言
//===============================================
enum class JUDGE_TYPE;
enum class JUDGE_TIMING;
class JudgeAction;
class FullComboEffect;
class JudgeEffect;
class JudgeManager;


/********************************************/
//	判定アクション(エフェクトとか)を管理する
/********************************************/
class JudgeActionManager
{
public:
	//===============================================
	//	前方宣言
	//===============================================
	JudgeActionManager(UINT NumNotes, JudgeManager *pJudgeMgr);
	~JudgeActionManager();

	//===============================================
	//	初期化
	//===============================================
	void Initialize(bool bShowTiming);
	void Clear();

	//===============================================
	//	更新・描画
	//===============================================
	void Update();
	void Render();


	//===============================================
	//	アクセサ
	//===============================================
	void Set(char cLaneNo, int iCombo, JUDGE_TYPE type, JUDGE_TIMING eTiming);
	bool isFullComboNow() { return m_bFullCombo; }	// フルコン演出中かどうか
	bool isFullComboEnd();							// フルコン演出終わったか

	//===============================================
	//	ボム用の構造体
	//===============================================
	struct ExplosionInfo
	{
		tdn2DObj *pImage;		// 爆発エフェクト画像
		int iNumPanel;			// コマの数
		ExplosionInfo() :pImage(nullptr), iNumPanel(0) {}
		~ExplosionInfo() { SAFE_DELETE(pImage); }
	}m_tagExplosion;
private:

	//===============================================
	//	メンバ変数
	//===============================================
	JudgeManager *m_pJudgeMgr;					// 自分の実体を持っている親である判定管理くんのポインタ(CNフラグの取得で使う)
	const UINT m_iNumNote;						// 総ノーツ数(フルコン判定用)
	tdn2DObj *m_pJudgeStr;						// 判定の文字画像
	tdn2DObj *m_pCNEffect;						// CN中のエフェクト(これは共通なので、ここに置く)
	static const int m_ciJudgeAppTime;			// 判定が出てる時間
	std::list<JudgeEffect*> m_JudgeEffectList;	// 爆発のエフェクトのリスト
	JudgeAction **m_pJudgeActions;				// 判定アクション(コンボ数と判定の文字)ここでは可変配列ポインタで使う
	FullComboEffect *m_pFullCombo;				// フルコン演出用
	bool m_bFullCombo;							// フルコンしてからデータ読むと酷い処理落ちでダサいので先に容量確保してこのフラグをONにしたら発動
	int m_iSuperAnimFrame, m_iSuperAnimPanel;	// SUPERのアニメーション
	bool m_bShowTiming;						// 判定表示

	//===============================================
	//	メンバ関数
	//===============================================
	void OnFullCombo();							// フルコン演出ON
};


/********************************************/
//	判定結果(SUPER)とか
/********************************************/
class JudgeAction
{
public:
	//===============================================
	//	コンストラクタ
	//===============================================
	JudgeAction(int iOctave, bool bShowTiming);

	//===============================================
	//	更新・描画
	//===============================================
	void Update() { if(m_iAppTime > 0)m_iAppTime--; }
	void Render(tdn2DObj *pJudgeStr, int iPanel);

	//===============================================
	//	アクセサー
	//===============================================
	void Set(int iCombo, int iTime, JUDGE_TYPE eType, JUDGE_TIMING eTiming);
	void Off() { m_iCombo = 0; m_iAppTime = 0; }

private:
	//===============================================
	//	メンバ変数
	//===============================================
	int m_iOctave;			// 1オクターブが自分の判定描画領域
	int m_iCombo;			// コンボ数
	int m_iAppTime;			// 出現時間
	int m_iPosX;			// 自分の座標(Yは共通なのでなし)
	BYTE m_byAlpha;			// 判定文字のアルファ
	bool m_bShowTiming;	// 判定表示
	JUDGE_TYPE m_eJudgeType;	// 何の判定か
	JUDGE_TIMING m_eJudgeTiming;	// 判定タイミング

	//===============================================
	//	デフォルトコンストラクタ封印
	//===============================================
	JudgeAction(){}
};


/********************************************/
//	ボム
/********************************************/
class JudgeEffect
{
public:
	//===============================================
	//	コンストラクタ
	//===============================================
	JudgeEffect(char cLaneNo, JudgeActionManager::ExplosionInfo *pExplosion) : 
		m_cLaneNo(cLaneNo), 
		m_pRefExplosion(pExplosion), 
		m_iCurrentFrame(0), 
		m_iCurrentPanel(0){}

	//===============================================
	//	更新・描画
	//===============================================
	bool Update();
	void Render();

private:
	//===============================================
	//	メンバ変数
	//===============================================
	char m_cLaneNo;			// レーン番号
	int m_iCurrentFrame;	// 現在フレーム
	int m_iCurrentPanel;	// 現在のコマ
	JudgeActionManager::ExplosionInfo *m_pRefExplosion;	// 画像とそのコマ数が入ってる情報
};


/********************************************/
//	フルコン演出
/********************************************/
class FullComboEffect
{
public:
	//===============================================
	//	コンストラクタ
	//===============================================
	FullComboEffect() :
		m_pFullComboCross(new FullComboCross),
		m_pFullComboExplosion(new FullComboExplosion){}

	//===============================================
	//	更新・描画
	//===============================================
	void Update();
	void Render();

	//===============================================
	//	アクセサ
	//===============================================
	void Reset(){ m_pFullComboCross->Reset(); m_pFullComboExplosion->Reset(); }
	bool isEnd(){ return m_pFullComboCross->isEnd(); }

private:
	//===============================================
	//	フルコンボ文字の交差
	//===============================================
	class FullComboCross
	{
	public:
		FullComboCross():m_iStep(0), m_byAlpha(0),
			m_pFull(new Info("DATA/UI/FullCombo/full.png", Vector2(50, 340), 12)),
			m_pCombo(new Info("DATA/UI/FullCombo/combo.png", Vector2(950, 400), -12)){}

		void Update();
		void Render();
		void Reset(){ m_pFull->Reset(); m_pCombo->Reset(); m_byAlpha = 0; m_iStep = 0; }
		bool isEnd(){ return (m_iStep == 4); }
	private:
		class Info
		{
		public:
			Vector2 pos;		// 座標
			const Vector2 OrgPos;
			std::unique_ptr<tdn2DObj> pImage;	// 画像
			Info(char *filename, const Vector2 &pos, float force) :pImage(new tdn2DObj(filename)), OrgPos(pos), OrgForce(force){ Reset(); }
			void Update()
			{
				moveX = moveX*.5f + force * .5f;	// 移動値補間
				pos.x += moveX;					// 座標更新
			}
			void Render(){ pImage->Render((int)pos.x, (int)pos.y); }
			void Deceleration(){ force = OrgForce * .065f; }	// 減速
			void ReAcceleration(){ force = OrgForce; }		// 再加速
			void Reset(){ pos = OrgPos; force = OrgForce; moveX = 0; pImage->SetARGB(0x00ffffff); }
		private:
			float moveX;		// 移動値
			float force;		// 移動にかかる力(補間で使う)
			const float OrgForce;	// 最初の移動力(減速して戻る時に使う)
		};
		std::unique_ptr<Info> m_pFull, m_pCombo;

		int m_iStep;		// 移動→減速→元の速度で移動のステップ
		BYTE m_byAlpha;	// 文字のアルファ(Info内でしないのは、ステップ分けしたかったから)
	};

	//===============================================
	//	フルコン爆発
	//===============================================
	class FullComboExplosion
	{
	public:
		FullComboExplosion();
		void Update();
		void Render();
		void Reset(){ m_iAnimFrame = m_iAnimPanel = 0; }
		bool isEnd(){ return (m_iAnimPanel == 59); }
	private:
		std::unique_ptr<tdn2DObj> m_pImage;
		int m_iAnimFrame, m_iAnimPanel;
	};


	//===============================================
	//	メンバ変数
	//===============================================
	std::unique_ptr<FullComboCross> m_pFullComboCross;
	std::unique_ptr<FullComboExplosion> m_pFullComboExplosion;
};