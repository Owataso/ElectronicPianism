#include	"TDNLIB.h"

/********************************************/
//	tdnPolygon
//	2DObjの2D・3D全てのポリゴンをここで作る
/********************************************/

// http://rudora7.blog81.fc2.com/blog-entry-422.html

//DrawPrimitive関数・・・モデリングファイルで作成する、動的に変更する必要の無く、頂点数が多いオブジェクト。
//DrawPrimitiveUp関数・・・ポイントスプライトや板ポリなど、動的に変更かつ頂点数が少ないオブジェクト。
//http://nutti2.seesaa.net/article/236658336.html

//DrawPrimitive
//頂点データがいっぱいあって呼び出しオーバーヘッドが無視でき、テクスチャとかのパラメータも個別にあるメッシュの描画に使う。
//DrawPrimitiveUP
//頂点データの書き換えが比較的容易でテクスチャなどのパラメータが共通しやすい板ポリ系に使う。
//http://d.hatena.ne.jp/arCtan/20091101/1257083345

//(∵)DrawIndexedPrimitiveを使いこなせれるようになれば処理速度が新世界になれる

//*****************************************************************************
//	３Ｄポリゴン
//*****************************************************************************
//------------------------------------------------------
//		固定機能版	
//------------------------------------------------------

void	tdnPolygon::Render3D(LPLVERTEX lpVertex, int Num, tdn2DObj* lpObj, u32 dwFlags)
{
	LPDEVICE	lpDevice = tdnSystem::GetDevice();// 長いので

	//	(?)ワールド行列設定 単一デバイスのトランスフォーム関連ステートを設定します。 2 現在の返還を修正するMatrix
	Matrix	mat(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	lpDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&mat);

	//	レンダーステート更新
	if (lpObj) tdnRenderState::Set(dwFlags , NULL, lpObj->GetTexture());
	else		tdnRenderState::Set(dwFlags , NULL, NULL);

	// Direct3D のライティングを有効にするには TRUE、無効にするには FALSE 
	lpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//	レンダリング
	lpDevice->SetFVF(D3DFVF_LVERTEX);// オリジナルカスタムFVFマクロで頂点フォーマットの内容を伝達する。
	lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, Num, lpVertex, sizeof(LVERTEX));

	lpDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

// 行列を送るよう
void tdnPolygon::Render3D(LPLVERTEX lpVertex, int Num, tdn2DObj* lpObj, u32 dwFlags, D3DXMATRIX mat)// 追加
{
	LPDEVICE	lpDevice = tdnSystem::GetDevice();
	//	ワールド行列設定
	// Matrix	mat( 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 );
	lpDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&mat);

	//	レンダーステート更新
	if (lpObj) tdnRenderState::Set(dwFlags, NULL, lpObj->GetTexture());
	else		tdnRenderState::Set(dwFlags, NULL, NULL);

	lpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//	レンダリング
	lpDevice->SetFVF(D3DFVF_LVERTEX);// オリジナルカスタムFVFマクロ 
	lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, Num, lpVertex, sizeof(LVERTEX));

	lpDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//------------------------------------------------------
//		シェーダー版
//------------------------------------------------------
void	tdnPolygon::Render3D(LPLVERTEX lpVertex, int Num, tdn2DObj* lpObj, tdnShader* shader, char* name)
{
	LPDEVICE	lpDevice = tdnSystem::GetDevice();
	
	// ローカル-射影変換行列
	Matrix m = matView * matProjection;
	shader->SetWVPMatrix(&m);									//WVPを送る

	lpDevice->SetFVF(D3DFVF_LVERTEX);							// 頂点フォーマットの内容を伝達する。
	if (lpObj) shader->SetDecaleTexture(lpObj->GetTexture());	// シェーダー側にTextureを送る

	// ■シェーダー『開始』
	u32 pass = shader->Begin(name);
	for (u32 p = 0; p<pass; p++)
	{
		// ■シェーダー『パス開始』
		shader->BeginPass(p);
		shader->CommitChanges();
		//	レンダリング
		lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, Num, lpVertex, sizeof(LVERTEX));
		// ■シェーダー『パス終了』
		shader->EndPass();
	}
	// ■シェーダー『終了』
	shader->End();
}

//*****************************************************************************
//	２Ｄポリゴン
//*****************************************************************************
//------------------------------------------------------
//		固定機能版	
//------------------------------------------------------
void tdnPolygon::Render2D(LPTLVERTEX v, int Num, tdn2DObj* lpObj, u32 dwFlags)
{
	LPDEVICE	lpDevice = tdnSystem::GetDevice();

	//	レンダーステート更新
	if (lpObj) tdnRenderState::Set(dwFlags, NULL, lpObj->GetTexture());
	else		tdnRenderState::Set(dwFlags, NULL, NULL);

	//	レンダリング	
	lpDevice->SetFVF(D3DFVF_TLVERTEX);											// 頂点フォーマットの内容を伝達する。
	lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, Num, v, sizeof(TLVERTEX));
}

//------------------------------------------------------
//		シェーダー版
//------------------------------------------------------
void tdnPolygon::Render2D(LPTLVERTEX v, int Num, tdn2DObj* lpObj, tdnShader* shader, char* name)
{
	LPDEVICE	lpDevice = tdnSystem::GetDevice();
	
	lpDevice->SetFVF(D3DFVF_TLVERTEX);							// 頂点フォーマットの内容を伝達する。
	if (lpObj) shader->SetDecaleTexture(lpObj->GetTexture());	// シェーダー側にTextureを送る

	// ■シェーダー『開始』
	u32 pass = shader->Begin(name);
	for (u32 p = 0; p<pass; p++)
	{
		// ■シェーダー『パス開始』
		shader->BeginPass(p);
		shader->CommitChanges();// BeginPass～EndPass内でセッターを通して値を変更した際に呼び出す必要がある関数です
		//	レンダリング
		lpDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, Num, v, sizeof(TLVERTEX));
		// ■シェーダー『パス終了』
		shader->EndPass();
	}
	// ■シェーダー『終了』
	shader->End();
}

//*****************************************************************************
//	矩形描画
//*****************************************************************************
//------------------------------------------------------
//		固定機能版	
//------------------------------------------------------
void tdnPolygon::Rect(int DstX, int DstY, int DstW, int DstH, u32 dwFlags, COLOR color, float z)
{
	TLVERTEX	v[4];

	v[0].sx = v[2].sx = (FLOAT)DstX;
	v[1].sx = v[3].sx = (FLOAT)(DstX + DstW);

	v[0].sy = v[1].sy = (FLOAT)DstY;
	v[2].sy = v[3].sy = (FLOAT)(DstY + DstH);

	v[0].sz = v[1].sz = v[2].sz = v[3].sz = z;
	v[0].color = v[1].color = v[2].color = v[3].color = color;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;

	Render2D(v, 2, NULL, dwFlags);
}

//------------------------------------------------------
//		シェーダー版
//------------------------------------------------------
void tdnPolygon::Rect(int DstX, int DstY, int DstW, int DstH, tdnShader* shader, char* name, COLOR color, float z)
{
	TLVERTEX	v[4];

	v[0].sx = v[2].sx = (FLOAT)DstX;
	v[1].sx = v[3].sx = (FLOAT)(DstX + DstW);

	v[0].sy = v[1].sy = (FLOAT)DstY;
	v[2].sy = v[3].sy = (FLOAT)(DstY + DstH);

	v[0].sz = v[1].sz = v[2].sz = v[3].sz = z;
	v[0].color = v[1].color = v[2].color = v[3].color = color;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;

	Render2D(v, 2, NULL, shader, name);
}


//*****************************************************************************
//	線描画
//*****************************************************************************
void tdnPolygon::DrawLine(float x1, float y1, float x2, float y2, DWORD color, float size)
{
	D3DXVECTOR2 v[2] =
	{
		{ x1, y1 },
		{ x2, y2 },
	};

	tdnSystem::GetLine()->SetWidth(size);
	tdnSystem::GetLine()->Begin();
	tdnSystem::GetLine()->Draw(v, 2, color);
	tdnSystem::GetLine()->End();

}

void tdnPolygon::DrawLine3D(Vector3 pos1, Vector3 pos2, DWORD color, float size, u32 dwFlag)
{
	VECTOR_LINE v[2] =
	{
		{ pos1.x, pos1.y, pos1.z, color },
		{ pos2.x, pos2.y, pos2.z, color },
	};


	LPDEVICE	lpDevice = tdnSystem::GetDevice();
	//	ワールド行列設定
	Matrix	mat(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	lpDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&mat);

	tdnRenderState::Set(dwFlag & 0x0F, NULL, NULL);

	lpDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//	レンダリング
	lpDevice->SetFVF(D3DFVF_LVERTEX);// (D3DFVF_XYZ)
	lpDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(VECTOR_LINE));

	//lpDevice->SetRenderState(D3DRS_LIGHTING, TRUE);



}


//*****************************************************************************
//	円(2D)描画
//*****************************************************************************
//------------------------------------------------------
//		固定機能版	
//------------------------------------------------------
void tdnPolygon::Circle2D(int cx, int cy, float r, u32 dwFlags, COLOR color, float z)
{
}

//------------------------------------------------------
//		シェーダー版
//------------------------------------------------------
void tdnPolygon::Circle2D(int cx, int cy, float r, tdnShader* shader, char* name, COLOR color, float z)
{
}


//*****************************************************************************
//	三角形(2D)描画
//*****************************************************************************
void tdnPolygon::Triangle(int DstX, int DstY, int DstW, int DstH, COLOR color, float angle, u32 dwFlags, float z)
{
	// ▲(時計回り)
	TLVERTEX v[3]
	{
		// 左下
		{ (float)DstX,					(float)(DstY + DstH),	z,	1.0f, color },
		// 上
		{ (float)(DstX + DstW * .5f),	(float)DstY,			z,	1.0f, color },
		// 右下
		{ (float)(DstX + DstW),			(float)(DstY + DstH),	z,	1.0f, color }
	};

	// 回転
	FOR(3)
	{
		// 原点に戻す
		v[i].sx -= DstX + DstW*.5f;
		v[i].sy -= DstY + DstH*.5f;

		const float x{ v[i].sx }, y{ v[i].sy };
		v[i].sx = x * cosf(angle) - y * sinf(angle);
		v[i].sy = x * sinf(angle) + y * cosf(angle);

		// 元の座標に戻す
		v[i].sx += DstX + DstW*.5f;
		v[i].sy += DstY + DstH*.5f;
	}

	Render2D(v, 1, NULL, dwFlags);
}