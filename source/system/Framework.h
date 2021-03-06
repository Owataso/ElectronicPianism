#pragma once

#include	"TDNLIB.h"
#include	"BaseScene.h"

//*****************************************************************************
//		フレームワーク
//*****************************************************************************

//*****************************************************************************
//
//*****************************************************************************
#define	FPS_60		0	//	固定６０フレーム
#define	FPS_30		1	//	固定３０フレーム
#define	FPS_FLEX	2	//	可変フレーム

//*****************************************************************************
//
//*****************************************************************************

class Framework
{
private:
	//------------------------------------------------------
	//	シーン
	//------------------------------------------------------
	BaseScene	*scene;

	//------------------------------------------------------
	//	更新
	//------------------------------------------------------
	BOOL	bRender;

	//------------------------------------------------------
	//	フレームレート関連
	//------------------------------------------------------
	DWORD	dwGameFrame;	//	起動からのフレーム数

	BYTE	FPSMode;		//	フレーム制御モード
	DWORD	dwFrameTime;

	DWORD	dwFPS;			//	処理フレームレート
	DWORD	dwRenderFPS;	//	描画フレームレート

	DWORD	dwCurFrame;		//	処理カウンタ
	DWORD	dwRCurFrame;	//	描画カウンタ

	bool movie_play = false; //	動画を再生しているかどうか

public:
	//------------------------------------------------------
	//	初期化・解放
	//------------------------------------------------------
	Framework(int FPSMode = FPS_FLEX);
	~Framework();

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	bool Update();
	void Render();

	//------------------------------------------------------
	//	シーンの切り替え
	//------------------------------------------------------
	void ChangeScene(BaseScene* newScene, bool bLoadingScene = false);
};

// 実体
extern Framework *MainFrame;