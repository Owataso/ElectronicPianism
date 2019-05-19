#pragma once

//===============================================
//	�O���錾
//===============================================
struct NoteInfo;
struct SoflanInfo;
//struct MusicInfo;
class SoflanManager;
class JudgeManager;


/********************************************/
//	����Ă���m�[�c�̏��
/********************************************/
struct PlayNoteInfo
{
	//UINT *EventTimeArray;	// ���\�t��������Ƃ����̂ŁA�\�t�������̃C�x���g�^�C��������
	NoteInfo note;				// �m�[�c����
	int position;			// �ʒu
	bool erase;				// ��ڂ��I������
	int SoflanNo;			// �������ǂ̃\�t�����̈ʒu��

	PlayNoteInfo(){}
	~PlayNoteInfo(){}
};


/********************************************/
//	�m�[�c�Ǘ��N���X
/********************************************/
class NoteManager
{
public:
	//===============================================
	//	�R���X�g���N�^�E�f�X�g���N�^
	//===============================================
	NoteManager();
	~NoteManager();

	//===============================================
	//	�������E���
	//===============================================
	void Initialize(SoflanManager *mng);
	void Release();

	//===============================================
	//	�X�V
	//===============================================
	void Update(JudgeManager *judge);


	//===============================================
	//	�A�N�Z�T
	//===============================================
	std::list<PlayNoteInfo*> *GetList(int lane);	// ���X�g�擾
	UINT GetNumNorts(){ return m_iNumNorts; }
	void GetStartNotesLane(std::vector<int> *pOutLanes);
	void GetNoteAbsolutePos(const PlayNoteInfo &note, int *pos_y);	// ��ΓI�ȃm�[�c���W(��ɔ���Ŏg�p)
	void GetNoteHiSpeedPos(const PlayNoteInfo &note, int *pos_y);	// BPM�Ƃ��A�n�C�X�s�Ɉˑ������m�[�c�̍��W(���UI�Ŏg�p)
	void Set(MusicInfo *info);
	
private:
	//===============================================
	//	�����o�ϐ�
	//===============================================
	SoflanManager *m_pSoflanMgr;		// �\�t�����Ǘ����w������
	UINT m_iNumNorts;					// �m�[�c��
	char m_cBaseOctave;					// ��{�I�N�^�[�u
	std::list<PlayNoteInfo*> *m_list;	// �m�[�c���X�g

	//===============================================
	//	�����o�֐�
	//===============================================
	void Clear();	// �m�[�c�N���A
	void SetTick(int second, float bpm);
	// �œK���O�ƍœK����ŕ������m�[�c�X�V
	void BeforeOptimisationUpdate(JudgeManager *pJudge);
	void OptimisationedUpdate(JudgeManager *pJudge);
};