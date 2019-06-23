#include "tdnlib.h"
#include "../Music/MusicInfo.h"
#include "Player.h"
#include "../UI/ui.h"
#include "../Input/InputMIDI.h"
#include "../Music/Note.h"
#include "../Judge/Judge.h"
#include "../Data/PlayerData.h"

//*****************************************************************************************************************************
//	初期化・解放
//*****************************************************************************************************************************
//------------------------------------------------------
//	初期化
//------------------------------------------------------
Player::Player() :m_pInput(nullptr), m_pUI(nullptr), m_byKeyList(nullptr), m_bON(new bool[NUM_KEYBOARD])
{
	// MIDI音フラグON
	InputMIDIMgr->SetSoundOn(PlayerDataMgr->m_PlayerOption.bMIDISoundOn);
}

void Player::Initialize(int iOctaveWidth, UI *Ui, NoteManager *mng_n, JudgeManager *mng_j, MODE mode)
{
#ifdef MIDI_INPUT_ON
#else
	LoadKeyCode(iOctaveWidth);
#endif
	m_pUI = Ui;
	m_pNoteMgr = mng_n;
	m_pJudgeMgr = mng_j;

	if (mode == MODE::YOU) m_pInput = new Input::You(this);
	else if (mode == MODE::AUTO) m_pInput = new Input::DJ_AUTO(this);
}

void Player::LoadKeyCode(int iOctaveWidth)
{
	BYTE buf[NUM_KEYBOARD][5];	// とりあえず4オクターブ分
	FOR(NUM_KEYBOARD) buf[i][0] = '\0';

	int iOffset;
	int iEnd;
	std::fstream infs;

	switch (iOctaveWidth)
	{
	case 1:
		infs.open("DATA/Input/keyboard_data1.txt");
		iOffset = 12;
		iEnd = iOffset + 12;
		break;
	case 2:
		infs.open("DATA/Input/keyboard_data2.txt");
		iOffset = 12;
		iEnd = iOffset + 24;
		break;
	default:
		infs.open("DATA/Input/keyboard_data4.txt");
		iOffset = 0;
		iEnd = iOffset + NUM_KEYBOARD;
		break;
	}

	for (int i = iOffset; i < iEnd; i++)
	{
		// キーコード読み込み
		infs >> buf[i];
	}

	m_byKeyList = new BYTE[NUM_KEYBOARD];
	
	// ラムダ式("0xff"→255)
	auto Sixteen_to_Ten = [](BYTE x[]){
		BYTE ret(0);

		for (int i = 3; i >= 2; i--)
		{
			int trans = 0;
			if ('0' <= x[i] && '9' >= x[i]) trans = (x[i] - 0x30);//0x30は'0'の文字コード
			else if ('A' <= x[i] && 'F' >= x[i]) trans = (x[i] + 0x0A - 0x41);//0x41は'A'の文字コード
			else if ('a' <= x[i] && 'f' >= x[i]) trans = (x[i] + 0x0A - 0x61);//0x61は'a'の文字コード
			ret += trans*(unsigned int)pow(16, 3-i);
		}
		return ret;
	};

	for (int i = 0; i < NUM_KEYBOARD; i++)
	{
		m_bON[i] = false;
		m_byKeyList[i] = (buf[i][0] != '\0') ? Sixteen_to_Ten(buf[i]) : 0;
	}

	infs.close();
}


//------------------------------------------------------
//	解放
//------------------------------------------------------
Player::~Player()
{
	// MIDI音フラグOFF
	InputMIDIMgr->SetSoundOn(false);

	delete m_pInput;
	delete[] m_bON;
	if(m_byKeyList) delete[] m_byKeyList;
}

//*****************************************************************************************************************************
//		更新
//*****************************************************************************************************************************
//------------------------------------------------------
//		更新
//------------------------------------------------------
void Player::Update()
{
	// オプション
	ControlHiSpeed();
	ControlSudden();

	m_pInput->Update();
}

void Player::Input::You::Update()
{
#ifdef MIDI_INPUT_ON
	bool InputList[NUM_KEYBOARD];
	memcpy_s(InputList, sizeof(bool)*NUM_KEYBOARD, InputMIDIMgr->m_InputList, sizeof(bool)*NUM_KEYBOARD);

	// UIにビーム情報セット(MIDIインプットの方はシングルトンで常にコールバック状態なので、うけとって渡す必要がある)
	me->m_pUI->SetBeam(InputList);

	FOR(NUM_KEYBOARD)
	{
		// 押した瞬間に判定をとる
		if(!me->m_bON[i] && InputList[i]) me->m_pJudgeMgr->JudgeNote(i);

		// 離した瞬間の判定をとる(CNで使う)
		if(me->m_bON[i] && !InputList[i]) me->m_pJudgeMgr->JudgeUpKey(i);

		// 押した情報更新
		me->m_bON[i] = InputList[i];
	}
#else

	static bool PrevON[NUM_KEYBOARD] = { false };
	FOR(NUM_KEYBOARD)
	{
		if (!me->m_byKeyList[i]) continue;

		int on_frame = KeyBoard(me->m_byKeyList[i]);
		if (on_frame >= 1)
		{
			if (on_frame == 1)
			{
				me->m_pJudgeMgr->JudgeNote(i);
				PrevON[i] = true;
				me->m_pUI->SetBeam(i, true);
				InputMIDIMgr->SetON(true, i);
			}
		}
		else if (on_frame == 0)
		{
			if (PrevON[i]) me->m_pJudgeMgr->JudgeUpKey(i);	// 離した瞬間
			me->m_pUI->SetBeam(i, false);
			InputMIDIMgr->SetON(false, i);
			PrevON[i] = false;
		}
	}
#endif
}

void Player::Input::DJ_AUTO::Update()
{
	bool flag[NUM_KEYBOARD];
	memset(flag, false, sizeof(flag));

	FOR(NUM_KEYBOARD)
	{
		// ノーツリストループ
		for (auto &it : *me->m_pNoteMgr->GetList(i))
		{
			int pos_x = i, pos_y;
			me->m_pNoteMgr->GetNoteAbsolutePos((*it), &pos_y);
			if (!flag[pos_x]) if (pos_y < 32 && pos_y > 0)
			{
				if (it->note.cNoteType == (int)NOTE_TYPE::CN_END)
				{
					me->m_pJudgeMgr->JudgeUpKey(pos_x);
				}
				else
				{
					flag[pos_x] = true;
					me->m_pJudgeMgr->JudgeNote(pos_x);
				}
				break;
			}
		}

		me->m_pUI->SetBeam(i, flag[i]);
		InputMIDIMgr->SetON(flag[i], i, true);
	}
}


//------------------------------------------------------
//		サドプラ
//------------------------------------------------------
void Player::ControlSudden()
{
	static bool first_on = false;
	static int limit_time = 20;

	const int SpaceKeyFrame = KeyBoard(KB_SPACE);

	// サドゥンONのとき
	if (PlayerDataMgr->m_PlayerOption.bSudden){

		if (SpaceKeyFrame){
			if (KeyBoard(KB_DOWN)){
				if (PlayerDataMgr->m_PlayerOption.SuddenPos < 420)
					PlayerDataMgr->m_PlayerOption.SuddenPos += 4;
			}
			else if (KeyBoard(KB_UP)){
				if (PlayerDataMgr->m_PlayerOption.SuddenPos > 0)
					PlayerDataMgr->m_PlayerOption.SuddenPos -= 4;
			}

			// 押された瞬間
			if (SpaceKeyFrame == 1)
			{
				// 1回目の押した瞬間
				if (first_on == false)
					first_on = true;

				// 2連打
				else
					PlayerDataMgr->m_PlayerOption.bSudden = FALSE;
			}
		}

		// 2連打の猶予計算
		if (first_on){
			if (limit_time-- <= 0){		// 時間切れ
				limit_time = 20;
				first_on = false;
			}
		}
	}

	// OFFのとき(2回高速押しでONにする)
	else{
		// 押した瞬間
		if (SpaceKeyFrame == 1){

			// 1回目の押した瞬間
			if (first_on == false)
				first_on = true;

			else
				PlayerDataMgr->m_PlayerOption.bSudden = TRUE;
		}

		if (first_on){
			if (limit_time-- <= 0){		// 時間切れ
				limit_time = 20;
				first_on = false;
			}
		}
	}
}

//------------------------------------------------------
//		ハイスピ
//------------------------------------------------------
void Player::ControlHiSpeed()
{
	// スペースキー押してなかったら帰れ！！
	if (!KeyBoard(KB_SPACE)) return;

	// ハイスピうｐ
	if (KeyBoardTRG(KB_RIGHT))
		PlayerDataMgr->m_PlayerOption.HiSpeed = min(10, PlayerDataMgr->m_PlayerOption.HiSpeed + 1);

	// ハイスピどwん
	else if (KeyBoardTRG(KB_LEFT))
		PlayerDataMgr->m_PlayerOption.HiSpeed = (PlayerDataMgr->m_PlayerOption.HiSpeed > 0) ? PlayerDataMgr->m_PlayerOption.HiSpeed - 1: 0;
}


//*****************************************************************************************************************************
//		set&get
//*****************************************************************************************************************************