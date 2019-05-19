#pragma once

#include "TDNLIB.h"

/********************************************/
//	�萔��\���̂Ȃǂ̏��̑q��
/********************************************/


/********************************************/
//	�萔
/********************************************/
static const int MAX_MUSIC = 127;	// �Ȃ̍ő吔
static const int MAX_OCTAVE = 4;
static const int JUDGE_LINE = 618;	// ����Y���W
static const int NUM_KEYBOARD = 12 * MAX_OCTAVE;
static const float HI_SPEED_WIDTH = .5f;	// �n�C�X�s�̍��ݕ�
static const int NOTE_WIDTH = 40;		// �m�[�c�̕�
static const int KEYLANE_LEFT = 0;		// �L�[���C����
static const int KEYLANE_TOP = 116;		// �L�[���C����
static const int KEYLANE_HEIGHT = 497;	// �����Ă����̏㉺��
static const int MAX_SCORE = 100000;	// �X�R�A�̍ő吔


/********************************************/
//	�m�[�c���W�v�Z(��)
/********************************************/
#define NOTE_POS_COMPTE(key) (key*(NOTE_WIDTH / 2) + ((key % 12 >= 5) ? (NOTE_WIDTH / 2) : 0) + key / 12 * NOTE_WIDTH)

//------------------------------------------------------
//	�X�R�A
//------------------------------------------------------
enum class SCORE_RANK
{
	AAA, AA, A, B, C, D, E, F, MAX
};
static const int SCORE_BORDER[(int)SCORE_RANK::MAX] =
{
	90000,	// AAA
	80000,	// AA
	70000,	// A
	60000,	// B
	50000,	// C
	40000,	// D
	30000,	// E
	// F�͂���ȉ�
};
SCORE_RANK ScoreRankCheck(int score);// �����̃X�R�A����A���̃X�R�A�̃����N��f���o��


//------------------------------------------------------
//	��Փx
//------------------------------------------------------
enum class DIFFICULTY
{
	NORMAL,		// �܂�������
	HYPER,		// ���\������������
	EXPERT,		// �߂�����ނ���
	VIRTUOSO,	// ����Z�I(�N���A�����痧�h�ȃs�A�j�X�g�ł�)
	MAX
};
static const DWORD DifficultyColor[(int)DIFFICULTY::MAX] =
{
	0xff10ff10,	// ��
	0xffffff10,	// ��
	0xffff1010,	// ��
	0xffca10ca	// ��
};

//------------------------------------------------------
//	�N���A�����v
//------------------------------------------------------
enum class CLEAR_LAMP
{
	NO_PLAY,	// ���v���C
	FAILED,		// ���N���A
	EASY,		// �C�[�W�[
	CLEAR,		// �m�}�Q
	HARD,		// ��
	EX_HARD,	// EX��
	FULL_COMBO,	// �t����
	MAX
};


//------------------------------------------------------
//	�Q�[�W�I�v�V����
//------------------------------------------------------
enum class GAUGE_OPTION
{
	NORMAL,
	EASY,
	HARD,
	EX_HARD,
	HAZARD,
	GRADE,
	MAX
};

//------------------------------------------------------
//	����
//------------------------------------------------------
enum class JUDGE_TYPE
{
	SUPER,	// �ō�
	GREAT,	// �ǂ�
	GOOD,	// �ӂ�
	BAD,	// ����
	MAX
};
enum class JUDGE_TIMING
{
	JUST,
	FAST,
	SLOW,
	MAX
};


//------------------------------------------------------
//	�W������
//------------------------------------------------------
enum class MUSIC_GENRE
{
	ORIGINAL,	// �I���W�i��
	GAME,		// �Q�[�����y
	CLASSIC,	// �N���V�b�N
	VOCALOID,	// �{�J��
	POPS,		// �A�j���Ƃ��Ō��̗�
	OTHER,		// ���̑�
	MAX
};


//------------------------------------------------------
//	�m�[�c
//------------------------------------------------------
enum class NOTE_TYPE
{
	NORMAL,		// CN�Ȃ�(�ʏ�m�[�c)
	CN_START,	// CN�J�n
	CN_END,		// CN�I��
};
struct NoteInfo
{
	UINT iEventTime;		// �C�x���g����
	char cLaneNo;			// ���[���ԍ�
	char cNoteType;			// �m�[�c�^�C�v���i�[(���enum���i�[����)	// char�^�ɂ����̂́A�O�̎g���Ă����(tone)���g���܂킹�邩��Ƃ����������
};

// ���x�ω�
struct SoflanInfo
{
	UINT iEventTime;
	float fBPM;
};

// �t�H���_�[�̏����(�G�f�B�b�g�p�ƒʏ�p�̕ʂŕ�����)
class FolderBase
{
public:
	std::string sMusicName;				// �Ȗ�(�t�H���_�[�̖��O�ł�����)
	int level[(int)DIFFICULTY::MAX];	// ���x��

	FolderBase() :sMusicName(""){}
	virtual ~FolderBase(){}
};


class FolderInfo : public FolderBase
{
public:
	bool bNameEnglish;								// �Ȗ����p�ꂩ�ǂ���
	bool bMovieExist;								// ���悪���邩�ǂ���
	int iOctaveWidths[(int)DIFFICULTY::MAX];			// �I�N�^�[�u��
	std::string sSortMusicName;						// �\�[�g�p�̃W��������
	float fMinBPM, fMaxBPM;	// �I�ȏ���BPM100~200�Ƃ��̃\�t�����Ŏg��

	BYTE byID;				// ID(0�`255)
	std::string sArtistName;	// ��ȎҖ�
	MUSIC_GENRE eMusicGenre;	// �W������

	FolderInfo() :FolderBase(), sArtistName(""), bNameEnglish(true){}
};

// �G�f�B�b�g�p�̃t�H���_�[�f�[�^
class EditFolderInfo : public FolderBase
{
public:
	bool bMusicExistance[(int)DIFFICULTY::MAX];			// �Ȃ������Ă邩(��{�I��0�Ԃ����̗��p�ƂȂ邪�AAnother�ŋȕς��Ƃ���肽���̂œ�Փx�Ԃ�ێ�����)�����{���Ɋ�{�I�ɂ�0�ɂ���true������
	bool bAudition;										// �����p��wav�f�[�^�������Ă邩
	bool bDifficultyExistance[(int)DIFFICULTY::MAX];	// ��Փx�����݂��Ă��邩

	EditFolderInfo(){
		assert(0);// �f�t�H���g�R���X�g���N�^�͌Ăяo���Ȃ���
	}
	EditFolderInfo(LPCSTR FileName);
};

// �t�@�C���ǂݏ����p���킽�����ʍ\����(�r�b�g�t�B�[���h)
struct OMSInfo
{
	//unsigned int level		 : 5;	// �Ȃ̃��x�� 5�r�b�g(31)
	unsigned int PlayMusicNo : 3;	// �t�H���_�̒��̂ǂ̔ԍ��̋Ȃ��Đ����邩(1�`4�̒l�A0�Ȃ�ȂȂ�) 3�r�b�g(7)
	unsigned int PlayTime	 : 10;	// �Đ�����(�b�P�ʁA�Ȃ�����ꍇ�s�v) 10�r�b�g(1023) 17��5�b
	unsigned int NumNotes	 : 14;	// �m�[�g�� 14�r�b�g(16383)
	unsigned int NumSoflans	 : 10;	// �\�t������ 10�r�b�g(1023)
	signed int shift		 : 16;	// �Ȃƕ��ʂ̍��𒲐�����p ��������16�r�b�g(-32768 �` 32767)
	unsigned int division	 : 16;	// ���\�l 16�r�b�g(65535)
	unsigned int BaseOctave	 : 7;	// �J�n�I�N�^�[�u 7�r�b�g(127)
};

struct OMSInfoV2
{
	unsigned int PlayMusicNo : 3;	// �t�H���_�̒��̂ǂ̔ԍ��̋Ȃ��Đ����邩(1�`4�̒l�A0�Ȃ�ȂȂ�) 3�r�b�g(7)
	unsigned int PlayTime : 10;	// �Đ�����(�b�P�ʁA�Ȃ�����ꍇ�s�v) 10�r�b�g(1023) 17��5�b
	unsigned int NumNotes : 14;	// �m�[�g�� 14�r�b�g(16383)
	unsigned int NumSoflans : 10;	// �\�t������ 10�r�b�g(1023)
	signed int shift : 16;	// �Ȃƕ��ʂ̍��𒲐�����p ��������16�r�b�g(-32768 �` 32767)
	unsigned int division : 16;	// ���\�l 16�r�b�g(65535)
	unsigned int BaseOctave : 7;	// �J�n�I�N�^�[�u 7�r�b�g(127)
	unsigned int OctaveWidth : 3;	// �I�N�^�[�u��(1�`4) 3�r�b�g(7)
	unsigned int Other1 : 16;		// ����̒ǉ���z�肵���󂫘g
	unsigned int Other2 : 16;
	unsigned int Other3 : 16;
};

struct OMSInfoV3
{
	BYTE byPlayMusicNo;	// �t�H���_�̒��̂ǂ̔ԍ��̋Ȃ��Đ����邩
	WORD wNumNotes;		// �m�[�c��
	WORD wNumSoflans;	// �\�t������
	WORD wDivision;		// ���\�l
	int other1;			// ����̒ǉ���z�肵���󂫘g
	int other2;
	int other3;
};

// �Ȃ̏��
struct MusicInfo
{
	bool bMovieExist;			// ���悪���邩�ǂ���
	DIFFICULTY eDifficulty;
	BYTE byID;						// ID(0�`255)�I�ȉ�ʂ���ړ����ɒl��ێ���������������

	OMSInfoV2 omsInfo;

	NoteInfo *notes;				// �m�[�c�C�x���g(�ϒ�)
	SoflanInfo *soflans;			// BPM�ω��C�x���g(�ϒ�)
};