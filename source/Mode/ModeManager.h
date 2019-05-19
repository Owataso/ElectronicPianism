#pragma once

//===============================================
//	�O���錾
//===============================================
enum class DIFFICULTY;
struct SelectMusicData;

//===============================================
//	�萔
//===============================================
enum class PLAY_MODE
{
	STANDARD,	// STANDARD�n
	FREE,		// FREE�n
	GRADE,		// �i�ʔF��n
	MAX
};
static const int NUM_GRADE_MUSIC(3);// �i�ʔF��̋Ȑ�


/********************************************/
//	�i�ʔF���p�̍\����
/********************************************/
struct GradeData
{
	// �I�������i��
	int iSelectGrade;

	// �Ȃ̏��
	SelectMusicData *MusicDatas;

	// �B�����֘A
	int FinesseGauge[NUM_GRADE_MUSIC];	// �������̃Q�[�W
	int ClearRate[NUM_GRADE_MUSIC];		// ����Ȍv�Z���瓱���o�����A�P�ȒB����(�����B�����͂���̕��ς��Ƃ�)
	GradeData();
	~GradeData();
	void Initialize(int SelectGrade, const SelectMusicData *data);
	void ComputeClearRate(int step, int RestGauge, int JudgeCounts[], int NumNorts);
	UINT GetAverageClearRate()
	{
		UINT sum(0);
		FOR(NUM_GRADE_MUSIC) sum += ClearRate[i];
		return sum / NUM_GRADE_MUSIC;
	}
};


/********************************************/
//	�V�[���ړ����̕���p<Singleton>
/********************************************/
class ModeManager
{
public:
	//===============================================
	//	���̎擾
	//===============================================
	static ModeManager *GetInstance(){ static ModeManager i; return &i; }

	//===============================================
	//	�A�N�Z�T
	//===============================================
	void SetMode(PLAY_MODE m)
	{
		m_iStep = 0;
		m_eModeType = m;

		if (m != PLAY_MODE::GRADE) { ZeroMemory(&m_tagGradeData, sizeof(GradeData)); }
	}
	PLAY_MODE GetMode(){ return m_eModeType; }
	int GetStep(){ return m_iStep; }
	void AddStep(){ m_iStep++; }
	GradeData *GetGradeData(){ return &m_tagGradeData; }

	//===============================================
	//	�i�ʔF��
	//===============================================
	void SetSelectGrade(int SelectGrade);
	void SettingGradeMusic();
	void SetGradeClearRate(int RestGauge, int JudgeCounts[], int NumNorts) { m_tagGradeData.ComputeClearRate(m_iStep, RestGauge, JudgeCounts, NumNorts); }

private:
	//===============================================
	//	�����o�ϐ�
	//===============================================
	PLAY_MODE m_eModeType;			// �v���C���[���I�����Ă��郂�[�h
	int m_iStep;					// �X�^���_�[�h�̃X�e�[�W��������A�i�ʂ̉��Ȗڂ�������
	GradeData m_tagGradeData;		// �i�ʗp�f�[�^

	//===============================================
	//	�V���O���g���̍�@
	//===============================================
	ModeManager() :m_eModeType(PLAY_MODE::MAX), m_iStep(0){}
	ModeManager(const ModeManager&){}
	ModeManager &operator=(const ModeManager&){}
};

/********************************************/
//	�C���X�^���X�ȗ���
/********************************************/
#define ModeMgr (ModeManager::GetInstance())