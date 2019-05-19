#pragma once

#ifdef _DEBUG
#else
//#define MIDI_INPUT_ON
#endif

//===============================================
//	ピアノ入力用の定数
//===============================================
enum class EP_INPUT
{
	LEFT,		// 左(C)
	RIGHT,		// 右(D)
	START,		// スタートボタン(選曲画面のオプション開く)
	SELECT,		// セレクトボタン(選曲画面の難易度変更)
	ENTER,		// 決定(F or G or A or B)	要は後半4つの白鍵
	BACK,		// 戻る(F# or G# or A#)		要は後半3つの黒鍵
	C,			// オプション用(ド)
	D,			// オプション用(レ)
	E,			// オプション用(ミ)
	WHITE,		// 白鍵
	BLACK,		// 黒鍵
	MAX
};

/********************************************/
//	MIDI入力管理<Singleton>
/********************************************/
class InputMIDIManager
{
public:
	//===============================================
	//	実体取得
	//===============================================
	static InputMIDIManager *GetInstance(){ static InputMIDIManager instance; return &instance; }

	//===============================================
	//	初期化・解放
	//===============================================
	void Initialize();
	void Release();

	//===============================================
	//	更新
	//===============================================
	void Update();

	//===============================================
	//	MIDI信号取得用のコールバック関数
	//===============================================
	static void CALLBACK MidiInProc(HMIDIIN midi_in_handle, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

	//===============================================
	//	アクセサー
	//===============================================
	void SetBaseOctave(char val){ m_cBaseOctave = val; }
	void SetTone(char val){ if (val != val){ midiOutShortMsg(m_hMIDIOut, (0x000000C0 | (val << 8))); } }// 音色を変える
	void SetVelocity(char val){ m_cVelocity = val; }
	void SetON(bool on, char no, bool bAuto = false);
	bool GetON(char no);
	void SetSoundOn(bool b){ m_bSoundOn = b; }
	void SetInputBasisOctave(char c){ m_cInputBasisOctave = c; }
	char GetInputBasisOctave(){ return m_cInputBasisOctave; }
	UINT GetNumOctave(){ return m_uiNumOctave; }
	void AllOff(){ memset(m_InputList, false, sizeof(m_InputList)); }

	//===============================================
	//	入力情報の取得
	//===============================================
	int GetEPInputFrame(EP_INPUT type){return m_iInputFrames[(int)type]; }
	bool GetEPInputTRG(EP_INPUT type){ return (m_iInputFrames[(int)type] == 1); }
	bool GetEPSelectInput(EP_INPUT type, int speed = 11);
	bool GetAnyTrigger(){ FOR((int)EP_INPUT::MAX) { if (m_iInputFrames[i] == 1) return true; }return false; }

	//===============================================
	//	入力情報を格納するbool型の配列
	//===============================================
	bool *m_InputList;	// 定数をヘッダーに書くとアウトなので、可変長ポインタにして、cpp側で確保する(48の配列と思っていただければ)

private:
	//===============================================
	//	メンバ変数
	//===============================================
	HMIDIOUT m_hMIDIOut;					// MIDIアウトハンドル
	HMIDIIN m_hMIDIIn;						// MIDIインハンドル
	char m_cTone;							// 音色
	char m_cBaseOctave;						// 一番下の音
	char m_cVelocity;						// 音の強さ
	char m_cInputBasisOctave;				// MIDIインプットの番号-この数ってやる。
	UINT m_uiNumOctave;						// 接続しているキーボードのオクターブの数
	bool m_bSoundOn;						// キーを押して音が出るかでないかのフラグ
	bool m_bInit;							// 初期化済みか(初期化前にコールバックから呼ばれると落ちるからはじく)
	bool m_bMIDIIn;							// MIDIささってるか
	char errmsg[MAXERRORLENGTH];			// エラーメッセージ
	int m_iInputFrames[(int)EP_INPUT::MAX];	// EP操作の入力情報格納(今は押したフレーム数にする)

	//===============================================
	//	入力チェック用関数
	//===============================================
	bool(InputMIDIManager::*m_InputCheckFunctions[(int)EP_INPUT::MAX])();
	bool CheckLeft();
	bool CheckRight();
	bool CheckStart();
	bool CheckSelect();
	bool CheckEnter();
	bool CheckBack();
	bool CheckC();
	bool CheckD();
	bool CheckE();
	bool CheckWhite();
	bool CheckBlack();

	//===============================================
	//	シングルトンの作法
	//===============================================
	InputMIDIManager();
	InputMIDIManager(const InputMIDIManager&){}
	InputMIDIManager &operator =(const InputMIDIManager&){}
};

/********************************************/
//	インスタンス簡略化
/********************************************/
#define InputMIDIMgr (InputMIDIManager::GetInstance())