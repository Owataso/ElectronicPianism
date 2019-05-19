#include	"tdnlib.h"
#include	"OMS_Loader.h"
#include	"../Music/MusicInfo.h"
#include	"PlayCursor.h"
//*****************************************************************************************************************************
//	OMS�t�@�C���ǂݍ���
//*****************************************************************************************************************************
int OMS_Loader::GetOctaveWidth(char *filename)
{
	// �ǂݏo������
	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs) return 0;	// �I�[�v���Ɏ��s

	/* �w�b�_�`�����N�N�� */
	char work[5];
	ifs.read(work, 4);										// �w�b�_�`�����N�ł��邱�Ƃ�����"OMhd"�Ƃ���4�����̃A�X�L�[�R�[�h
	assert(
		work[0] == 'O'&&
		work[1] == 'M'&&
		work[2] == 'h'&&
		work[3] == 'd'
		);

	// ���ʃf�[�^�̏�񂾂���ǂݍ���(�m�[�c�Ƃ��͓ǂݍ��܂Ȃ�)
	OMSInfoV2 info;
	ifs.read((char*)&info, sizeof(OMSInfoV2));

	// ���̕��ʂ̃I�N�^�[�u����Ԃ�
	return info.OctaveWidth;
}


bool OMS_Loader::LoadOMS(LPCSTR filename, MusicInfo *out, OMS_VERSION version)
{
	// �ǂݏo������
	std::ifstream ifs(filename, std::ios::binary);
	//MyAssert(ifs,"OMS�t�@�C���I�[�v���Ɏ��s");
	if (!ifs) return false;	// �I�[�v���Ɏ��s

	/* �w�b�_�`�����N�N�� */
	char work[5];
	ifs.read(work, 4);										// �w�b�_�`�����N�ł��邱�Ƃ�����"OMhd"�Ƃ���4�����̃A�X�L�[�R�[�h
	assert(
		work[0] == 'O'&&
		work[1] == 'M'&&
		work[2] == 'h'&&
		work[3] == 'd'
		);

	//infs.read((char*)out->music_name, sizeof(char[64]));	// �Ȗ�
	//infs.read((char*)&out->shift, 4);						// �Ȃƕ��ʂƂ̍��𒲐�����
	//infs.read((char*)&out->division, 4);					// ���\�l
	//infs.read((char*)&out->MaxBPM, sizeof(float));			// �ő�BPM
	//infs.read((char*)&out->MinBPM, sizeof(float));			// �ŏ�BPM
	//infs.read((char*)&out->num_notes, 4);					// �m�[�g��
	//infs.read((char*)&out->num_soflans, 4);					// �\�t������

	// ���r�b�g�t�B�[���h�\���̂ł����؂�ɓǂݏo��
	if (version == OMS_VERSION::V1)
	{
		OMSInfo info;
		ifs.read((char*)&info, sizeof(OMSInfo));
		out->omsInfo.BaseOctave = info.BaseOctave;
		out->omsInfo.division = info.division;
		out->omsInfo.NumNotes = info.NumNotes;
		out->omsInfo.NumSoflans = info.NumSoflans;
		out->omsInfo.OctaveWidth = 1;
		out->omsInfo.Other1 = 0;
		out->omsInfo.Other2 = 0;
		out->omsInfo.Other3 = 0;
		out->omsInfo.PlayMusicNo = info.PlayMusicNo;
		out->omsInfo.PlayTime = info.PlayTime;
		out->omsInfo.shift = info.shift;
	}
	else if (version == OMS_VERSION::V2)
	{
		ifs.read((char*)&out->omsInfo, sizeof(OMSInfoV2));
	}

	/* �f�[�^�`�����N�N�� */
	ifs.read(work, 4);										// �f�[�^�`�����N�ł��邱�Ƃ�����"OMdt"�Ƃ���4�����̃A�X�L�[�R�[�h
	assert(
		work[0] == 'O'&&
		work[1] == 'M'&&
		work[2] == 'd'&&
		work[3] == 't'
		);

	// �܂��m�[�g
	out->notes = new NoteInfo[out->omsInfo.NumNotes];
	for (UINT i = 0; i < out->omsInfo.NumNotes; i++)
	{
		ifs.read((char*)&out->notes[i].iEventTime, 4);		// �C�x���g����
		ifs.read((char*)&out->notes[i].cLaneNo, 1);			// ���[���ԍ�
		ifs.read((char*)&out->notes[i].cNoteType, 1);		// CN�t���O
		//MyAssert(out->notes[i].lane >= 0 && out->notes[i].lane < NUM_KEYBOARD, "�Z�[�u�f�[�^�̃m�[�c��񂪂�������");

		//tdnDebug::OutPutLog("%d\n", out->notes[i].iEventTime);
	}

	// BPM�ύX���
	out->soflans = new SoflanInfo[out->omsInfo.NumSoflans];
	for (UINT i = 0; i < out->omsInfo.NumSoflans; i++)
	{
		ifs.read((char*)&out->soflans[i].iEventTime, 4);			// �C�x���g����
		ifs.read((char*)&out->soflans[i].fBPM, sizeof(float));		// �\�t����BPM
	}

	return true;

	// �Ō��wav�f�[�^
	//unsigned long wav_size;
	//WAVEFORMATEX wfx;
	//infs.read((char*)&wav_size, sizeof(long));				// wav�f�[�^�̗�
	//infs.read((char*)&wfx, sizeof(WAVEFORMATEX));			// wav�t�H�[�}�b�g
	//
	//unsigned char *buf = new unsigned char[wav_size];

	// �����؂��wav�f�[�^�ǂݍ��ނ̂͊�Ȃ��������̂ŕ����ēǂݍ���
	//int remain = wav_size; // �ǂݍ��ނׂ��c��̃o�C�g��
	//while (remain > 0)
	//{
	//	// 1024Bytes����������(�������p���N�����Ȃ��悤��)
	//	int readSize = (1024 < remain) ? 1024 : remain;
	//	infs.read((char*)buf, sizeof(unsigned char)* readSize);
	//	remain -= readSize;
	//}
	//infs.read((char*)buf, wav_size);

	// �����f�[�^�ݒ�
	//play_cursor->SetMusic(buf, wav_size, &wfx);
}