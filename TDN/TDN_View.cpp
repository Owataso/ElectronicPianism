#include	"TDNLIB.h"


/********************************************/
//	tdnView
/********************************************/

/* 実体宣言 */
Vector tdnView::Pos;				// カメラの原点
Vector tdnView::Target;			// カメラのターゲット
D3DVIEWPORT9 tdnView::Viewport;	// 3Dを描画するときの　表示するWindow画面の横幅と縦幅　手前と奥　　
float tdnView::FovY;				// 視野角
float tdnView::Near;				// ビューポートの手前
float tdnView::Far;				// 一番奥の位置 (狭いほどZファイティングが無くなる)
float tdnView::Aspect;				// 

/*************/
//	クリア
/*************/
/*
*   Clear関数
*	クリアする対象を指定できる。組み合わせて使える
*   種類
*	D3DCLEAR_STENCIL  ステンシル バッファをクリアして、Stencil パラメータの値にする  
*	D3DCLEAR_TARGET  レンダリング ターゲットをクリアして、Color パラメータの色にする  
*	D3DCLEAR_ZBUFFER  深度バッファをクリアして、Z パラメータの値にする  
*	NULLにすると全部消える
*/

// 視界クリア
void tdnView::Clear(DWORD color, bool bClearZ)
{
	DWORD flag = D3DCLEAR_TARGET;					// 色をクリア
	if (bClearZ == true) flag |= D3DCLEAR_ZBUFFER;	// ZフラグがTRUEならZバッファも消去

	// 画面クリア
	tdnSystem::GetDevice()->Clear(0, NULL, flag, color, 1.0f, 0);// 0 NULLで全部クリア

}
// Zバッファのみクリア
void tdnView::ClearZ()
{
	tdnSystem::GetDevice()->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
}


/*************/
//	視点設定
/*************/
//	視点・注視点設定
void tdnView::Set(const Vector& pos, const Vector& target)
{
	Pos = pos;
	Target = target;
}

//	位置・角度設定
void tdnView::Set(float x, float y, float z, float ax, float ay, float az)
{
	float sx = sinf(ax);
	float sy = sinf(ay);
	float cx = cosf(ax);
	float cy = cosf(ay);

	Vector	p, t;
	p.x = x;
	p.y = y;
	p.z = z;
	t.x = x + (cx*sy);
	t.y = y + (-sx);
	t.z = z + (cx*cy);

	Set(p, t);
}


/***********************************************/
//	投影平面設定 Windowに映し出す幅を変えるだけ
/***********************************************/
void tdnView::SetViewport()
{
	Viewport.X = Viewport.Y = 0;
	Viewport.Width = tdnSystem::GetScreenSize().right;
	Viewport.Height =  tdnSystem::GetScreenSize().bottom;
	Viewport.MaxZ = 1.0f; 
	Viewport.MinZ = 0.0f;	

	Aspect = (float)Viewport.Width / (float)Viewport.Height;
}
void tdnView::SetViewport(int x, int y, int w, int h)
{
	Viewport.X = x;
	Viewport.Y = y;
	Viewport.Width = w;
	Viewport.Height = h;
	Viewport.MaxZ = 1.0f;
	Viewport.MinZ = 0.0f;

	Aspect = (float)w / (float)h;
}

/***********************************************/
//	投影設定 　描画する手前と奥を決めるだけ
/***********************************************/
void tdnView::SetProjection(float FovY, float Near, float Far)
{
	if (Viewport.Width == 0)
	{
		Viewport.Width = tdnSystem::GetScreenSize().right;
		Viewport.Height = tdnSystem::GetScreenSize().bottom;
	}
	SetProjection(FovY, Near, Far, (float)Viewport.Width / (float)Viewport.Height);
}

void tdnView::SetProjection(float _FovY, float _Near, float _Far, float _asp)
{
	FovY = _FovY;
	Near = _Near;
	Far = _Far;
	Aspect = _asp;
}

//*****************************************************************************
//		ビュー変換・プロジェクション変換・ビューポート適用
//*****************************************************************************
void tdnView::Activate()
{
	// 行列
	Matrix matV, matP;

	//	ビュー行列設定
	Vector p, t;
	p = Pos;
	t = Target;
	
	Math::LookAtLH(matV, p, t, Vector3(.0f, 1.0f, .0f));

	//	ビューポート設定
	if (Viewport.Width != 0) tdnSystem::GetDevice()->SetViewport(&Viewport);

	//	投影行列設定
	Math::PerspectiveLH(matP, FovY, Aspect, Near, Far);

	//	グローバルへの適用
	::matProjection = matP;
	::matView = matV;

	//	DirectX設定 単一デバイスのトランスフォーム関連ステートを設定する。
	tdnSystem::GetDevice()->SetTransform(D3DTS_PROJECTION, &matP);
	tdnSystem::GetDevice()->SetTransform(D3DTS_VIEW, &matV);
}


