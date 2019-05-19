#include "TDNLIB.h"
#include "../resource.h"

/********************************************/
//	tdnSystem
/********************************************/

/* 実体宣言 */
HINSTANCE tdnSystem::hinst;				// アプリケーション自体を表している変化しない柱のような「値」
HWND	tdnSystem::Window;				// ウィンドウハンドル// １つのウィンドウを表す「値」です
BOOL	tdnSystem::WindowActive =FALSE; // ウィンドウを止めるフラグ
LPDIRECT3D9	tdnSystem::lpD3D;			// DirectX9にアクセスするためのポインタ			
LPDEVICE	tdnSystem::Device;			// ビデオカードにアクセスするための3Dデバイス
D3DPRESENT_PARAMETERS tdnSystem::d3dpp;	// ディスプレイのパラメーター フルスクリーンなどに
ID3DXLine* tdnSystem::pLine;			// ID3DXLine インターフェイスはテクスチャー化された三角形を使用した線描画を実装します。
RECT	tdnSystem::ScreenSize;			// スクリーンのサイズ
D3DFORMAT tdnSystem::ScreenFormat;		// スクリーンのフォーマット
BOOL tdnSystem::FullScreen;				// フルスクリーンなのか？
ENDIAN tdnSystem::endian;				// エンディアン(ビッグorリトル)
RECT tdnSystem::DisplaySize;			// ディスプレイのサイズ格納

/* グローバル変数 */
//	変換行列
Matrix		matView;					// ビュー行列
Matrix		matProjection;				// プロジェクション行列

//BOOL SetClientSize(HWND hWnd, int width, int height)
//{
//	// フレーム込みのサイズへ
//	RECT WindowSize = tdnSystem::GetScreenSize();
//	AdjustWindowRectEx(&WindowSize, // クライアント領域の座標が入った構造体へのポインタ　フレーム込みの大きさへ変換
//		WS_OVERLAPPEDWINDOW,		// サイズを計算するウィンドウの を指定
//		FALSE,						// ウィンドウがメニューを持つかどうかの指定
//		NULL);						// サイズを計算するウィンドウの拡張ウィンドウスタイルを指定　何か↓でいじって大きさが変わるならここで調整
//
//
//	RECT rw, rc;
//	::GetWindowRect(hWnd, &rw);
//	::GetClientRect(hWnd, &rc);
//
//	int new_width = (rw.right - rw.left) - (rc.right - rc.left) + width;
//	int new_height = (rw.bottom - rw.top) - (rc.bottom - rc.top) + height;
//
//	return ::SetWindowPos(hWnd, HWND_NOTOPMOST, WindowSize.left, WindowSize.top, new_width, new_height, SWP_SHOWWINDOW);
//}


// ウィンドウプロシージャ(Winmainの様な存在)
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 常に通ってる
	// ウインドウから送られてくるメッセージをキャッチして処理する関数の事．コールバック関数である。
	switch (message){
	case WM_CREATE:/*ウィンドウが呼び出されたときに1度だけ入る*/
		break;
	case WM_ACTIVATEAPP:
		if (wParam){
			tdnSystem::SetWindowActive(TRUE);
		}
		else
		{
			tdnSystem::SetWindowActive(FALSE);
		}
		break;
	case WM_DESTROY: PostQuitMessage(0); return 0;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE: PostMessage(hWnd, WM_CLOSE, 0, 0); return 0;// Escキーを押したらウィンドウを閉じる
		case VK_F1:		tdnSystem::OpenDebugWindow(); return 0;		// コンソール画面を出す
		case VK_F2: 
		{	
			// 仮想フルスクリーンフラグ
			static bool VirtualFullScreenFlag = false;

			// フルスクリーンの状態なら弾く
			if (tdnSystem::GetFullScreen() == TRUE)return 0;
			
			// 中心に画面を持ってく処理
			int centerX = tdnSystem::GetDisplaySize().right / 2 - tdnSystem::GetScreenSize().right / 2;
			int centerY = tdnSystem::GetDisplaySize().bottom / 2 - tdnSystem::GetScreenSize().bottom / 2;

			// ウィンドウの状態なら仮想フルスクリーン化
			if (VirtualFullScreenFlag == false)
			{	
				// フレーム込みのサイズに変換
				RECT WindowSize = tdnSystem::GetScreenSize();
				AdjustWindowRectEx(&WindowSize, WS_OVERLAPPEDWINDOW, FALSE, NULL);

				// スタイル変更
				LONG lStyle;
				lStyle = GetWindowLong(hWnd, GWL_STYLE);
				lStyle = WS_POPUPWINDOW;
				lStyle = SetWindowLong(hWnd, GWL_STYLE, lStyle);

				// [メモ]　SetWindowPos　windowのサイズよりも大きく表示できない
				// WS_POPUPなら全力で引き延ばしてフルスクにできる
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0,
					tdnSystem::GetDisplaySize().right,	// 現在のディスプレイサイズ分大きく
					tdnSystem::GetDisplaySize().bottom,	// 現在のディスプレイサイズ分大きく
					SWP_SHOWWINDOW | SWP_FRAMECHANGED);

				VirtualFullScreenFlag = true;
			}
			else
			{
				// フレーム込みのサイズに変換
				RECT WindowSize = tdnSystem::GetScreenSize();
				AdjustWindowRectEx(&WindowSize, WS_OVERLAPPEDWINDOW, FALSE, NULL);

				// メモ　反応するのがWS_OVERLAPPEDWINDOWだけだったので　そこから比率を弄る部分だけ抜いた
				LONG lStyle;
				lStyle = GetWindowLong(hWnd, GWL_STYLE);
				lStyle = WS_OVERLAPPEDWINDOW;
				lStyle &= ~WS_THICKFRAME;
				lStyle &= ~WS_MAXIMIZEBOX;
				lStyle = SetWindowLong(hWnd, GWL_STYLE, lStyle);

				SetWindowPos(hWnd, HWND_NOTOPMOST, centerX, centerY,
					WindowSize.right - WindowSize.left,
					WindowSize.bottom - WindowSize.top, SWP_SHOWWINDOW | SWP_FRAMECHANGED);


				VirtualFullScreenFlag = false;
			}

		}
		return 0;
		case VK_F3:
		{
					  tdnSystem::ScreenShot();
		}
		return 0;
		}
		break;
	case WM_MOUSEWHEEL:	// マウスホイールイベント
	{
						  static short mouse_wheel_delta = 0;
						  mouse_wheel_delta += GET_WHEEL_DELTA_WPARAM(wParam);
						  if (mouse_wheel_delta >= 120){
							  mouse_wheel_delta = 0;
							  tdnMouse::SetWheel(WHEEL_FLAG::UP);
						  }
						  else if (mouse_wheel_delta <= 120){
							  mouse_wheel_delta = 0;
							  tdnMouse::SetWheel(WHEEL_FLAG::DOWN);
						  }
	}
		break;

	case WM_DROPFILES:	// ドラッグ&ドロップイベント
	{
							//char filename[256] = {};
							//DRAG_AND_DROP::Get_file_path(filename, 256, wParam);
							break;
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);// 自分で処理しないメッセージをWindowsの方で処理してもらう

}

// ウィンドウを初期化
BOOL tdnSystem::InitWindow(HINSTANCE hInstance, int nCmdShow, char* AppTitle, DWORD ScreenMode, BOOL isFullScreen)
{
	// ディスプレイのサイズ取得
	DisplaySize.left = 0;
	DisplaySize.right = GetSystemMetrics(SM_CXSCREEN);
	DisplaySize.top = 0;
	DisplaySize.bottom= GetSystemMetrics(SM_CYSCREEN);

	// スクリーンサイズを取得
	ScreenSize = GetScreenRect(ScreenMode); // モードに合わせサイズを取得

	// フルスクリーンにするフラグを保存
	FullScreen = isFullScreen;

	// ★D3Dオブジェクトの作成
	lpD3D = Direct3DCreate9(D3D_SDK_VERSION);

	std::vector<D3DDISPLAYMODE> DModes;	// PCのディスプレイモードを保存する可変長配列
	D3DDISPLAYMODE    d3dspMode;		// PCのディスプレイモードを格納

	// PCに何種類のディスプレイがあるか調べる
	int DISPLAY_COUNT = lpD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);

	// ディスプレイモードを調べる
	for (int i = 0; i < DISPLAY_COUNT; ++i) {
		lpD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &d3dspMode);
		// ディスプレイモードを可変長配列に記憶
		DModes.push_back(d3dspMode);
	}


	if (FullScreen == TRUE) // まずフルスクリーンにできる解像度がパソコンにあるか探す
	{
		for (int i = 0; i < DISPLAY_COUNT; i++)
		{
			if (DModes[i].Width == ScreenSize.right && DModes[i].Height == ScreenSize.bottom &&
				DModes[i].RefreshRate == 60)// フルスクリーンのサイズやリフレッシュレートの設定
			{
				// (∵) ここでサイズ保存なりなんなり。
				// ここに通ってそれでもHandleエラーが出た場合はデュアルモニターなどしてる可能性があるよ。
				break;// 解像度一致->OK!
			}
			else if (i == (DISPLAY_COUNT - 1))// 最後まで確かめても一致する解像度がなかったら
			{
				MessageBox(0, "フルスクリーンにするための解像度があなたのパソコンに\n設定されていなかったのでウィンドウモードに移行します。", "System", MB_OK);
				FullScreen = false;	//　強制的にウィンドウモードへ
			}
		}
	}


	// ウィンドウクラスEXの設定
	WNDCLASSEX wcex;											// ウィンドウクラスEXとは、ウィンドウの生成に必要な情報を持つ構造体
	wcex.cbSize = sizeof(WNDCLASSEX);							// 構造体に必要なバイト数でsizeof(WNDCLASSEX)を指定する。
	wcex.style = CS_HREDRAW | CS_VREDRAW;						// ウィンドウのスタイルを設定します、ビット演算子でスタイルを組み合わせる
	wcex.lpfnWndProc = WindowProc;								// ウィンドウプロシージャへの関数ポインタを設定します
	wcex.cbClsExtra = 0;										// この構造体に割り当てる余分なメモリを設定し、0で初期化します
	wcex.cbWndExtra = 0;										// ウィンドウインスタンス(実際のウィンドウ)に割り当てられる 余分なメモリを設定し、0で初期化されます
	wcex.hInstance = hInstance;									// このクラスのウィンドウプロシージャを含むアプリケーション インスタンスハンドルを設定します。
	wcex.hIcon = LoadIcon(hInstance, LPSTR(IDI_ICON1));			// アイコンのハンドルを設定します。
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);					// カーソルのハンドルを設定します。 
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	// 背景のハンドルを設定します。 
	wcex.lpszMenuName = NULL;									// メニューリソースの名前を設定します
	wcex.lpszClassName = "classname";							// ウィンドウクラスの名前を設定します
	wcex.hIconSm = NULL;										// スモールアイコンのハンドル
	
	// 設定したらRegisterClassExで登録!
	RegisterClassEx(&wcex);

	// 大きさが変わるので移す
	RECT WindowSize = ScreenSize;

	// 設定したら実際にウィンドウを作ります。　戻り値にウィンドウを識別するためのハンドルが帰ってきます。
	// フルスクリーンで描画するか？
	if (FullScreen == false)
	{
		//	WindowSizeにこのまま1280*720と入力するとフレーム込みで画面がかわってしまう
		// それを避ける為に↓のAdjustWindowRectExを使いフレーム込み計算する
		AdjustWindowRectEx(&WindowSize, // クライアント領域の座標が入った構造体へのポインタ　フレーム込みの大きさへ変換
			WS_OVERLAPPEDWINDOW,		// サイズを計算するウィンドウの を指定
			FALSE,						// ウィンドウがメニューを持つかどうかの指定
			NULL);						// サイズを計算するウィンドウの拡張ウィンドウスタイルを指定　何か↓でいじって大きさが変わるならここで調整

		// 中心に画面を持ってく処理
		int centerX = GetDisplaySize().right / 2 - tdnSystem::GetScreenSize().right / 2;
		int centerY = GetDisplaySize().bottom / 2 - tdnSystem::GetScreenSize().bottom / 2;

		// [メモ]フレームを消す->WS_POPUP
		Window = CreateWindowEx(WS_EX_ACCEPTFILES/*| WS_EX_TOPMOST*/, wcex.lpszClassName, AppTitle,
			WS_OVERLAPPED| WS_CAPTION |WS_SYSMENU| WS_MINIMIZEBOX,	// 1.拡張ウィンドウスタイル　2.登録したクラス名　3.ウィンドウ名　4.ウィンドウスタイル
			centerX, centerY, WindowSize.right - WindowSize.left, WindowSize.bottom - WindowSize.top,		// ウィンドウの初期位置やサイズ
			NULL, NULL, hInstance, NULL);														// NULLには特に意味はないアプリケーションのインスタンスハンドルだけ送る

	}
	else // フルスクリーン
	{

		Window = CreateWindow(wcex.lpszClassName, AppTitle, WS_POPUP,
			0, 0, WindowSize.right, WindowSize.bottom,
			NULL, NULL, hInstance, NULL);
		ShowCursor(FALSE);

	}

	ShowWindow(Window, nCmdShow);	// 指定されたウィンドウの表示状態を設定します。
	ValidateRect(Window, 0);		// WM_PAINTが呼ばれないようにする
	UpdateWindow(Window);			// そのウィンドウのクライアント領域を更新します

	// D3D初期化
	if (!InitD3D())
	{
		MessageBox(0, "D3D初期化失敗。", "System", MB_OK);
	}

	// エンディアンチェック
	auto EndianCheck = [](){ int bit = 1; return (*(char*)&bit) ? LITTLE_ENDIAN : BIG_ENDIAN; };
	endian = EndianCheck();





	return TRUE;
}

// (∵) 後でD3Dの初期化から先に->フルスクリーン 
// DirectXを初期化
BOOL tdnSystem::InitD3D()
{
	//lpD3D = Direct3DCreate9(D3D_SDK_VERSION);
	
	// アダプタの「現在」のディスプレイのサイズを取得する(FullScreen用)
	D3DDISPLAYMODE d3ddm;
	lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);// アダプタの現在のディスプレイ モードを取得する。今は使っていない


	if (FullScreen) // フルスクリーン
	{
		// D3Dデバイス(ウィンドウ)パラメータの設定
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Flags = 0;											// 
		d3dpp.BackBufferHeight = ScreenSize.bottom;					// バックバッファの幅
		d3dpp.BackBufferWidth = ScreenSize.right;					// バックバッファの高さ
		d3dpp.BackBufferCount = 1;									// バックバッファの数　厳密に1つでなければいけない
		d3dpp.hDeviceWindow = Window;								// ウィンドウ　フルスクリーンの場合はカバーウィンドウに
		d3dpp.EnableAutoDepthStencil = TRUE;						// TRUEなら深度ステンシルバッファを作成する
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;				// デバイスが作成する自動深度ステンシルサーフェイスのフォーマット
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// アダプタのリフレッシュレート　D3DPRESENT　
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;				// _NONE->いずれのレベルのフルシーンマルチサンプリングも利用できない
		d3dpp.MultiSampleQuality = 0;								// サンプル品質レベル

		d3dpp.Windowed = FALSE;										// TRUEでウィンドウ　FALSEでフルスクリーン
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// スワップチェーンの定義 D3DSWAPEFFECT_DISCARD
		d3dpp.BackBufferFormat = d3ddm.Format;						// フルスクリーンで描画するならd3ddmから取得するFormatを使用する。
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// ディスプレイ アダプタによるスクリーンのリフレッシュレート,この値は0でなければならない
	}
	else // ウィンドウモード
	{
		// D3Dデバイス(ウィンドウ)パラメータの設定
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Flags = 0;											// 
		d3dpp.BackBufferHeight = ScreenSize.bottom;					// バックバッファの幅
		d3dpp.BackBufferWidth = ScreenSize.right;					// バックバッファの高さ
		d3dpp.BackBufferCount = 1;									// バックバッファの数　厳密に1つでなければいけない
		d3dpp.hDeviceWindow = Window;								// ウィンドウ　フルスクリーンの場合はカバーウィンドウに
		d3dpp.EnableAutoDepthStencil = TRUE;						// TRUEなら深度ステンシルバッファを作成する
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;				// デバイスが作成する自動深度ステンシルサーフェイスのフォーマット
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// アダプタのリフレッシュレート　D3DPRESENT　
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;				// _NONE->いずれのレベルのフルシーンマルチサンプリングも利用できない
		d3dpp.MultiSampleQuality = 0;								// サンプル品質レベル

		d3dpp.Windowed = TRUE;										// TRUEでウィンドウ　FALSEでフルスクリーン
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// スワップチェーンの定義 D3DSWAPEFFECT_DISCARD
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;					// D3DFMT_UNKNOWN->現在のディスプレイモードフォーマットを使う
	}

	//	デバイス作成
	D3DCAPS9 Caps;
	ZeroMemory(&Caps, sizeof(Caps));
	//	HALチェック
	//D3DDEVTYPE_HAL->ハードウェアによるラスター化。シェーディングは、ソフトウェア、ハードウェア、またはトランスフォームとライティングの組み合わせによって実行されます。
	if (SUCCEEDED(lpD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &Caps)))
	{
		// ここでOKだったら◎
		// ハードウェアによる頂点処理を指定します。 と マルチスレッドを安全に行う(パフォーマンスは低い)
		if (FAILED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dpp, &Device)))
		{
			// ↑に失敗したら
			// ソフトウェアおよびハードウェアによるミックスの頂点処理を指定します。 と　マルチスレッドを安全に行う(パフォーマンスは低い)
			if (FAILED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dpp, &Device)))
			{
				// ↑に失敗したら
				// ソフトウェアによる頂点処理を指定しますと　マルチスレッドを安全に行う(パフォーマンスは低い)
				if (FAILED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dpp, &Device)))
				{
					return FALSE;	// 無理ゲー
				}
			}
		}
	}
	else 
	{
		ZeroMemory(&Caps, sizeof(Caps));
		// Direct3D 機能が「ソフトウェア」に実装されます。ただし、リファレンスラスタライザーは、可能な場合、特殊なCPU 命令を利用します。
		lpD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, &Caps);
		if (FAILED(lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &d3dpp, &Device)))
			return FALSE;
	}
	
	// 画面バックバッファフォーマットを保存
	ScreenFormat = d3dpp.BackBufferFormat;	// FMT2D::RENDERTARGET

	// その他
	// ID3DXLineインターフェイスの生成
	D3DXCreateLine(tdnSystem::Device, &pLine);


	return TRUE;
}

// 解放
void tdnSystem::Release()
{
	SAFE_RELEASE(Device);
	SAFE_RELEASE(lpD3D);
	SAFE_RELEASE(pLine);
	////クラス解放
	//UnregisterClass("classname", hinst);
}

//*****************************************************************************
//		ウィンドウメッセージ処理関数
//*****************************************************************************
//BOOL tdnSystem::Message()
//{
//	MSG msg;
//	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))// メッセージがあればTRUE
//	{
//		if (!GetMessage(&msg, NULL, 0, 0)) return FALSE;
//		TranslateMessage(&msg);// メッセージ入力をプログラムで処理できる形式に変換
//		DispatchMessage(&msg);// WindowProcを呼びだし、メッセージ処理を行う。
//	}
//
//
//}

//*****************************************************************************
//		モード別スクリーンサイズ取得
//*****************************************************************************
RECT tdnSystem::GetScreenRect(DWORD mode)
{
	RECT rc;
	rc.left = rc.top = 0;

	switch (mode){
	case SCREEN800x450:		rc.right = 800;  rc.bottom = 450;	break;
	case SCREEN1280x720:	rc.right = 1280; rc.bottom = 720;	break;
	case SCREEN1920x1080:	rc.right = 1920; rc.bottom = 1080;	break;
	case SCREEN800x600:		rc.right = 800; rc.bottom = 600;	break;
	default:
		rc.right = (mode >> 16);
		rc.bottom = (mode & 0xFFFF);
		MessageBox( 0, "スクリーンサイズのモードがおかしい", NULL, MB_OK );
		break;
	}

	return rc;
}


//*****************************************************************************
//		
//		IEXに入っていたやつ。皆が使っているのなら残しておきます。
//		デバッグ用ウィンドウ
//
//*****************************************************************************
#include <stdio.h>
static FILE* DebugFP = NULL;

void tdnSystem::OpenDebugWindow()
{
#ifdef _DEBUG
	AllocConsole();
	freopen_s(&DebugFP, "CON", "w", stdout);
	freopen_s(&DebugFP, "CON", "r", stdin);		// この文がないとユーザーからの入力が受け付けられない
#endif
}

void tdnSystem::CloseDebugWindow()
{
#ifdef _DEBUG
	if (DebugFP)
	{
		fclose(DebugFP);
		FreeConsole();
	}
#endif
}

void tdnSystem::printf(const char* format, ...)
{
#ifdef _DEBUG
	va_list arg;
	va_start(arg, format);

	vprintf(format, arg);

	va_end(arg);
#endif
}
