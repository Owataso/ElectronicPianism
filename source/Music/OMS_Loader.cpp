#include	"tdnlib.h"
#include	"OMS_Loader.h"
#include	"../Music/MusicInfo.h"
#include	"PlayCursor.h"
//*****************************************************************************************************************************
//	OMSファイル読み込み
//*****************************************************************************************************************************
int OMS_Loader::GetOctaveWidth(char *filename)
{
	// 読み出しくん
	std::ifstream ifs(filename, std::ios::binary);
	if (!ifs) return 0;	// オープンに失敗

	/* ヘッダチャンク侵入 */
	char work[5];
	ifs.read(work, 4);										// ヘッダチャンクであることを示す"OMhd"という4文字のアスキーコード
	assert(
		work[0] == 'O'&&
		work[1] == 'M'&&
		work[2] == 'h'&&
		work[3] == 'd'
		);

	// 譜面データの情報だけを読み込み(ノーツとかは読み込まない)
	OMSInfoV2 info;
	ifs.read((char*)&info, sizeof(OMSInfoV2));

	// その譜面のオクターブ数を返す
	return info.OctaveWidth;
}


bool OMS_Loader::LoadOMS(LPCSTR filename, MusicInfo *out, OMS_VERSION version)
{
	// 読み出しくん
	std::ifstream ifs(filename, std::ios::binary);
	//MyAssert(ifs,"OMSファイルオープンに失敗");
	if (!ifs) return false;	// オープンに失敗

	/* ヘッダチャンク侵入 */
	char work[5];
	ifs.read(work, 4);										// ヘッダチャンクであることを示す"OMhd"という4文字のアスキーコード
	assert(
		work[0] == 'O'&&
		work[1] == 'M'&&
		work[2] == 'h'&&
		work[3] == 'd'
		);

	//infs.read((char*)out->music_name, sizeof(char[64]));	// 曲名
	//infs.read((char*)&out->shift, 4);						// 曲と譜面との差を調整する
	//infs.read((char*)&out->division, 4);					// 分能値
	//infs.read((char*)&out->MaxBPM, sizeof(float));			// 最大BPM
	//infs.read((char*)&out->MinBPM, sizeof(float));			// 最少BPM
	//infs.read((char*)&out->num_notes, 4);					// ノート数
	//infs.read((char*)&out->num_soflans, 4);					// ソフラン数

	// ★ビットフィールド構造体でいっぺんに読み出す
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

	/* データチャンク侵入 */
	ifs.read(work, 4);										// データチャンクであることを示す"OMdt"という4文字のアスキーコード
	assert(
		work[0] == 'O'&&
		work[1] == 'M'&&
		work[2] == 'd'&&
		work[3] == 't'
		);

	// まずノート
	out->notes = new NoteInfo[out->omsInfo.NumNotes];
	for (UINT i = 0; i < out->omsInfo.NumNotes; i++)
	{
		ifs.read((char*)&out->notes[i].iEventTime, 4);		// イベント時間
		ifs.read((char*)&out->notes[i].cLaneNo, 1);			// レーン番号
		ifs.read((char*)&out->notes[i].cNoteType, 1);		// CNフラグ
		//MyAssert(out->notes[i].lane >= 0 && out->notes[i].lane < NUM_KEYBOARD, "セーブデータのノーツ情報がおかしい");

		//tdnDebug::OutPutLog("%d\n", out->notes[i].iEventTime);
	}

	// BPM変更情報
	out->soflans = new SoflanInfo[out->omsInfo.NumSoflans];
	for (UINT i = 0; i < out->omsInfo.NumSoflans; i++)
	{
		ifs.read((char*)&out->soflans[i].iEventTime, 4);			// イベント時間
		ifs.read((char*)&out->soflans[i].fBPM, sizeof(float));		// ソフランBPM
	}

	return true;

	// 最後にwavデータ
	//unsigned long wav_size;
	//WAVEFORMATEX wfx;
	//infs.read((char*)&wav_size, sizeof(long));				// wavデータの量
	//infs.read((char*)&wfx, sizeof(WAVEFORMATEX));			// wavフォーマット
	//
	//unsigned char *buf = new unsigned char[wav_size];

	// いっぺんにwavデータ読み込むのは危なっかしいので分けて読み込む
	//int remain = wav_size; // 読み込むべき残りのバイト数
	//while (remain > 0)
	//{
	//	// 1024Bytesずつ書き込む(メモリパンクさせないように)
	//	int readSize = (1024 < remain) ? 1024 : remain;
	//	infs.read((char*)buf, sizeof(unsigned char)* readSize);
	//	remain -= readSize;
	//}
	//infs.read((char*)buf, wav_size);

	// 音源データ設定
	//play_cursor->SetMusic(buf, wav_size, &wfx);
}