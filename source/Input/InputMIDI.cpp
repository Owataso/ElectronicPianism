#include "tdnlib.h"
#include "../Music/MusicInfo.h"
#include "InputMIDI.h"
#include "../UI/ui.h"

#define MIDIMSG(a,b,c,d) ((DWORD)((a<<24) | (b<<16) | (c<<8) | (d)))

bool InputMIDIManager::GetEPSelectInput(EP_INPUT type, int speed)
{
	return (
		GetEPInputFrame(type) == 1 || 
		GetEPInputFrame(type) > 0 && InputMIDIMgr->GetEPInputFrame(type) % speed == 0);
}

void CALLBACK InputMIDIManager::MidiInProc(HMIDIIN midi_in_handle,UINT wMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	switch (wMsg)
	{
	case MIM_OPEN:
		//printf("MIDI device was opened\n");
		break;
	case MIM_CLOSE:
		//printf("MIDI device was closed\n");
		break;
	case MIM_DATA:
	{
					 const BYTE l_byEvent(dwParam1 & 0x000000ff);

					 // ノートオフイベント
					 if (l_byEvent == 0x80)
					 {
						 const BYTE note((dwParam1 & 0x0000ff00) >> 8);

						 // コールバックじゃない方に渡してあげる
						 InputMIDIMgr->SetON(false, note);
					 }
					 // ノートオンイベント
					 else if (l_byEvent == 0x90)
					 {
						 const BYTE velocity((BYTE)((dwParam1 & 0x00ff0000) >> 16));
						 const BYTE note((dwParam1 & 0x0000ff00) >> 8);
						 //printf("MIM_DATA: wMsg=%08X, p1=%08X, p2=%08X\n", wMsg, dwParam1, dwParam2);
						 //printf("note = %u, velocity = %u\n", note, velocity);

						 // コールバックじゃない方に渡してあげる
						 InputMIDIMgr->SetON((velocity != 0), note);	// velocityが0という事はすなわち鍵盤が離された瞬間
					 }
					 break;
	}
		break;
	case MIM_LONGDATA:
	case MIM_ERROR:
	case MIM_LONGERROR:
	case MIM_MOREDATA:
	default:
		break;
	}
}


//*****************************************************************************************************************************
//	初期化・解放
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
InputMIDIManager::InputMIDIManager() :m_bInit(false), m_bSoundOn(false), m_cBaseOctave(0), m_cVelocity(127), m_bMIDIIn(false), m_uiNumOctave(MAX_OCTAVE){}
void InputMIDIManager::Initialize()
{
	HRESULT hr;

#ifdef MIDI_INPUT_ON
	//tdnSystem::OpenDebugWindow();	// コンソールON

	// MIDIアウトデバイス検索
	//MIDIOUTCAPS OutCaps;
	//UINT NumDevice = midiOutGetNumDevs();
	//printf("MIDIOutputDecices:\n");
	//for (UINT i = 0; i < NumDevice; i++)
	//{
	//	if (midiOutGetDevCaps(i, &OutCaps, sizeof(OutCaps)) == MMSYSERR_NOERROR)
	//	{
	//		printf("ID = %d, %s\n", i, OutCaps.szPname);
	//	}
	//}

	//// MIDIインプットデバイス検索
	//MIDIINCAPS InCaps;
	//printf("MIDIInputDecices:\n");
	//UINT NumDevice = midiInGetNumDevs();
	//for (UINT i = 0; i < NumDevice; i++)
	//{
	//	if (midiInGetDevCaps(i, &InCaps, sizeof(InCaps)) == MMSYSERR_NOERROR)
	//	{
	//		printf("ID = %d, %s\n", i, InCaps.szPname);
	//	}
	//}
	//
	//printf("使用するInputMIDIの番号を選択してください->");
	UINT DeviceID(0);
	//std::cin >> DeviceID;

	// 直に入ってるデバイスIDと基本オクターブのテキストを読み込み
	{
		// ifstreamでやってもいいのだが、FILEでやってみたい気分だったのでやってみる
		FILE *fp;
		fopen_s(&fp, "Keyboard.txt", "rb");
		MyAssert(fp, "キーボードデバイスのテキスト入ってない！");

		auto ReadNumber = [](FILE *fp)
		{
			int ret(0);
			const int DIGIT = 4;	// とりあえず4桁
			char receive[DIGIT];
			for (int i = 0; i < DIGIT; i++)
			{
				fread(&receive[i], 1, 1, fp);

				// 数字じゃない(終了)
				if ((receive[i] < '0') || (receive[i] > '9'))
				{
					//int mul(1);	// 桁加算用
					//for (int j = i - 1; j >= 0; j--)
					//{
					//	if ((receive[j] < '0') || (receive[j] > '9')) MyAssert((receive[j] >= '0' && receive[j] <= '9'), "KeyboardInfo.txtが数字じゃない何かが入ってる");
					//	ret += (int)(receive[j] - '0') * mul;
					//	mul *= 10;
					//}
					receive[i] = '\0';
					ret = atoi(receive);
					break;	// 忘れてたからひどいことになった
				}
			}
			return ret;
		};

		char skip;
		// 1行目の:まで
		do
		{
			fread(&skip,1,1,fp);
		}while(skip != ':');

		// 数字読み込み
		DeviceID = ReadNumber(fp);

		// 2行目の:まで
		do
		{
			fread(&skip, 1, 1, fp);
		} while (skip != ':');

		// 数字読み込み
		m_cInputBasisOctave = ReadNumber(fp);
		m_cInputBasisOctave *= 12;	// オクターブ分かりにくかったので、この仕様にした

		// 3行目の:まで
		do
		{
			fread(&skip,1,1,fp);
		}while(skip != ':');

		// 数字読み込み
		m_uiNumOctave = (UINT)ReadNumber(fp);

		fclose(fp);
	}

	hr = midiInOpen(&m_hMIDIIn, DeviceID, (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);
	if (hr == MMSYSERR_NOERROR)
	{
		// MIDIインプット開始
		midiInStart(m_hMIDIIn);
		m_bMIDIIn = true;
	}
	else
	{
		midiInGetErrorText(hr, errmsg, sizeof(errmsg));
		std::string msg = "MIDIキーボード接続失敗！\nKeyboardCheckerでキーボードの設定をしてもう一度起動してください。\nErrMsg:";
		msg += errmsg;
		MessageBox(0, msg.c_str(), "ElectronicPianism", MB_OK);
		m_bMIDIIn = false;
	}

	//tdnSystem::CloseDebugWindow();
#endif

	// MIDIの音出すやつ初期化
	hr = midiOutOpen(&m_hMIDIOut, MIDI_MAPPER, 0, 0, CALLBACK_NULL);
	assert(hr == MMSYSERR_NOERROR);

	// キーボードのONOFF状態
	m_InputList = new bool[NUM_KEYBOARD];
	memset(m_InputList, false, sizeof(bool)*NUM_KEYBOARD);

	// ゲーム操作用キーボードインプットフレーム
	memset(m_iInputFrames, 0, sizeof(m_iInputFrames));
	// インプットチェック用関数と関数ポインタ
	m_InputCheckFunctions[(int)EP_INPUT::LEFT] = &InputMIDIManager::CheckLeft;
	m_InputCheckFunctions[(int)EP_INPUT::RIGHT] = &InputMIDIManager::CheckRight;
	m_InputCheckFunctions[(int)EP_INPUT::START] = &InputMIDIManager::CheckStart;
	m_InputCheckFunctions[(int)EP_INPUT::SELECT] = &InputMIDIManager::CheckSelect;
	m_InputCheckFunctions[(int)EP_INPUT::ENTER] = &InputMIDIManager::CheckEnter;
	m_InputCheckFunctions[(int)EP_INPUT::BACK] = &InputMIDIManager::CheckBack;
	m_InputCheckFunctions[(int)EP_INPUT::C] = &InputMIDIManager::CheckC;
	m_InputCheckFunctions[(int)EP_INPUT::D] = &InputMIDIManager::CheckD;
	m_InputCheckFunctions[(int)EP_INPUT::E] = &InputMIDIManager::CheckE;
	m_InputCheckFunctions[(int)EP_INPUT::WHITE] = &InputMIDIManager::CheckWhite;
	m_InputCheckFunctions[(int)EP_INPUT::BLACK] = &InputMIDIManager::CheckBlack;

	// 初期化OK
	m_bInit = true;
}

//------------------------------------------------------
//	解放
//------------------------------------------------------
void InputMIDIManager::Release()
{
	//AllOff();

	delete[] m_InputList;
	midiOutClose(m_hMIDIOut);

#ifdef MIDI_INPUT_ON
	if (m_bMIDIIn)
	{
		// MIDIインプット解放
		midiInStop(m_hMIDIIn);
		midiInReset(m_hMIDIIn);
		midiInClose(m_hMIDIIn);
	}
#endif
}


//*****************************************************************************************************************************
//	更新
//*****************************************************************************************************************************
//------------------------------------------------------
//	更新
//------------------------------------------------------
void InputMIDIManager::Update()
{
	FOR((int)EP_INPUT::MAX)
	{
		// インプット更新
		m_iInputFrames[i] = (this->*m_InputCheckFunctions[i])() ?m_iInputFrames[i] + 1 : 0;	// 押してたらフレームを足していく
	}
}

//------------------------------------------------------
//		音のオンオフ
//------------------------------------------------------
bool InputMIDIManager::GetON(char no)
{
	assert(no >= 0 && no < NUM_KEYBOARD);
	return m_InputList[no];
}

//------------------------------------------------------
//		ノートオン
//------------------------------------------------------
void InputMIDIManager::SetON(bool ON, char no, bool bAuto)
{
#ifdef MIDI_INPUT_ON
	// 初期化してなかったらはじく(コールバックからこの関数が呼ばれるから)
	if (!m_bInit) return;

	if(!bAuto)
	{
		if (no < m_cInputBasisOctave ||				// 設定した範囲より小さい
			no > m_cInputBasisOctave + NUM_KEYBOARD - 1	// 設定した範囲より大きい
			)return;

		no -= m_cInputBasisOctave;	// 設定したオクターブに調整
	}
#endif

	if (ON)
	{
		if (!m_InputList[no])
		{
			m_InputList[no] = true;
			if (m_bSoundOn) midiOutShortMsg(m_hMIDIOut, MIDIMSG(0x00, m_cVelocity, (m_cBaseOctave + no), 0x90)); // 発音
		}
	}
	else
	{
#ifdef MIDI_INPUT_ON
		//if (m_InputList[no]) midiOutShortMsg(hmo, MIDIMSG(0x00, 0x70, (base_octave + no), 0x80)); // 消音
#endif
		m_InputList[no] = false;
	}
}

//------------------------------------------------------
//		EP用の入力
//------------------------------------------------------
bool InputMIDIManager::CheckLeft()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12]) return true;		// このフレームにCが押されている
	return false;	// このフレームにCは押されなかった
#else
	return ((KeyBoard(KB_LEFT)) || 
		//(KeyBoard(KB_A)) || 
		(KeyBoard(KB_UP))
		//|| (KeyBoard(KB_W))
		);	// キーボードで左か上関連のキー
#endif
}

bool InputMIDIManager::CheckRight()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12 + 2]) return true;		// このフレームにDが押されている
	return false;	// このフレームにDは押されなかった
#else
	return ((KeyBoard(KB_RIGHT)) || 
		//(KeyBoard(KB_D)) || 
		(KeyBoard(KB_DOWN))
		//|| (KeyBoard(KB_S))
		);	// キーボードで右か下関連のキー
#endif
}

bool InputMIDIManager::CheckStart()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12 + 1]) return true;		// このフレームにC#が押されている
	return false;	// このフレームにCは押されなかった
#else
	return (KeyBoard(KB_W) > 0);	// キーボードで1キー
#endif
}

bool InputMIDIManager::CheckSelect()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12 + 3]) return true;		// このフレームにD#が押されている
	return false;	// このフレームにCは押されなかった
#else
	return (KeyBoard(KB_E) > 0);	// キーボードで2キー
#endif
}

bool InputMIDIManager::CheckEnter()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12 + 5] || m_InputList[i * 12 + 7] || m_InputList[i * 12 + 9] || m_InputList[i * 12 + 11]) return true;		// このフレームにF or G or A or Bが押されている
	return false;	// このフレームにF or G or A or Bは押されなかった
#else
	return (KeyBoard(KB_ENTER) > 0);	// キーボードでエンターキー
#endif
}

bool InputMIDIManager::CheckBack()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12 + 6] || m_InputList[i * 12 + 8] || m_InputList[i * 12 + 10]) return true;		// このフレームにF# or G# or A#が押されている
	return false;	// このフレームにF# or G# or A#は押されなかった
#else
	return (KeyBoard(KB_SPACE) > 0);	// キーボードでスペースキー
#endif
}

bool InputMIDIManager::CheckC()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12]) return true;		// このフレームにCが押されている
	return false;	// このフレームにCは押されなかった
#else
	return (KeyBoard(KB_A) > 0);	// キーボードで2キー
#endif
}

bool InputMIDIManager::CheckD()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12 + 2]) return true;		// このフレームにDが押されている
	return false;	// このフレームにF or G or A or Bは押されなかった
#else
	return (KeyBoard(KB_S) > 0);	// キーボードでXキー
#endif
}

bool InputMIDIManager::CheckE()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12 + 4]) return true;		// このフレームにEが押されている
	return false;	// このフレームにF# or G# or A#は押されなかった
#else
	return (KeyBoard(KB_D) > 0);	// キーボードでCキー
#endif
}

bool InputMIDIManager::CheckWhite()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12] || m_InputList[i * 12 + 2] || m_InputList[i * 12 + 4] || m_InputList[i * 12 + 5] || m_InputList[i * 12 + 7] || m_InputList[i * 12 + 9] || m_InputList[i * 12 + 11]) return true;		// このフレームに白鍵が押されている
	return false;	// このフレームに白鍵は押されなかった
#else
	return (KeyBoard(KB_ENTER) > 0);	// キーボードでエンターキー
#endif
}

bool InputMIDIManager::CheckBlack()
{
#ifdef MIDI_INPUT_ON
	FOR(MAX_OCTAVE)if (m_InputList[i * 12 + 1] || m_InputList[i * 12 + 3] || m_InputList[i * 12 + 6] || m_InputList[i * 12 + 8] || m_InputList[i * 12 + 10]) return true;		// このフレームに黒鍵が押されている
	return false;	// このフレームに黒鍵は押されなかった
#else
	return (KeyBoard(KB_SPACE) > 0);	// キーボードでスペースキー
#endif
}

