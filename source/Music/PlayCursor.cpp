#include	"TDNLIB.h"
#include	"PlayCursor.h"
#include "../Sound/SoundManager.h"
#include <time.h>

//*****************************************************************************************************************************
//	�������E���
//*****************************************************************************************************************************
//------------------------------------------------------
//	������
//------------------------------------------------------
PlayCursor::PlayCursor() :m_pSoundPlayer(nullptr), m_bSoundPlay(false){}
void PlayCursor::Initialize(){}
//------------------------------------------------------
//	���
//------------------------------------------------------
void PlayCursor::Release()
{
	SAFE_DELETE(m_pSoundPlayer);
}


//*****************************************************************************************************************************
//		�T�E���h�A�N�Z�X
//*****************************************************************************************************************************
//------------------------------------------------------
//		�����Z�b�g
//------------------------------------------------------
void PlayCursor::SetMusic(LPSTR filename)
{
	// null����Ȃ�������Ȑݒ�Anull�Ȃ�ȂȂ�
	if (filename)
	{
		if(!m_pSoundPlayer)m_pSoundPlayer = new tdnSoundBGM;
		m_pSoundPlayer->Set(0, filename);
	}

	m_bSoundPlay = (filename) ? true : false;
}
//void PlayCursor::SetMusic(LPBYTE data, DWORD size, LPWAVEFORMATEX wfx)
//{
//	if (!m_player) m_player = new tdnSoundBGM;
//	m_player->Set(0, data, size, wfx);
//}

//------------------------------------------------------
//		�Đ��E��~
//------------------------------------------------------
void PlayCursor::Play(int Mlimit)
{
	// �Ȃ������Ă�Ȃ�A������Đ�
	if (m_bSoundPlay)
		m_pSoundPlayer->Play(0, false);

	// �����łȂ��Ȃ�clock()�Ń^�C���v��
	else
	{
		// �Đ����Ԃ��ݒ肳��Ă��Ȃ�(�f�t�H���g������0�ɂȂ��Ă�)
		if (Mlimit == 0) MessageBox(0, "������񂪂Ȃ��̂ɍĐ����Ԃ��ݒ肳��Ă��Ȃ�", "�G���[", MB_OK);
		m_iLimit = Mlimit;
		m_iStart = clock();
	}
	m_iMSecond = m_iDelta= 0;
}
void PlayCursor::Stop()
{
	if (m_bSoundPlay)
	{
		m_pSoundPlayer->Stop(0);
	}
	else
	{
		m_iMSecond = m_iStart = 0;
	}
}
//------------------------------------------------------
//		���ݍĐ��ʒu�擾
//------------------------------------------------------
int PlayCursor::GetMSecond()
{
	// ���Ԋi�[�ϐ�
	int val;

	// �Ȃ��Đ����Ă���AdirectSound�̍Đ����W�Ƃ��Ă���֐�
	if (m_bSoundPlay)
	{
		//val = m_pStreaming->GetPlayCursor();
		//
		//if (val >= m_delta)
		//{
		//	m_delta = val;
		//	val += m_MSecond;
		//}
		//else
		//{
		//	m_MSecond += tdnStreamSound::STRSECOND * 1000;
		//	m_delta = val;
		//}
		//
		//val += m_MSecond;

		val = m_pSoundPlayer->GetPlayMSecond(0);
	}

	// ��������Ȃ�������v�������Ԃƍ��̎��Ԃ̍���
	else
	{
		m_iMSecond = clock() - m_iStart;
		val = m_iMSecond;
	}

	// ���ۂ̎��Ԃɕ␳�l�����Z�����̂�Ԃ�
	return val + m_iShift;
}


//------------------------------------------------------
//		�I��������ǂ���
//------------------------------------------------------
bool PlayCursor::isEnd()
{
	if (m_bSoundPlay)return !m_pSoundPlayer->isPlay(0);	// �Ȃ��Đ����Ă邩�ǂ���
	else
	{
		return (clock() - m_iStart >= m_iLimit);	// �ݒ肵���������ԂɒB������
	}
}