#pragma once

// DirevtShow�Ɋւ�����N���X��All.h�I�ȃw�b�_�[
#include "DirectShow/baseclasses/streams.h"

// �f�o�b�O�ƃ����[�X�Ŏg�����C�u�����𕪂��Ă���
#ifdef _DEBUG
#ifdef _V120_
#pragma comment( lib, "DirectShow/v120/strmbasd.lib" )
#endif
#ifdef _V140_
#pragma comment( lib, "DirectShow/v140/strmbasd.lib" )
#endif
#else
#ifdef _V120_
#pragma comment( lib, "DirectShow/v120/strmbase.lib" )
#endif
#ifdef _V140_
#pragma comment( lib, "DirectShow/v140/strmbase.lib" )
#endif
#endif

// Define GUID for Texture Renderer
struct __declspec(uuid("{71771540-2017-11cf-ae26-0020afd79767}")) CLSID_TextureRenderer;

/**
*@brief		�e�N�X�`���ɓ�����������ލ�Ƃ����ۂɍs�����߂̃N���X�B���������ɗ��p�������
*@author	Yamagoe
*/
class TextureRenderer : public CBaseVideoRenderer
{
	IDirect3DDevice9	*m_pd3dDevice;
	Texture2D	*m_pTexture;

	D3DFORMAT		m_TextureFormat;

	BOOL	m_bUseDynamicTextures;	//! @param �_�C�i�~�b�N�e�N�X�`�����g�����ǂ����ǂ����̃t���O

	LONG	m_lVidWidth;	//! @param �r�f�I�̕�
	LONG	m_lVidHeight;	//! @param �r�f�I�̍���
	LONG	m_lVidPitch;	//! @param �r�f�I�̃s�b�`

public:
	TextureRenderer(LPUNKNOWN pUnk, HRESULT *phr);
	~TextureRenderer();

	void SetDevice(IDirect3DDevice9 * pd3dDevice) { m_pd3dDevice = pd3dDevice; };

	HRESULT CheckMediaType(const CMediaType *pmt);     // Format acceptable?
	HRESULT SetMediaType(const CMediaType *pmt);       // Video format notification
	HRESULT DoRenderSample(IMediaSample *pMediaSample); // New video sample

	Texture2D *GetTexture() { return m_pTexture; };

	void GetVideoDesc(LONG* plVidWidth, LONG* plVidHeight, LONG* plVidPitch)
	{
		*plVidWidth = m_lVidWidth;
		*plVidHeight = m_lVidHeight;
		*plVidPitch = m_lVidPitch;
	};

	void Update()
	{
		// ����𖈃t���[���Ăяo���ƃe�A�����O�����������
		// �悭�킩���ĂȂ��̂��{�������A�Ƃ肠��������ŁB
		CAutoLock cVideoLock(&m_InterfaceLock);
	}
};

/**
*@brief		����t�@�C���̐���A�e�N�X�`�����擾�����肷��N���X
*@author		Yamagoe
*/
class tdnMovie
{
public:
	tdnMovie(char *filename, bool bSound = false, bool bLoop = true);
	~tdnMovie();

	// ���e�N�X�`���擾
	Texture2D* GetTexture()
	{
		return m_pTexture;
	}

	TextureRenderer *GetTextureRenderer() { return m_pTextureRenderer; }

	void Play();	// �Đ�
	void Stop();	// ��~

	void Update()
	{
		// ���e�A�����O�h�~�̍X�V(���b�N���Ă�)
		m_pTextureRenderer->Update();

		// ���[�v�Đ�����X�V
		if (m_bLoop && isEndPos())SetTime(0.0);
	}

	// �Q�b�^�[
	double GetStopTime() { double ret; m_pMediaPosition->get_StopTime(&ret); return ret; };				// �I�����Ԃ̎擾
	double GetDuration() { double ret; m_pMediaPosition->get_Duration(&ret); return ret; };				// �X�g���[���̎��ԕ��̎擾
	double GetCurrentPosition() { double ret; m_pMediaPosition->get_CurrentPosition(&ret); return ret; }	// ���݂̍Đ��ʒu�̎擾
	void GetUV(float2 *out) { out->x = m_fu; out->y = m_fv; };
	bool isEndPos() { return (GetCurrentPosition() >= GetStopTime()); };

	// �Z�b�^�[
	void SetTime(double val) { m_pMediaPosition->put_CurrentPosition(val); };				// ���݂̍Đ��ʒu���w��ʒu�ɃZ�b�g
	void SetSpeed(double val) { m_pMediaPosition->put_Rate(val); };
	void SetLoop(bool loop) { m_bLoop = loop; }
private:
	// ������e�N�X�`���������Ă���邭��
	TextureRenderer     *m_pTextureRenderer;        // DirectShow Texture renderer

													// Direct3D�Ƃ��Ă̕ϐ�
	Texture2D	*m_pTexture;		// ����̃e�N�X�`��

	IMediaControl		*m_pMediaControl;	// ���f�B�A�Ǘ�
	IGraphBuilder		*m_pGraphBuilder;	// 
	IMediaEvent			*m_pMediaEvent;
	IMediaPosition		*m_pMediaPosition;	// �Đ��ʒu

											//IBaseFilter			*m_pVMR9;		// �����_�t�B���^�[(VMR9)

											// �t�B���^�[�ڑ��w���p�[�C���^�[�t�F�[�X
											//ICaptureGraphBuilder2 *m_pCaptureGraphBuilder2;

	LONG	m_lWidth;			// ��  
	LONG	m_lHeight;			// ����
	LONG	m_lPitch;			// �s�b�`
	FLOAT	m_fu, m_fv;			// ���̃��[�r�[�̃T�C�Y(���A����) / �e�N�X�`���̃T�C�Y(���A����)�ŎZ�o����UV�l

	FILTER_STATE		m_state;	// �����̃X�e�[�g
	bool				m_bLoop;	// ���[�v�t���O
	bool				m_bSound;	// �T�E���h�t���O
	bool				m_bWMV;		// wmv�t�@�C�����ǂ���
};
class tdnMovieManager
{
public:
	static void Initialize();
	static void Release();
};