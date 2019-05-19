#include "TDNLIB.h"
#include "EditData.h"
#include "../MIDI_Loader/MIDI_Loader.h"
#include "../Music/PlayCursor.h"
#include "../Music/OMS_Loader.h"
#include "../Data/SelectMusicData.h"

void EditDataManager::Initialize()
{
	m_tagMusicInfo.omsInfo.NumNotes = m_tagMusicInfo.omsInfo.NumSoflans = m_tagMusicInfo.omsInfo.shift = 0;
	//m_tagMusicInfo.MinBPM = FLT_MAX;
	//m_tagMusicInfo.MaxBPM = FLT_MIN;

	m_tagMusicInfo.notes = nullptr;
	m_tagMusicInfo.soflans = nullptr;

	m_vNoteList.clear();
	m_vSoflanList.clear();
}

void EditDataManager::Release()
{
	Clear();
}

void EditDataManager::Clear()
{
	if (m_tagMusicInfo.notes)
	{
		delete[] m_tagMusicInfo.notes;
		m_tagMusicInfo.notes = nullptr;
	}
	if (m_tagMusicInfo.soflans)
	{
		delete[] m_tagMusicInfo.soflans;
		m_tagMusicInfo.soflans = nullptr;
	}

	for (auto it : m_vNoteList) delete it;
	m_vNoteList.clear();
	for (auto it : m_vSoflanList) delete it;
	m_vSoflanList.clear();
}

void EditDataManager::LoadOMS_MakeMusicScore(LPCSTR lpcFileName)
{
	// 既存バッファの開放
	Clear();

	// OMSからデータ読み込み
	MyAssert(OMS_Loader::LoadOMS(lpcFileName, &m_tagMusicInfo, OMS_Loader::OMS_VERSION::V2), "OMSロードに失敗！");

	// ★選曲情報構造体に書き込む
	SelectMusicMgr->Set(&m_tagMusicInfo);
}

void EditDataManager::LoadMIDI_MakeMusicScore(LPCSTR lpcFileName)
{
	// 既存バッファの開放
	Clear();

	/* MIDI読み込む */
	MIDILoadData data;
	MIDI_Loader::LoadMIDI(&data, lpcFileName);

	//m_tagMusicInfo.shift = 1000;

	ZeroMemory(&m_tagMusicInfo, sizeof(MusicInfo));

	/* 読み込んだMIDI情報から必要な情報を抜き取っていく */
	m_tagMusicInfo.omsInfo.division = data.sDivision;
	m_tagMusicInfo.omsInfo.BaseOctave = 60;					// デフォルトはA4

	BYTE temp(0);	 // イベント解析の際に使用する一時保存用変数
	BYTE status(0);   // ステータスバイト用の一時変数
	int cnt(0);
	unsigned int current(0);	// カーソルバーみたいな感じ
	//char tone(0);				// 音色
	BYTE max_octave = 0, min_octave = 127;	// オクターブ幅判定用
	unsigned int delta(0);	// デルタタイム
	bool bCNList[127];
	memset(bCNList, false, sizeof(bool)* 127);

	for (int no = 0; no < data.sNumTrack; no++)
	{
		printf("# Track =====================\n");
		printf("track_datas.size : %d\n", data.tagTrackDatas[no].iSize);
		printf("track_datas.data\n");

		// ★現在位置の初期化
		current = 0;

		for (int j = 0; j < data.tagTrackDatas[no].iSize; j++){   // データ分だけ繰返し
			
			delta = 0;

			// デルタタイム計測(0なら多分同時押し、もしくはノートオフとオンが重なったとか)
			while (true)
			{
				//デルタタイムの長さ
				delta = ((delta) << 7) | (data.tagTrackDatas[no].cData[j] & 0x7F);

				// 7ビット目が0ならデルタタイム終了(デルタタイムは4バイト)
				if ((data.tagTrackDatas[no].cData[j++] & 0x80) == 0x00) break;
			}

			// 先頭からの位置
			current += delta;
			if (delta > 5000)
			{
				int i = 0;
				//assert(0); //なんか変な値入ってるかも
			}

			printf("%d:: \n", delta);   // デルタタイム出力

			// ランニングステータスルールに対する処理
			if ((data.tagTrackDatas[no].cData[j] & 0x80) == 0x80){
				// ランニングステータスルールが適用されない場合は、ステータスバイト用変数を更新。
				status = (BYTE)data.tagTrackDatas[no].cData[j];   // ステータスバイトを保持しておく
			}
			else {
				printf("@\n");   // ランニングステータスルール適用のしるし
				j--;   // データバイトの直前のバイト（デルタタイムかな？）を指すようにしておく。
				// 次の処理でj++するはずなので、そうすればデータバイトにアクセスできる。
			}

			// データ判別
			if ((status & 0xf0) == 0x80){
				// ノート・オフ【ボイスメッセージ】
				printf("Note Off [%02dch] : \n", (status & 0x0f));
				j++;
				
				char lane = data.tagTrackDatas[no].cData[j];
				printf("Note%d\n", (BYTE)lane);

				// 前のレーンがCNなら
				if (bCNList[lane])
				{
					NoteInfo note;
					note.cLaneNo = lane;
					note.iEventTime = current;
					note.cNoteType = (char)NOTE_TYPE::CN_END;	// CN終了ノーツ
					/* ★PushZone */
					this->PushNote(note);
					m_tagMusicInfo.omsInfo.NumNotes++;

					// フラグリスト解除
					bCNList[lane] = false;
				}

				printf("[0x%02x] \n", (BYTE)data.tagTrackDatas[no].cData[j++]);
				printf("Velocity=%d\n", (BYTE)data.tagTrackDatas[no].cData[j]);
			}
			else if ((status & 0xf0) == 0x90){
				NoteInfo note;
				note.iEventTime = current;
				//note.tone = tone;

				// ノート・オン【ボイスメッセージ】
				printf("Note On  [%02dch] : \n", (status & 0x0f));
				j++;

				// ノート番号読み込み
				note.cLaneNo = (BYTE)data.tagTrackDatas[no].cData[j];
				printf("Note%d\n", note.cLaneNo);
				j++;

				// 番号に応じてオクターブ調整
				max_octave = max(max_octave, note.cLaneNo);
				min_octave = min(min_octave, note.cLaneNo);

				//while (note.lane < (int)m_tagMusicInfo.omsInfo.BaseOctave)
				//{
				//	m_tagMusicInfo.omsInfo.BaseOctave -= 12;
				//}
				//while (note.lane > (int)m_tagMusicInfo.omsInfo.BaseOctave + (NUM_KEYBOARD))
				//{
				//	m_tagMusicInfo.omsInfo.BaseOctave += 12;
				//}

				printf("Velocity=%d\n", (BYTE)data.tagTrackDatas[no].cData[j]);

				// ★★★velocity値が127ならCNフラグ立つ(なんでvelocityにしたかというと、ノートオンイベントと同時に判定をとりたかったから唯一、一緒に取ってこれるvelocityを使おうという事にした)
				bool bCN = (data.tagTrackDatas[no].cData[j] == 127);
				if (bCN)
				{
					MyAssert(!bCNList[note.cLaneNo], "2重CNが発生してる");
					bCNList[note.cLaneNo] = true;
					note.cNoteType = (char)NOTE_TYPE::CN_START;	// CNフラグをスタートにする
				}
				else note.cNoteType = (char)NOTE_TYPE::NORMAL;	// 通常ノーツ

				/* ★PushZone */
				this->PushNote(note);
				m_tagMusicInfo.omsInfo.NumNotes++;
			}
			else if ((status & 0xf0) == 0xa0){
				// ポリフォニック・キー・プレッシャー【ボイスメッセージ】
				printf("Polyphonic Key Pressure [%02dch] : \n", (status & 0x0f));
				j++;
				printf("Note%d\n", (BYTE)data.tagTrackDatas[no].cData[j]);
				printf("[0x%02x] \n", (BYTE)data.tagTrackDatas[no].cData[j++]);
				printf("Pressure=%d\n", (BYTE)data.tagTrackDatas[no].cData[j]);
			}
			else if ((status & 0xf0) == 0xb0){
				// コントロールチェンジ【ボイスメッセージ】、【モードメッセージ】
				printf("Control Change [%02dch] : \n", (status & 0x0f));
				j++;
				temp = (BYTE)data.tagTrackDatas[no].cData[j++];
				if (temp <= 63){   // 連続可変タイプのエフェクトに関するコントロール情報（MSBもLSBも）
					// （ホントは「0<=c && c<=63」と書きたいけど、warningが出るので「c<=63」にする）
					printf("VariableEffect(\n");
					switch (temp){
					case 0:    // 上位バイト[MSB]
					case 32:   // 下位バイト[LSB]
						printf("BankSelect[%s]\n", (temp == 0) ? "MSB\n" : "LSB\n");   // バンク・セレクト
						break;
					case 1:
					case 33:
						printf("ModulationDepth[%s]\n", (temp == 1) ? "MSB\n" : "LSB\n");   // モジュレーション・デプス
						break;
					case 2:
					case 34:
						printf("BreathType[%s]\n", (temp == 2) ? "MSB\n" : "LSB\n");   // ブレス・タイプ
						break;
					case 4:
					case 36:
						printf("FootType[%s]\n", (temp == 4) ? "MSB\n" : "LSB\n");   // フット・タイプ
						break;
					case 5:
					case 37:
						printf("PortamentoTime[%s]\n", (temp == 5) ? "MSB\n" : "LSB\n");   // ポルタメント・タイム
						break;
					case 6:
					case 38:
						printf("DataEntry[%s]\n", (temp == 6) ? "MSB\n" : "LSB\n");   // データ・エントリー
						break;
					case 7:
					case 39:
						printf("MainVolume[%s]\n", (temp == 7) ? "MSB\n" : "LSB\n");   // メイン・ボリューム
						break;
					case 8:
					case 40:
						printf("BalanceControl[%s]\n", (temp == 8) ? "MSB\n" : "LSB\n");   // バランス・コントロール
						break;
					case 10:
					case 42:
						printf("Panpot[%s]\n", (temp == 10) ? "MSB\n" : "LSB\n");   // パンポット
						break;
					case 11:
					case 43:
						printf("Expression[%s]\n", (temp == 11) ? "MSB\n" : "LSB\n");   // エクスプレッション
						break;
					case 16:
					case 17:
					case 18:
					case 19:
					case 48:
					case 49:
					case 50:
					case 51:
						printf("SomethingElse_No_%d[%s]\n", temp, (temp == 16) ? "MSB\n" : "LSB\n");   // 汎用操作子
						break;
					default:
						printf("##UndefinedType_No_%d[%s]", (temp < 32) ? temp : temp - 32, (temp < 32) ? "MSB\n" : "LSB\n");   // よくわからないコントロール
					}

					printf(")=%d\n", (BYTE)data.tagTrackDatas[no].cData[j]);
				}
				else if (120 <= temp && temp <= 127){   // モード・メッセージ
					printf("ModeMessage(\n");
					switch (temp){
					case 120:
						printf("AllSoundOff\n");   // オール・サウンド・オフ
						break;
					case 121:
						printf("ResetAllController\n");   // リセット・オール・コントローラー
						break;
					case 122:
						printf("LocalControl\n");   // ローカル・コントロール
						break;
					case 123:
						printf("AllNoteOff\n");   // オール・ノート・オフ
						break;
					case 124:
						printf("OmniOn\n");   // オムニ・オン
						break;
					case 125:
						printf("OmniOff\n");   // オムニ・オフ
						break;
					case 126:
						printf("MonoModeOn\n");   // モノモード・オン（ポリモード・オフ）
						break;
					case 127:
						printf("PolyModeOn\n");   // ポリモード・オン（モノモード・オフ）
						break;
					default:
						printf("##UndefinedType_No_%d\n", temp);   // よくわからないコントロール
					}
					printf(")=%d\n", temp);
				}
				printf("\n");
			}
			else if ((status & 0xf0) == 0xc0){
				// プログラム・チェンジ【ボイスメッセージ】
				printf("Program Change [%02dch] : \n", (status & 0x0f));
				j++;
				printf("ToneNo=%d\n", (BYTE)data.tagTrackDatas[no].cData[j]);
				//tone = data.tagTrackDatas[no].cData[j];
			}
			else if ((status & 0xf0) == 0xd0){
				// チャンネル・プレッシャー【ボイスメッセージ】
				printf("Channel Pressure [%02dch] : \n", (status & 0x0f));
				j++;
				printf("Pressure=%d\n", (BYTE)data.tagTrackDatas[no].cData[j]);
			}
			else if ((status & 0xf0) == 0xe0){
				// ピッチ・ベンド・チェンジ【ボイスメッセージ】
				printf("Pitch Bend Change [%02dch] : \n", (status & 0x0f));
				j++;
				printf(" (LSB:%d\n", (BYTE)data.tagTrackDatas[no].cData[j++]);
				printf(", MSB:%d)\n", (BYTE)data.tagTrackDatas[no].cData[j]);
			}
			else if ((status & 0xf0) == 0xf0){
				// 【システム・メッセージ】
				switch (status & 0x0f){
				case 0x00:   // エクスクルーシブ・メッセージ
					printf("F0 Exclusive Message : \n");
					j++;

					// SysExのデータ長を取得
					cnt = 0;   // 初期化
					while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){   // フラグビットが1の間はループ
						cnt = cnt | temp;   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | temp;   // 合成
					printf(" Length=%u\n", (unsigned int)cnt);   // SysExのデータ長を取得

					for (int k = 1; k <= cnt; k++){   // 長さの分だけデータ取得
						printf("[%02x]\n", (BYTE)data.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント

					break;
				case 0x01:   // MIDIタイムコード
					j++;
					break;
				case 0x02:   // ソング・ポジション・ポインタ
					j++;
					break;
				case 0x03:   // ソング・セレクト
					j++;
					break;
				case 0x07:   // エンド・オブ・エクスクルーシブでもあるけども...
					// F7ステータスの場合のエクスクルーシブ・メッセージ
					//printf("@End of Exclusive");
					printf("F7 Exclusive Message : \n");
					j++;

					// SysExのデータ長を取得
					cnt = 0;   // 初期化
					while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){   // フラグビットが1の間はループ
						cnt = cnt | temp;   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | temp;   // 合成
					printf(" Length=%u\n", (unsigned int)cnt);   // SysExのデータ長を取得

					for (int k = 1; k <= cnt; k++){   // 長さの分だけデータ取得
						printf("[%02x]\n", (BYTE)data.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント

					break;
				case 0x0F:   // メタイベント

					printf("Meta Ivent : \n");
					j++;
					switch ((BYTE)data.tagTrackDatas[no].cData[j]){
					case 0x00:   // シーケンス番号
						printf("Sequence Number=\n");
						j += 2;   // データ長の分を通り越す
						cnt = (BYTE)data.tagTrackDatas[no].cData[j++];
						cnt <<= 8;   // 8bit左シフト
						cnt = cnt | (BYTE)data.tagTrackDatas[no].cData[j];
						printf("%d\n", cnt);
						break;
					case 0x01:   // テキスト[可変長]
						printf("Text=\n");
						cnt = 0;
						j += 1;
						while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){
							cnt = cnt | (temp & 0x7F);   // 合成
							cnt <<= 7;   // 7bit左シフト
						}
						cnt = cnt | (temp & 0x7F);   // 合成
						for (int k = 1; k <= cnt; k++){
							printf("%c", data.tagTrackDatas[no].cData[j++]);
						}
						j--;   // 行き過ぎた分をデクリメント
						break;
					case 0x02:   // 著作権表示[可変長]
						printf("Copyright=\n");
						cnt = 0;
						j += 1;
						while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){
							cnt = cnt | (temp & 0x7F);   // 合成
							cnt <<= 7;   // 7bit左シフト
						}
						cnt = cnt | (temp & 0x7F);   // 合成
						for (int k = 1; k <= cnt; k++){
							printf("%c", data.tagTrackDatas[no].cData[j++]);
						}
						j--;   // 行き過ぎた分をデクリメント
						break;
					case 0x03:   // シーケンス名（曲タイトル）・トラック名[可変長]
						printf("Title=\n");
						cnt = 0;
						j += 1;
						while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){
							cnt = cnt | (temp & 0x7F);   // 合成
							cnt <<= 7;   // 7bit左シフト
						}
						cnt = cnt | (temp & 0x7F);   // 合成
						for (int k = 1; k <= cnt; k++){
							printf("%c", data.tagTrackDatas[no].cData[j++]);
						}
						j--;   // 行き過ぎた分をデクリメント
						break;
					case 0x04:   // 楽器名[可変長]
						printf("InstName=\n");
						cnt = 0;
						j += 1;
						while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){
							cnt = cnt | (temp & 0x7F);   // 合成
							cnt <<= 7;   // 7bit左シフト
						}
						cnt = cnt | (temp & 0x7F);   // 合成
						for (int k = 1; k <= cnt; k++){
							printf("%c", data.tagTrackDatas[no].cData[j++]);
						}
						j--;   // 行き過ぎた分をデクリメント
						break;
					case 0x05:   // 歌詞[可変長]
						printf("Words=\n");
						cnt = 0;
						j += 1;
						while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){
							cnt = cnt | (temp & 0x7F);   // 合成
							cnt <<= 7;   // 7bit左シフト
						}
						cnt = cnt | (temp & 0x7F);   // 合成
						for (int k = 1; k <= cnt; k++){
							printf("%c", data.tagTrackDatas[no].cData[j++]);
						}
						j--;   // 行き過ぎた分をデクリメント
						break;
					case 0x06:   // マーカー[可変長]
						printf("Marker=\n");
						cnt = 0;
						j += 1;
						while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){
							cnt = cnt | (temp & 0x7F);   // 合成
							cnt <<= 7;   // 7bit左シフト
						}
						cnt = cnt | (temp & 0x7F);   // 合成
						for (int k = 1; k <= cnt; k++){
							printf("%c", data.tagTrackDatas[no].cData[j++]);
						}
						j--;   // 行き過ぎた分をデクリメント
						break;
					case 0x07:   // キューポイント[可変長]
						printf("CuePoint=\n");
						cnt = 0;
						j += 1;
						while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){
							cnt = cnt | (temp & 0x7F);   // 合成
							cnt <<= 7;   // 7bit左シフト
						}
						cnt = cnt | (temp & 0x7F);   // 合成
						for (int k = 1; k <= cnt; k++){
							printf("%c", data.tagTrackDatas[no].cData[j++]);
						}
						j--;   // 行き過ぎた分をデクリメント
						break;
					case 0x08:   // プログラム名（音色名）[可変長]
						printf("ProgramName=\n");
						cnt = 0;
						j += 1;
						while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){
							cnt = cnt | (temp & 0x7F);   // 合成
							cnt <<= 7;   // 7bit左シフト
						}
						cnt = cnt | (temp & 0x7F);   // 合成
						for (int k = 1; k <= cnt; k++){
							printf("%c", data.tagTrackDatas[no].cData[j++]);
						}
						j--;   // 行き過ぎた分をデクリメント
						break;
					case 0x09:   // デバイス名（音源名）[可変長]
						printf("DeviceName=\n");
						cnt = 0;
						j += 1;
						while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){
							cnt = cnt | (temp & 0x7F);   // 合成
							cnt <<= 7;   // 7bit左シフト
						}
						cnt = cnt | (temp & 0x7F);   // 合成
						for (int k = 1; k <= cnt; k++){
							printf("%c", data.tagTrackDatas[no].cData[j++]);
						}
						j--;   // 行き過ぎた分をデクリメント
						break;
					case 0x20:   // MIDIチャンネルプリフィックス[1byte]
						printf("MidiChannelPrefix=\n");
						j += 2;   // データ長の分を通り越す
						cnt = (BYTE)data.tagTrackDatas[no].cData[j];
						printf("%d\n", cnt);
						break;
					case 0x21:   // ポート指定[1byte]
						printf("Port=\n");
						j += 2;   // データ長の分を通り越す
						cnt = (BYTE)data.tagTrackDatas[no].cData[j];
						printf("%d\n", cnt);
						break;
					case 0x2F:   // トラック終端[0byte]
						printf("End of Track\n");
						j += 1;   // データ長の分を通り越す

						// ★よくわからんけど、これで直ることもある
						//current -= delta;

						break;
					case 0x51:   // テンポ設定[3byte]

						printf("Temp=\n");
						j += 2;   // データ長の分を通り越す
						cnt = (BYTE)data.tagTrackDatas[no].cData[j++];
						cnt <<= 8;   // 8bit左シフト
						cnt = cnt | (BYTE)data.tagTrackDatas[no].cData[j++];
						cnt <<= 8;   // 8bit左シフト
						cnt = cnt | (BYTE)data.tagTrackDatas[no].cData[j];
						printf("%d\n", cnt);

						{
							SoflanInfo soflan;
							soflan.iEventTime = current;

							// BPM割り出し
							soflan.fBPM = 60000000 / (float)cnt;

							// 小数点第1で切り捨て処理(10にすると第一位、100にすると第2位まで切り捨てられる)
							soflan.fBPM = floorf(soflan.fBPM * 10) / 10;

							// リストにぶちこむ/*PushZone*/
							this->PushSoflan(soflan);
							m_tagMusicInfo.omsInfo.NumSoflans++;
						}

						// ★よくわからんけど、これで直ることもある
						//current -= delta;

						break;
					case 0x54:   // SMPTEオフセット[5byte]
						printf("SMPTE_Offset=\n");
						j += 2;   // データ長の分を通り越す
						cnt = (BYTE)data.tagTrackDatas[no].cData[j++];
						switch (cnt & 0xC0){   // フレーム速度
						case 0x00:
							printf("24fps\n");
							break;
						case 0x01:
							printf("25fps\n");
							break;
						case 0x10:
							printf("30fps(DropFrame)\n");
							break;
						case 0x11:
							printf("30fps\n");
							break;
						}
						printf(",Hour:%d\n", (cnt & 0x3F));   // 時間
						printf(",Min:%d\n", (BYTE)data.tagTrackDatas[no].cData[j++]);   // 分
						printf(",Sec:%d\n", (BYTE)data.tagTrackDatas[no].cData[j++]);   // 秒
						printf(",Frame:%d\n", (BYTE)data.tagTrackDatas[no].cData[j++]);   // フレーム
						printf(",SubFrame:%d\n", (BYTE)data.tagTrackDatas[no].cData[j]);   // サブフレーム
						break;
					case 0x58:   // 拍子設定[4byte]
					{
						printf("Rhythm=\n");
						j += 2;   // データ長の分を通り越す
						int a = data.tagTrackDatas[no].cData[j];
						printf("%d\n", (BYTE)data.tagTrackDatas[no].cData[j++]);
						cnt = 1;
						for (int k = 0; k < (int)data.tagTrackDatas[no].cData[j]; k++) {   // 拍子の分母を算出する
							cnt *= 2;   // 2の累乗
						}
						printf("/%d\n", cnt);
						j++;
						a = data.tagTrackDatas[no].cData[j];
						printf(" ClockPerBeat=%d\n", (BYTE)data.tagTrackDatas[no].cData[j++]);
						a = data.tagTrackDatas[no].cData[j];
						printf(" 32NotePer4Note=%d\n", (BYTE)data.tagTrackDatas[no].cData[j]);
					}
						break;
					case 0x59:   // 調設定[2byte]
						printf("Key=\n");
						j += 2;   // データ長の分を通り越す
						cnt = (int)data.tagTrackDatas[no].cData[j++];
						if (temp > 0){
							printf("Sharp[%d]\n", temp);
						}
						else if (temp == 0){
							printf("C\n");
						}
						else {
							printf("Flat[%d]\n", temp);
						}
						cnt = (int)data.tagTrackDatas[no].cData[j];
						if (temp == 0){
							printf("_Major\n");
						}
						else {
							printf("_Minor\n");
						}

						break;
					case 0x7F:   // シーケンサ特定メタイベント
						printf("SpecialIvent=\n");
						cnt = 0;
						j += 1;
						while ((temp = (BYTE)data.tagTrackDatas[no].cData[j++]) & 0x80){
							cnt = cnt | (temp & 0x7F);   // 合成
							cnt <<= 7;   // 7bit左シフト
						}
						cnt = cnt | (temp & 0x7F);   // 合成
						for (int k = 1; k <= cnt; k++){
							printf("[%02x]\n", (BYTE)data.tagTrackDatas[no].cData[j++]);
						}
						j--;   // 行き過ぎた分をデクリメント
						break;
					}
					break;
				}
			}
		}
	}

	// データ解放
	for (int i = 0; i < data.sNumTrack; i++)
	{
		delete data.tagTrackDatas[i].cData;
	}
	delete data.tagTrackDatas;

	// ノーツ数とソフラン数に応じて実体作成
	m_tagMusicInfo.notes = new NoteInfo[m_tagMusicInfo.omsInfo.NumNotes];
	m_tagMusicInfo.soflans = new SoflanInfo[m_tagMusicInfo.omsInfo.NumSoflans];

	int i(0);
	for (auto it : m_vNoteList)
	{
		memcpy_s(&m_tagMusicInfo.notes[i], sizeof(NoteInfo), it, sizeof(NoteInfo));
		i++;
	}
	i = 0;
	for (auto it : m_vSoflanList)
	{
		memcpy_s(&m_tagMusicInfo.soflans[i], sizeof(SoflanInfo), it, sizeof(SoflanInfo));
		i++;
	}

	for (int i = 0;; i += 12)
	{
		if (i > max_octave)
		{
			max_octave = i;
			break;
		}
	}
	for (int i = 120;; i -= 12)
	{
		if (i <= min_octave)
		{
			min_octave = i;
			break;
		}
	}

	const char OctaveWidth = max_octave - min_octave;
	if (OctaveWidth == 12)
	{
		m_tagMusicInfo.omsInfo.OctaveWidth = 1;
		m_tagMusicInfo.omsInfo.BaseOctave = min_octave - 12;
	}
	else if (OctaveWidth == 24)
	{
		m_tagMusicInfo.omsInfo.OctaveWidth = 2;
		m_tagMusicInfo.omsInfo.BaseOctave = min_octave - 12;
	}
	else if (OctaveWidth == 36)
	{
		m_tagMusicInfo.omsInfo.OctaveWidth = 3;
		m_tagMusicInfo.omsInfo.BaseOctave = min_octave;
	}
	else if (OctaveWidth == 48)
	{
		m_tagMusicInfo.omsInfo.OctaveWidth = 4;
		m_tagMusicInfo.omsInfo.BaseOctave = min_octave;
	}
	else MessageBox(0, "MIDIで配置したオクターブ幅が譜面幅を超えています。\nこのまま書き出すといくつかのノーツが失われるので、オクターブの幅を狭くしてください", nullptr, MB_OK);

	// ★選曲情報構造体に書き込む
	SelectMusicMgr->Set(&m_tagMusicInfo);
}


void EditDataManager::Sort()
{
	//// ラムダ式
	//auto Ascending = [](EditData::Base *val1, EditData::Base *val2){
	//	return (val1->GetEventTime() < val2->GetEventTime());
	//};
	//
	//for (int i = 0; i < (int)LIST_TYPE::MAX; i++)
	//{
	//	if (Lists[i].size() <= 1)return;
	//	Lists[i].sort(Ascending); // 昇順に並べる
	//}
}


void EditDataManager::WriteMusicScore(LPCSTR lpcFileName)
{
	// 一旦ソート(イベント時間順に)
	//Sort();

	/*
	out 書き込み専用に開く。デフォルト。 
	app 追記用に開く。 
	ate ファイルを開くと、自動的に末尾に移動する。 
	trunc ファイルを開くと、以前の内容を全て削除する。 
	binary 特殊な文字を置換しない。 
	*/

	// binary write
	if (true)
	{
		// 書き出しくん
		std::ofstream ofs(lpcFileName, std::ios::binary);

		MyAssert(ofs,"OMS書き出しに失敗！");	// 何かしらの例外が生じて書き込めない(パス間違えてるとか)

		/* ヘッダチャンク作成 */
		//ofs << "OMhd";
		ofs.write("OMhd", sizeof(char[4]));
		//ofs.write((char*)&m_tagMusicInfo.music_name, sizeof(char[64]));	// 曲名
		//ofs.write((char*)&m_tagMusicInfo.PlayMusicNo, 4);				// フォルダの中の再生する曲の番号
		//ofs.write((char*)&m_tagMusicInfo.PlayTime, 4);					// 曲なしの場合、曲から再生時間を取れないので再生時間を設定させる
		//ofs.write((char*)&m_tagMusicInfo.shift, 4);						// 曲と譜面との差を調整する
		//ofs.write((char*)&m_tagMusicInfo.division, 4);					// 分能値
		//ofs.write((char*)&m_tagMusicInfo.MaxBPM, sizeof(float));		// 最大BPM
		//ofs.write((char*)&m_tagMusicInfo.MinBPM, sizeof(float));		// 最小BPM
		//ofs.write((char*)&m_tagMusicInfo.num_notes, 4);					// ノート数
		//ofs.write((char*)&m_tagMusicInfo.num_soflans, 4);				// ソフラン数

		// ★ビットフィールド構造体でいっぺんに書き出す
		ofs.write((char*)&m_tagMusicInfo.omsInfo, sizeof(OMSInfoV2));

		/* データチャンク作成 */
		//ofs << "OMdt";
		ofs.write("OMdt", sizeof(char[4]));

		// まずノート
		for (auto it : m_vNoteList)
		{
			if (it->cLaneNo > 256)
			{
				MessageBox(0, "ノーツの値がおかしいです。もう一度書いてみてください", "エラー", MB_OK);
				return;
			}
			ofs.write((char*)&it->iEventTime, 4);		// イベント時間
			ofs.write((char*)&it->cLaneNo, 1);			// レーン番号
			ofs.write((char*)&it->cNoteType, 1);			// CNフラグ
		}

		// BPM変更情報
		for (auto it : m_vSoflanList)
		{
			if (it->fBPM < 0)
			{
				MessageBox(0, "ソフランの値がおかしいです。もう一度書いてみてください", "エラー", MB_OK);
				return;
			}
			ofs.write((char*)&it->iEventTime, 4);			// イベント時間
			ofs.write((char*)&it->fBPM, sizeof(float));		// ソフラン
		}

		// 最後にwavをぶちこむ
		//unsigned long wav_size;
		//WAVEFORMATEX wfx;
		//BYTE *buf = LoadWavData(wav_path, &wav_size, &wfx);
		//
		//ofs.write((char*)&wav_size, sizeof(long));			// wavデータの量
		//ofs.write((char*)&wfx, sizeof(WAVEFORMATEX));		// wavデータフォーマット
		//
		//// いっぺんにwavデータ書き込むのは危なっかしいので分けて書き込む
		//int remain = wav_size; // 書き込むべき残りのバイト数
		//while (remain > 0)
		//{
		//	// 1024Bytesずつ書き込む(メモリパンクさせないように)
		//	int writeSize = min(1024, remain);
		//	ofs.write((char*)buf, sizeof(char)* writeSize);
		//	remain -= writeSize;
		//}
		//ofs.write((char*)buf, sizeof(BYTE)*wav_size);
	}

	else
	{
		assert(0);
		//// 書き出しくん
		//std::ofstream ofs(filename);
		//
		///* ヘッダチャンク作成 */
		//ofs << "OMhd\n";
		//ofs << "TITLE:" << m_tagMusicInfo.music_name << "\n";
		//ofs << "MAX_BPM:" << m_tagMusicInfo.max_bpm << "\n";
		//ofs << "MIN_BPM:" << m_tagMusicInfo.min_bpm << "\n";
		//ofs << "NOTES:" << m_tagMusicInfo.num_notes << "\n";
		//ofs << "BPM_CHANGES:" << m_tagMusicInfo.num_soflans << "\n";
		//
		///* データチャンク作成 */
		//ofs << "OMdt\n";
		//
		//// まずノート
		//ofs << "NOTE_ON:\n";
		//for (auto it : Lists[(int)LIST_TYPE::NOTE])
		//{
		//	ofs << it->GetDataTime() << " ";
		//	int lane;
		//	it->GetValue(&lane);
		//	ofs << lane << "\n";
		//}
		//
		//// BPM変更情報
		//ofs << "\nCHANGE_BPM:\n";
		//for (auto it : Lists[(int)LIST_TYPE::SOFLAN])
		//{
		//	ofs << it->GetDataTime() << " ";
		//	float bpm;
		//	it->GetValue(&bpm);
		//	ofs << bpm << "\n";
		//}
	}

}