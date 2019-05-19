#pragma once

// DirevtShowに関する基底クラスのAll.h的なヘッダー
#include "DirectShow/baseclasses/streams.h"

// デバッグとリリースで使うライブラリを分けている
#ifdef _DEBUG
#ifdef _V120_
#pragma comment( lib, "DirectShow/v120/strmbasd.lib" )
#endif
#ifdef _V140_
#pragma comment( lib, "DirectShow/v140/strmbasd.lib" )
#endif
#else
#ifdef _V120_
#pragma comment( lib, "DirectShow/v120/strmbase.lib" )
#endif
#ifdef _V140_
#pragma comment( lib, "DirectShow/v140/strmbase.lib" )
#endif
#endif

// Define GUID for Texture Renderer
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

/**
*@brief		テクスチャに動画を書き込む作業を実際に行うためのクラス。内部処理に利用するもの
*@author	Yamagoe
*/
class TextureRenderer : public CBaseVideoRenderer
{
	IDirect3DDevice9	*m_pd3dDevice;
	Texture2D	*m_pTexture;

	D3DFORMAT		m_TextureFormat;

	BOOL	m_bUseDynamicTextures;	//! @param ダイナミックテクスチャを使うかどうかどうかのフラグ

	LONG	m_lVidWidth;	//! @param ビデオの幅
	LONG	m_lVidHeight;	//! @param ビデオの高さ
	LONG	m_lVidPitch;	//! @param ビデオのピッチ

public:
	TextureRenderer(LPUNKNOWN pUnk, HRESULT *phr);
	~TextureRenderer();

	void SetDevice(IDirect3DDevice9 * pd3dDevice) { m_pd3dDevice = pd3dDevice; };

	HRESULT CheckMediaType(const CMediaType *pmt);     // Format acceptable?
	HRESULT SetMediaType(const CMediaType *pmt);       // Video format notification
	HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

	Texture2D *GetTexture() { return m_pTexture; };

	void GetVideoDesc(LONG* plVidWidth, LONG* plVidHeight, LONG* plVidPitch)
	{
		*plVidWidth = m_lVidWidth;
		*plVidHeight = m_lVidHeight;
		*plVidPitch = m_lVidPitch;
	};

	void Update()
	{
		// これを毎フレーム呼び出すとテアリングが解消される
		// よくわかってないのが本音だが、とりあえずこれで。
		CAutoLock cVideoLock(&m_InterfaceLock);
	}
};

/**
*@brief		動画ファイルの制御、テクスチャを取得したりするクラス
*@author		Yamagoe
*/
class tdnMovie
{
public:
	tdnMovie(char *filename, bool bSound = false, bool bLoop = true);
	~tdnMovie();

	// ★テクスチャ取得
	Texture2D* GetTexture()
	{
		return m_pTexture;
	}

	TextureRenderer *GetTextureRenderer() { return m_pTextureRenderer; }

	void Play();	// 再生
	void Stop();	// 停止

	void Update()
	{
		// ★テアリング防止の更新(ロックしてる)
		m_pTextureRenderer->Update();

		// ループ再生する更新
		if (m_bLoop && isEndPos())SetTime(0.0);
	}

	// ゲッター
	double GetStopTime() { double ret; m_pMediaPosition->get_StopTime(&ret); return ret; };				// 終了時間の取得
	double GetDuration() { double ret; m_pMediaPosition->get_Duration(&ret); return ret; };				// ストリームの時間幅の取得
	double GetCurrentPosition() { double ret; m_pMediaPosition->get_CurrentPosition(&ret); return ret; }	// 現在の再生位置の取得
	void GetUV(float2 *out) { out->x = m_fu; out->y = m_fv; };
	bool isEndPos() { return (GetCurrentPosition() >= GetStopTime()); };

	// セッター
	void SetTime(double val) { m_pMediaPosition->put_CurrentPosition(val); };				// 現在の再生位置を指定位置にセット
	void SetSpeed(double val) { m_pMediaPosition->put_Rate(val); };
	void SetLoop(bool loop) { m_bLoop = loop; }
private:
	// 動画をテクスチャ処理してくれるくん
	TextureRenderer     *m_pTextureRenderer;        // DirectShow Texture renderer

													// Direct3Dとしての変数
	Texture2D	*m_pTexture;		// 動画のテクスチャ

	IMediaControl		*m_pMediaControl;	// メディア管理
	IGraphBuilder		*m_pGraphBuilder;	// 
	IMediaEvent			*m_pMediaEvent;
	IMediaPosition		*m_pMediaPosition;	// 再生位置

											//IBaseFilter			*m_pVMR9;		// レンダフィルター(VMR9)

											// フィルター接続ヘルパーインターフェース
											//ICaptureGraphBuilder2 *m_pCaptureGraphBuilder2;

	LONG	m_lWidth;			// 幅  
	LONG	m_lHeight;			// 高さ
	LONG	m_lPitch;			// ピッチ
	FLOAT	m_fu, m_fv;			// 元のムービーのサイズ(幅、高さ) / テクスチャのサイズ(幅、高さ)で算出するUV値

	FILTER_STATE		m_state;	// 自分のステート
	bool				m_bLoop;	// ループフラグ
	bool				m_bSound;	// サウンドフラグ
	bool				m_bWMV;		// wmvファイルかどうか
};
class tdnMovieManager
{
public:
	static void Initialize();
	static void Release();
};