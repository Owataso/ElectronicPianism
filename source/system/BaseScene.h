#pragma once

/*****************************/
//		ベースシーン
/*****************************/
class BaseScene
{
public:
	//	生成・解放
	BaseScene() :m_bLoad(false), m_fLoadPercentage(0){}
	virtual ~BaseScene(){ }
	//	初期化
	virtual bool Initialize(){ return true; }
	virtual bool Initialize(BaseScene *newScene){ return true; }	// ★sceneLoading専用
	//	更新・描画
	virtual bool Update(){ return false; }
	virtual void Render(){}

	bool m_bLoad;				// sceneLoadingで2重に読み込むので、1回読み終わったらこれをtrueにして2回読み込まないようにする
	float m_fLoadPercentage;	// ロードの割合(0～1)	ロードシーンのロードバーで使う
};

