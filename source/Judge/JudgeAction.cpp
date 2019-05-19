#include "TDNLib.h"
#include "../Music/MusicInfo.h"
#include "JudgeAction.h"
#include "Judge.h"
#include "../Music/PlayCursor.h"
#include "../Data/CustomizeData.h"
//JudgeEffect *JudgeEffect::roots = nullptr;

const int JudgeActionManager::m_ciJudgeAppTime(45);

//*****************************************************************************************************************************
//
//		判定アクション総合管理
//
//*****************************************************************************************************************************
//*****************************************************************************************************************************
//	初期化・解放
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
JudgeActionManager::JudgeActionManager(UINT NumNote, JudgeManager *pJudgeMgr) :
	m_pJudgeMgr(pJudgeMgr), 
	m_iNumNote(NumNote), 
	m_pJudgeActions(nullptr), 
	m_pJudgeStr(nullptr), 
	m_pFullCombo(nullptr), 
	m_bFullCombo(false), 
	m_pCNEffect(nullptr),
	m_iSuperAnimFrame(0),
	m_iSuperAnimPanel(0)
{
}
void JudgeActionManager::Initialize(bool bShowTiming)
{
	// 爆発エフェクト
	m_tagExplosion.pImage = new tdn2DObj((char*)CustomizeDataMgr->GetSetedSkin(CUSTOMIZE_TYPE::EXPLOSION).c_str());	// 画像
	m_tagExplosion.iNumPanel = m_tagExplosion.pImage->GetWidth() / 128;

	// CNエフェクト
	m_pCNEffect = new tdn2DObj("DATA/Effect/CN.png");												// アニメのコマの数

	// 判定文字画像
	m_pJudgeStr = new tdn2DObj("DATA/UI/Playing/judge.png");
	m_pJudgeStr->SetARGB((BYTE)245, (BYTE)255, (BYTE)255, (BYTE)255);

	// 判定アクションくん
	m_pJudgeActions = new JudgeAction*[MAX_OCTAVE];
	for (int i = 0; i < MAX_OCTAVE; i++) m_pJudgeActions[i] = new JudgeAction(i, bShowTiming);

	// フルコンエフェクトを先に生成
	m_pFullCombo = new FullComboEffect;
}

//------------------------------------------------------
//	解放
//------------------------------------------------------
JudgeActionManager::~JudgeActionManager()
{
	SAFE_DELETE(m_pCNEffect);
	SAFE_DELETE(m_pJudgeStr);
	SAFE_DELETE(m_pFullCombo);
	if (m_pJudgeActions)
	{
		for (int i = 0; i < MAX_OCTAVE; i++)
		{
			delete m_pJudgeActions[i];
		}
		delete[] m_pJudgeActions;
		m_pJudgeActions = nullptr;
	}

	// 判定エフェクト全削除
	//for (JudgeEffect *e = JudgeEffect::roots; e != nullptr;)
	//{
	//	JudgeEffect *prev = e->next;
	//	delete e;
	//	e = prev;
	//}
	for (auto it = m_JudgeEffectList.begin(); it != m_JudgeEffectList.end();)
	{
		delete (*it);
		it = m_JudgeEffectList.erase(it);
	}
}


//*****************************************************************************************************************************
//	更新
//*****************************************************************************************************************************
//------------------------------------------------------
//	更新
//------------------------------------------------------
void JudgeActionManager::Update()
{
	// SUPERアニメ更新
	if (++m_iSuperAnimFrame > 3)
	{
		m_iSuperAnimFrame = 0;
		if (++m_iSuperAnimPanel > 3) m_iSuperAnimPanel = 0;
	}

	// 判定文字たち更新
	for (int i = 0; i < MAX_OCTAVE; i++)
	{
		m_pJudgeActions[i]->Update();
	}

	// 判定エフェクト更新
	//for (JudgeEffect *e = JudgeEffect::roots; e != nullptr;)
	//{
	//	JudgeEffect *next = e->next;
	//	// 更新
	//	if(!e->Update()) delete e;
	//	e = next;
	//}
	for (auto it = m_JudgeEffectList.begin(); it != m_JudgeEffectList.end();)
	{
		if ((*it)->Update())
		{
			delete (*it);
			it = m_JudgeEffectList.erase(it);
		}
		else it++;
	}

	// フルコン演出更新
	if (m_bFullCombo)m_pFullCombo->Update();

	// フルコン演出デバッグ
	if (KeyBoardTRG(KB_F) && KeyBoard(KB_SPACE))
	{
		OnFullCombo();
	}
}

bool JudgeActionManager::isFullComboEnd()
{
	assert(m_pFullCombo);
	return m_pFullCombo->isEnd();
}


//*****************************************************************************************************************************
//	描画
//*****************************************************************************************************************************
//------------------------------------------------------
//	描画
//------------------------------------------------------
void JudgeActionManager::Render()
{
	// 判定文字たち描画
	for (int i = 0; i < MAX_OCTAVE; i++)
	{
		// 描画
		m_pJudgeActions[i]->Render(m_pJudgeStr, m_iSuperAnimPanel);
	}

	// 判定エフェクト描画
	//for (JudgeEffect *e = JudgeEffect::roots; e != nullptr; e = e->next)
	//{
	//	// 描画
	//	e->Render(m_explosion.pImage);
	//}
	for (auto & it : m_JudgeEffectList) it->Render();

	// CNエフェクト描画
	static int iEffectFrame(0);
	if(++iEffectFrame > 16) iEffectFrame=0;
	FOR(NUM_KEYBOARD)
	{
		if(m_pJudgeMgr->isCN(i)) m_pCNEffect->Render(-43 + KEYLANE_LEFT + NOTE_POS_COMPTE(i), JUDGE_LINE - 60, 128, 128, (iEffectFrame / 2) * 128, 0, 128, 128);
	}

	// フルコン演出描画
	if (m_bFullCombo)m_pFullCombo->Render();
}

//*****************************************************************************************************************************
//	セット
//*****************************************************************************************************************************
//------------------------------------------------------
//	リスト追加
//------------------------------------------------------
void JudgeActionManager::Set(char lane, int combo, JUDGE_TYPE type, JUDGE_TIMING eTiming)
{
	m_pJudgeActions[lane / 12]->Set(combo, m_ciJudgeAppTime, type, eTiming);	// 超危なっかしい書き方。レーン番号が何オクターブ目にあるかあまり無しの切り捨て割り算で配列参照

	// GREAT以上の判定なら、エフェクトを呼び出す
	if(type == JUDGE_TYPE::SUPER || type== JUDGE_TYPE::GREAT) m_JudgeEffectList.push_back(new JudgeEffect(lane, &m_tagExplosion));

	// フルコンボしてたら演出ON
	if (combo == m_iNumNote)
	{
		OnFullCombo();
	}
}

//------------------------------------------------------
//	判定クリア
//------------------------------------------------------
void JudgeActionManager::Clear()
{
	// 判定エフェクト全削除
	//for (JudgeEffect *e = JudgeEffect::roots; e != nullptr;)
	//{
	//	JudgeEffect *prev = e->next;
	//	delete e;
	//	e = prev;
	//}
	for (auto it = m_JudgeEffectList.begin(); it != m_JudgeEffectList.end();)
	{
		delete (*it);
		it = m_JudgeEffectList.erase(it);
	}

	for (int i = 0; i < MAX_OCTAVE; i++)
	{
		// 表示時間とか全部0にする
		m_pJudgeActions[i]->Off();
	}
}

//------------------------------------------------------
//	フルコンON
//------------------------------------------------------
void JudgeActionManager::OnFullCombo()
{
	// フルコンフラグON
	m_bFullCombo = true;

	// 初期状態に戻す(何回も再生できるようにをかねて)
	m_pFullCombo->Reset();
}

//*****************************************************************************************************************************
//
//		判定アクション
//
//*****************************************************************************************************************************
//*****************************************************************************************************************************
//	初期化・解放
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
JudgeAction::JudgeAction(int octave, bool bShowTiming) :
	m_iOctave(octave), 
	m_iCombo(0), 
	m_iAppTime(0), 
	m_eJudgeType(JUDGE_TYPE::MAX), 
	m_eJudgeTiming(JUDGE_TIMING::JUST),
	m_iPosX(KEYLANE_LEFT + octave * (40 * 7)),
	m_bShowTiming(bShowTiming)
{

}

//*****************************************************************************************************************************
//	描画
//*****************************************************************************************************************************
//------------------------------------------------------
//	描画
//------------------------------------------------------
void JudgeAction::Render(tdn2DObj *pJudgeStr, int iPanel)
{
	// 表示してない！
	if (m_iAppTime <= 0) return;

	// ゴリくんが書いたゴリゴリなコード
	const int l_ciSrcX((int)m_eJudgeType * 256), l_ciSrcY(iPanel * 32);

	pJudgeStr->Render(m_iPosX, JUDGE_LINE - 65, 256, 32, l_ciSrcX, l_ciSrcY, 256, 32);
	tdnText::Draw(m_iPosX + 110, JUDGE_LINE - 100, 0xccffffff, "%d",m_iCombo);

	if (m_bShowTiming)
	{
		if (m_eJudgeTiming == JUDGE_TIMING::FAST)tdnText::Draw(m_iPosX + 90, JUDGE_LINE - 130, 0xccff0000, "FAST");
		else if (m_eJudgeTiming == JUDGE_TIMING::SLOW)tdnText::Draw(m_iPosX + 90, JUDGE_LINE - 130, 0xcc0000ff, "SLOW");
	}
}

//------------------------------------------------------
//	判定セット
//------------------------------------------------------
void JudgeAction::Set(int iCombo, int iTime, JUDGE_TYPE eType, JUDGE_TIMING eTiming)
{
	m_iCombo = iCombo;
	m_iAppTime = iTime;
	m_eJudgeType = eType;
	m_eJudgeTiming = eTiming;
}


//*****************************************************************************************************************************
//
//		判定エフェクト
//
//*****************************************************************************************************************************

//*****************************************************************************************************************************
//	更新
//*****************************************************************************************************************************
//------------------------------------------------------
//	更新
//------------------------------------------------------
bool JudgeEffect::Update()
{
	// コマ割り処理
	if (++m_iCurrentFrame > 1)	// 2/60秒にアニメーション
	{
		m_iCurrentFrame = 0;
		m_iCurrentPanel++;
	}

	const bool l_cbEnd(m_iCurrentPanel >= m_pRefExplosion->iNumPanel);
	return l_cbEnd;
}

//*****************************************************************************************************************************
//	描画
//*****************************************************************************************************************************
//------------------------------------------------------
//	描画
//------------------------------------------------------
void JudgeEffect::Render()
{
	m_pRefExplosion->pImage->Render(-43 + KEYLANE_LEFT + NOTE_POS_COMPTE(m_cLaneNo), JUDGE_LINE - 60, 128, 128, m_iCurrentPanel * 128, 0, 128, 128);
}







//*****************************************************************************************************************************
//
//		フルコン演出
//
//*****************************************************************************************************************************

//*****************************************************************************************************************************
//	更新
//*****************************************************************************************************************************
//------------------------------------------------------
//	更新
//------------------------------------------------------
void FullComboEffect::Update()
{
	// フルコンクロス更新
	m_pFullComboCross->Update();

	// フル子爆発更新
	m_pFullComboExplosion->Update();
}

//*****************************************************************************************************************************
//	描画
//*****************************************************************************************************************************
//------------------------------------------------------
//	描画
//------------------------------------------------------
void FullComboEffect::Render()
{
	// フルコン爆発描画
	m_pFullComboExplosion->Render();

	// フルコンクロス描画
	m_pFullComboCross->Render();
}


//*****************************************************************************************************************************
//
//		フルコンクロス
//
//*****************************************************************************************************************************

void FullComboEffect::FullComboCross::Update()
{
	m_pFull->Update();
	m_pCombo->Update();

	static int VanishWaitFrame = 0;	// 消えてから一定時間待つフレーム
	switch (m_iStep)
	{
	case 0:
		// α加算
		m_byAlpha = min(m_byAlpha + 12, 255);
		m_pFull->pImage->SetARGB((m_byAlpha << 24) | 0x00ffffff);
		m_pCombo->pImage->SetARGB((m_byAlpha << 24) | 0x00ffffff);

		// 交差差し掛かったら
		if (m_pCombo->pos.x - m_pFull->pos.x < 150)
		{
			// 速度減速
			m_pFull->Deceleration();
			m_pCombo->Deceleration();

			m_iStep++;
		}
		break;

	case 1:	// 減速状態
		// お互い交差も済んだら
		if (m_pCombo->pos.x - m_pFull->pos.x < 0)
		{
			// 速度を戻す
			m_pFull->ReAcceleration();
			m_pCombo->ReAcceleration();

			m_iStep++;
		}
		break;

	case 2:
		// α減算
		m_byAlpha = max(m_byAlpha - 12, 0);
		m_pFull->pImage->SetARGB((m_byAlpha << 24) | 0x00ffffff);
		m_pCombo->pImage->SetARGB((m_byAlpha << 24) | 0x00ffffff);

		// お互いだいぶ離れたら
		if (m_pCombo->pos.x - m_pFull->pos.x < -1280)
		{
			VanishWaitFrame = 0;
			m_iStep++;
		}

		break;

	case 3:
		// 消えてちょっと待つフレーム
		if (++VanishWaitFrame > 90) m_iStep++;	// とりあえず役目終了
		break;
	case 4:
		break;	// 終了待ち
	}
}

void FullComboEffect::FullComboCross::Render()
{
	m_pFull->Render();
	m_pCombo->Render();
}


//*****************************************************************************************************************************
//
//		フルコン爆発
//
//*****************************************************************************************************************************
FullComboEffect::FullComboExplosion::FullComboExplosion() :m_iAnimFrame(0), m_iAnimPanel(0)
{
	// 爆発画像初期化
	m_pImage = std::make_unique<tdn2DObj>("DATA/UI/FullCombo/Default/explosion.png");
}

void FullComboEffect::FullComboExplosion::Update()
{
	// いつもの出ていけぇ！！
	if (isEnd()) return;

	// アニメーション処理
	if (++m_iAnimFrame > 1)
	{
		m_iAnimFrame = 0;
		m_iAnimPanel++;
	}
}

void FullComboEffect::FullComboExplosion::Render()
{
	// いつもの出ていけぇ！！
	if (isEnd()) return;

	m_pImage->Render(120, 134, 880, 495, (m_iAnimPanel % 6) * 512, (m_iAnimPanel / 6) * 256, 512, 256);
}