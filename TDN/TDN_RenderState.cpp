#include	"TDNLIB.h"


/********************************************/
//	tdnRenderState
//	デバイスのレンダリング ステートを定義する。
//	
/********************************************/


// レンダーステート初期化
void tdnRenderState::Initialize()
{
	//	ポリゴン描画設定
	tdnSystem::GetDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// カリングの設定　左周りでカリングする
	tdnSystem::GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);				// 塗りつぶしモード	面を塗りつぶす
	tdnSystem::GetDevice()->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);			// シェーディングモード　グーロー(線形補間)		
																						   
	//	アルファブレンド設定															
	tdnSystem::GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドによる透明化を有効にする	 
	tdnSystem::GetDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);				// ピクセルごとのアルファテストを有効にする
	tdnSystem::GetDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);				// アルファ値を基準とするピクセルの受け取りや受け取り拒否を許可する　デフォルトに
	tdnSystem::GetDevice()->SetRenderState(D3DRS_ALPHAREF, 0);							// D3DRS_ALPHATESTENABLEが有効に設定されている場合に、D3DRS_ALPHAFUNCから送られる比較関数を使い
																						// ピクセルをテストするための基準アルファ値を指定
	//	ブレンド設定 テクスチャブレンディング																	 
	tdnSystem::GetDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);	// (?)カラー ブレンディング処理　デフォルト値がD3DTOP_MODULATE
	tdnSystem::GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);	// (?) 最初のカラーの引数。デフォルトのパラメーターは D3DTA_TEXTURE です。
	tdnSystem::GetDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);	// (?) デフォルトのパラメーターは D3DTA_CURRENT 
	tdnSystem::GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// D3DTSS_ALPHAOP を使った場合には↑二つにアルファ引数に影響を与えます。 
	tdnSystem::GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);	// (?)意味不明。以上
	tdnSystem::GetDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);	// (?)
																						   
	//	テクスチャ設定																	    
	tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);		// ミップマップ　NONE->ミップマップ処理を無効にする。ラスタライザは代わりに拡大フィルタを使う
	tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);		// 縮小フィルタ D3DTEXF_LINEAR->バイリニア補間
	tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);		// 拡大フィルタ D3DTEXF_LINEAR->バイリニア補間
	//tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	//tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);		// u座標で使うテクスチャアドレシング WRAP->繰り返し CLAMP->伸ばす
	tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);		// v座標で使うテクスチャアドレシング WRAP->繰り返し 
																						   
	//	ライティング設定																   
	//tdnLight::SetAmbient(0x20202020);												   
	tdnSystem::GetDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);						// Direct3D のライティングを有効にするには TRUE
																						   
	//	スクリーン設定																	    
	tdnSystem::GetDevice()->SetRenderState(D3DRS_ZENABLE, TRUE);						// zバッファリングを有効にするには D3DZB_TRUE
	tdnSystem::GetDevice()->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);			// FALSE の場合は、複数のサンプルはすべて同じサンプリング値 (ピクセルの中心でサンプリングされた値) で記述されて、マルチサンプル バッファに対する非アンチエイリアシング レンダリングが可能になる

	tdnSystem::GetDevice()->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);				// 頂点法線の自動正規化を有効にするには TRUE

}

/****************************/
//	レンダリングステート
/****************************/
void tdnRenderState::Set(DWORD state, D3DMATERIAL9* lpMaterial, Texture2D* lpTexture)
{
	switch (state)
	{

	case RS::COPY:
		tdnSystem::GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// 結果 = 転送元 + 転送先
		tdnSystem::GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// 今書いているものをどれくらい乗っけるか
		tdnSystem::GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// 後に描いていた物体を↑の反対分薄めて描画
		tdnSystem::GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);					// フォグブレンディングを無効。シェーダーでフォグ作ればよくね？
		tdnSystem::GetDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);					// Direct3D のライティングを有効にする
		tdnSystem::GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// 深度バッファへの書き込みを有効にする
		break;
	case RS::COPY_NOZ:// 2Dに適用すると3Dの奥に描画できたり
		tdnSystem::GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// 深度バッファへの書き込みを有効にする
		break;
	case RS::ADD:	//	加算合成
		tdnSystem::GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);			// 後に描いている物体も全力描画
		tdnSystem::GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		break;

	case RS::SUB:
		tdnSystem::GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);	// 転送先から転送元が減算される。結果 = 転送先 - 転送元
		tdnSystem::GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);			// 後に描いている物体も全力描画
		tdnSystem::GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		break;

	case RS::MUL:
		tdnSystem::GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);			// 書かない
		tdnSystem::GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);		// 前に書いてた物に色を掛ける
		tdnSystem::GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		break;

	case RS::NEGA:
		tdnSystem::GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		break;

	case RS::INVERT:	/*	反転		*/
		tdnSystem::GetDevice()->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT);	// 結果は転送元から転送先が減算される。結果 = 転送元 - 転送先
		tdnSystem::GetDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
		tdnSystem::GetDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		break;
	}

	//	マテリアル設定
	if (lpMaterial) tdnSystem::GetDevice()->SetMaterial(lpMaterial);
	//	テクスチャ設定 テクスチャをデバイスのステージに割り当てる。
	tdnSystem::GetDevice()->SetTexture(0, lpTexture);
}

//	サンプラーステート　TRUE：LINER(補間)　FLASE:POINT(処理は早い)　
void	tdnRenderState::Filter(BOOL bFilter)
{
	if (bFilter){
		tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	}
	else {
		tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		tdnSystem::GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	}


}