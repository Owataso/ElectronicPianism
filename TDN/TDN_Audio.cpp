//*****************************************************************************************************************************
#include	"TDNLIB.h"

// acm関数(MP3等の圧縮音声データの解凍を行う)用のインクルード
#include<mmreg.h>
#include<msacm.h>


HRESULT result_sound;

//**************************************************************************************************************
//
//		サウンドバッファ
//
//**************************************************************************************************************
const int tdnSoundBuffer::READBYTE = 1024;	// ここの値が大きくなるにつれて読み込み速度は上がるが、相対的にメモリオーバーの可能性も上がる

//**************************************************************************************************************
//
//**************************************************************************************************************
tdnSoundBuffer::tdnSoundBuffer(LPDIRECTSOUND8 lpDS, char* filename, bool b3D)
{
	/*	サウンドファイルのロード	*/
	lpWBuf = LoadFile(filename, &size, &wfx);
	/*	ロード失敗	*/
	if (lpWBuf == nullptr){
		return;
	}

	/* ファイル名保存 */
	sprintf_s(wav_file_path, sizeof(wav_file_path), "%s", filename);

	Initialize(lpDS, lpWBuf, size, &wfx, b3D);
}

void tdnSoundBuffer::Initialize(LPDIRECTSOUND8 lpDS, unsigned char* data, DWORD size, LPWAVEFORMATEX fmt, bool b3D)
{
	DSBUFFERDESC	dsbd;
	LPVOID			lpbuf1, lpbuf2;
	DWORD			dwbuf1, dwbuf2;

	size = size;
	wfx = *fmt;
	lpWBuf = data;

	lpBuf3D = nullptr;
	lpBuf = nullptr;

	/* 二次バッファ作成	*/
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	if (b3D) dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D | DSBCAPS_CTRLFX;	// CTRL_FX等、サウンド制御に必要なフラグをONにする(CTRL3DをONにするとCTRLPANを使えない)
	else dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFX;
	dsbd.dwBufferBytes = size;
	dsbd.lpwfxFormat = &wfx;

	/* 本来はDIRECTSOUNDBUFFERを使っていたが、BUFFER8に切り替えるために、一旦BUFFERを作る→そのBUFFERのQueriInterfaceの流れでBUFFER8を作成 */
	LPDIRECTSOUNDBUFFER lpWork;
	lpDS->CreateSoundBuffer(&dsbd, &lpWork, nullptr);
	lpWork->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&lpBuf);
	lpWork->Release();	// 用済み

	MyAssert(!b3D || result_sound != E_INVALIDARG || dsbd.lpwfxFormat->nChannels != 2, "3Dサウンドだからステレオ音源は使えないよ！\nモノラルに変換してね");
	//if (b3D && result == E_INVALIDARG && dsbd.lpwfxFormat->nChannels == 2)

	if (result_sound != DS_OK)
	{
		return;
	}

	lpBuf3D = nullptr;
	if (b3D == true)
	{
		/* サウンドバッファから3Dバッファに変換&作成 */
		lpBuf->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&lpBuf3D);
		/* 3Dパラメータ初期化 */
		this->SetAll3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, Vector3(0, 0, 0), Vector3(0, 0, -1), DS3D_MAXCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, Vector3(0, 0, 0));
	}

	/* 二次バッファのロック	*/
	lpBuf->Lock(0, size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);
	/* 音源データの設定	*/
	memcpy(lpbuf1, lpWBuf, dwbuf1);
	if (dwbuf2 != 0) memcpy(lpbuf2, lpWBuf + dwbuf1, dwbuf2);
	/* 音源データの解放	*/
	GlobalFree(lpWBuf);
	/* 二次バッファのロック解除	*/
	lpBuf->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

	/* 自作初期化 */
	PlayCursor = 0;
	BufferSize = size;
	format = wfx;
}

void tdnSoundBuffer::Create_and_copy(LPDIRECTSOUND8 lpDS, char* filename, bool b3D, tdnSoundBuffer **buffers, int dst, int count)
{
	DSBUFFERDESC	dsbd;
	LPVOID			lpbuf1, lpbuf2;
	DWORD			dwbuf1, dwbuf2;


	//===========================================================
	//		コピー元作成
	//===========================================================
	buffers[dst]->lpBuf3D = nullptr;
	buffers[dst]->lpBuf = nullptr;

	/*	WAVファイルのロード	*/
	buffers[dst]->lpWBuf = buffers[dst]->LoadWAV(filename, &buffers[dst]->size, &buffers[dst]->wfx);

	/* ファイル名保存 */
	sprintf_s(buffers[dst]->wav_file_path, sizeof(buffers[dst]->wav_file_path), "%s", filename);

	/* 二次バッファ作成	*/
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	if (b3D) dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D | DSBCAPS_CTRLFX;
	else dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFX;
	dsbd.dwBufferBytes = buffers[dst]->size;
	dsbd.lpwfxFormat = &buffers[dst]->wfx;

	LPDIRECTSOUNDBUFFER lpWork;
	result_sound = lpDS->CreateSoundBuffer(&dsbd, &lpWork, nullptr);

	result_sound = lpWork->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&buffers[dst]->lpBuf);	// LPDIRECTSOUNDBUFFER→LPDIRECTSOUNDBUFFER8
	lpWork->Release();	// 用済み

	MyAssert(!b3D || result_sound != E_INVALIDARG || dsbd.lpwfxFormat->nChannels != 2, "3Dサウンドだからステレオ音源は使えないよ！\nモノラルに変換してね");

	if (result_sound != DS_OK)
	{
		return;
	}

	buffers[dst]->lpBuf3D = nullptr;
	if (b3D)
	{
		buffers[dst]->lpBuf->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&buffers[dst]->lpBuf3D);
		buffers[dst]->SetAll3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, Vector3(0, 0, 0), Vector3(0, 0, -1), DS3D_MAXCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, Vector3(0, 0, 0));
	}

	/* 二次バッファのロック	*/
	buffers[dst]->lpBuf->Lock(0, buffers[dst]->size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);
	/* 音源データの設定	*/
	memcpy(lpbuf1, buffers[dst]->lpWBuf, dwbuf1);
	if (dwbuf2 != 0) memcpy(lpbuf2, buffers[dst]->lpWBuf + dwbuf1, dwbuf2);
	/* 音源データの解放	*/
	//GlobalFree(lpWBuf);	// この文をコメントアウトするのに全てが詰まってる
	/* 二次バッファのロック解除	*/
	buffers[dst]->lpBuf->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

	/* 自作初期化 */
	buffers[dst]->PlayCursor = 0;
	buffers[dst]->BufferSize = buffers[dst]->size;
	buffers[dst]->format = buffers[dst]->wfx;


	//===========================================================
	//		コピー先作成
	//===========================================================
	for (int no = dst + 1; no < dst + count; no++)
	{

		DSBUFFERDESC	dsbd;
		LPVOID			lpbuf1 = nullptr, lpbuf2 = nullptr;
		DWORD			dwbuf1 = 0, dwbuf2 = 0;

		buffers[no]->lpBuf3D = nullptr;
		buffers[no]->lpBuf = nullptr;

		/* 二次バッファ作成	*/
		ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
		dsbd.dwSize = sizeof(DSBUFFERDESC);
		if (b3D == true) dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D | DSBCAPS_CTRLFX;
		else dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFX;
		dsbd.dwBufferBytes = buffers[dst]->size;
		dsbd.lpwfxFormat = &buffers[dst]->wfx;

		LPDIRECTSOUNDBUFFER lpWork2;
		result_sound = lpDS->CreateSoundBuffer(&dsbd, &lpWork2, nullptr);

		if (result_sound == E_OUTOFMEMORY)
		{
			MessageBox(0, "wavファイルが読み込まれすぎてメモリが足りなくなってるよ。セットの数を減らすか、wavファイルの長さを短くしてね", nullptr, MB_OK);
			assert(0);
			return;
		}

		result_sound = lpWork2->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&buffers[no]->lpBuf);	// LPDIRECTSOUNDBUFFER→LPDIRECTSOUNDBUFFER8
		lpWork2->Release();	// 用済み

		buffers[no]->lpBuf3D = nullptr;
		if (b3D)
		{
			result_sound = buffers[no]->lpBuf->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&buffers[no]->lpBuf3D);
			buffers[no]->SetAll3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, Vector3(0, 0, 0), Vector3(0, 0, -1), DS3D_MAXCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, Vector3(0, 0, 0));
		}

		/* 二次バッファのロック	*/
		buffers[no]->lpBuf->Lock(0, buffers[dst]->size, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0);
		/* 音源データの設定	*/
		memcpy(lpbuf1, buffers[dst]->lpWBuf, dwbuf1);
		if (dwbuf2 != 0) memcpy(lpbuf2, buffers[dst]->lpWBuf + dwbuf1, dwbuf2);
		/* 音源データの解放	*/
		//GlobalFree(buffers[no].lpWBuf);
		/* 二次バッファのロック解除	*/
		buffers[no]->lpBuf->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2);

		/* 自作初期化 */
		buffers[no]->PlayCursor = 0;
		buffers[no]->BufferSize = buffers[dst]->size;
		buffers[no]->format = buffers[dst]->wfx;
	}

	/* コピー元音源データの解放	*/
	GlobalFree(buffers[dst]->lpWBuf);
}

tdnSoundBuffer::~tdnSoundBuffer()
{
	if (lpBuf != nullptr)lpBuf->Release();
	if (lpBuf3D != nullptr) lpBuf3D->Release();
}

//**************************************************************************************************************
//		ＷＡＶファイルの読み込み
//**************************************************************************************************************
LPBYTE tdnSoundBuffer::LoadFile(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx)
{
	//	ファイル読み込み
	std::string e = tdnFile::GetFileExtention(fname);
	char* ext = (char*)e.c_str();
	if (lstrcmpi(ext, ".wav") == 0) return LoadWAV(fname, size, wfx);
	else if (lstrcmpi(ext, ".owd") == 0) return LoadOWD(fname, size, wfx);
	else if (lstrcmpi(ext, ".mp3") == 0) return LoadMP3(fname, size, wfx);
	else if (lstrcmpi(ext, ".ogg") == 0) return LoadOGG(fname, size, wfx);
	else
	{
		MyAssert(0, "ファイルオープンに失敗！\nファイル名: %s", fname);
		return nullptr;
	}
}

LPBYTE tdnSoundBuffer::LoadWAV(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx)
{
	// バイナリ読み込み
	std::ifstream infs(fname, std::ios::binary);
	MyAssert(infs, "エラーファイル名[%s]\n原因:wavファイルが入っていないか、wavファイル名が間違っているよ", fname);

	char chunkID[4];
	int ChunkSize;
	BYTE *buf = nullptr;

	/* RIFFチャンク侵入 */
	infs.read(chunkID, 4);
	MyAssert(
		chunkID[0] == 'R'&&
		chunkID[1] == 'I'&&
		chunkID[2] == 'F'&&
		chunkID[3] == 'F',
		"エラーファイル名[%s]\n原因:wavファイルが入っていないか、wavファイル名が間違っているよ", fname);

	infs.read((char*)&ChunkSize, 4);		// RIFFチャンクヘッダのサイズ
	infs.read(chunkID, 4);					// フォーマットタイプ
	MyAssert(
		chunkID[0] == 'W'&&
		chunkID[1] == 'A'&&
		chunkID[2] == 'V'&&
		chunkID[3] == 'E',
		"エラーファイル名[%s]\n原因:これwavじゃない", fname);

	/* フォーマットチャンク侵入 */
	infs.read(chunkID, 4);


	// ★Broadcast Wave Formatか確認
	if (chunkID[0] == 'b' &&
		chunkID[1] == 'e' &&
		chunkID[2] == 'x' &&
		chunkID[3] == 't')
	{
		/* bext chunk とは　「Broad Audio Extension chunk」で、BWFファイル独自のチャンク。名前的にたぶん拡張じゃね？　ただ、ここでは使わないので読み飛ばすだけ */

		// フォーマットチャンクまで読み飛ばす
		const char FMTCHUNKID[5] = "fmt ";
		int i = 0;
		while (i < 4)
		{
			infs.read(&chunkID[i], 1);
			i = (chunkID[i] == FMTCHUNKID[i]) ? i + 1 : 0;
		}
	}


	MyAssert(
		chunkID[0] == 'f'&&
		chunkID[1] == 'm'&&
		chunkID[2] == 't',
		"エラーファイル名[%s]\n原因:フォーマットチャンク内でエラー", fname);

	infs.read((char*)&ChunkSize, 4);		// フォーマットチャンクヘッダのサイズ

	// フォーマット吸い上げ
	//infs.read((char*)wfx, sizeof(WAVEFORMATEX));
	infs.read((char*)&wfx->wFormatTag, 2);				// フォーマットID
	infs.read((char*)&wfx->nChannels, 2);				// チャンネル数
	infs.read((char*)&wfx->nSamplesPerSec, 4);			// サンプリングレート
	infs.read((char*)&wfx->nAvgBytesPerSec, 4);			// データ速度 byte/sec
	infs.read((char*)&wfx->nBlockAlign, 2);				// ブロックサイズ
	infs.read((char*)&wfx->wBitsPerSample, 2);			// サンプルあたりのビット数
	//infs.read((char*)&wfx->cbSize, 2);					// よくわからん

	MyAssert(
		wfx->wFormatTag == WAVE_FORMAT_PCM,
		"エラーファイル名[%s]\n原因:フォーマットがwavじゃない", fname);

	/* データチャンク侵入 */

	// チャンクID検索
	const char DATACHUNKID[5] = "data";
	for (int i = 0; i < 4; i++)
	{
		do
		{
			infs.read(&chunkID[i], 1);
		} while (chunkID[i] != DATACHUNKID[i]);
	}

	MyAssert(
		chunkID[0] == 'd'&&
		chunkID[1] == 'a'&&
		chunkID[2] == 't'&&
		chunkID[3] == 'a',
		"エラーファイル名[%s]\n原因:データチャンク内でエラー。フォーマットのずれが原因かもしれない", fname);

	infs.read((char*)size, 4);				// データサイズ
	buf = (LPBYTE)GlobalAlloc(LPTR, *size);

	// 音データ吸い上げ
	DWORD remain = *size; // 書き込むべき残りのバイト数
	BYTE work[READBYTE]; 

	// xBytesずつ読み込む(メモリパンクさせないように)
	for (int i = 0; remain > 0; i++)
	{
		int readSize = min(READBYTE, remain);
		infs.read((char*)work, readSize);
		remain -= readSize;

		memcpy(&buf[i*READBYTE], work, readSize);
	}

	return buf;
}

LPBYTE tdnSoundBuffer::LoadOWD(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx)
{
	// バイナリ読み込み
	std::ifstream infs(fname, std::ios::binary);

	BYTE *buf = nullptr;

	/* バージョンチェック */
	BYTE ver;
	infs.read((char*)&ver, 1);

	if (ver == 0)
	{
		/* フォーマット吸い上げ */
		infs.read((char*)wfx, sizeof(WAVEFORMATEX));

		/* WAVサイズ取得 */
		infs.read((char*)size, 4);
		buf = (LPBYTE)GlobalAlloc(LPTR, *size);

		/* データ吸い上げ */
		DWORD remain = *size; // 書き込むべき残りのバイト数
		BYTE work[READBYTE];

		// xBytesずつ読み込む(メモリパンクさせないように)
		for (int i = 0; remain > 0; i++)
		{
			int readSize = min(READBYTE, remain);
			infs.read((char*)work, readSize);
			remain -= readSize;

			memcpy(&buf[i*READBYTE], work, readSize);
		}
	}


	else if (ver == 1)
	{

	}

	return buf;
}

//-------------------------------------------------------------
//	MP3
//-------------------------------------------------------------
LPBYTE tdnSoundBuffer::LoadMP3(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx)
{
	// バイナリ読み込み
	std::ifstream infs(fname, std::ios::binary);
	MyAssert(infs, "エラーファイル名[%s]\n原因:MP3ファイル名が間違っているか、ファイルが存在しないよ", fname);

	BYTE *buf = nullptr;

	// フレームヘッダは４バイト(３２ビット）で出来ている。ので、DWORDでいっぺんに読んで分けていく
	DWORD receive;
	infs.read((char*)&receive, 32);

	WORD frame_synchronism = (receive >> 20) & 0x000007ff;	// フレーム同期。オール1ビット(11ビット)
	MyAssert(frame_synchronism == 0x000007ff, "エラーファイル名[%s]\n原因:形式はMP3ですが、中身がMP3でない可能性があります", fname);

	BYTE ver = (receive >> 19) & 0x00000003;	// MPEG Audio のバージョンID(2ビット)

	enum class MPEG_VERSION
	{
		MPEG_V2_5,	// バージョン2.5
		RESERVE,	// 予約らしい
		MPEG_V2,	// バージョン2
		MPEG_V1		// バージョン1
	}mpeg_ver;
	switch (ver & 0x03)
	{
	case 0x00:mpeg_ver = MPEG_VERSION::MPEG_V2_5; break;
	case 0x01:mpeg_ver = MPEG_VERSION::RESERVE; break;
	case 0x02:mpeg_ver = MPEG_VERSION::MPEG_V2; break;
	case 0x03:mpeg_ver = MPEG_VERSION::MPEG_V1; break;
	}


	BYTE layer			= (receive >> 17)	& 0x00000003;	// レイヤーの種類(2ビット)
	BOOL CRCerror		= (receive >> 16)	& 0x00000001;	// CRCエラー検査による保護(1ビット)
	BYTE bit_rate		= (receive >> 12)	& 0x0000000f;	// ビットレート(4ビット)
	BYTE sampling_rate	= (receive >> 10)	& 0x0000003;	// サンプリングレート(2ビット)
	BOOL padding		= (receive >> 9)	& 0x00000001;	// パディング(1ビット)	0:なし 1:あり
	BOOL private_bit	= (receive >> 8)	& 0x00000001;	// プライベートビットの使用不使用(1ビット) 0:不使用 1:使用
	BYTE channel_mode	= (receive >> 6)	& 0x0000003;	// チャンネルモード(2ビット)
	BYTE mode_extention = (receive >> 4)	& 0x0000003;	// モードエクステンション。チャンネルモードが01の場合に有効、他の場合0に設定(2ビット)
	BOOL copy_light		= (receive >> 3)	& 0x0000001;	// 著作権(1ビット)	0:保護なし 1:保護あり
	BOOL original		= (receive >> 2)	& 0x00000001;	// オリジナル(1ビット)	0:コピー 1:オリジナル
	BYTE emphasis		= receive			& 0x00000003;	// エンファシス。信号調整の一種(2ビット)

	/*
	MPEG Audio のバージョンID
	[00] - MPEG v2.5 / [01] - (予約)
	[10] - MPEG v2 / [11] - MPEG v1

	レイヤーの種類
	[00] - (予約) / [01] - Layer3
	[10] - Layer2 / [11] - Layer1

	CRCエラー検査による保護
	[0] - 保護あり。この場合、16ビットのCRCがフレームヘッダの後に付加される
	[1] - 保護なし

	*/

	// サンプリングレート表
	static const int sampring_rate_list[3][3]=
	{
		{ 44100, 22050, 11025 },
		{ 48000, 24000, 12000 },
		{ 32000, 16000, 8000 }
	};

	// ビットレート表(頭おかしい)
	static const int bit_rate_list[14][5] =
	{
		{ 32,	32,		32,		32,		8 },
		{ 64,	48,		40,		48,		16 },
		{ 96,	56,		48,		56,		24 },
		{ 128,	64,		56,		64,		32 },
		{ 160,	80,		64,		80,		40 },
		{ 192,	96,		80,		96,		48 },
		{ 224,	112,	96,		112,	56 },
		{ 256,	128,	112,	128,	64 },
		{ 288,	160,	128,	144,	80 },
		{ 320,	192,	160,	160,	96 },
		{ 352,	224,	192,	176,	112 },
		{ 384,	256,	224,	192,	128 },
		{ 416,	320,	256,	224,	144 },
		{ 448,	384,	320,	256,	160 }
	};


	// サンプリングレート設定
	DWORD samplesPerSec;
	{
		int column(0), row(0);
		// リストの列
		switch (mpeg_ver)
		{
		case MPEG_VERSION::MPEG_V1:column = 0; break;
		case MPEG_VERSION::MPEG_V2:column = 1; break;
		case MPEG_VERSION::MPEG_V2_5:column = 2; break;
		}

		// リストの行
		switch (sampling_rate & 0x03)
		{
		case 0x00:row = 0; break;
		case 0x01:row = 1; break;
		case 0x02:row = 2; break;
		case 0x03:break;	// 「予約」らしい。よく分からん
		}

		samplesPerSec = sampring_rate_list[row][column];			// サンプリングレート
	}

	// ビットレート設定
	DWORD bitRate;
	{
		int column(0), row(0);
		// リストの列
		column = 4;		// 現状リストの右端の数字以外の数字がまず見ないし使わない数字なので今は右端を指定

		// リストの行
		switch (bit_rate & 0x0f)
		{
		case 0x01:row = 0; break;
		case 0x02:row = 1; break;
		case 0x03:row = 2; break;
		case 0x04:row = 3; break;
		default:assert(0); break;	// まずビットレートで40以上ってあるのか・・・？
		}
		bitRate = bit_rate_list[row][column];				// サンプルあたりのビット数
	}

	MPEGLAYER3WAVEFORMAT    mwf;        // Source MP3

	mwf.wfx.cbSize = MPEGLAYER3_WFX_EXTRA_BYTES;
	mwf.wfx.nChannels = (channel_mode & 0x3) ? 1 : 2;
	mwf.wfx.wFormatTag = WAVE_FORMAT_MPEGLAYER3;
	mwf.wfx.nBlockAlign = 1;
	mwf.wfx.wBitsPerSample = 0;
	mwf.wfx.nSamplesPerSec = samplesPerSec;
	mwf.wfx.nAvgBytesPerSec = bitRate * 1000 / 8;

	mwf.wID = MPEGLAYER3_ID_MPEG;
	mwf.fdwFlags = padding ? MPEGLAYER3_FLAG_PADDING_ON : MPEGLAYER3_FLAG_PADDING_OFF;

	mwf.nFramesPerBlock = 1;

	DWORD frameSize;

	// MPEGバージョン1
	if (mpeg_ver == MPEG_VERSION::MPEG_V1)
	{
		frameSize = (DWORD)(144000 * bitRate / samplesPerSec) + padding;
	}
	// MPEGバージョン2・バージョン2.5
	else
	{
		frameSize = (DWORD)(72000 * bitRate / samplesPerSec) + padding;
	}
	mwf.nBlockSize = (WORD)(frameSize * mwf.nFramesPerBlock);
	mwf.nCodecDelay = 0x571;

	
	wfx->wFormatTag = WAVE_FORMAT_PCM;

	// フォーマット変換が可能かどうか調べます。
	MMRESULT MMresult = acmFormatSuggest(
		NULL,
		(LPWAVEFORMATEX)&mwf,
		wfx,
		sizeof(WAVEFORMATEX),
		ACM_FORMATSUGGESTF_WFORMATTAG);
	MyAssert(MMresult == MMSYSERR_NOERROR, "エラー: MP3変換のacmFormatSuggest関数");

	// ストリームハンドル
	HACMSTREAM has;

	// ★フォーマット変換ストリームオープン！！！
	MMresult = acmStreamOpen(
		&has,								// 変換に用いるストリームハンドルへのポインターです。ストリームがオープンされるとハンドルが戻ります。
		NULL,								// 変換を行うドライバーを指定します。NULLを指定すると、システムは最も適当と思われるドライバーを検索します。
		(LPWAVEFORMATEX)&mwf,				// 変換元のウェーブフォーマットを指定します。acmFormatSuggest()に渡したものと同じものを使います。
		wfx,								// 変換先のウェーブフォーマットを指定します。acmFormatSuggest()によって設定されたものを使います。
		NULL,								// 変換操作で用いるフィルターを指定します。フォーマット変換の時は用いないのでNULLを指定します。
		(DWORD_PTR)tdnSystem::GetWindow(),	// 変換操作を非同期で行う場合、アプリケーションは変換処理の終了を知ることができます。
		NULL,								// コールバック関数で渡されるユーザーインスタンスデータです。コールバック関数以外の方式では使いませんのでNULLを指定します。
		CALLBACK_WINDOW);					// 変換ストリームを開くためのフラグで以下のものが定義されています。
	MyAssert(MMresult == MMSYSERR_NOERROR, "エラー: MP3変換のacmStreamOpen関数");


	// 変換前後のサイズ計算
	MMresult = acmStreamSize(
		has,						// ストリームハンドル
		mwf.nBlockSize,				// 変換前のサイズ
		size,						// 変換後のサイズ
		ACM_STREAMSIZEF_SOURCE);

	// ACMSTREAMHEADER構造体を準備する。
	ACMSTREAMHEADER ash;
	ZeroMemory(&ash, sizeof(ACMSTREAMHEADER));
	ash.cbStruct = sizeof(ACMSTREAMHEADER);

	acmStreamPrepareHeader(
		has,		// ストリームハンドル
		&ash,		// 準備するACMSTREAMHEADER構造体のアドレス。
		0			// 予約済みなので0を指定するらしい
		);


	// ★フォーマット変換！！
	acmStreamConvert(
		has,						// ストリームハンドル
		&ash,						// ACMSTREAMHEADER構造体のアドレス。acmStreamPrepareHeaderで準備する必要がある
		ACM_STREAMCONVERTF_END);	// 資料が少ないため不明だが、これが無難っぽい
	// MyAssert(ash.cbDstLengthUsed > 0, "エラー: MP3変換のacmStreamConvert関数");	// 非同期の場合のみ、よってここのエラーチェックは不採用


	/* データ吸い上げ */
	buf = (LPBYTE)GlobalAlloc(LPTR, *size);
	DWORD remain = *size; // 書き込むべき残りのバイト数
	BYTE work[READBYTE];

	// xBytesずつ読み込む(メモリパンクさせないように)
	for (int i = 0; remain > 0; i++)
	{
		int readSize = min(READBYTE, remain);
		memcpy(work, &ash.pbDst[i*READBYTE], readSize);
		remain -= readSize;

		memcpy(&buf[i*READBYTE], work, readSize);
	}

	// SCMDTREAMHEADERの開放
	acmStreamUnprepareHeader(
		has,	// ストリームハンドル
		&ash,	// 解放するACMSTREAMHEADER構造体のアドレス。
		0		// 予約済みなので0を指定するらしい
		);

	// フォーマット変換ストリームを閉じる
	acmStreamClose(has, 0);

	return buf;
}


//-------------------------------------------------------------
//	OGG
//-------------------------------------------------------------
LPBYTE tdnSoundBuffer::LoadOGG(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx)
{
	LPBYTE buf{};
	OggVorbis_File vf{};
	FILE *fp{};
	auto Close=[](FILE *fp, OggVorbis_File *vf)
	{
		// OggVorbisファイルを閉じる
		if (fp) fclose(fp);
		ov_clear(vf);
	};

	//	ファイルオープン 
	MyAssert(fopen_s(&fp, fname, "rb") == S_OK, "ファイルオープンに失敗！\nファイル名: %s", fname);

	//Oggを開く
	ov_open(fp, &vf, nullptr, 0);

	// OggVorbisファイルの情報を取得する
	vorbis_info *vi{ ov_info(&vf, -1) };
	MyAssert(vi, "ov_info関数に失敗");

	// WAVEフォーマット設定
	wfx->wFormatTag = WAVE_FORMAT_PCM;
	wfx->nChannels = vi->channels;
	wfx->nSamplesPerSec = vi->rate;
	wfx->nAvgBytesPerSec = vi->rate * vi->channels * 2;
	wfx->nBlockAlign = vi->channels * 2;
	wfx->wBitsPerSample = 16;
	wfx->cbSize = sizeof(WAVEFORMATEX);

	// サイズ取得
	*size = (DWORD)ov_pcm_total(&vf, -1) * wfx->nBlockAlign;

	// 再生用バッファの作成
	buf = (LPBYTE)GlobalAlloc(LPTR, *size);

	DWORD dwTotalReadSize{};
	int bitstream{};
	bool result{true};

	while (dwTotalReadSize < *size)
	{
		// データのデコード
		LONG readSize = ov_read(
			&vf,
			(char*)buf + dwTotalReadSize,
			*size - dwTotalReadSize,
			0,
			2,
			1,
			&bitstream
			);

		if (readSize == 0)     // ファイルの終端まで読み込んだ
			break;
		else if (readSize < 0)// エラー発生
		{
			MyAssert(0, "おかC");

			// 解放
			Close(fp, &vf);

			return nullptr;
		}

		dwTotalReadSize += readSize;
	}

	// 解放
	Close(fp, &vf);

	return buf;
}

//**************************************************************************************************************
//	再生管理
//**************************************************************************************************************
//-------------------------------------------------------------
//	再生	
//-------------------------------------------------------------
void tdnSoundBuffer::Play(bool loop, DWORD cursor)
{
	lpBuf->Stop();
	lpBuf->SetCurrentPosition(cursor);
	//	ループ再生
	if (loop) lpBuf->Play(0, 0, DSBPLAY_LOOPING);
	//	ノーマル再生
	else	   lpBuf->Play(0, 0, 0);

	//	lpBuf->SetFrequency(8000);
	PlayCursor = 0;
	loop_flag = loop;
}

//-------------------------------------------------------------
//	停止	
//-------------------------------------------------------------
void tdnSoundBuffer::Stop()
{
	lpBuf->Stop();
}

void tdnSoundBuffer::Pause()
{
	lpBuf->GetCurrentPosition(&PlayCursor, nullptr);
	lpBuf->Stop();
}
void tdnSoundBuffer::PauseOff()
{
	//lpBuf->Stop();
	lpBuf->SetCurrentPosition(PlayCursor);
	//	ループ再生
	if (loop_flag) lpBuf->Play(0, 0, DSBPLAY_LOOPING);
	//	ノーマル再生
	else	   lpBuf->Play(0, 0, 0);

	//PlayCursor = 0;
}

//-------------------------------------------------------------
//	ボリューム変更
//-------------------------------------------------------------
void tdnSoundBuffer::SetVolume(int volume)
{
	lpBuf->SetVolume(volume);
}

//-------------------------------------------------------------
//	ボリュームゲッター
//-------------------------------------------------------------
int	tdnSoundBuffer::GetVolume()
{
	LONG ret;
	lpBuf->GetVolume(&ret);
	return ret;
}

//-------------------------------------------------------------
//	ステレオ(左右音)関係
//-------------------------------------------------------------
void tdnSoundBuffer::SetPan(int pan)
{
	lpBuf->SetPan(pan);
}

int tdnSoundBuffer::GetPan()
{
	LONG ret;
	lpBuf->GetPan(&ret);
	return ret;
}


//-------------------------------------------------------------
//	周波数関係(再生速度・ピッチ)
//-------------------------------------------------------------
void tdnSoundBuffer::SetFrequency(int pitch)
{
	lpBuf->SetFrequency(pitch);
}

int tdnSoundBuffer::GetFrequency()
{
	DWORD ret;
	lpBuf->GetFrequency(&ret);
	return ret;
}

//-------------------------------------------------------------
//	再生速度(上の事やってるだけ)
//-------------------------------------------------------------
void tdnSoundBuffer::SetSpeed(float speed)
{
	DWORD frequency = (DWORD)(format.nSamplesPerSec*speed);
	lpBuf->SetFrequency(frequency);
}

float tdnSoundBuffer::GetSpeed()
{
	DWORD work;
	lpBuf->GetFrequency(&work);

	return ((float)work / format.nSamplesPerSec);
}


//-------------------------------------------------------------
//	再生チェック	
//-------------------------------------------------------------
bool tdnSoundBuffer::isPlay()
{
	DWORD	dwAns;
	lpBuf->GetStatus(&dwAns);
	return ((dwAns&DSBSTATUS_PLAYING) != 0) ? true : false;
}


//-------------------------------------------------------------
//	再生位置関係
//-------------------------------------------------------------
DWORD tdnSoundBuffer::GetPlayCursor()
{
	/* 再生位置(データ量的な) */
	DWORD ret;
	lpBuf->GetCurrentPosition(&ret, nullptr);

	return ret;
}

void tdnSoundBuffer::SetPlayCursor(DWORD new_position)
{
	lpBuf->SetCurrentPosition(new_position);
}

DWORD tdnSoundBuffer::GetPlayFrame()
{
	/* 再生位置(フレーム換算) */
	return (GetPlayCursor() / (format.nAvgBytesPerSec / 60));
}

int tdnSoundBuffer::GetPlaySecond()
{
	/* 再生位置(秒換算) */
	return (GetPlayCursor() / format.nAvgBytesPerSec);
}

DWORD tdnSoundBuffer::GetPlayMSecond()
{
	/* 再生位置(ミリ秒換算) */
	return (int)(GetPlayCursor() / (format.nAvgBytesPerSec * .001f));
}

void tdnSoundBuffer::SetPlaySecond(int sec)
{
	DWORD set = sec * format.nAvgBytesPerSec;
	lpBuf->SetCurrentPosition(set);
}

DWORD tdnSoundBuffer::GetSize()
{
	return BufferSize;
}

int tdnSoundBuffer::GetLengthSecond()
{
	return (BufferSize / format.nAvgBytesPerSec);
}

//-------------------------------------------------------------
//	3Dサウンド関係
//-------------------------------------------------------------
void tdnSoundBuffer::SetDist(float max_dist, float min_dist)
{
	lpBuf3D->SetMaxDistance(max_dist, DS3D_DEFERRED), lpBuf3D->SetMinDistance(min_dist, DS3D_DEFERRED);
}
void tdnSoundBuffer::SetPos(const Vector3 &pos)
{
	lpBuf3D->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
}
void tdnSoundBuffer::SetFront(const Vector3 &front)
{
	lpBuf3D->SetConeOrientation(front.x, front.y, front.z, DS3D_DEFERRED);
}
void tdnSoundBuffer::SetRange(int degreeIn)
{
	lpBuf3D->SetConeAngles(degreeIn, (DS3D_MAXCONEANGLE - degreeIn), DS3D_DEFERRED);
}
void tdnSoundBuffer::SetOutRange_volume(int out_vol)
{
	lpBuf3D->SetConeOutsideVolume(out_vol, DS3D_DEFERRED);
}
void tdnSoundBuffer::SetMove(const Vector3 &move)
{
	lpBuf3D->SetVelocity(move.x, move.y, move.z, DS3D_DEFERRED);
}
void tdnSoundBuffer::SetAll3D(float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move)
{
	DS3DBUFFER set;
	ZeroMemory(&set, sizeof(DS3DBUFFER));
	set.dwSize = sizeof(DS3DBUFFER);

	/*
	DS3DMODE_HEADRELATIVE
	サウンド パラメータ(位置・速度・向き) は、リスナーのパラメータに対して相対的なものである。
	このモードでは、リスナーのパラメータが変化するとサウンドの絶対パラメータは自動的に更新されるので、
	相対パラメータは一定に保たれる。

	DS3DMODE_NORMAL 
	標準の処理。これはデフォルトのモードである。
	*/
	set.dwMode = DS3DMODE_HEADRELATIVE;

	set.flMaxDistance = max_dist;
	set.flMinDistance = min_dist;
	set.vPosition = D3DXVECTOR3(pos.x, pos.y, pos.z);
	set.vConeOrientation = D3DXVECTOR3(front.x, front.y, front.z);
	set.dwInsideConeAngle = degreeIn;
	set.dwOutsideConeAngle = (DS3D_MAXCONEANGLE - degreeIn);
	set.lConeOutsideVolume = out_vol;
	set.vVelocity = D3DXVECTOR3(move.x, move.y, move.z);

	lpBuf3D->SetAllParameters(&set, DS3D_DEFERRED);
}

//-------------------------------------------------------------
//	エフェクト
//-------------------------------------------------------------
void tdnSoundBuffer::SetFX(DXA_FX flag)
{
	bool isPlay(this->isPlay());
	// 演奏を停止し、エフェクトを全削除する(演奏中は設定不可らしい)
	if (isPlay)this->Pause();
	lpBuf->SetFX(0, nullptr, nullptr);
	if (flag == DXA_FX::OFF)
	{
		if (isPlay)this->PauseOff();
		return;
	}

	// エフェクト構造体設定
	DSEFFECTDESC ed;
	ZeroMemory(&ed, sizeof(ed));
	ed.dwSize = sizeof(DSEFFECTDESC);

	switch (flag)
	{
	case DXA_FX::CHORUS:ed.guidDSFXClass = GUID_DSFX_STANDARD_CHORUS;
		break;
	case DXA_FX::COMPRESSOR:ed.guidDSFXClass = GUID_DSFX_STANDARD_COMPRESSOR;
		break;
	case DXA_FX::DISTORTION:ed.guidDSFXClass = GUID_DSFX_STANDARD_DISTORTION;
		break;
	case DXA_FX::ECHO:ed.guidDSFXClass = GUID_DSFX_STANDARD_ECHO;
		break;
	case DXA_FX::FLANGER:ed.guidDSFXClass = GUID_DSFX_STANDARD_FLANGER;
		break;
	case DXA_FX::GARGLE:ed.guidDSFXClass = GUID_DSFX_STANDARD_GARGLE;
		break;
	case DXA_FX::ENVREVERB:ed.guidDSFXClass = GUID_DSFX_STANDARD_I3DL2REVERB;
		break;
	case DXA_FX::PARAMEQ:ed.guidDSFXClass = GUID_DSFX_STANDARD_PARAMEQ;
		break;
	case DXA_FX::WAVESREVERB:ed.guidDSFXClass = GUID_DSFX_WAVES_REVERB;
		break;
	}

	// DirectSoundに渡す
	result_sound = lpBuf->SetFX(1, &ed, nullptr);

	MyAssert(result_sound == S_OK || format.wBitsPerSample == 16 || flag != DXA_FX::WAVESREVERB, "ミュージックリバーブエフェクトの設定は16bitのオーディオフォーマットのみです");

	// 再生
	if (isPlay)this->PauseOff();
}

//**************************************************************************************************************
//
//		ストリームサウンド
//
//**************************************************************************************************************

static int	NumStream = 0;
static bool ForceEnd = false;
DWORD tdnStreamSound::m_PlayMSecond = 0;

//**************************************************************************************************************
//		管理スレッド
//**************************************************************************************************************
DWORD WINAPI ThreadIIDX(LPDWORD lpdwParam)
{
	DWORD	param;
	tdnStreamSound*	lpStream;

	lpStream = (tdnStreamSound*)(lpdwParam);
	for (;;){
		if (lpStream == nullptr)
			break;
		param = WaitForMultipleObjects(3, lpStream->hEvent, FALSE, 100);

		if (ForceEnd == true)
			param = -1;

		switch (param){
		case WAIT_OBJECT_0:
			if (lpStream->type == TYPE_WAV) lpStream->SetBlockWAV(1);
			if (lpStream->type == TYPE_OGG) lpStream->SetBlockOGG(1);
			break;
		case WAIT_OBJECT_0 + 1:	
			//tdnStreamSound::m_PlayMSecond += lpStream->GetBlockMSecond();
			if (lpStream->type == TYPE_WAV) lpStream->SetBlockWAV(0);
			if (lpStream->type == TYPE_OGG) lpStream->SetBlockOGG(0);
			break;

		case WAIT_TIMEOUT:		switch (lpStream->GetMode()){
		case STR_NORMAL:		break;
		case STR_FADEIN:	lpStream->SetVolume(lpStream->GetVolume() + lpStream->GetParam());
			if (lpStream->GetVolume() >= 255) lpStream->SetMode(STR_NORMAL, 0);
			break;
		case STR_FADEOUT:	lpStream->SetVolume(lpStream->GetVolume() - lpStream->GetParam());
			if (lpStream->GetVolume() <= 0){
				lpStream->Stop();
				delete lpStream, lpStream = nullptr;
				ExitThread(TRUE);
				return 0;
			}
			break;
		}
								break;

		default:
			lpStream->Stop();
			delete lpStream, lpStream = nullptr;
			ExitThread(TRUE);
			return 0;

		}
	}
	return 0;
}

//**************************************************************************************************************
//
//**************************************************************************************************************
tdnStreamSound::tdnStreamSound(LPDIRECTSOUND lpDS, LPSTR filename, BYTE mode, int param, bool bLoop) :m_bLoop(bLoop), mode(mode), param(param), m_bEnd(false)
{
	m_PlayMSecond = 0;
	NumStream++;

	LPSTR	work;
	BOOL	bInit;
	//	ストリームファイルを開く
	work = &filename[lstrlen(filename) - 4];
	if (lstrcmpi(work, ".wav") == 0) bInit = SetWAV(lpDS, filename);
	if (lstrcmpi(work, ".ogg") == 0) bInit = SetOGG(lpDS, filename);

	lpStream->SetCurrentPosition(0);
	if (mode != STR_FADEIN) SetVolume(255);
	else SetVolume(0);
	/*	管理スレッドの作成	*/
	hStrThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ThreadIIDX, this, 0, &dwThreadId);
	if (hStrThread == nullptr) return;
	/*	再生開始	*/
	lpStream->Play(0, 0, DSBPLAY_LOOPING);
}



tdnStreamSound::~tdnStreamSound()
{
	if (lpStream != nullptr){
		if (type == TYPE_OGG) ov_clear(&vf);
		else if (hStrFile != nullptr) fclose(hStrFile);

		if (lpStrNotify != nullptr) lpStrNotify->Release();
		lpStrNotify = nullptr;
		/*	バッファ開放	*/
		if (lpStream != nullptr) lpStream->Release();
		lpStream = nullptr;
	}

	NumStream--;
}

//**************************************************************************************************************
//
//**************************************************************************************************************
//-------------------------------------------------------------
//	ブロック作成
//-------------------------------------------------------------
BOOL	tdnStreamSound::OGGRead(LPBYTE dst, unsigned long size)
{
	DWORD	remain = size;
	char*	dstPtr = (char*)dst;
	while (remain > 0){
		long actualRead = ov_read(&vf, dstPtr, remain, 0, 2, 1, nullptr);
		//終端チェック
		if (actualRead <= 0){
			if (ov_pcm_seek(&vf, 0) && m_bLoop)
			{			
				return FALSE;
			}
			if (!m_bLoop) return false;
		}
		remain -= actualRead;
		dstPtr += actualRead;
	}
	return TRUE;
}

BOOL	tdnStreamSound::SetBlockOGG(int block)
{
	//MyDebugString("a");
	LPBYTE	blk1, blk2;
	DWORD	bs1, bs2;

	DWORD	CurPos;
	BOOL bEnd;

	CurPos = StrSize - StrPos;
	//	バッファロック
	lpStream->Lock((rate * 4 * STRSECOND)*block, rate * 4 * STRSECOND, (LPVOID*)&blk1, &bs1, (LPVOID*)&blk2, &bs2, 0);
	//	ブロック１へのデータセット
	bEnd = !OGGRead(blk1, bs1);
	StrPos += bs1;
	//	ブロック２へのデータセット
	if (blk2){
		(bEnd)? bEnd = !OGGRead(blk2, bs2) : OGGRead(blk2, bs2);
		StrPos += bs2;
	}
	lpStream->Unlock(blk1, bs1, blk2, bs2);
	
	// 終端かつループじゃないなら止める
	if (bEnd && !m_bLoop)
	{
		m_bEnd = true;
	}

	return TRUE;
}


BOOL	tdnStreamSound::SetBlockWAV(int block)
{
	LPBYTE	blk1, blk2;
	DWORD	bs1, bs2, work;

	DWORD	CurPos;

	CurPos = StrSize - StrPos;
	lpStream->Lock((rate * 4 * STRSECOND)*block, rate * 4 * STRSECOND, (LPVOID*)&blk1, &bs1, (LPVOID*)&blk2, &bs2, 0);

	work = fread(blk1, bs1, 1, hStrFile);
	StrPos += work;
	if (work < bs1){
		/*	巻き戻し	*/
		fseek(hStrFile, LoopPtr, SEEK_SET);
		work = fread(blk1 + work, bs1 - work, 1, hStrFile);
		StrPos = work;
	}

	if (blk2){
		work = fread(blk2, bs2, 1, hStrFile);
		StrPos += work;
		if (work < bs2){
			/*	巻き戻し	*/
			fseek(hStrFile, LoopPtr, SEEK_SET);
			work = fread(blk2 + work, bs2 - work, 1, hStrFile);
			StrPos = work;
		}
	}

	lpStream->Unlock(blk1, bs1, blk2, bs2);
	return TRUE;
}


//**************************************************************************************************************
//	データ管理
//**************************************************************************************************************
void tdnStreamSound::Initialize(LPDIRECTSOUND lpDS, int rate)
{
	/*	初期化チェック	*/
	if (lpDS == nullptr) return;
	/*	ＷＡＶＥフォーマット初期化	*/
	WAVEFORMATEX wfx{};
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;			/*	stereo	*/
	wfx.nSamplesPerSec = rate;
	wfx.wBitsPerSample = 16;			/*	16bit	*/
	wfx.nBlockAlign = 4;
	wfx.nAvgBytesPerSec = m_AvgBytesPerSec = rate * 4;
	/* 二次バッファの初期化	*/
	DSBUFFERDESC dsbd{};
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE;
	dsbd.dwBufferBytes = rate * 4 * STRSECOND * 2;
	dsbd.lpwfxFormat = &wfx;
	if (lpDS->CreateSoundBuffer(&dsbd, &lpStream, nullptr) != DS_OK) return;
	lpStream->SetFormat(&wfx);

	if (lpStream->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&lpStrNotify) != DS_OK) return;
	/*	位置イベント作成	*/
	hEvent[0] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	hEvent[1] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	hEvent[2] = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	pn[0].dwOffset = 0;
	pn[0].hEventNotify = hEvent[0];
	pn[1].dwOffset = rate * 4 * STRSECOND;
	pn[1].hEventNotify = hEvent[1];

	pn[2].dwOffset = DSBPN_OFFSETSTOP;
	pn[2].hEventNotify = hEvent[2];

	if (lpStrNotify->SetNotificationPositions(3, pn) != DS_OK) return;

	this->rate = rate;
	return;
}


BOOL	tdnStreamSound::SetWAV(LPDIRECTSOUND lpDS, char* filename)
{
	HMMIO			hMMIO = nullptr;		/*	ファイルハンドル	*/
	MMCKINFO		ckinfo, ckparent;	/*	RIFFチャンク情報	*/
	LRESULT			ptr;

	/* オープン	*/
	hMMIO = mmioOpen(filename, nullptr, MMIO_ALLOCBUF | MMIO_READ);
	mmioDescend(hMMIO, &ckparent, nullptr, 0);
	/*	ｄａｔａチャンクに侵入		*/
	ckinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmioDescend(hMMIO, &ckinfo, &ckparent, MMIO_FINDCHUNK);
	//	現在位置取得
	ptr = mmioSeek(hMMIO, 0, SEEK_CUR);
	/*	ファイルアクセス終了	*/
	mmioClose(hMMIO, 0);
	if (ptr == -1) return FALSE;

	/*	ファイルオープン	*/
	hStrFile = fopen(filename, "rb");
	if (hStrFile == nullptr) return FALSE;
	//	ストリーム情報設定
	StrPos = 0;
	LoopPtr = ptr;

	fseek(hStrFile, 0L, SEEK_END);
	StrSize = ftell(hStrFile) - LoopPtr;

	StrSize = GetFileSize(hStrFile, nullptr) - LoopPtr;
	//	ファイルシーク
	fseek(hStrFile, LoopPtr, SEEK_SET);

	type = TYPE_WAV;
	Initialize(lpDS, 22050);

	/*	再生準備	*/
	SetBlockWAV(0);

	return TRUE;
}

//
//	OGG用ストリーム初期化
//

BOOL	tdnStreamSound::SetOGG(LPDIRECTSOUND lpDS, char* filename)
{
	//	ファイルオープン 
	hStrFile = fopen(filename, "rb");
	if (hStrFile == nullptr) return FALSE;

	//Oggを開く
	ov_open(hStrFile, &vf, nullptr, 0);

	//シーク可能?
	if (!ov_seekable(&vf)){
		ov_clear(&vf);
		return FALSE;
	}

	//情報設定
	const vorbis_info *info = ov_info(&vf, -1);
	Initialize(lpDS, info->rate);

	type = TYPE_OGG;
	StrPos = 0;
	StrSize = (DWORD)ov_pcm_total(&vf, -1);

	/*	再生準備	*/
	SetBlockOGG(0);

	return TRUE;
}


//**************************************************************************************************************
//	再生管理
//**************************************************************************************************************

void tdnStreamSound::Stop()
{
	if (lpStream == nullptr) return;
	if (hStrFile == nullptr) return;

	lpStream->Stop();
}

void tdnStreamSound::SetVolume(int volume)
{
	int		vol;
	if (lpStream == nullptr) return;
	/*	音量セット	*/
	if (volume > 255) volume = 255;
	if (volume < 0) volume = 0;
	this->volume = volume;
	volume -= 255;
	vol = (volume*volume * 100 / (255 * 255));
	lpStream->SetVolume(-vol*vol);
}

void tdnStreamSound::SetMode(BYTE mode, int param)
{
	this->mode = mode;
	this->param = param;
}

bool tdnStreamSound::isPlay()
{
	// そもそも死んでる
	if (!lpStream) return false;

	// ストリームさんに再生状況を聞く
	DWORD	dwAns;
	lpStream->GetStatus(&dwAns);
	return ((dwAns&DSBSTATUS_PLAYING) != 0) ? true : false;
}


//**************************************************************************************************************
//
//		サウンドマネージャ基底
//
//**************************************************************************************************************

//=============================================================================================
//		初	期	化
tdnSoundBase::tdnSoundBase()
{
	hWndWAV = tdnSystem::GetWindow();
	CoInitialize(nullptr);
	//	DirectSoundの初期化
	if (DirectSoundCreate8(nullptr, &lpDS, nullptr) != DS_OK){
		lpDS = nullptr;
		return;
	}

	lpDS->SetCooperativeLevel(hWndWAV, DSSCL_PRIORITY);

	lpPrimary = nullptr;
	DSBUFFERDESC	dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	lpDS->CreateSoundBuffer(&dsbd, &lpPrimary, nullptr);

	// 3Dリスナー作成
	lp3DListener = nullptr;
	result_sound = lpPrimary->QueryInterface(IID_IDirectSound3DListener, (LPVOID *)&lp3DListener);

	if (result_sound == E_INVALIDARG)	// プロシージャの呼び出し、または引数が不正らしい
	{
		assert(0);
	}

	this->SetListenerAll(Vector3(0, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0), Vector3(0, 0, 0));
	this->UpdateListener();

	WAVEFORMATEX   wfx;
	ZeroMemory(&wfx, sizeof(WAVEFORMATEX));
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 1;
	wfx.nSamplesPerSec = 22050;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	lpPrimary->SetFormat(&wfx);

}
//
//=============================================================================================


//=============================================================================================
//		解		放
tdnSoundBase::~tdnSoundBase()
{
	//	バッファの解放
	//for (int i = 0; i < WavNum; i++) for (UINT j = 0; j < buffer[i].size(); j++)
	//{
	//	SAFE_DELETE(buffer[i][j]);
	//}

	//	Direct Sound解放
	if (lpPrimary != nullptr) lpPrimary->Release();
	if (lpDS != nullptr) lpDS->Release();

	lpDS = nullptr;
	lpPrimary = nullptr;
}
//
//=============================================================================================

//**************************************************************************************************************
//
//		サウンドマネージャ(SE用)
//
//**************************************************************************************************************

//=============================================================================================
//		初	期	化
tdnSoundSE::tdnSoundSE()
{
	for (int i = 0; i < WavNum; i++)data[i].clear();
}
//
//=============================================================================================


//=============================================================================================
//		解		放
tdnSoundSE::~tdnSoundSE()
{
	// バッファ解放
	for (int i = 0; i < WavNum; i++)
	{
		for (UINT j = 0; j < data[i].size(); j++)
		{
			delete data[i][j]->buffer;
			delete data[i][j];
		}
	}
}
//
//=============================================================================================

//=============================================================================================
//		waveファイルセット
void tdnSoundSE::Set(int ID, int num_of_play_simultaneously, char* filename, bool b3D)
{
	//	初期化チェック
	assert(lpDS);
	//	既存のバッファの解放
	if (ID < (int)data[ID].size())
	{
		for (UINT i = 0; i < data[i].size(); i++) SAFE_DELETE(data[ID][i]);
		data[ID].clear();
	}
	assert(filename);

	if (num_of_play_simultaneously == 1)
	{	// 1個しかないならコピーする意味ないじゃない！
		SEData *set = new SEData;
		set->buffer = new tdnSoundBuffer(lpDS, filename, b3D);
		set->b3D = b3D;
		data[ID].push_back(set);
	}
	else
	{
		tdnSoundBuffer **work = new tdnSoundBuffer*[num_of_play_simultaneously];
		// 同時再生可能分のWAVファイルロード
		for (int i = 0; i < num_of_play_simultaneously; i++)
		{
			SEData *set = new SEData;
			set->buffer = new tdnSoundBuffer();
			set->b3D = b3D;
			data[ID].push_back(set);
			work[i] = data[ID][i]->buffer;	// アドレスを渡す
		}
		tdnSoundBuffer::Create_and_copy(lpDS, filename, b3D, work, 0, num_of_play_simultaneously);	// 上でworkにアドレスを渡したのでここでもうbuffer[ID]には作られている
		delete[] work;
		assert(data[ID][0]->buffer->GetBuf());
	}
}
//
//=============================================================================================

//=============================================================================================
//		再		生
int tdnSoundSE::Play(int ID, bool loop)
{
	//	初期化チェック
	assert(lpDS);
	//	データが無い！！
	assert(data[ID].size() != 0);

	for (UINT play_no = 0; play_no < data[ID].size(); play_no++)
	{
		// 再生してないからいつでも514状態の人を検索
		if (!data[ID][play_no]->buffer->isPlay())
		{	// 見つかった！
			data[ID][play_no]->buffer->Play(loop);
			return play_no;
		}
	}
	
	// 全員再生状態だったので、再生失敗
	return TDNSOUND_PLAY_NONE;
}

int tdnSoundSE::Play(int ID, const Vector3 &pos, const Vector3 &move, bool loop)
{
	MyAssert(data[ID][0]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");

	//	初期化チェック
	assert(lpDS);
	//	データが無い！！
	assert(data[ID].size() != 0);

	for (UINT play_no = 0; play_no < data[ID].size(); play_no++)
	{
		// 再生してないからいつでも514状態の人を検索
		if (!data[ID][play_no]->buffer->isPlay())
		{	// 見つかった！
			data[ID][play_no]->buffer->SetPos(pos);
			data[ID][play_no]->buffer->SetMove(move);
			data[ID][play_no]->buffer->Play(loop);
			return play_no;
		}
	}

	// 全員再生状態だったので、再生失敗
	return TDNSOUND_PLAY_NONE;
}

int tdnSoundSE::Play(int ID, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop)
{
	MyAssert(data[ID][0]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");

	//	初期化チェック
	assert(lpDS);
	//	データが無い！！
	assert(data[ID].size() != 0);

	for (UINT play_no = 0; play_no < data[ID].size(); play_no++)
	{
		// 再生してないからいつでも514状態の人を検索
		if (!data[ID][play_no]->buffer->isPlay())
		{	// 見つかった！
			data[ID][play_no]->buffer->SetAll3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, pos, front, DS3D_DEFAULTCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, move);
			data[ID][play_no]->buffer->Play(loop);
			return play_no;
		}
	}

	// 全員再生状態だったので、再生失敗
	return TDNSOUND_PLAY_NONE;
}
//
//=============================================================================================

//=============================================================================================
//		停		止
void tdnSoundSE::Stop(int ID, int no)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->Stop();
}
void tdnSoundSE::AllStop()
{
	assert(lpDS);
	for (int i = 0; i < WavNum; i++) for (UINT j = 0; j < data[i].size(); j++)if (data[i][j]->buffer->isPlay())data[i][j]->buffer->Stop();
}
//
//=============================================================================================

//=============================================================================================
//		ボリューム(-10000～0)
void tdnSoundSE::SetVolume(int ID, int volume)
{
	if (data[ID][0]->b3D)return;	// 3Dサウンドに処理を任せているのでこちら側で音はいじれない
	assert(lpDS);
	assert(data[ID].size() != 0);
	for (UINT i = 0; i < data[ID].size(); i++) data[ID][i]->buffer->SetVolume(volume);	// ID分全部設定してるが、各自設定したい場合は、また作ります。
}
void tdnSoundSE::SetVolume(int ID, float volume)
{
	SetVolume(ID, (int)(-5000 * volume));
}
int	tdnSoundSE::GetVolume(int ID)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	return data[ID][0]->buffer->GetVolume();
}
//
//=============================================================================================

//=============================================================================================
//		ステレオ(-10000～0～10000)
void	tdnSoundSE::SetPan(int ID, int pan)
{
	if (data[ID][0]->b3D)return;	// 3Dサウンドに処理を任せているのでこちら側で音はいじれない
	assert(lpDS);
	assert(data[ID].size() != 0);
	for (UINT i = 0; i < data[ID].size(); i++) data[ID][i]->buffer->SetPan(pan);
}
int		tdnSoundSE::GetPan(int ID)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	return data[ID][0]->buffer->GetPan();
}
//
//=============================================================================================

//=============================================================================================
//		周波数
void tdnSoundSE::SetFrequency(int ID, int frequency)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	for (UINT i = 0; i < data[ID].size(); i++) data[ID][i]->buffer->SetFrequency(frequency);
}
void tdnSoundSE::SetFrequency(int ID, int no, int frequency)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetFrequency(frequency);
}
int	tdnSoundSE::GetFrequency(int ID, int no)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	return data[ID][no]->buffer->GetFrequency();
}
//
//=============================================================================================

//=============================================================================================
//		再生速度(周波数いじってるだけ)
void tdnSoundSE::SetSpeed(int ID, float speed)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	for (UINT i = 0; i < data[ID].size(); i++) data[ID][i]->buffer->SetSpeed(speed);
}

void tdnSoundSE::SetSpeed(int ID, int no, float speed)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetSpeed(speed);
}
//
//=============================================================================================

//=============================================================================================
//		再生状態(再生中or停止中)
bool tdnSoundSE::isPlay(int ID, int no)
{
	assert(lpDS);
	assert(data[ID].size() != 0);
	return data[ID][no]->buffer->isPlay();
}
//
//=============================================================================================

//=============================================================================================
//		3Dサウンドでの聞こえる人情報設定
/*
DS3D_DEFERRED
アプリケーションが IDirectSound3DListener8::CommitDeferredSettings メソッドを呼び出すまで、
設定は適用されない。複数の設定を変更した後、1 回の再計算でそれらを適用できる。

DS3D_IMMEDIATE
設定を直ちに適用し、システムはすべての 3D サウンド バッファの 3D 座標を再計算する。
*/
void tdnSoundBase::SetListenerPos(const Vector3 &pos)
{
	lp3DListener->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
}
void tdnSoundBase::SetListenerOrientation(const Vector3 &front, const Vector3 &up)
{
	lp3DListener->SetOrientation(front.x, front.y, front.z, up.x, up.y, up.z, DS3D_DEFERRED);
}
void tdnSoundBase::SetListenerMove(const Vector3 &move)
{
	lp3DListener->SetVelocity(move.x, move.y, move.z, DS3D_DEFERRED);
}
void tdnSoundBase::SetListenerAll(const Vector3 &pos, const Vector3 &front, const Vector3 &up, const Vector3 &velocity)
{
	DS3DLISTENER set;
	ZeroMemory(&set, sizeof(DS3DLISTENER));
	set.dwSize = sizeof(DS3DLISTENER);

	// リスナー情報設定
	set.vPosition = D3DXVECTOR3(pos.x, pos.y, pos.z);
	set.vOrientFront = D3DXVECTOR3(front.x, front.y, front.z);
	set.vOrientTop = D3DXVECTOR3(up.x, up.y, up.z);
	set.vVelocity = D3DXVECTOR3(velocity.x, velocity.y, velocity.z);

	// サウンド計算情報設定(デフォルト値)
	set.flDistanceFactor = DS3D_DEFAULTDISTANCEFACTOR;	// ベクトル単位におけるメートル数
	set.flDopplerFactor = DS3D_DEFAULTDOPPLERFACTOR;	// ドップラー効果についての値
	set.flRolloffFactor = 0.01f;	// 距離による減衰についての値

	lp3DListener->SetAllParameters(&set, DS3D_DEFERRED);
}
void tdnSoundBase::UpdateListener()
{
	result_sound = lp3DListener->CommitDeferredSettings();
	assert(result_sound == DS_OK);
}
//
//=============================================================================================

//=============================================================================================
//		3Dサウンドでの音源情報設定
void tdnSoundSE::SetDist(int ID, int no, float max_dist, float min_dist)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetDist(max_dist, min_dist);
}
void tdnSoundSE::SetPos(int ID, int no, const Vector3 &pos)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetPos(pos);
}
void tdnSoundSE::SetFront(int ID, int no, const Vector3 &front)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetFront(front);
}
void tdnSoundSE::SetRange(int ID, int no, int degreeIn)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetRange(degreeIn);
}
void tdnSoundSE::SetOutRange_volume(int ID, int no, int out_vol)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetOutRange_volume(out_vol);
}
void tdnSoundSE::SetMove(int ID, int no, const Vector3 &move)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetMove(move);
}
void tdnSoundSE::SetAll3D(int ID, int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move)
{
	MyAssert(data[ID][no]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data[ID].size() != 0);
	data[ID][no]->buffer->SetAll3D(max_dist, min_dist, pos, front, degreeIn, out_vol, move);
}
//
//=============================================================================================

//=============================================================================================
//		エフェクトセット
void tdnSoundSE::SetFX(DXA_FX flag)
{
	for (int i = 0; i < WavNum; i++)for (UINT j = 0; j < data[i].size(); j++)data[i][j]->buffer->SetFX(flag);
}
void tdnSoundSE::SetFX(int ID, DXA_FX flag)
{
	for (UINT i = 0; i < data[ID].size(); i++)data[ID][i]->buffer->SetFX(flag);
}
//
//=============================================================================================


//**************************************************************************************************************
//
//		サウンドマネージャ(BGM用)
//
//**************************************************************************************************************

//=============================================================================================
//		初	期	化
tdnSoundBGM::tdnSoundBGM()
{
	data.clear();
	Fade_funk[(int)MODE::NONE] = &tdnSoundBGM::None;
	Fade_funk[(int)MODE::FADE_IN] = &tdnSoundBGM::In;
	Fade_funk[(int)MODE::FADE_OUT] = &tdnSoundBGM::Out;
}
//
//=============================================================================================


//=============================================================================================
//		解		放
tdnSoundBGM::~tdnSoundBGM()
{
	// バッファ解放
	for (UINT i = 0; i < data.size(); i++)
	{
		delete data[i]->buffer;
		delete data[i];
	}
	data.clear();
}
//
//=============================================================================================

//=============================================================================================
//		更		新
void tdnSoundBGM::Update()
{
	for (UINT i = 0; i < data.size(); i++)
	{
		(this->*Fade_funk[(int)data[i]->fade_mode])(i);	// モード分岐
	}
}

void tdnSoundBGM::None(int no){}
void tdnSoundBGM::In(int no)
{
	// ボリューム上げていく
	if ((data[no]->volume += data[no]->fade_speed) >= 1.0f)
	{
		// フェードしきった！
		data[no]->volume = 1.0f;
		data[no]->fade_mode = MODE::NONE;
	}
	const int vol = (int)(MinVolume * (1.0f - data[no]->volume));
	data[no]->buffer->SetVolume(vol);
}
void tdnSoundBGM::Out(int no)
{
	// ボリューム下げていく
	if ((data[no]->volume -= data[no]->fade_speed) <= 0)
	{
		// フェードしきった！
		data[no]->volume = 0;
		data[no]->fade_mode = MODE::NONE;
		data[no]->buffer->Stop();
	}
	const int vol = (int)(MinVolume * (1.0f - data[no]->volume));
	data[no]->buffer->SetVolume(vol);
}
//
//=============================================================================================

//=============================================================================================
//		waveファイルセット
void tdnSoundBGM::Set(int ID, char* filename, bool b3D)
{
	//	初期化チェック
	assert(lpDS);

	//	既存のバッファの解放
	if (ID < (int)data.size())
	{
		SAFE_DELETE(data[ID]->buffer);

		data[ID]->b3D = b3D;
		data[ID]->fade_mode = MODE::NONE;
		data[ID]->fade_speed = 0;
		data[ID]->volume = 1;
		data[ID]->buffer = new tdnSoundBuffer(lpDS, filename, b3D);
		assert(data[ID]->buffer->GetBuf());
		return;
	}

	// 情報設定
	tagBGMData *set = new tagBGMData;
	set->b3D = b3D;
	set->buffer = new tdnSoundBuffer(lpDS, filename, b3D);
	set->fade_mode = MODE::NONE;
	set->fade_speed = 0;
	set->volume = 1;
	data.push_back(set);
	assert(data[ID]->buffer->GetBuf());
}
void tdnSoundBGM::Set(int ID, BYTE *wav_data, DWORD size, LPWAVEFORMATEX wfx, bool b3D)
{
	//	初期化チェック
	assert(lpDS);
	//	既存のバッファの解放
	if (ID < (int)data.size())SAFE_DELETE(data[ID]->buffer);

	// 情報設定
	tagBGMData *set = new tagBGMData;
	set->b3D = b3D;
	set->buffer = new tdnSoundBuffer();
	set->buffer->Initialize(lpDS, wav_data, size, wfx, b3D);
	set->fade_mode = MODE::NONE;
	set->fade_speed = 0;
	set->volume = 1;
	data.push_back(set);
	assert(data[ID]->buffer->GetBuf());
}
//
//=============================================================================================

//=============================================================================================
//		再		生
void tdnSoundBGM::Play(int ID, bool loop, DWORD cursor)
{
	//	初期化チェック
	assert(lpDS);
	//	データが無い！！
	assert(data[ID]->buffer);
	data[ID]->buffer->Play(loop, cursor);
}
void tdnSoundBGM::Play(int ID, const Vector3 &pos, const Vector3 &front, const Vector3 &move, bool loop)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->buffer->SetAll3D(DS3D_DEFAULTMAXDISTANCE, DS3D_DEFAULTMINDISTANCE, pos, front, DS3D_DEFAULTCONEANGLE, DS3D_DEFAULTCONEOUTSIDEVOLUME, move);
	data[ID]->buffer->Play(loop);
}
//
//=============================================================================================


//=============================================================================================
//		停		止
void tdnSoundBGM::Stop(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->buffer->Stop();
}
void tdnSoundBGM::AllStop()
{
	assert(lpDS);
	for (UINT i = 0; i < data.size(); i++)if (data[i]->buffer->isPlay())data[i]->buffer->Stop();
}
void tdnSoundBGM::Pause(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->buffer->Pause();
}
void tdnSoundBGM::PauseOff(int ID, float FadeSpeed)
{
	assert(lpDS);
	assert(data[ID]->buffer);

	if (FadeSpeed < 1)
	{
		data[ID]->volume = 0;
		data[ID]->fade_mode = MODE::FADE_IN;
		data[ID]->fade_speed = FadeSpeed;
		data[ID]->buffer->SetVolume(DSBVOLUME_MIN);
	}
	data[ID]->buffer->PauseOff();
}
//
//=============================================================================================

//=============================================================================================
//		フェード関係
void tdnSoundBGM::FadeIn(int ID, float fade_speed, bool loop, DWORD cursor)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->volume = 0;
	data[ID]->fade_mode = MODE::FADE_IN;
	data[ID]->fade_speed = fade_speed;
	data[ID]->buffer->SetVolume(DSBVOLUME_MIN);
	data[ID]->buffer->Play(loop, cursor);
}
void tdnSoundBGM::FadeOut(int ID, float fade_speed)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->volume = 1.0f;
	data[ID]->fade_mode = MODE::FADE_OUT;
	data[ID]->fade_speed = fade_speed;
	data[ID]->buffer->SetVolume(DSBVOLUME_MAX);
}
void tdnSoundBGM::CrossFade(int inID, int outID, float fade_speed, CROSS_FADE_TYPE type, bool loop)
{
	CrossFade(inID, outID, fade_speed, fade_speed, type, loop);
}
void tdnSoundBGM::CrossFade(int inID, int outID, float in_speed, float out_speed, CROSS_FADE_TYPE type, bool loop)
{
	assert(lpDS);
	assert(data[inID]->buffer && data[outID]->buffer);

	// フェードアウト設定
	this->FadeOut(outID, out_speed);

	// フェードイン設定
	DWORD cursor;
	switch (type)
	{
	case CROSS_FADE_TYPE::NORMAL: cursor = 0;
		break;
	case CROSS_FADE_TYPE::END_OF_ETERNITY: cursor = data[outID]->buffer->GetPlayCursor();
		break;
	}
	this->FadeIn(inID, in_speed, loop, cursor);
}
//
//=============================================================================================

//=============================================================================================
//		ボリューム(-10000～0)
void tdnSoundBGM::SetVolume(int ID, int volume)
{
	if (data[ID]->b3D)return;	// 3Dサウンドに処理を任せているのでこちら側で音はいじれない
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetVolume(volume);
}
void tdnSoundBGM::SetVolume(int ID, float volume)
{
	SetVolume(ID, (int)(-5000 * volume));
}
int	tdnSoundBGM::GetVolume(int ID)
{
	assert(lpDS);
	assert(data.size() != 0);
	return data[ID]->buffer->GetVolume();
}
//
//=============================================================================================

//=============================================================================================
//		ステレオ(-10000～0～10000)
void tdnSoundBGM::SetPan(int ID, int pan)
{
	if (data[ID]->b3D)return;	// 3Dサウンドに処理を任せているのでこちら側で音はいじれない
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetPan(pan);
}
int	tdnSoundBGM::GetPan(int ID)
{
	assert(lpDS);
	assert(data.size() != 0);
	return data[ID]->buffer->GetPan();
}
//
//=============================================================================================

//=============================================================================================
//		周波数
void tdnSoundBGM::SetFrequency(int ID, int frequency)
{
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetFrequency(frequency);
}
int	tdnSoundBGM::GetFrequency(int ID, int no)
{
	assert(lpDS);
	assert(data.size() != 0);
	return data[ID]->buffer->GetFrequency();
}
//
//=============================================================================================

//=============================================================================================
//		再生速度(周波数いじってるだけ)
void tdnSoundBGM::SetSpeed(int ID, float speed)
{
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetSpeed(speed);
}

void tdnSoundBGM::SetSpeed(int ID, int no, float speed)
{
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetSpeed(speed);
}
//
//=============================================================================================

//=============================================================================================
//		再生状態(再生中or停止中)
bool tdnSoundBGM::isPlay(int ID)
{
	assert(lpDS);
	assert(data.size() != 0);
	return data[ID]->buffer->isPlay();
}
//
//=============================================================================================

//=============================================================================================
//		再生カーソル
DWORD tdnSoundBGM::GetPlayCursor(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetPlayCursor();
}
DWORD tdnSoundBGM::GetPlayFrame(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetPlayFrame();
}
int	tdnSoundBGM::GetPlaySecond(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetPlaySecond();
}
DWORD tdnSoundBGM::GetPlayMSecond(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetPlayMSecond();
}

void tdnSoundBGM::SetPlaySecond(int ID, int sec)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	data[ID]->buffer->SetPlaySecond(sec);
}


DWORD tdnSoundBGM::GetSize(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetSize();
}

int	tdnSoundBGM::GetLengthSecond(int ID)
{
	assert(lpDS);
	assert(data[ID]->buffer);
	return data[ID]->buffer->GetLengthSecond();
}
//
//=============================================================================================

//=============================================================================================
//		3Dサウンドでの音源情報設定
void tdnSoundBGM::SetDist(int ID, int no, float max_dist, float min_dist)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetDist(max_dist, min_dist);
}
void tdnSoundBGM::SetPos(int ID, int no, const Vector3 &pos)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetPos(pos);
}
void tdnSoundBGM::SetFront(int ID, int no, const Vector3 &front)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetFront(front);
}
void tdnSoundBGM::SetRange(int ID, int no, int degreeIn)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetRange(degreeIn);
}
void tdnSoundBGM::SetOutRange_volume(int ID, int no, int out_vol)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetOutRange_volume(out_vol);
}
void tdnSoundBGM::SetMove(int ID, int no, const Vector3 &move)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetMove(move);
}
void tdnSoundBGM::SetAll3D(int ID, int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move)
{
	MyAssert(data[ID]->b3D, "ERROR:b3DフラグOFFの状態で3Dサウンドを使用しています。Setのb3Dをtrueにすると解決します");
	assert(lpDS);
	assert(data.size() != 0);
	data[ID]->buffer->SetAll3D(max_dist, min_dist, pos, front, degreeIn, out_vol, move);
}
//
//=============================================================================================

//=============================================================================================
//		エフェクトセット
void tdnSoundBGM::SetFX(DXA_FX flag)
{
	for (UINT i = 0; i < data.size(); i++)data[i]->buffer->SetFX(flag);
}
void tdnSoundBGM::SetFX(int ID, DXA_FX flag)
{
	data[ID]->buffer->SetFX(flag);
}
//
//=============================================================================================

//**************************************************************************************************************
//	ストリームサウンド管理
//**************************************************************************************************************
tdnStreamSound* tdnSoundBGM::PlayStream(char* filename, bool bLoop)
{
	return PlayStream(filename, STR_NORMAL, 0, bLoop);
}

tdnStreamSound* tdnSoundBGM::PlayStream(char* filename, BYTE mode, int param, bool bLoop)
{
	// ファイル名あってるか確認
	FILE *fp;
	fopen_s(&fp, filename, "rb");
	MyAssert(fp, "tdnStreamSound: ファイル名間違えてるよ！");
	fclose(fp);

	tdnStreamSound*	lpStream;

	//	初期化チェック
	if (lpDS == nullptr)
		return nullptr;

	lpStream = new tdnStreamSound(lpDS, filename, mode, param, bLoop);
	return lpStream;
}


LPBYTE LoadWavData(LPSTR filename, LPDWORD size, LPWAVEFORMATEX wfx)
{
	HMMIO			hMMIO = nullptr;		/*	ファイルハンドル	*/
	PCMWAVEFORMAT	pwf;				/*	WAVデータ形式		*/
	MMCKINFO		ckparent, ckinfo;	/*	RIFFチャンク情報	*/
	MMIOINFO		mminfo;				/*	ファイル情報		*/
	DWORD			i;
	LPBYTE			buf = nullptr;			/*	読み込みバッファ	*/

	/* オープン	*/
	MyAssert((hMMIO = mmioOpen(filename, nullptr, MMIO_ALLOCBUF | MMIO_READ)), "エラーファイル名[%s]\n原因:wavファイルが入っていないか、wavファイル名が間違っているよ", filename);
	if (mmioDescend(hMMIO, &ckparent, nullptr, 0) != 0) goto WAVE_LoadError;
	/*	ＷＡＶ(RIFF)ファイルチェック		*/
	if ((ckparent.ckid != FOURCC_RIFF) || (ckparent.fccType != mmioFOURCC('W', 'A', 'V', 'E'))) goto WAVE_LoadError;
	/*	ｆｍｔチャンクに侵入		*/
	ckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hMMIO, &ckinfo, &ckparent, MMIO_FINDCHUNK) != 0) goto WAVE_LoadError;
	if (ckinfo.cksize < sizeof(PCMWAVEFORMAT)) goto WAVE_LoadError;
	/*	チャンクからリード	*/
	if (mmioRead(hMMIO, (HPSTR)&pwf, sizeof(pwf)) != sizeof(pwf)) goto WAVE_LoadError;
	if (pwf.wf.wFormatTag != WAVE_FORMAT_PCM) goto WAVE_LoadError;
	/*	WAVフォーマットの保存	*/
	memset(wfx, 0, sizeof(WAVEFORMATEX));
	memcpy(wfx, &pwf, sizeof(pwf));
	/*	データの読み込み	*/
	if (mmioSeek(hMMIO, ckparent.dwDataOffset + sizeof(FOURCC), SEEK_SET) == -1) goto WAVE_LoadError;
	/*	ｄａｔａチャンクに侵入		*/
	ckinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hMMIO, &ckinfo, &ckparent, MMIO_FINDCHUNK) != 0) goto WAVE_LoadError;
	if (mmioGetInfo(hMMIO, &mminfo, 0) != 0) goto WAVE_LoadError;
	/*	バッファサイズ保存	*/
	if (size != nullptr) *size = ckinfo.cksize;
	/*	ＷＡＶ用バッファの取得	*/
	buf = (LPBYTE)GlobalAlloc(LPTR, ckinfo.cksize);
	if (buf == nullptr) goto WAVE_LoadError;
	/*	データの読みとり	*/
	for (i = 0; i < ckinfo.cksize; i++){
		/*	エラーチェック	*/
		if (mminfo.pchNext >= mminfo.pchEndRead){
			if (mmioAdvance(hMMIO, &mminfo, MMIO_READ) != 0) goto WAVE_LoadError;
			if (mminfo.pchNext >= mminfo.pchEndRead) goto WAVE_LoadError;
		}
		*(buf + i) = *((LPBYTE)mminfo.pchNext);
		mminfo.pchNext++;
	}
	/*	ファイルアクセス終了	*/
	mmioSetInfo(hMMIO, &mminfo, 0);
	mmioClose(hMMIO, 0);
	return buf;

WAVE_LoadError:	/*	エラー終了	*/
	mmioClose(hMMIO, 0);
	if (buf != nullptr) GlobalFree(buf);
	return nullptr;
}

void WriteOWD(char* ReadPath, char *WritePath)
{
	// ファイル拡張子取得
	std::string e = tdnFile::GetFileExtention(ReadPath);
	char* ext = (char*)e.c_str();
		
	BYTE ver;		// OWDバージョン
	OWDInfo OWDinfo;// OWD情報
	ZeroMemory(&OWDinfo, sizeof(OWDInfo));

	// 拡張子でバージョン
	if (lstrcmpi(ext, ".wav") == 0) ver = 0;
	else if (lstrcmpi(ext, ".ogg") == 0) ver = 1;
	else  throw std::runtime_error("OWD: 対応していないファイル");

	// WAVファイルそのまま書き出しバージョン
	if (ver == 0)
	{
		// データ取得
		OWDinfo.data = LoadWavData(ReadPath, &OWDinfo.size, &OWDinfo.wfx);

		// バイナリ書き出し
		std::ofstream ofs(WritePath, std::ios::binary);

		// 何かファイルパス間違えてね？
		assert(ofs);

		ofs.write((char*)&ver, 1);

		ofs.write((char*)&OWDinfo.wfx, sizeof(WAVEFORMATEX));
		ofs.write((char*)&OWDinfo.size, 4);
		int remain = OWDinfo.size; // 書き込むべき残りのバイト数
		for (int i = 0; remain > 0; i++)
		{
			// 1024Bytesずつ書き込む(メモリパンクさせないように)
			int writeSize = min(1024, remain);
			ofs.write((char*)&OWDinfo.data[i * 1024], sizeof(char)* writeSize);
			remain -= writeSize;
		}
		//ofs.write((char*)info->data, info->size);
	}


	// OGGファイル書き出しバージョン
	else if (ver == 1)
	{
		OggVorbis_File *ovf(new OggVorbis_File());
		FILE *f;
		if (fopen_s(&f, ReadPath, "rb")) {
			throw std::runtime_error("oggファイルを開くのに失敗");
		}
		if (ov_open(f, ovf, nullptr, 0)) {
			fclose(f);
			throw std::runtime_error("oggvorbisを開くのに失敗");
		}

		const vorbis_info *info(ov_info(ovf, -1));
		if (!info) {
			throw std::runtime_error("oggフォーマットの取得に失敗");
		}

		// oggファイルから得た情報等を使ってWAVEFORMATEXを初期化
		OWDinfo.wfx.wFormatTag = WAVE_FORMAT_PCM;
		OWDinfo.wfx.nChannels = static_cast<WORD>(info->channels);
		OWDinfo.wfx.nSamplesPerSec = info->rate;
		OWDinfo.wfx.wBitsPerSample = 16;
		OWDinfo.wfx.nBlockAlign = static_cast<WORD>(info->channels * 2);
		OWDinfo.wfx.nAvgBytesPerSec = OWDinfo.wfx.nSamplesPerSec * OWDinfo.wfx.nBlockAlign;
	}
}