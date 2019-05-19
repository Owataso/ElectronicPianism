#include "TDNLIB.h"

//*****************************************************************************************************************************
//
//				管理
//
//*****************************************************************************************************************************
//------------------------------------------------------
//		静的メンバ変数
//------------------------------------------------------
LPDIRECTINPUT8 tdnInputManager::m_lpDI = nullptr;
int tdnInputManager::m_NumDevice = 0;
DIDEVICEINSTANCE tdnInputManager::m_DeviceInstances[tdnInputEnum::INPUT_DEVICE_MAX];
char tdnInputManager::m_GroupID[tdnInputEnum::INPUT_DEVICE_MAX][32];

//------------------------------------------------------
//		コントローラー列挙
//------------------------------------------------------
BOOL CALLBACK tdnInputManager::EnumDeviceCallback(const DIDEVICEINSTANCE* pdidi, VOID* pContext)
{
	//	情報のコピー
	memcpy(&m_DeviceInstances[m_NumDevice], pdidi, sizeof(DIDEVICEINSTANCE));

	/*
	•DI8DEVTYPE_GAMEPAD
	ゲームパッド。次のサブタイプが定義される。
	◦DI8DEVTYPEGAMEPAD_LIMITED
	アクション マップ対象のデバイス オブジェクトの最小数を指定しないゲームパッド。

	◦DI8DEVTYPEGAMEPAD_STANDARD
	アクション マップ対象のデバイス オブジェクトの最小数を指定しない標準ゲームパッド。

	◦DI8DEVTYPEGAMEPAD_TILT
	コントローラの姿勢から x 軸と y 軸のデータを報告するゲームパッド。


	•DI8DEVTYPE_JOYSTICK
	ジョイスティック。次のサブタイプが定義される。
	◦DI8DEVTYPEJOYSTICK_LIMITED
	アクション マップ対象のデバイス オブジェクトの最小数を指定しないジョイスティック。

	◦DI8DEVTYPEJOYSTICK_STANDARD
	アクション マップ対象のデバイス オブジェクトの最小数を指定する標準ジョイスティック。
	*/

	//if (device_instances[num_device].dwDevType & DI8DEVTYPE_GAMEPAD)
	//{
	//	if (device_instances[num_device].dwDevType & DI8DEVTYPEGAMEPAD_STANDARD)
	//		int i = 0;
	//}
	//if (device_instances[num_device].dwDevType & DI8DEVTYPE_JOYSTICK)
	//{
	//	if (device_instances[num_device].dwDevType & DI8DEVTYPEJOYSTICK_STANDARD)
	//		int i = 0;
	//}


	/*
	tszInstanceName
	インスタンスの登録名。たとえば、"Joystick 1"。
	*/
	std::ifstream infs("DATA/Input/list.txt");

	// 過去にcontrollerをさしたことがあるか検索
	bool find(false);
	while (!infs.eof())
	{
		char yomitobashi[64];

		// 名前まで読み飛ばし
		{
			do
			{
				infs >> yomitobashi;
			} while (strcmp(yomitobashi, "NAME:") != 0 && !infs.eof());
			infs.read(yomitobashi, 1);	// TDN空白
		}
		char work[128];

		// ファイルの読み込みが空白で区切りやがるので、1バイトずつ読まないといけないし、終端も付けないといけない(とても面倒くさい)
		{
			int i(0);
			while (!infs.eof())
			{
				char byte;
				infs.read(&byte, 1);
				if (byte == '|')		// 自分でつけた終端マーク
				{
					work[i] = '\0';
					break;
				}
				else work[i++] = byte;
			}
		}

		// 同一controllerチェック
		if (strcmp(m_DeviceInstances[m_NumDevice].tszInstanceName, work) == 0)
		{
			// ID読み飛ばし
			infs >> yomitobashi;
			// 見つかったcontrollerの名前のグループID登録
			infs >> m_GroupID[m_NumDevice];
			find = true;
			break;
		}
	}
	infs.close();

	if (!find)
	{
		// 見つからなかったのでリスト追加します
		std::ofstream ofs("DATA/Input/list.txt", std::ios::out | std::ios::ate | std::ios::app);
		ofs << "NAME: " << m_DeviceInstances[m_NumDevice].tszInstanceName << "|";
		ofs << "\nID: DEFAULT\n\n";
	}

	//	最大
	if (++m_NumDevice >= tdnInputEnum::INPUT_DEVICE_MAX) return DIENUM_STOP;

	return DIENUM_CONTINUE;
}
//------------------------------------------------------
//	初期化
//------------------------------------------------------
void tdnInputManager::Initialize()
{
	m_lpDI = nullptr;
	MyAssert(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_lpDI, nullptr) == DI_OK, "DirectInputの初期化でエラー");
	m_lpDI->Initialize(GetModuleHandle(nullptr), DIRECTINPUT_VERSION);

	m_NumDevice = 0;

	// 最初は全員デフォルトのID
	for (int i = 0; i < tdnInputEnum::INPUT_DEVICE_MAX; i++) strcpy_s(m_GroupID[i], 32, "DEFAULT");

	//	ゲームパッドの列挙(この中でIDの振り分けしてます)
	m_lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDeviceCallback, m_lpDI, DIEDFL_ATTACHEDONLY);
}

//------------------------------------------------------
//		軸列挙
//------------------------------------------------------
BOOL CALLBACK tdnInputManager::EnumAxes(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	// 軸範囲を設定（-1000～1000）
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwObj = lpddoi->dwType;
	diprg.diph.dwHow = DIPH_BYID;
	diprg.lMin = -tdnInputEnum::STICK_WIDTH;
	diprg.lMax = +tdnInputEnum::STICK_WIDTH;
	if (((LPDIRECTINPUTDEVICE8)pvRef)->SetProperty(DIPROP_RANGE, &diprg.diph) != DI_OK) return DIENUM_STOP;

	return DIENUM_CONTINUE;
}

//------------------------------------------------------
//	デバイス取得
//------------------------------------------------------
LPDIRECTINPUTDEVICE8 tdnInputManager::GetDevice(int no)
{
	HRESULT	hr;
	LPDIRECTINPUTDEVICE8	lpDevice;

	//	デバイス生成
	hr = m_lpDI->CreateDevice(m_DeviceInstances[no].guidInstance, &lpDevice, NULL);
	if (FAILED(hr)) return nullptr;

	// フォーマット設定(キーボードとかジョイスティックとか)
	if (lpDevice->SetDataFormat(&c_dfDIJoystick2) != DI_OK) return nullptr;

	// 強調レベル設定
	/*
	DISCL_FOREGROUND	アプリケーションが非アクティブ状態になると自動的にデバイスの入力を停止する こっちを採用
	DISCL_BACKGROUND	常に入力を続ける

	DISCL_EXCLUSIVE(排他占有)	アクセス権を解放するまで強調レベルを設定した以外のウィンドウは制御できない(ジョイパッドならこれだが、マウスとかキーボードでやると最悪OS終了できない危険性がある)
	DISCL_NOEXCLUSIVE(非排他占有)	↑の逆
	*/
	if (lpDevice->SetCooperativeLevel(tdnSystem::GetWindow(), DISCL_FOREGROUND | DISCL_EXCLUSIVE) != DI_OK) return nullptr;


	// 情報設定
	DIPROPDWORD	dipdw;
	ZeroMemory(&dipdw, sizeof(dipdw));
	dipdw.diph.dwSize = sizeof(dipdw);				// 使用される構造体のサイズ
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);	// DIPROPHEADER構造体のサイズ
	dipdw.diph.dwObj = 0;							// デバイスを対象とする時はいつも0
	dipdw.diph.dwHow = DIPH_DEVICE;					// 対象（ここではデバイス）
	dipdw.dwData = DIPROPAUTOCENTER_OFF;			// 自動センタリング無効

	/*
	DIPROP_AUTOCENTER
	デバイス オブジェクトが自動センタリングを行うかどうかを指定する。
	この設定値は、個別のオブジェクトではなくデバイス全体に適用されるので、割り当てられた DIPROPDWORD 構造体の dwHow メンバは、DIPH_DEVICE でなければならない。
	dwData メンバには、次のいずれかの値を指定できる。
	DIPROPAUTOCENTER_OFF : ユーザーがデバイスを離しても、自動的にセンタリングを行わない。フォース フィードバックを使うアプリケーションは、エフェクトの再生前に自動センタリングを無効にする必要がある。
	DIPROPAUTOCENTER_ON : ユーザーがデバイスを離すと、自動的にセンタリングを行う。
	自動センタリング機能をサポートしていないデバイスもある。
	*/

	// 要するに振動とかのエフェクトを円滑に行えるようにセンタリング設定をOFFにしている
	lpDevice->SetProperty(
		DIPROP_AUTOCENTER,
		&dipdw.diph);


	// 各軸設定
	lpDevice->EnumObjects(EnumAxes, lpDevice, DIDFT_AXIS);

	// 入力制御開始
	lpDevice->Acquire();

	return lpDevice;
}


//*****************************************************************************************************************************
//				入力デバイス
//*****************************************************************************************************************************
const int tdnInputDevice::default_key_map[20] = { 'W', 'S', 'A', 'D', 'Z', 'X', 'C', 'V', 'F', 'G', 'R', 'T', '4', '5', VK_RETURN, VK_SPACE };
const int tdnInputDevice::default_joy_map[20] = { AXIS_X, AXIS_Y, AXIS_Z, AXIS_RZ, 2, 3, 1, 4, 7, 5, 11, 8, 6, 12, 9, 10 };

//------------------------------------------------------
//	初期化
//------------------------------------------------------
tdnInputDevice::tdnInputDevice(int n) :lpDevice(nullptr), pEffect(nullptr)
{
	if (n != -1){
		lpDevice = tdnInputManager::GetDevice(n);
		//if (lpDevice) InitVibration();
	}

	// デフォルト設定
	memcpy(key_map, default_key_map, sizeof(key_map));
	memcpy(joy_map, default_joy_map, sizeof(joy_map));

	// キー初期化
	pad_axisX = pad_axisY = pad_axisX2 = pad_axisY2 = 0;
	for (int i = 0; i<20; i++) key_info[i] = 0;
}

//------------------------------------------------------
//	解放
//------------------------------------------------------
tdnInputDevice::~tdnInputDevice()
{
	if (lpDevice){
		lpDevice->Unacquire();	// アクセス権解放
		lpDevice->Release();
	}
	if (pEffect){
		pEffect->Stop();
		pEffect->Release();
	}
}

//------------------------------------------------------
//	更新
//------------------------------------------------------
void tdnInputDevice::Update()
{
	// スティック初期化
	pad_axisX = pad_axisX2 = pad_axisY = pad_axisY2 = 0;

	if (lpDevice)
	{
		//------------------------------------------------------
		//	ゲームパッド
		//------------------------------------------------------
		// アクセス権が万が一入ってなかったら
		if (FAILED(lpDevice->Poll())){
			HRESULT hr;
			do { hr = lpDevice->Acquire(); } while (hr == DIERR_INPUTLOST);	// アクセス権の再取得
			lpDevice->Poll();
		}

		// パッド情報構造体
		DIJOYSTATE2 js;

		if (lpDevice->GetDeviceState(sizeof(DIJOYSTATE2), &js) == DI_OK){
			//	軸状態保存
			long	axis[6];
			axis[0] = js.lX;	// スティックx軸
			axis[1] = js.lY;	// スティックy軸
			axis[2] = js.lZ;	// スロットル制御を表す場合が多い。ジョイスティックにこの軸がない場合、値はゼロである。 
			axis[3] = js.lRx;	// x 軸回転。ジョイスティックにこの軸がない場合、値はゼロである
			axis[4] = js.lRy;	// y 軸回転。ジョイスティックにこの軸がない場合、値はゼロである
			axis[5] = js.lRz;	// z 軸回転 (方向舵と呼ばれる)。ジョイスティックにこの軸がない場合、値はゼロである。

			//	軸設定
			pad_axisX = axis[joy_map[0]];	//	左スティックＸ軸
			pad_axisY = axis[joy_map[1]];	//	左スティックＹ軸
			pad_axisX2 = axis[joy_map[2]];	//	右スティックＸ軸
			pad_axisY2 = axis[joy_map[3]];	//	右スティックＹ軸

			//	上下左右キー設定
			u32	angle = 8;
			u8	povkey[9] = { 0x01, 0x09, 0x08, 0x0A, 0x02, 0x06, 0x04, 0x05, 0x00 };
			//	８方向取得
			if (LOWORD(js.rgdwPOV[0]) != 0xFFFF) angle = js.rgdwPOV[0] / 4500;
			for (int dir = 0; dir < 4; dir++)
			{
				if (povkey[angle] & (0x01 << dir)){
					if (joy_info[dir] & 0x01) joy_info[dir] = 1; else joy_info[dir] = 3;
				}
				else {
					if (joy_info[dir] & 0x01) joy_info[dir] = 2; else joy_info[dir] = 0;
				}
			}

			//	ボタン(0,1,2,3はスティックなので省く)
			for (int i = 4; i < 16; i++){
				if (js.rgbButtons[joy_map[i]] & 0x80){
					if (joy_info[i] & 0x01) joy_info[i] = 1; else joy_info[i] = 3;
				}
				else {
					if (joy_info[i] & 0x01) joy_info[i] = 2; else joy_info[i] = 0;
				}
			}
		}
	}

	//------------------------------------------------------
	//	キーボード
	//------------------------------------------------------
	//	キーによる左スティック設定
	if (KeyBoard(KB_UP))	pad_axisY = -tdnInputEnum::STICK_WIDTH;
	if (KeyBoard(KB_DOWN))	pad_axisY = tdnInputEnum::STICK_WIDTH;
	if (KeyBoard(KB_LEFT))	pad_axisX = -tdnInputEnum::STICK_WIDTH;
	if (KeyBoard(KB_RIGHT))	pad_axisX = tdnInputEnum::STICK_WIDTH;
	//	軸正規化
	int	work = pad_axisX*pad_axisX + pad_axisY*pad_axisY;
	if (work > tdnInputEnum::STICK_WIDTH * tdnInputEnum::STICK_WIDTH)
	{
		work = (int)sqrtf((float)work);
		pad_axisX = pad_axisX * tdnInputEnum::STICK_WIDTH / work;
		pad_axisY = pad_axisY * tdnInputEnum::STICK_WIDTH / work;
	}

	//	キーによる右スティック設定
	if (KeyBoard(KB_NUMPAD8)) pad_axisY2 = -tdnInputEnum::STICK_WIDTH;
	if (KeyBoard(KB_NUMPAD2)) pad_axisY2 = tdnInputEnum::STICK_WIDTH;
	if (KeyBoard(KB_NUMPAD4)) pad_axisX2 = -tdnInputEnum::STICK_WIDTH;
	if (KeyBoard(KB_NUMPAD6)) pad_axisX2 = tdnInputEnum::STICK_WIDTH;
	//	軸正規化
	work = pad_axisX2*pad_axisX2 + pad_axisY2*pad_axisY2;
	if (work > tdnInputEnum::STICK_WIDTH * tdnInputEnum::STICK_WIDTH)
	{
		work = (int)sqrtf((float)work);
		pad_axisX2 = pad_axisX2 * tdnInputEnum::STICK_WIDTH / work;
		pad_axisY2 = pad_axisY2 * tdnInputEnum::STICK_WIDTH / work;
	}

	//	ボタン情報設定
	for (int i = 0; i < 16; i++)
	{
		if (lpDevice && joy_info[i] != 0x00) key_info[i] = joy_info[i];
		else {
			work = (KeyBoard(key_map[i])) ? 1 : 0;
			(key_info[i] & 0x01) ? (key_info[i] = (work) ? 1 : 2) : (key_info[i] = (work) ? 3 : 0);
		}
	}
}

//------------------------------------------------------
//	情報取得
//------------------------------------------------------


//------------------------------------------------------
//	パッド設定
//------------------------------------------------------
void tdnInputDevice::PadAsign(const PADSET &padset)
{
	//	左スティック
	joy_map[0] = padset.lx;
	joy_map[1] = padset.ly;
	//	右スティック
	joy_map[2] = padset.rx;
	joy_map[3] = padset.ry;
	//	メインボタン
	joy_map[KEY_A] = padset.A - 1;
	joy_map[KEY_B] = padset.B - 1;
	joy_map[KEY_C] = padset.X - 1;
	joy_map[KEY_D] = padset.Y - 1;
	//	ＬＲキー
	joy_map[KEY_L1] = padset.L1 - 1;
	joy_map[KEY_L2] = padset.L2 - 1;
	joy_map[KEY_L3] = padset.L3 - 1;

	joy_map[KEY_R1] = padset.R1 - 1;
	joy_map[KEY_R2] = padset.R2 - 1;
	joy_map[KEY_R3] = padset.R3 - 1;
	//	スタート・セレクト
	joy_map[KEY_START] = padset.START - 1;
	joy_map[KEY_SELECT] = padset.SELECT - 1;
}

//*****************************************************************************************************************************
//		アクセス関数群
//*****************************************************************************************************************************
tdnInputDevice* tdnInput::device[tdnInputEnum::INPUT_DEVICE_MAX];

//------------------------------------------------------
//	初期化
//------------------------------------------------------
void tdnInput::Initialize()
{
	// デバイス初期化
	tdnInputManager::Initialize();
	for (int i = 0; i < tdnInputEnum::INPUT_DEVICE_MAX; i++) device[i] = new tdnInputDevice(i);

	// とりあえず全員デフォルトのキーコンフィグ
	for (int i = 0; i < tdnInputEnum::INPUT_DEVICE_MAX; i++) PadAsign(tdnInputManager::GetGroupID(i), i);

	OKB_Init();
}

//------------------------------------------------------
//	解放
//------------------------------------------------------
void tdnInput::Release()
{
	for (int i = 0; i < tdnInputEnum::INPUT_DEVICE_MAX; i++) SAFE_DELETE(device[i]);
	OKB_Release();
}

//------------------------------------------------------
//	更新
//------------------------------------------------------
void tdnInput::Update()
{
	for (int i = 0; i < tdnInputEnum::INPUT_DEVICE_MAX; i++) device[i]->Update();
	OKB_Update();
}


//------------------------------------------------------
//	キーコンフィグで設定したのをアサイン
//------------------------------------------------------
void tdnInput::PadAsign(LPSTR ID, int no)
{
	std::ifstream infs("DATA/Input/key_config.txt");

	char str[64];

	// ID検索処理
	bool find(false);
	do
	{
		infs >> str;
		if (strcmp(ID, str) == 0)
		{
			find = true;
			break;
		}

		// 読み飛ばし
		for (int i = 0; i < 12; i++) infs >> str;

		// ID検索に引っかからなかった！
		MyAssert(!infs.eof(), "tdnInputでエラー : key_configに存在しないIDが含まれています。listのIDを確認してください");

	} while (!find);

	// ID情報の後にキーコンフィグがある
	u8 Z[16];
	Z[0] = AXIS_X, Z[1] = AXIS_Y, Z[2] = AXIS_Z, Z[3] = AXIS_RZ;
	for (int i = 4; i < 16; i++)
	{
		int n;
		infs >> n;
		Z[i] = n;
	}

	PADSET padset;
	memcpy(&padset, Z, 16);
	device[no]->PadAsign(padset);
}

//------------------------------------------------------
//	微量の傾きを排除したスティック傾き情報
//------------------------------------------------------

void tdnInput::GetAxisXYf(float *outX, float *outY, int no)
{
	*outX = (float)device[no]->GetAxisX() / tdnInputEnum::STICK_WIDTH, *outY = (float)device[no]->GetAxisY() / tdnInputEnum::STICK_WIDTH;

	//// 絶対値処理(2乗sqrtfとどっちが早いんだろう?)
	//float X = (*outX < 0) ? *outX * -1 : *outX;
	//float Y = (*outY < 0) ? *outY * -1 : *outY;
	//
	//float dist = *outX + *outY;

	//// 誤差排除
	//if (dist < tdnInputEnum::MIN_MOVE_STICK)
	//{
	//	*outX = 0;
	//	*outY = 0;
	//}

	//// 正規化(これをしないと斜め移動が早くなってしまう)
	//if (dist > 1.0f)
	//{
	//	*outX /= dist;
	//	*outY /= dist;
	//}
}
void tdnInput::GetAxisXY2f(float *outX, float *outY, int no)
{
	*outX = (float)device[no]->GetAxisX2() / tdnInputEnum::STICK_WIDTH, *outY = (float)device[no]->GetAxisY2() / tdnInputEnum::STICK_WIDTH;

	//// 絶対値処理(2乗sqrtfとどっちが早いんだろう?)
	//*outX = (AX < 0) ? AX*-1 : AX;
	//*outY = (AY < 0) ? AY*-1 : AY;
	//
	//float dist = *outX + *outY;

	//// 誤差排除
	//if (dist < tdnInputEnum::MIN_MOVE_STICK)
	//{
	//	*outX = 0;
	//	*outY = 0;
	//}

	//// 正規化(これをしないと斜め移動が早くなってしまう)
	//if (dist > 1.0f)
	//{
	//	*outX /= dist;
	//	*outY /= dist;
	//}
}

//*****************************************************************************************************************************
//		OKB
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
OwatasoKeyBoard::OwatasoKeyBoard()
{
	// キー配列初期化
	for (int i = 0; i < 256; i++)
		info[i] = on_frame[i] = 0;
}

//------------------------------------------------------
//	更新
//------------------------------------------------------
void OwatasoKeyBoard::Update()
{
	// 取得できたかどうかを確認 & infoにキー入力情報格納
	BOOL ret = GetKeyboardState(info);

	// 取得できなかったら強制終了(毎回止まるようであれば、コメントアウトしてください)
	assert(ret);

	//=========================
	//	全キー

	for (int index = 0; index < 240; index++){	// 240以降、有益なキーはないと思う…

		// 対応キーが押されているなら
		if (info[index] & 0x80)
			on_frame[index]++;		// その番号の入力フレームを加算

		else
			on_frame[index] = 0;	// そうでないならフレーム0にする(押されていない)
	}


	//=========================
	// 基本キーのみ
	//BYTE key_list[] =
	//{
	//	KB_ENTER, KB_SPACE,
	//	KB_0, KB_1, KB_2, KB_3, KB_4, KB_5, KB_6, KB_7, KB_8, KB_9,
	//	KB_A, KB_B, KB_C, KB_D, KB_E, KB_F, KB_G, KB_H, KB_I, KB_J, KB_K, KB_L, KB_M, KB_N, KB_O, KB_P, KB_Q, KB_R, KB_S, KB_T, KB_U, KB_V, KB_W, KB_X, KB_Y, KB_Z,
	//	KB_COMMA, KB_DOT, KB_HYPHEN, KB_COLON, KB_SEMICORON, KB_UP, KB_DOWN, KB_LEFT, KB_RIGHT,
	//	0x00
	//};

	//for (int index = 0; key_list[index] != 0x00; index++){

	//	// 対応キーが押されているなら
	//	if (info[key_list[index]] & 0x80)
	//		on_frame[key_list[index]]++;		// その番号の入力フレームを加算

	//	else
	//		on_frame[key_list[index]] = 0;	// そうでないならフレーム0にする(押されていない)

	//}


}


//------------------------------------------------------
//	どれか押したか
//------------------------------------------------------
BYTE OwatasoKeyBoard::AnyTRG()
{
	//=========================
	//	全キー

	for (int index = 0; index < 240; index++)	// 240以降、有益なキーはないと思う…
	{
		// 何かのキー入力時間が1フレーム目だったらそれを返す
		if (on_frame[info[index]] == 1)
			return index;
	}

	//=========================
	// 基本キーのみ
	//BYTE key_list[] =
	//{
	//	KB_ENTER, KB_SPACE,
	//	KB_0, KB_1, KB_2, KB_3, KB_4, KB_5, KB_6, KB_7, KB_8, KB_9,
	//	KB_A, KB_B, KB_C, KB_D, KB_E, KB_F, KB_G, KB_H, KB_I, KB_J, KB_K, KB_L, KB_M, KB_N, KB_O, KB_P, KB_Q, KB_R, KB_S, KB_T, KB_U, KB_V, KB_W, KB_X, KB_Y, KB_Z,
	//	KB_COMMA, KB_DOT, KB_HYPHEN, KB_COLON, KB_SEMICORON,
	//	0x00
	//};

	//for (int index = 0; key_list[index] != 0x00; index++){

	//	// 何かのキー入力時間が1フレーム目だったらそれを返す
	//	if (on_frame[key_list[index]] == 1)
	//		return key_list[index];
	//}

	// このフレームで新しく押されたキーはなかったので0(キーコードなし)を返す
	return 0x00;

}


//*****************************************************************************************************************************
//	非クラス領域
//*****************************************************************************************************************************
static OwatasoKeyBoard *key_board;

//------------------------------------------------------
//	初期化
//------------------------------------------------------
void OKB_Init()
{
	key_board = new OwatasoKeyBoard;
}

//------------------------------------------------------
//	解放
//------------------------------------------------------
void OKB_Release()
{
	if (key_board){
		delete key_board;
		key_board = nullptr;
	}
}


//------------------------------------------------------
//	更新
//------------------------------------------------------

void OKB_Update()
{
	key_board->Update();
}


//*****************************************************************************************************************************
//		キーアクセス関数
//*****************************************************************************************************************************

UINT KeyBoard(BYTE KeyCode){ return key_board->on_frame[KeyCode]; }
bool KeyBoardTRG(BYTE KeyCode, UINT frame) { return (key_board->on_frame[KeyCode] == frame); }

BYTE KeyBoardAnyTRG(){ return key_board->AnyTRG(); }















//*****************************************************************************************************************************
//		マウス
//*****************************************************************************************************************************
//------------------------------------------------------
//	静的変数
//------------------------------------------------------
Vector2 tdnMouse::m_Axis = Vector2(0, 0);
Vector2 tdnMouse::m_Pos = Vector2(0, 0);
int tdnMouse::m_FlagRight = 0;
int tdnMouse::m_FlagLeft = 0;
POINT tdnMouse::m_CurrentPoint;
POINT tdnMouse::m_PrevPoint;
RECT tdnMouse::m_Rc;
int tdnMouse::m_PrevWheel;
int tdnMouse::m_CurrentWheel;
WHEEL_FLAG tdnMouse::m_FlagW = WHEEL_FLAG::NONE;

//------------------------------------------------------
//	初期化
//------------------------------------------------------
void tdnMouse::Initialize(BOOL show)
{
	ShowCursor(show);
}


//------------------------------------------------------
//	更新
//------------------------------------------------------
void tdnMouse::Update()
{
	if (m_CurrentWheel == m_PrevWheel) m_FlagW = WHEEL_FLAG::NONE;
	else
	{
		m_FlagW = (m_CurrentWheel < m_PrevWheel) ? WHEEL_FLAG::DOWN : WHEEL_FLAG::UP;
		m_PrevWheel = m_CurrentWheel;
	}

	static const float Max = 0.6f;
	static const float Min = 0.25f;

	// 前回座標保存
	m_PrevPoint = m_CurrentPoint;
	//ShowCursor(TRUE);			// 勝手にファルスに
	// 取得
	GetCursorPos(&m_CurrentPoint);
	GetWindowRect(tdnSystem::GetWindow(), &m_Rc);
	// 窓位置と縁による調整
	m_Pos.x = (float)(m_CurrentPoint.x - m_Rc.left - 8);
	m_Pos.y = (float)(m_CurrentPoint.y - m_Rc.top - 29);
	////中央オフセット＆正規化
	m_Axis.x = ((float)m_Pos.x - (tdnSystem::GetScreenSize().right / 2)) / (tdnSystem::GetScreenSize().right / 2);
	m_Axis.y = -((float)m_Pos.y - (tdnSystem::GetScreenSize().bottom / 2)) / (tdnSystem::GetScreenSize().bottom / 2);

	// 最大値制御
	if (m_Axis.x > Max)m_Axis.x = Max;
	else if (m_Axis.x < -Max)m_Axis.x = -Max;
	if (m_Axis.y > Max)m_Axis.y = Max;
	else if (m_Axis.y < -Max)m_Axis.y = -Max;

	// 最小値なら0
	float val_x = sqrtf(m_Axis.x * m_Axis.x), val_y = sqrtf(m_Axis.y * m_Axis.y);
	if (val_x < Min) m_Axis.x = 0;
	if (val_y < Min * 1.5f) m_Axis.y = 0;

	if (KeyBoard(MOUSE_LEFT))
	{
		m_FlagLeft = (m_FlagLeft == 0) ? 3 : 1;
	}
	else m_FlagLeft = 0;
	if ((GetKeyState(0x02) & 0x80) != 0)
	{
		m_FlagRight = (m_FlagRight == 0) ? 3 : 1;
	}
	else m_FlagRight = 0;
}
