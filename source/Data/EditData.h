#pragma once

#include "../Music/MusicInfo.h"

//enum class EVENT_TYPE
//{
//	NOTE_ON,		// �m�[�g�I��
//	NOTE_OFF,		// �m�[�g�I�t
//	CHANGE_BPM		// BPM�ύX(�ŏ��̐ݒ�ƃ\�t�����Ƃ�)
//};
//
//namespace EditData
//{
//	class Base
//	{
//	protected:
//		unsigned int event_time;
//
//	public:
//		unsigned int GetEventTime(){ return event_time; }
//		void SetEventTime(unsigned int value){ event_time = value; }
//
//		virtual void SetValue(const NoteInfo &val){ assert(0); }
//		virtual void GetValue(NoteInfo *out){ assert(0); }
//		virtual void SetValue(const SoflanInfo &val){ assert(0); }
//		virtual void GetValue(SoflanInfo *out){ assert(0); }
//		virtual void SetValue(int val){ assert(0); }
//		virtual void GetValue(int *out){ assert(0); }
//
//		virtual EVENT_TYPE GetType() = 0;
//	};
//
//	class NoteOn : public Base
//	{
//	private:
//		/* value�̓��[���ԍ� */
//		NoteInfo note;
//	public:
//		void SetValue(const NoteInfo &val)override
//		{
//			memcpy_s(&note, sizeof(NoteInfo), &val, sizeof(NoteInfo));
//		}
//		void GetValue(NoteInfo *out)override{ *out = note; }
//		EVENT_TYPE GetType(){ return EVENT_TYPE::NOTE_ON; }
//	};
//
//	class NoteOff : public Base
//	{
//	private:
//		/* value�̓��[���ԍ� */
//		int lane;
//	public:
//		void SetValue(int val)override{ lane = val; }
//		void GetValue(int *out)override{ *out = lane; }
//		EVENT_TYPE GetType(){ return EVENT_TYPE::NOTE_OFF; }
//	};
//
//	class ChangeBPM : public Base
//	{
//	private:
//		/* value��BPM */
//		SoflanInfo soflan;
//	public:
//		void SetValue(const SoflanInfo &val)override
//		{
//			memcpy_s(&soflan, sizeof(SoflanInfo), &val, sizeof(SoflanInfo));
//		}
//		void GetValue(SoflanInfo *out)override{ *out = soflan; }
//		EVENT_TYPE GetType(){ return EVENT_TYPE::CHANGE_BPM; }
//	};
//}

/********************************************/
//	MIDI�ǂݍ��݃f�[�^�Ǘ�<Singleton>
/********************************************/
class EditDataManager
{
public:
	//===============================================
	//	���̎擾
	//===============================================
	static EditDataManager *GetInstance(){ static EditDataManager instance; return &instance; }

	//===============================================
	//	�������E���
	//===============================================
	void Initialize();
	void Release();

	//===============================================
	//	�f�X�g���N�^
	//===============================================
	MusicInfo *GetMusicInfo(){ return &m_tagMusicInfo; }
	void SetShift(int val)
	{
		m_tagMusicInfo.omsInfo.shift = val;
	}

	//===============================================
	//	�ǂݍ���MIDI�t�@�C�����畈�ʂ��쐬
	//===============================================
	void LoadMIDI_MakeMusicScore(LPCSTR lpcFileName);

	//===============================================
	//	�Ǝ����ʃt�@�C�����畈�ʂ��쐬
	//===============================================
	void LoadOMS_MakeMusicScore(LPCSTR lpcFileName);

	//===============================================
	//	���ʃf�[�^�����o��
	//===============================================
	void WriteMusicScore(LPCSTR lpcFileName);

	// ���X�g�ǉ�
	void PushNote(const NoteInfo &note)
	{
		NoteInfo *set = new NoteInfo;
		set->iEventTime = note.iEventTime;
		set->cLaneNo = note.cLaneNo;
		set->cNoteType = note.cNoteType;
		m_vNoteList.push_back(set);
	}
	void PushSoflan(const SoflanInfo &soflan)
	{
		SoflanInfo *set = new SoflanInfo;
		set->iEventTime = soflan.iEventTime;
		set->fBPM = soflan.fBPM;
		m_vSoflanList.push_back(set);
	}
private:
	// 
	MusicInfo m_tagMusicInfo;

	std::vector<NoteInfo*> m_vNoteList;
	std::vector<SoflanInfo*> m_vSoflanList;

	void Sort();
	void Clear();

	//===============================================
	//	�V���O���g���̍�@
	//===============================================
	EditDataManager(){}
	EditDataManager(const EditDataManager&){}
	EditDataManager &operator =(const EditDataManager&);
};

/********************************************/
//	�C���X�^���X�ȗ���
/********************************************/
#define EditDataMgr EditDataManager::GetInstance()