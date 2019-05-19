#include "tdnlib.h"
#include "../Sound/SoundManager.h"
#include "MusicInfo.h"
#include "Note.h"
#include "PlayCursor.h"
#include "Soflan.h"
#include "../Data/PlayerData.h"
#include "../Judge/Judge.h"

//*****************************************************************************************************************************
//	�������E���
//*****************************************************************************************************************************
//------------------------------------------------------
//	������
//------------------------------------------------------
NoteManager::NoteManager()
{
	// ���X�g�z��̓��I�m��
	m_list = new std::list<PlayNoteInfo*>[NUM_KEYBOARD];
}
void NoteManager::Initialize(SoflanManager *pSoflanManager)
{
	m_pSoflanMgr = pSoflanManager;

	/*
	���\�l�́A�S�����������e�B�b�N����ݒ肵�܂��B�Ⴆ�΁A�^�C���x�[�X��120�Ȃ�W��������60�e�B�b�N�A�O�A�W��������40�e�B�b�N�A�S������480�e�B�b�N�ɂȂ�܂��B
	���\�l�́A�ʏ�120�A196�A240�A480�Ȃǂ��g���܂��B���̒l��ݒ肵�Ă��\���܂��񂪁A�V�[�P���T�ɂ���Ă̓T�|�[�g���Ă��Ȃ��ꍇ������܂��B
	�P�e�B�b�N�̎���(�b) = 60 / �e���| / �^�C���x�[�X

	�Ⴆ�΁A�e���|120�Ń^�C���x�[�X��480�Ȃ��1�e�B�b�N��1 / 960�b�A��P�~���b�ɂȂ�܂��B
	*/
	FOR(NUM_KEYBOARD)
	{
		m_list[i].clear();
	}
}
//------------------------------------------------------
//	���
//------------------------------------------------------
NoteManager::~NoteManager()
{
	Clear();

	// ���X�g�z��̊J��
	delete[] m_list;
}

void NoteManager::Clear()
{
	FOR(NUM_KEYBOARD)
	{
		for (auto it = m_list[i].begin(); it != m_list[i].end();)
		{
			delete (*it);
			it = m_list[i].erase(it);
		}
		m_list[i].clear();
	}
}

//*****************************************************************************************************************************gwq
//		�X�V
//*****************************************************************************************************************************
//------------------------------------------------------
//		�X�V
//------------------------------------------------------
void NoteManager::Update(JudgeManager *judge)
{
	// �œK����r�p
	static bool bOptimisation = true;

	// [O]�L�[�ōœK���؂�ւ�
	if (KeyBoardTRG(KB_O) && KeyBoard(KB_SPACE)) bOptimisation = !bOptimisation;

	// �X�V����
	bOptimisation ? OptimisationedUpdate(judge) : BeforeOptimisationUpdate(judge);
}

void NoteManager::BeforeOptimisationUpdate(JudgeManager *pJudge)
{
	FOR(NUM_KEYBOARD)
	{
		for (auto it = m_list[i].begin(); it != m_list[i].end();)
		{
			(*it)->position = (int)(
				play_cursor->GetMSecond() -
				(*it)->note.iEventTime
				);		// ���݂̍Đ����� - �����̃C�x���g�^�C��

			// �����`�F�b�N
			if ((*it)->erase)
			{
				//InputMIDIMgr->SetTone((*it)->note.tone);
				delete (*it);
				it = m_list[i].erase(it);
			}

			// ������poor
			else if ((*it)->position > 200)
			{
				// ���苭��BAD
				pJudge->LostNote((*it)->note.cLaneNo - m_cBaseOctave);
				delete (*it);
				it = m_list[i].erase(it);
			}
			else
			{
				// �m�[�c���W�v�Z
				it++;
			}
		}
	}
}

void NoteManager::OptimisationedUpdate(JudgeManager *pJudge)
{
	std::list<PlayNoteInfo*>::iterator it;
	FOR(NUM_KEYBOARD)
	{
		it = m_list[i].begin();
		while (it != m_list[i].end())
		{
			// �m�[�c���W�v�Z(���݂̍Đ����� - �����̃C�x���g�^�C��)
			if (((*it)->position = (int)(play_cursor->GetMSecond() - (*it)->note.iEventTime))
				//_/_/_/_/_/_/_/_���X�[�p�[VIRTUOSO���̍œK��/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
				< -5000)
				break;// �������v�Z����K�v�Ȃ�
				//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

			// �����`�F�b�N
			else if ((*it)->erase)
			{
				//InputMIDIMgr->SetTone((*it)->note.tone);
				delete (*it);
				it = m_list[i].erase(it);
			}

			// ������poor
			else if ((*it)->position > 200)
			{
				// ���苭��BAD
				pJudge->LostNote((*it)->note.cLaneNo - m_cBaseOctave);

				// CN�J�n���������ƁACN�I��������
				if ((*it)->note.cNoteType == (int)NOTE_TYPE::CN_START)
				{
					// CN�J�n�m�[�c����
					delete (*it);
					it = m_list[i].erase(it);

					// CN�I���m�[�c����
					// ���苭��BAD
					pJudge->LostNote((*it)->note.cLaneNo - m_cBaseOctave);
					delete (*it);
					it = m_list[i].erase(it);
				}
				else
				{
					// �m�[�c����
					delete (*it);
					it = m_list[i].erase(it);
				}
			}

			else it++;
		}
	}
}

//*****************************************************************************************************************************
//		�Z�b�g�Q�b�g
//*****************************************************************************************************************************
std::list<PlayNoteInfo*> *NoteManager::GetList(int lane)
{
	return &m_list[lane];
}
void NoteManager::GetNoteAbsolutePos(const PlayNoteInfo &note, int *pos_y)
{
	*pos_y = note.position;
}

void NoteManager::GetNoteHiSpeedPos(const PlayNoteInfo &note, int *pos_y)
{
	*pos_y = note.position;

	// BPM�ɂ��e��
	if (!PlayerDataMgr->m_PlayerOption.bRegulSpeed)
		*pos_y = (int)(*pos_y / m_pSoflanMgr->GetTicks()[note.SoflanNo]);
	// �n�C�X�s�[�h�ɂ��e��
	const float speed = PlayerDataMgr->m_PlayerOption.HiSpeed * HI_SPEED_WIDTH;

	//static int type(0);
	//if (KeyBoardTRG(KB_1)) type = 0;
	//if (KeyBoardTRG(KB_2)) type = 1;
	//if (KeyBoardTRG(KB_3)) type = 2;
	//if (KeyBoardTRG(KB_4)) type = 3;

	//switch (type)
	//{
	//case 0:
		*pos_y = (int)(*pos_y * (.1f + (speed / .1f * .01f)));
	//	break;
	//case 1:
	//	*pos_y = (int)(*pos_y * (.1f + (speed / .75f * .1f)));
	//	break;
	//case 2:
	//	*pos_y = (int)(*pos_y * (.1f + (speed / .5f * .05f)));
	//	break;
	//case 3:
	//	*pos_y = (int)(*pos_y * (.1f + (speed / .75f * .1f)));
	//	break;
	//}
}

void NoteManager::Set(MusicInfo *info)
{
	// ��O����(SoflanManager��Set���ɌĂ�ł�������)
	assert(m_pSoflanMgr->GetSoflans());

	// �x�[�X�I�N�^�[�u�ۑ�
	m_cBaseOctave = info->omsInfo.BaseOctave;

	// �����̃o�b�t�@�J��
 	Clear();

	m_iNumNorts = info->omsInfo.NumNotes;

	// �m�[�c�������X�g�ɓ���Ă���
	for (UINT i = 0; i < m_iNumNorts; i++)
	{
		PlayNoteInfo *set = new PlayNoteInfo;
		set->erase = false;
		memcpy_s(&set->note, sizeof(NoteInfo), &info->notes[i], sizeof(NoteInfo));
		set->position = -114514;

		// �\�t�����Ή��p�̃C�x���g�^�C���z��쐬
		//set->EventTimeArray = new UINT[m_info->omsInfo.NumSoflans];
		//memcpy_s(set->EventTimeArray, sizeof(UINT)*m_info->omsInfo.NumSoflans, &set->note->iEventTime, sizeof(UINT)*m_info->omsInfo.NumSoflans);

		//float tick = (60 / m_SoflanMgr->m_soflans[0].fBPM / m_info->omsInfo.division) * 1000;
		//set->EventTimeArray[0] = (int)(note->iEventTime * tick);
		//UINT debug[300];

		for (UINT j = info->omsInfo.NumSoflans - 1; j >= 0; j--)
		{
			if (info->notes[i].iEventTime >= info->soflans[j].iEventTime)	// >�ł͂Ȃ��A>=�ɂ��Ȃ��ƃ\�t�����������
			{
				const float tick((60 / m_pSoflanMgr->GetSoflans()[j].fBPM / info->omsInfo.division) * 1000);
				const int val(info->notes[i].iEventTime - info->soflans[j].iEventTime);
				set->note.iEventTime = (int)(val * tick) + m_pSoflanMgr->GetSoflans()[j].iEventTime;	// �\�t�����̃C�x���g����+��������̎����̎��ԓI�ȉ���
				set->SoflanNo = j;
				break;
			}
		}

		int lane = info->notes[i].cLaneNo - m_cBaseOctave;	// �m�[�c�̃��[��

		MyAssert(lane >= 0 && lane < NUM_KEYBOARD, "�Z�[�u�f�[�^�̃m�[�c��񂪂�������");
		m_list[lane].push_back(set);
	}


	// �ω���̍�(���̂܂ܕ����Ă�����BPM���ς�����ۂ����B)
	//int *shift = new int[m_info->omsInfo.NumSoflans];
	//
	//// �ω��O�̍�(BPM���ς��O�̍���ۑ�)
	//int *shift2 = new int[m_info->omsInfo.NumSoflans];
	//
	//// 0�͎g��Ȃ�
	//shift[0] = shift2[0] = 0;
	//
	//for (UINT i = 1; i < m_info->omsInfo.NumSoflans; i++)
	//{
	//	// �ω���̍��{��
	//	float tick2 = (60 / m_SoflanMgr->m_soflans[i].fBPM / m_info->omsInfo.division) * 1000;
	//
	//	// �ω��O�̍��{��
	//	float tick = (60 / m_SoflanMgr->m_soflans[i - 1].fBPM / m_info->omsInfo.division) * 1000;
	//
	//	// �\�t�����J�n����I���܂ł̍���ۑ�
	//	int val = m_SoflanMgr->m_soflans[i].iEventTime - m_SoflanMgr->m_soflans[i - 1].iEventTime;
	//
	//	// �ς��O�ƕς������̔{�����ꂼ����|����
	//	shift[i] = (int)(val * tick2);
	//	shift2[i] = (int)(val * tick);
	//}
	//for (UINT i = 1; i < m_info->omsInfo.NumSoflans; i++)
	//{
	//	for (auto it : nList)
	//	{
	//		if (it->note->iEventTime <= m_SoflanMgr->m_soflans[i].iEventTime)
	//			continue;
	//
	//		//it->EventTimeArray[i] = it->note->iEventTime;
	//		for (int j = i; j >= 0; j--)
	//		{
	//			it->EventTimeArray[i] = it->EventTimeArray[i] - shift[j] + shift2[j];
	//			if (j == 2)
	//			{
	//				int val = it->EventTimeArray[i] - m_SoflanMgr->m_soflans[j].iEventTime;
	//				float tick2 = (60 / m_SoflanMgr->m_soflans[j].fBPM / m_info->omsInfo.division) * 1000;
	//				it->EventTimeArray[i] = it->EventTimeArray[i] - val + (int)(val * tick2);
	//			}
	//		}
	//	}
	//}
	//
	//delete shift;
	//delete shift2;
}

void NoteManager::GetStartNotesLane(std::vector<int> *pOutLanes)
{
	UINT iMinEventTime(INT_MAX);

	// �S�m�[�c����A�ŏ��ɉ������m�[�c��T��
	FOR(NUM_KEYBOARD)
	{
		// ���̃��[���ɂ͉����~���Ă��Ȃ��ꍇ�̏���
		if (m_list[i].empty()) continue;

		// ���[���̈�ԍŏ��̃m�[�c(�ŏ��̃C�x���g���Ԃ̌v�������ł����̂ŁA2�ڈȍ~�̃��[���̃m�[�c�͂���Ȃ�)
		auto LaneFirstNote = m_list[i].begin();

		const UINT iNoteEventTime((*LaneFirstNote)->note.iEventTime);
		// �ŒZ���Ԃ��Z��������
		if (iNoteEventTime < iMinEventTime)
		{
			// �m�[�c�J�n�ŒZ���ԍX�V
			iMinEventTime = iNoteEventTime;

			pOutLanes->clear();
			pOutLanes->push_back(i);
		}

		// ���������ɂ��Ή�
		else if (iNoteEventTime == iMinEventTime) pOutLanes->push_back(i);
	}
}