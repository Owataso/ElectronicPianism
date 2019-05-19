#include "TDNLIB.h"
#include "TDN_Movie.h"

// CoInitializeEx使う用
#include<objbase.h>
#define _WIN32_DCOM

// VMR9使う用
//#include<Vmr9.h>


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/使う上でのメモ_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
/*

1:
LIBCMTD.LIB が重複したら
「リンカ」→「コマンドライン」→「追加のオプション」に
/NODEFAULTLIB:LIBCMTD.LIB

2:
「リンカ」→「全般」→「追加のライブラリディレクトリ」に
./TDN/DirectShow/

*/

//#pragma comment (lib, "msvcr120.lib")

//****************************************************************************************
//		初期化
//****************************************************************************************
void tdnMovieManager::Initialize()
{
	// インターフェース初期化
	// これはCOMコンポーネントを扱う上でのお作法でして、これに失敗するとCOMが扱えません。これによって内部で初期化に必要な諸々が整理されています。
	MyAssert(!FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)), "エラー: tdnMovieのインターフェース初期化");
}

void tdnMovieManager::Release()
{
	// COM終了
	CoUninitialize();

	//VMR9AllocationInfo *lpAllocInfo;
}


//****************************************************************************************
//		初期化
//****************************************************************************************
tdnMovie::tdnMovie(char *cFilename, bool bSound, bool bLoop) :m_pGraphBuilder(nullptr), m_pMediaControl(nullptr), m_pMediaEvent(nullptr), m_pMediaPosition(nullptr),
m_state(FILTER_STATE::State_Stopped), m_bLoop(bLoop)
{
	IBaseFilter	*pSourceFilter(nullptr);	// ソースフィルター
	IPin *pFSrcPinOut(nullptr);				// ソースフィルターのOUTピン

	// FilterGraphインターフェース生成
	MyAssert(!FAILED(CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (LPVOID*)&m_pGraphBuilder)), "エラー: tdnMovieのFilterGraphインターフェース生成");

	// テクスチャレンダラ―初期化
	HRESULT hr;
	TextureRenderer *pRenderer = new TextureRenderer(nullptr, &hr); // リーク
	pRenderer->SetDevice(tdnSystem::GetDevice());
	MyAssert(!FAILED(hr) && pRenderer, "エラー: テクスチャレンダラ―の初期化");

	// テクスチャレンダラ―をレンダフィルターにつっこむ
	m_pTextureRenderer = pRenderer;
	/* ※ */
	MyAssert(!FAILED(m_pGraphBuilder->AddFilter(m_pTextureRenderer, L"TextureRenderer")), "エラー: レンダフィルター追加");

	/*
	// レンダフィルター生成・追加
	MyAssert(!FAILED(CoCreateInstance(CLSID_VideoMixingRenderer9, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (LPVOID*)&m_pVMR9)), "エラー: tdnMovieのレンダフィルター生成");
	m_pGraphBuilder->AddFilter(m_pVMR9, L"VMR9");

	// ウインドウレスモードにする
	IVMRFilterConfig *pVMRConfig;
	MyAssert(!FAILED(m_pVMR9->QueryInterface(IID_IVMRFilterConfig9, (LPVOID*)&pVMRConfig)),"エラー: tdnMovieのVMR9のインターフェース");
	MyAssert(!FAILED(pVMRConfig->SetRenderingMode(VMRMode_Windowless)),"エラー: tdnMovieのウィンドウ設定");
	pVMRConfig->Release();// 用済み

	// 描画ウィンドウの指定
	IVMRWindowlessControl *pVMRWndControl;
	MyAssert(!FAILED(m_pVMR9->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pVMRWndControl)), "エラー: tdnMovieのVMR9インターフェース");
	MyAssert(!FAILED(pVMRWndControl->SetVideoClippingWindow(tdnSystem::GetWindow())), "エラー: tdnMovieの描画ウィンドウ設定");
	pVMRWndControl->Release();// 用済み
	*/

	// ソースフィルター生成・追加
	// char → BSTR
	LPSTR lstr = cFilename;
	int blen = (int)MultiByteToWideChar(CP_ACP, 0, lstr, strlen(lstr), nullptr, 0);
	BSTR bFilename;
	bFilename = SysAllocStringLen(nullptr, blen);
	MultiByteToWideChar(CP_ACP, 0, lstr, strlen(lstr), bFilename, blen);
	MyAssert(!FAILED(m_pGraphBuilder->AddSourceFilter(bFilename, L"SourceFilter", &pSourceFilter)), "エラー: tdnMovieのソースフィルター生成");

	/* AVIとかMPEG1とかVFWとか */
	if (SUCCEEDED(pSourceFilter->FindPin(L"Output", &pFSrcPinOut)))
	{
		// サウンドを再生する場合
		if (bSound)
		{
			// ソースフィルターピンを描画？繋げるのか？
			MyAssert(!FAILED(m_pGraphBuilder->Render(pFSrcPinOut)), "エラー: tdnMovieのソースフィルタアウトプット");
		}

		// サウンド再生しない場合
		else
		{
			// テクスチャレンダラ―のインプットピン
			IPin *pFTRPinIn;

			// レンダラ―フィルターのインプット検索
			/* ※ */
			MyAssert(!FAILED(m_pTextureRenderer->FindPin(L"In", &pFTRPinIn)), "エラー: tdnMovieのレンダラ―フィルターのインプット検索");

			// ソースフィルターのアウトプットをレンダーフィルターのインプットにつなげる！！
			MyAssert(!FAILED(m_pGraphBuilder->Connect(pFSrcPinOut, pFTRPinIn)), "エラー: tdnMovieのフィルター接続");
		}
	}

	/* WMVとかMPEG2とか */
	else
	{
		IBasicAudio *pAudio;
		MyAssert(!FAILED(m_pGraphBuilder->QueryInterface(IID_IBasicAudio, (LPVOID*)&pAudio)), "エラー: tdnMovieのオーディオインターフェース");
		MyAssert(!FAILED(m_pGraphBuilder->RenderFile(bFilename, nullptr)), "エラー: tdnMovieのレンダーファイル");

		// サウンド再生フラグによって音の大きさを変える(-10000で無音)
		pAudio->put_Volume(bSound ? 0 : -10000);
	}

	/*
	// フィルター接続手伝ってくれるくん生成
	MyAssert(!FAILED(CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (LPVOID*)&m_pCaptureGraphBuilder2)),"エラー: tdnMovieのICaptureGraphBuilder2生成");
	// インターフェース切り替え
	m_pCaptureGraphBuilder2->SetFiltergraph(m_pGraphBuilder);

	// フィルター接続
	m_pCaptureGraphBuilder2->RenderStream(0, 0, m_pSourceFilter, 0, m_pVMR9);

	// オーディオ接続(書かないと無音の動画になる)
	m_pCaptureGraphBuilder2->RenderStream(0, &MEDIATYPE_Audio, m_pSourceFilter, 0, 0);

	// フィルタグラフが完成した後に、VMR9ではレンダリング先のウィンドウに対して動画の出力サイズを設定する必要があります。これはお作法がありますので、その通りにプログラムします。
	// 描画領域の設定（接続後でないとエラーになる）
	LONG W, H;
	RECT SrcR, DestR;
	MyAssert(!FAILED(pVMRWndControl->GetNativeVideoSize(&W, &H, nullptr, nullptr)),"エラー: tdnMovieの描画領域の設定");
	SetRect(&SrcR, 0, 0, W, H);
	GetClientRect(tdnSystem::GetWindow(), &DestR);
	pVMRWndControl->SetVideoPosition(&SrcR, &DestR);
	pVMRWndControl->Release();// 用済み
	*/

	// メディア管理インターフェース生成
	MyAssert(!FAILED(m_pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&m_pMediaControl)), "エラー: tdnMovieのメディアインターフェース生成");

	// メディアイベントインターフェース生成
	MyAssert(!FAILED(m_pGraphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID*)&m_pMediaEvent)), "エラー: tdnMovieのメディアインターフェース生成");

	// 再生位置インターフェース生成
	MyAssert(!FAILED(m_pGraphBuilder->QueryInterface(IID_IMediaPosition, (LPVOID*)&m_pMediaPosition)), "エラー: tdnMovieの再生位置インターフェース生成");

	// グラフスタート(意味あんま分かってない)
	m_pMediaControl->Run();
	// テクスチャの取得
	m_pTexture = m_pTextureRenderer->GetTexture();
	/* ※ */
	m_pTextureRenderer->GetVideoDesc(&m_lWidth, &m_lHeight, &m_lPitch);

	D3DSURFACE_DESC desc;
	/* ※ 落ちた */
	m_pTexture->GetLevelDesc(0, &desc);

	// テクスチャが2の累乗でしか確保できない場合に過剰分を表示しないためのUV計算
	m_fu = (FLOAT)m_lWidth / (FLOAT)desc.Width;
	m_fv = (FLOAT)m_lHeight / (FLOAT)desc.Height;

	m_pMediaControl->Stop();

	// Graphを生成
	//MyAssert(!FAILED(m_pMediaControl->RenderFile(bFilename)),"エラー: tdnMovieのグラフ生成");
	// ウインドウにくっつける(これを書かないと、勝手に動画用のウィンドウが生成される)
	//m_pVideoWindow->put_Owner((OAHWND)tdnSystem::GetWindow());
	//m_pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	//m_pVideoWindow->SetWindowPosition(0, 0, 1280, 720);
}

tdnMovie::~tdnMovie()
{
#ifdef REGISTER_FILTERGRAPH
	// Pull graph from Running Object Table (Debug)
	RemoveFromROT();
#endif

	// 動画止める
	if (m_state == FILTER_STATE::State_Running)m_pMediaControl->Stop();

	// テクスチャ化くん解放
	m_pGraphBuilder->RemoveFilter(m_pTextureRenderer);

	// DirectShow関連の解放
	if (m_pMediaEvent)		m_pMediaEvent->Release();		// メディアイベント解放
	if (m_pMediaPosition)	m_pMediaPosition->Release();	// ポジション解放
	if (m_pMediaControl)	m_pMediaControl->Release();		// メディア管理解放
	if (m_pGraphBuilder)	m_pGraphBuilder->Release();		// グラフ解放
}

//****************************************************************************************
//		再生・停止
//****************************************************************************************
void tdnMovie::Play()
{
	m_pMediaControl->Run();
	m_state = FILTER_STATE::State_Running;
}

void tdnMovie::Stop()
{
	// 動いてなかったら出ていけぇ！！！
	if (m_state != FILTER_STATE::State_Running)return;

	// 停止処理
	m_pMediaControl->Stop();
	m_state = FILTER_STATE::State_Stopped;
}










//
//			TextureRenderer
//

//グローバル
TCHAR g_achCopy[] = TEXT("Bitwise copy of the sample");
TCHAR g_achOffscr[] = TEXT("Using offscreen surfaces and StretchCopy()");
TCHAR g_achDynTextr[] = TEXT("Using Dynamic Textures");
TCHAR* g_pachRenderMethod = NULL;

//コンストラクタ
TextureRenderer::TextureRenderer(LPUNKNOWN pUnk, HRESULT *phr) :
CBaseVideoRenderer(__uuidof(CLSID_TextureRenderer), NAME("Texture Renderer"), pUnk, phr),
m_bUseDynamicTextures(FALSE)
{
	// Store and AddRef the texture for our use.
	ASSERT(phr);
	if (phr)
		*phr = S_OK;
}

//デストラクタ
TextureRenderer::~TextureRenderer()
{
}

//-----------------------------------------------------------------------------
// CheckMediaType: This method forces the graph to give us an R8G8B8 video
// type, making our copy to texture memory trivial.
//-----------------------------------------------------------------------------
HRESULT TextureRenderer::CheckMediaType(const CMediaType *pmt)
{
	HRESULT   hr = E_FAIL;
	VIDEOINFO *pvi = 0;

	CheckPointer(pmt, E_POINTER);

	// Reject the connection if this is not a video type
	if (*pmt->FormatType() != FORMAT_VideoInfo) {
		return E_INVALIDARG;
	}

	// Only accept RGB24 video
	pvi = (VIDEOINFO *)pmt->Format();

	if (IsEqualGUID(*pmt->Type(), MEDIATYPE_Video) &&
		IsEqualGUID(*pmt->Subtype(), MEDIASUBTYPE_RGB24))
	{
		hr = S_OK;
	}

	return hr;
}

//-----------------------------------------------------------------------------
// SetMediaType: Graph connection has been made. 
//-----------------------------------------------------------------------------
HRESULT TextureRenderer::SetMediaType(const CMediaType *pmt)
{
	HRESULT hr;

	UINT uintWidth = 2;
	UINT uintHeight = 2;

	// Retrive the size of this media type
	D3DCAPS9 caps;
	VIDEOINFO *pviBmp;                      // Bitmap info header
	pviBmp = (VIDEOINFO *)pmt->Format();

	m_lVidWidth = pviBmp->bmiHeader.biWidth;
	m_lVidHeight = abs(pviBmp->bmiHeader.biHeight);
	m_lVidPitch = (m_lVidWidth * 3 + 3) & ~(3); // We are forcing RGB24

	// here let's check if we can use dynamic textures
	ZeroMemory(&caps, sizeof(D3DCAPS9));
	hr = m_pd3dDevice->GetDeviceCaps(&caps);
	if (caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES)
	{
		m_bUseDynamicTextures = TRUE;
	}

	if (caps.TextureCaps & D3DPTEXTURECAPS_POW2
		&& caps.TextureCaps & !D3DPTEXTURECAPS_NONPOW2CONDITIONAL)
	{
		while ((LONG)uintWidth < m_lVidWidth)
		{
			uintWidth = uintWidth << 1;
		}
		while ((LONG)uintHeight < m_lVidHeight)
		{
			uintHeight = uintHeight << 1;
		}
	}
	else
	{
		uintWidth = m_lVidWidth;
		uintHeight = m_lVidHeight;
		m_bUseDynamicTextures = FALSE;
	}

	// Create the texture that maps to this media type
	hr = E_UNEXPECTED;
	if (m_bUseDynamicTextures)
	{
		hr = m_pd3dDevice->CreateTexture(uintWidth, uintHeight, 1, 0,
			D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
			&m_pTexture, NULL);
		g_pachRenderMethod = g_achDynTextr;
		if (FAILED(hr))
		{
			m_bUseDynamicTextures = FALSE;
		}
	}
	if (FALSE == m_bUseDynamicTextures)
	{
		hr = m_pd3dDevice->CreateTexture(uintWidth, uintHeight, 1, 0,
			D3DFMT_X8R8G8B8, D3DPOOL_MANAGED,
			&m_pTexture, NULL);
		g_pachRenderMethod = g_achCopy;
	}
	if (FAILED(hr))
	{
		return hr;
	}

	// CreateTexture can silently change the parameters on us
	D3DSURFACE_DESC ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));

	if (FAILED(hr = m_pTexture->GetLevelDesc(0, &ddsd))) {
		return hr;
	}


	IDirect3DSurface9 *pSurf;

	if (SUCCEEDED(hr = m_pTexture->GetSurfaceLevel(0, &pSurf)))
	{
		pSurf->GetDesc(&ddsd);
	}

	// Save format info
	m_TextureFormat = ddsd.Format;

	if (m_TextureFormat != D3DFMT_X8R8G8B8 &&
		m_TextureFormat != D3DFMT_A1R5G5B5) {
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// DoRenderSample: A sample has been delivered. Copy it to the texture.
//-----------------------------------------------------------------------------
HRESULT TextureRenderer::DoRenderSample(IMediaSample * pSample)
{
	BYTE  *pBmpBuffer, *pTxtBuffer; // Bitmap buffer, texture buffer
	LONG  lTxtPitch;                // Pitch of bitmap, texture

	BYTE  * pbS = NULL;
	DWORD * pdwS = NULL;
	DWORD * pdwD = NULL;
	UINT row, col, dwordWidth;

	CheckPointer(pSample, E_POINTER);
	CheckPointer(m_pTexture, E_UNEXPECTED);

	// Get the video bitmap buffer
	pSample->GetPointer(&pBmpBuffer);

	// Lock the Texture
	D3DLOCKED_RECT d3dlr;
	if (m_bUseDynamicTextures)
	{
		if (FAILED(m_pTexture->LockRect(0, &d3dlr, 0, D3DLOCK_DISCARD)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTexture->LockRect(0, &d3dlr, 0, 0)))
			return E_FAIL;
	}
	// Get the texture buffer & pitch
	pTxtBuffer = static_cast<byte *>(d3dlr.pBits);
	lTxtPitch = d3dlr.Pitch;


	// Copy the bits
	pTxtBuffer += lTxtPitch*(m_lVidHeight - 1);

	if (m_TextureFormat == D3DFMT_X8R8G8B8)
	{
		// Instead of copying data bytewise, we use DWORD alignment here.
		// We also unroll loop by copying 4 pixels at once.
		//
		// original BYTE array is [b0][g0][r0][b1][g1][r1][b2][g2][r2][b3][g3][r3]
		//
		// aligned DWORD array is     [b1 r0 g0 b0][g2 b2 r1 g1][r3 g3 b3 r2]
		//
		// We want to transform it to [ff r0 g0 b0][ff r1 g1 b1][ff r2 g2 b2][ff r3 b3 g3]
		// below, bitwise operations do exactly this.

		dwordWidth = m_lVidWidth / 4; // aligned width of the row, in DWORDS
		// (pixel by 3 bytes over sizeof(DWORD))

		for (row = 0; row< (UINT)m_lVidHeight; row++)
		{
			pdwS = (DWORD*)pBmpBuffer;
			pdwD = (DWORD*)pTxtBuffer;

			for (col = 0; col < dwordWidth; col++)
			{
				pdwD[0] = pdwS[0] | 0xFF000000;
				pdwD[1] = ((pdwS[1] << 8) | 0xFF000000) | (pdwS[0] >> 24);
				pdwD[2] = ((pdwS[2] << 16) | 0xFF000000) | (pdwS[1] >> 16);
				pdwD[3] = 0xFF000000 | (pdwS[2] >> 8);
				pdwD += 4;
				pdwS += 3;
			}

			// we might have remaining (misaligned) bytes here
			pbS = (BYTE*)pdwS;
			for (col = 0; col < (UINT)m_lVidWidth % 4; col++)
			{
				*pdwD = 0xFF000000 |
					(pbS[2] << 16) |
					(pbS[1] << 8) |
					(pbS[0]);
				pdwD++;
				pbS += 3;
			}

			pBmpBuffer += m_lVidPitch;
			pTxtBuffer -= lTxtPitch;
		}// for rows
	}

	if (m_TextureFormat == D3DFMT_A1R5G5B5)
	{
		for (int y = 0; y < m_lVidHeight; y++)
		{
			BYTE *pBmpBufferOld = pBmpBuffer;
			BYTE *pTxtBufferOld = pTxtBuffer;

			for (int x = 0; x < m_lVidWidth; x++)
			{
				*(WORD *)pTxtBuffer = (WORD)
					(0x8000 +
					((pBmpBuffer[2] & 0xF8) << 7) +
					((pBmpBuffer[1] & 0xF8) << 2) +
					(pBmpBuffer[0] >> 3));

				pTxtBuffer += 2;
				pBmpBuffer += 3;
			}

			pBmpBuffer = pBmpBufferOld + m_lVidPitch;
			pTxtBuffer = pTxtBufferOld + lTxtPitch;
		}
	}

	// Unlock the Texture
	if (FAILED(m_pTexture->UnlockRect(0)))
		return E_FAIL;

	return S_OK;
}