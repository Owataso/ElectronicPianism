#pragma once

//===============================================
//	�O���錾
//===============================================
class tdnSoundBase;
class tdnSoundBGM;


/********************************************/
//	�T�E���h�̍Đ��ʒu�Ǘ�
/********************************************/
class PlayCursor
{
public:
	//===============================================
	//	���̎擾
	//===============================================
	static PlayCursor* GetInstance() { static PlayCursor instance; return &instance; }

	//===============================================
	//	�������E���
	//===============================================
	void Initialize();
	void Release();

	//===============================================
	//	�T�E���h����
	//===============================================
	void SetMusic(LPSTR filename);
	void Play(int Mlimit = 0);	// �~���b�P�ʂł̍Đ����Ԃ�ݒ�
	void Stop();
	void Pause();
	void Resume();

	//===============================================
	//	�A�N�Z�T
	//===============================================
	int GetMSecond();	// �~���b
	void SetShift(int val){ m_iShift = val; }
	void SetSpeed(float speed){ m_pSoundPlayer->SetSpeed(0, speed); }
	bool isEnd();

private:
	//===============================================
	//	�����o�ϐ�
	//===============================================
	tdnSoundBGM *m_pSoundPlayer;	// �ȗ����p
	int m_iDelta;					// �f���^�^�C��
	int m_iStart;					// �v���J�n����
	int m_iMSecond;					// �o�ߎ���<�b/1000>
	int m_iLimit;					// �ݒ莞��
	int m_iShift;					// �␳����
	bool m_bSoundPlay;				// �T�E���h�Đ��t���O

	//===============================================
	//	�V���O���g���̍�@
	//===============================================
	PlayCursor();
	PlayCursor(const PlayCursor&){}
	PlayCursor operator=(const PlayCursor&){}
};

/********************************************/
//	�C���X�^���X�ȗ���
/********************************************/
#define play_cursor (PlayCursor::GetInstance())