#include<iostream>
#include<Windows.h>
#include<mmsystem.h>

#pragma comment (lib,"winmm.lib")

// プロトタイプ宣言
void CALLBACK MidiInProc(HMIDIIN midi_in_handle, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

class Singleton
{
public:
	static Singleton *GetInstance(){ static Singleton i; return &i; }
	~Singleton()
	{
		delete m_pState;
		// MIDIインプット終了
		if (m_bStartMIDIInHandle)
		{
			midiInStop(m_MIDIInHandle);
			midiInReset(m_MIDIInHandle);
		}
		if (m_bOpenMIDIInHandle) midiInClose(m_MIDIInHandle);
	}
	bool Update()
	{
		return m_pState->Update(this);
	}

	enum class STATE
	{
		ENUM_DEVICE,
		DEVICE_SELECT,
		BASIS_OCTAVE,
		END_SETTING
	};

	STATE GetState(){ return m_pState->GetState(); }

	void ChangeState(STATE s)
	{
		if (m_pState) delete m_pState;
		switch (s)
		{
		case STATE::ENUM_DEVICE:
			m_pState = new State::EnumDevice;
			break;
		case STATE::DEVICE_SELECT:
			m_pState = new State::DeviceSelect;
			break;
		case STATE::BASIS_OCTAVE:
			m_pState = new State::BasisOctave;
			break;
		case STATE::END_SETTING:
			m_pState = new State::EndSetting;
			break;
		}
	}

	void SetBasisOctave(char octave)
	{
		BasisOctave = octave; 
	}

	void SetNumOctave(char octave)
	{
		NumOctave = octave;
	}
private:
	Singleton() : BasisOctave(-1), NumOctave(0), DeviceID(0), m_pState(nullptr), m_NumDevice(0),m_bOpenMIDIInHandle(false),m_bStartMIDIInHandle(false) { ChangeState(STATE::ENUM_DEVICE); }
	Singleton(const Singleton&){}

	// テキストに書き出す情報
	UINT DeviceID;
	char BasisOctave;
	char NumOctave;

	// MIDIインプットハンドル
	HMIDIIN m_MIDIInHandle;
	bool m_bOpenMIDIInHandle;
	bool m_bStartMIDIInHandle;

	UINT m_NumDevice;

	class State
	{
	public:

		class Base
		{
		public:
			Base(){}
			virtual bool Update(Singleton *parent) = 0;
			virtual STATE GetState() = 0;
		};
		class EnumDevice : public Base
		{
		public:
			EnumDevice() : Base(){}
			bool Update(Singleton *parent)
			{
				MIDIINCAPS InCaps;
				printf_s("MIDIインプットデバイス一覧:\n");
				parent->m_NumDevice = midiInGetNumDevs();
				for (UINT i = 0; i < parent->m_NumDevice; i++)
				{
					if (midiInGetDevCaps(i, &InCaps, sizeof(InCaps)) == MMSYSERR_NOERROR)
					{
						printf_s("デバイス名[%s] デバイス番号[%d]\n", InCaps.szPname, i);
					}
				}
				// デバイス検索何もなかったら
				if (!parent->m_NumDevice)
				{
					// エラー吐き出す
					printf_s("MIDIインプットデバイスが見つかりません。\nキーボードが接続されていないか、キーボード側の設定の問題だと思われます\n");
					return true;
				}
				else
				{
					parent->ChangeState(STATE::DEVICE_SELECT);
					return false;
				}
			}
			STATE GetState(){ return STATE::ENUM_DEVICE; }
		};
		class DeviceSelect : public Base
		{
		public:
			DeviceSelect() : Base(){}
			bool Update(Singleton *parent)
			{
				printf_s("使用するキーボードのデバイス番号を入力してください->");
				do
				{
					// 正しい値を入力するまで回す
					std::cin >> parent->DeviceID;
				} while (parent->DeviceID >= parent->m_NumDevice || parent->DeviceID < 0);

				printf_s("MIDI_OpenID[%d] : ", parent->DeviceID);
				HRESULT hr = midiInOpen(&parent->m_MIDIInHandle, parent->DeviceID, (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);
				if (hr == MMSYSERR_NOERROR)
				{
					parent->m_bOpenMIDIInHandle = true;

					printf_s("MIDIキーボード接続成功！\n");
					// MIDIインプット開始
					if (midiInStart(parent->m_MIDIInHandle) != MMSYSERR_NOERROR)
					{
						printf_s("MIDIインプット失敗したやんけ！\n");
						return true;
					}

					parent->m_bStartMIDIInHandle = true;
					parent->ChangeState(STATE::BASIS_OCTAVE);
					return false;
				}
				else
				{
					char errmsg[MAXERRORLENGTH];
					midiInGetErrorText(hr, errmsg, sizeof(errmsg));
					printf_s("MIDI接続に失敗！\nERROR MESSAGE:%s", errmsg);
					return true;
				}
			}
			STATE GetState(){ return STATE::DEVICE_SELECT; }
		};
		class BasisOctave : public Base
		{
		public:
			BasisOctave() : Base(){ printf_s("基本オクターブを設定します。\n使用しているキーボードの1番低い「ド」を押してください\n\n※反応しない場合は再起動するとうまくいくことも。"); }
			bool Update(Singleton *parent)
			{
				if (parent->BasisOctave % 12 == 0)
				{
					parent->BasisOctave /= 12;

					//printf_s("基本オクターブ設定完了！%d\n\n次に鍵盤のオクターブ数を入力してください(2 ～ 4)->", parent->BasisOctave);
					//
					//// 正しい値(2～4)を入力するまで回す
					//do
					//{
					//	std::cin >> parent->NumOctave;
					//} while (parent->NumOctave > 4 || parent->DeviceID < 2);
					//
					//// 2オクターブまでなら1オクターブ進んでるｆ
					//if (parent->NumOctave == 2)
					//{
					//	parent->BasisOctave += 12;
					//}

					parent->NumOctave = 4;

					printf_s("基本オクターブ設定完了！%d\n", parent->BasisOctave);

					parent->ChangeState(Singleton::STATE::END_SETTING);
				}
				return false;
			}
			STATE GetState(){ return STATE::BASIS_OCTAVE; }
		};
		class EndSetting : public Base
		{
		public:
			EndSetting() : Base(){}
			bool Update(Singleton *parent)
			{
				printf_s("情報をテキストに書き出しています…\n");

				// 情報をテキストにOutput
				FILE *fp(nullptr);
				fopen_s(&fp, "Keyboard.txt", "w");
				//fwrite(&DeviceID, sizeof(UINT), 1, fp);	// 何故か空白が書き出される
				fprintf_s(fp, "デバイス番号:%d\n", parent->DeviceID);
				fprintf_s(fp, "ベースオクターブ:%d\n", parent->BasisOctave);
				fprintf_s(fp, "オクターブ数:%d", parent->NumOctave);
				fclose(fp);

				printf_s("書き出し完了！\n\n");

				return true;
			}
			STATE GetState(){ return STATE::END_SETTING; }
		};
	};
	State::Base *m_pState;
};


//bool g_bTest(false);		// テスト中に使う
//char g_PushKey(0);		// コールバックの外側で押したかどうか調べる用

// MIDIイベント受取り用のコールバック関数
void CALLBACK MidiInProc(HMIDIIN midi_in_handle,UINT wMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
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
	switch (wMsg)
	{
	case MIM_OPEN:
		printf("CallBack: MIDI device was opened\n");
		break;
	case MIM_CLOSE:
		printf("CallBack: MIDI device was closed\n");
		break;
	case MIM_DATA:
	{
					 const BYTE status_byte(dwParam1 & 0x000000ff);

					 // ノートオフイベント
					 if (status_byte == 0x80)
					 {

					 }
					 // ノートオンイベント
					 else if (status_byte == 0x90)
					 {
						 unsigned char velocity = (BYTE)((dwParam1 & 0x00ff0000) >> 16);
						 unsigned char note = (dwParam1 & 0x0000ff00) >> 8;
						 if (Singleton::GetInstance()->GetState() == Singleton::STATE::BASIS_OCTAVE)
						 {
							 printf_s("CallBack: %u\n", note);
							 //if (note % 12 == 0)
							 {
								 Singleton::GetInstance()->SetBasisOctave(note);
							 }
						 }
						 /*
						 else
						 {
						 printf_s("CallBack: MIM_DATA: wMsg=%08X, p1=%08X, p2=%08X\n", wMsg, dwParam1, dwParam2);
						 if (velocity != 0)printf_s("%u番の鍵盤が、%uの強さで押されたよ！\n", note, velocity);
						 else printf_s("%u番の鍵盤が、離されたよ！！\n", note);
						 }
						 */
					 }


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

void main()
{
	while (!Singleton::GetInstance()->Update())
	{

	}

	//// エンター押すまで抜けない
	printf_s("\n\nエンターキーで終了");
	while (!(GetKeyState(VK_RETURN) & 0x80));

	//g_bTest = false;
	//g_PushKey = -1;

	//// テキストに書き出す情報
	//UINT DeviceID;
	//char BasisOctave;

	//// MIDIインプットハンドル
	//HMIDIIN m_MIDIInHandle;

	//// MIDIインプットデバイス検索
	//MIDIINCAPS InCaps;
	//printf_s("MIDIインプットデバイス一覧:\n");
	//UINT NumDevice = midiInGetNumDevs();
	//for (UINT i = 0; i < NumDevice; i++)
	//{
	//	if (midiInGetDevCaps(i, &InCaps, sizeof(InCaps)) == MMSYSERR_NOERROR)
	//	{
	//		printf_s("デバイス名[%s] デバイス番号[%d]\n",InCaps.szPname,i);
	//	}
	//}
	//// デバイス検索何もなかったら
	//if (!NumDevice)
	//{
	//	// エラー吐き出す
	//	printf_s("MIDIインプットデバイスが見つかりません。\nキーボードが接続されていないか、キーボード側の設定の問題だと思われます\n");
	//}

	//else
	//{
	//	printf_s("使用するキーボードのデバイス番号を入力してください->");
	//	do
	//	{
	//		// 正しい値を入力するまで回す
	//		std::cin >> DeviceID;
	//	} while (DeviceID >= NumDevice || DeviceID < 0);

	//	printf_s("MIDI_OpenID[%d] : ", DeviceID);
	//	HRESULT hr = midiInOpen(&m_MIDIInHandle, DeviceID, (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);
	//	if (hr == MMSYSERR_NOERROR)
	//	{
	//		printf_s("MIDIキーボード接続成功！\n");
	//		// MIDIインプット開始
	//		midiInStart(m_MIDIInHandle);

	//		printf_s("基本オクターブを設定します。\n使用しているキーボードの1番低い「ド」を押してください");
	//		// 何かしらの「ド」を押すまでループ
	//		while (true)
	//		{
	//			const char ans = g_PushKey % 12;
	//			if (ans == 0) break;
	//		}

	//		BasisOctave = g_PushKey / 12;	// 12で割ったらオクターブ数が出る

	//		printf_s("基本オクターブ設定完了！%d\n\n", BasisOctave);

	//		int receive;
	//		printf_s("テストしてみますか？(0:しない, 1:する)->");
	//		do
	//		{
	//			std::cin >> receive;
	//		} while (receive != 0 && receive != 1);	// 0か1押すまでループ

	//		if (receive == 1)
	//		{
	//			g_bTest = true;
	//			printf_s("MIDIテスト中…いろいろキーボード押してみてね。押して文字が出たら成功だよ\n終了したかったら、スペースキーを押してね");

	//			// スペース押すまで抜けない(キーボードイベント処理はコールバックで行っているので問題なし)
	//			while (!(GetKeyState(VK_SPACE) & 0x80));
	//		}

	//		// MIDIインプット終了
	//		midiInStop(m_MIDIInHandle);
	//		midiInReset(m_MIDIInHandle);
	//		midiInClose(m_MIDIInHandle);

	//		// 情報をテキストにOutput
	//		FILE *fp(nullptr);
	//		fopen_s(&fp, "Keyboard.txt", "w");
	//		//fwrite(&DeviceID, sizeof(UINT), 1, fp);	// 何故か空白が書き出される
	//		fprintf_s(fp, "デバイス番号:%d\n", DeviceID);
	//		fprintf_s(fp, "ベースオクターブ:%d", BasisOctave);
	//		fclose(fp);
	//	}
	//	else
	//	{
	//		char errmsg[MAXERRORLENGTH];
	//		midiInGetErrorText(hr, errmsg, sizeof(errmsg));
	//		printf_s("MIDI接続に失敗！\nERROR MESSAGE:%s", errmsg);
	//	}
	//}

	//// エンター押すまで抜けない
	//printf_s("\n\nエンターキーで終了");
	//while (!(GetKeyState(VK_RETURN) & 0x80));
}