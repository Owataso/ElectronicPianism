#include<iostream>
#include<Windows.h>
#include<mmsystem.h>

#pragma comment (lib,"winmm.lib")

// �v���g�^�C�v�錾
void CALLBACK MidiInProc(HMIDIIN midi_in_handle, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

class Singleton
{
public:
	static Singleton *GetInstance(){ static Singleton i; return &i; }
	~Singleton()
	{
		delete m_pState;
		// MIDI�C���v�b�g�I��
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

	// �e�L�X�g�ɏ����o�����
	UINT DeviceID;
	char BasisOctave;
	char NumOctave;

	// MIDI�C���v�b�g�n���h��
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
				printf_s("MIDI�C���v�b�g�f�o�C�X�ꗗ:\n");
				parent->m_NumDevice = midiInGetNumDevs();
				for (UINT i = 0; i < parent->m_NumDevice; i++)
				{
					if (midiInGetDevCaps(i, &InCaps, sizeof(InCaps)) == MMSYSERR_NOERROR)
					{
						printf_s("�f�o�C�X��[%s] �f�o�C�X�ԍ�[%d]\n", InCaps.szPname, i);
					}
				}
				// �f�o�C�X���������Ȃ�������
				if (!parent->m_NumDevice)
				{
					// �G���[�f���o��
					printf_s("MIDI�C���v�b�g�f�o�C�X��������܂���B\n�L�[�{�[�h���ڑ�����Ă��Ȃ����A�L�[�{�[�h���̐ݒ�̖�肾�Ǝv���܂�\n");
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
				printf_s("�g�p����L�[�{�[�h�̃f�o�C�X�ԍ�����͂��Ă�������->");
				do
				{
					// �������l����͂���܂ŉ�
					std::cin >> parent->DeviceID;
				} while (parent->DeviceID >= parent->m_NumDevice || parent->DeviceID < 0);

				printf_s("MIDI_OpenID[%d] : ", parent->DeviceID);
				HRESULT hr = midiInOpen(&parent->m_MIDIInHandle, parent->DeviceID, (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);
				if (hr == MMSYSERR_NOERROR)
				{
					parent->m_bOpenMIDIInHandle = true;

					printf_s("MIDI�L�[�{�[�h�ڑ������I\n");
					// MIDI�C���v�b�g�J�n
					if (midiInStart(parent->m_MIDIInHandle) != MMSYSERR_NOERROR)
					{
						printf_s("MIDI�C���v�b�g���s������񂯁I\n");
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
					printf_s("MIDI�ڑ��Ɏ��s�I\nERROR MESSAGE:%s", errmsg);
					return true;
				}
			}
			STATE GetState(){ return STATE::DEVICE_SELECT; }
		};
		class BasisOctave : public Base
		{
		public:
			BasisOctave() : Base(){ printf_s("��{�I�N�^�[�u��ݒ肵�܂��B\n�g�p���Ă���L�[�{�[�h��1�ԒႢ�u�h�v�������Ă�������\n\n���������Ȃ��ꍇ�͍ċN������Ƃ��܂��������Ƃ��B"); }
			bool Update(Singleton *parent)
			{
				if (parent->BasisOctave % 12 == 0)
				{
					parent->BasisOctave /= 12;

					//printf_s("��{�I�N�^�[�u�ݒ芮���I%d\n\n���Ɍ��Ղ̃I�N�^�[�u������͂��Ă�������(2 �` 4)->", parent->BasisOctave);
					//
					//// �������l(2�`4)����͂���܂ŉ�
					//do
					//{
					//	std::cin >> parent->NumOctave;
					//} while (parent->NumOctave > 4 || parent->DeviceID < 2);
					//
					//// 2�I�N�^�[�u�܂łȂ�1�I�N�^�[�u�i��ł邆
					//if (parent->NumOctave == 2)
					//{
					//	parent->BasisOctave += 12;
					//}

					parent->NumOctave = 4;

					printf_s("��{�I�N�^�[�u�ݒ芮���I%d\n", parent->BasisOctave);

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
				printf_s("�����e�L�X�g�ɏ����o���Ă��܂��c\n");

				// �����e�L�X�g��Output
				FILE *fp(nullptr);
				fopen_s(&fp, "Keyboard.txt", "w");
				//fwrite(&DeviceID, sizeof(UINT), 1, fp);	// ���̂��󔒂������o�����
				fprintf_s(fp, "�f�o�C�X�ԍ�:%d\n", parent->DeviceID);
				fprintf_s(fp, "�x�[�X�I�N�^�[�u:%d\n", parent->BasisOctave);
				fprintf_s(fp, "�I�N�^�[�u��:%d", parent->NumOctave);
				fclose(fp);

				printf_s("�����o�������I\n\n");

				return true;
			}
			STATE GetState(){ return STATE::END_SETTING; }
		};
	};
	State::Base *m_pState;
};


//bool g_bTest(false);		// �e�X�g���Ɏg��
//char g_PushKey(0);		// �R�[���o�b�N�̊O���ŉ��������ǂ������ׂ�p

// MIDI�C�x���g����p�̃R�[���o�b�N�֐�
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

					 // �m�[�g�I�t�C�x���g
					 if (status_byte == 0x80)
					 {

					 }
					 // �m�[�g�I���C�x���g
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
						 if (velocity != 0)printf_s("%u�Ԃ̌��Ղ��A%u�̋����ŉ����ꂽ��I\n", note, velocity);
						 else printf_s("%u�Ԃ̌��Ղ��A�����ꂽ��I�I\n", note);
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

	//// �G���^�[�����܂Ŕ����Ȃ�
	printf_s("\n\n�G���^�[�L�[�ŏI��");
	while (!(GetKeyState(VK_RETURN) & 0x80));

	//g_bTest = false;
	//g_PushKey = -1;

	//// �e�L�X�g�ɏ����o�����
	//UINT DeviceID;
	//char BasisOctave;

	//// MIDI�C���v�b�g�n���h��
	//HMIDIIN m_MIDIInHandle;

	//// MIDI�C���v�b�g�f�o�C�X����
	//MIDIINCAPS InCaps;
	//printf_s("MIDI�C���v�b�g�f�o�C�X�ꗗ:\n");
	//UINT NumDevice = midiInGetNumDevs();
	//for (UINT i = 0; i < NumDevice; i++)
	//{
	//	if (midiInGetDevCaps(i, &InCaps, sizeof(InCaps)) == MMSYSERR_NOERROR)
	//	{
	//		printf_s("�f�o�C�X��[%s] �f�o�C�X�ԍ�[%d]\n",InCaps.szPname,i);
	//	}
	//}
	//// �f�o�C�X���������Ȃ�������
	//if (!NumDevice)
	//{
	//	// �G���[�f���o��
	//	printf_s("MIDI�C���v�b�g�f�o�C�X��������܂���B\n�L�[�{�[�h���ڑ�����Ă��Ȃ����A�L�[�{�[�h���̐ݒ�̖�肾�Ǝv���܂�\n");
	//}

	//else
	//{
	//	printf_s("�g�p����L�[�{�[�h�̃f�o�C�X�ԍ�����͂��Ă�������->");
	//	do
	//	{
	//		// �������l����͂���܂ŉ�
	//		std::cin >> DeviceID;
	//	} while (DeviceID >= NumDevice || DeviceID < 0);

	//	printf_s("MIDI_OpenID[%d] : ", DeviceID);
	//	HRESULT hr = midiInOpen(&m_MIDIInHandle, DeviceID, (DWORD_PTR)MidiInProc, 0, CALLBACK_FUNCTION);
	//	if (hr == MMSYSERR_NOERROR)
	//	{
	//		printf_s("MIDI�L�[�{�[�h�ڑ������I\n");
	//		// MIDI�C���v�b�g�J�n
	//		midiInStart(m_MIDIInHandle);

	//		printf_s("��{�I�N�^�[�u��ݒ肵�܂��B\n�g�p���Ă���L�[�{�[�h��1�ԒႢ�u�h�v�������Ă�������");
	//		// ��������́u�h�v�������܂Ń��[�v
	//		while (true)
	//		{
	//			const char ans = g_PushKey % 12;
	//			if (ans == 0) break;
	//		}

	//		BasisOctave = g_PushKey / 12;	// 12�Ŋ�������I�N�^�[�u�����o��

	//		printf_s("��{�I�N�^�[�u�ݒ芮���I%d\n\n", BasisOctave);

	//		int receive;
	//		printf_s("�e�X�g���Ă݂܂����H(0:���Ȃ�, 1:����)->");
	//		do
	//		{
	//			std::cin >> receive;
	//		} while (receive != 0 && receive != 1);	// 0��1�����܂Ń��[�v

	//		if (receive == 1)
	//		{
	//			g_bTest = true;
	//			printf_s("MIDI�e�X�g���c���낢��L�[�{�[�h�����Ă݂ĂˁB�����ĕ������o���琬������\n�I��������������A�X�y�[�X�L�[�������Ă�");

	//			// �X�y�[�X�����܂Ŕ����Ȃ�(�L�[�{�[�h�C�x���g�����̓R�[���o�b�N�ōs���Ă���̂Ŗ��Ȃ�)
	//			while (!(GetKeyState(VK_SPACE) & 0x80));
	//		}

	//		// MIDI�C���v�b�g�I��
	//		midiInStop(m_MIDIInHandle);
	//		midiInReset(m_MIDIInHandle);
	//		midiInClose(m_MIDIInHandle);

	//		// �����e�L�X�g��Output
	//		FILE *fp(nullptr);
	//		fopen_s(&fp, "Keyboard.txt", "w");
	//		//fwrite(&DeviceID, sizeof(UINT), 1, fp);	// ���̂��󔒂������o�����
	//		fprintf_s(fp, "�f�o�C�X�ԍ�:%d\n", DeviceID);
	//		fprintf_s(fp, "�x�[�X�I�N�^�[�u:%d", BasisOctave);
	//		fclose(fp);
	//	}
	//	else
	//	{
	//		char errmsg[MAXERRORLENGTH];
	//		midiInGetErrorText(hr, errmsg, sizeof(errmsg));
	//		printf_s("MIDI�ڑ��Ɏ��s�I\nERROR MESSAGE:%s", errmsg);
	//	}
	//}

	//// �G���^�[�����܂Ŕ����Ȃ�
	//printf_s("\n\n�G���^�[�L�[�ŏI��");
	//while (!(GetKeyState(VK_RETURN) & 0x80));
}