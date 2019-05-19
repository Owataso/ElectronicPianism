#include "TDNLIB.h"
#include "MIDI_Loader.h"
#include "../Data/EditData.h"

void MIDI_Loader::LoadMIDI( MIDILoadData *out, LPCSTR lpcFileName)
{
	std::ifstream infs(lpcFileName, std::ios::binary); // input file stream

	MyAssert(infs, "MIDIファイルオープンに失敗。ファイル名が間違っているか、ファイルが存在しないよ\nFileName:%s", lpcFileName);	// ファイル名が間違ってるか、ファイルが存在しない

	// エンディアン判定
	ENDIAN endian = tdnSystem::GetEndian();

	/* ヘッダチャンク侵入 */
	char l_szType[4];
	infs.read(l_szType, 4);			// ヘッダチャンクであることを示す"MThd"という4文字のアスキーコード 「4D 54 68 64」(4バイト)
	MyAssert(
		l_szType[0] == 'M'&&
		l_szType[1] == 'T'&&
		l_szType[2] == 'h'&&
		l_szType[3] == 'd', "MIDIファイルがうまく読み込まれませんでした");

	infs.read((char*)&out->iHeadChunkSize, 4);		// ヘッダチャンクのデータ長 「00 00 00 06」(4バイト)
	infs.read((char*)&out->sFormat, 2);				// フォーマット「00 00」（フォーマットタイプが0の場合） (2バイト)
	infs.read((char*)&out->sNumTrack, 2);			// トラック数 「00 01」（トラック数が1個の場合）(2バイト)
	infs.read((char*)&out->sDivision, 2);			// 分解能(デルタタイム)	四分音符の長さ(デフォルトだと480)

	// 必要ならエンディアン変換
	if (endian == LITTLE_ENDIAN){   // リトルエンディアンなら要変換
		// エンディアン変換処理
		tdnSystem::ConvertEndian(&out->iHeadChunkSize, 4);
		tdnSystem::ConvertEndian(&out->sFormat, 2);
		(out->sFormat == 0) ? out->sNumTrack = 1 : tdnSystem::ConvertEndian(&out->sNumTrack, 2);
		tdnSystem::ConvertEndian(&out->sDivision, 2);
	}

	out->tagTrackDatas = new Track[out->sNumTrack];

	for (int i = 0; i < out->sNumTrack; i++)   // トラック数だけ繰返し
	{
		/* トラックチャンク侵入 */
		infs.read(l_szType, 4);				// トラックチャンクであることを示す識別子"MTrk" 「4D 54 72 6B」(4バイト)

		MyAssert(
			l_szType[0] == 'M'&&
			l_szType[1] == 'T'&&
			l_szType[2] == 'r'&&
			l_szType[3] == 'k',"MIDIファイルのトラックイベントががうまく読み込まれませんでした");

		infs.read((char*)&out->tagTrackDatas[i].iSize, 4);		// データ長（このトラックに入っているデータセクションのデータの長さ（16進法データの個数）。） (4バイト)

		if (endian == LITTLE_ENDIAN)   // リトルエンディアンなら要変換
			tdnSystem::ConvertEndian(&out->tagTrackDatas[i].iSize, 4);

		// データ領域確保
		out->tagTrackDatas[i].cData = new char[out->tagTrackDatas[i].iSize];

		// データセクション（可変長）データセクションの長さはデータ長で示される。データ長を数え誤って入力するなどデータ構成の整合性を欠くと標準MIDIファイルは演奏されない。
		infs.read(out->tagTrackDatas[i].cData, out->tagTrackDatas[i].iSize);
	}

	//// 読み取ったトラックチャンク情報を出力
	//for (int i = 0; i < tracks; i++)   // トラック数だけ繰返し
	//	LoadTrack(*tagTrackDatas[i], division);
	//
	//// データ解放
	//for (int i = 0; i < tracks; i++)
	//{
	//	delete tagTrackDatas[i]->data;
	//	delete tagTrackDatas[i];
	//}
	//delete[] tagTrackDatas;
}

void MIDI_Loader::LoadTrack(MIDILoadData &track, int no)
{
	//BYTE temp(0);	 // イベント解析の際に使用する一時保存用変数
	//BYTE status(0);   // ステータスバイト用の一時変数
	//int cnt(0);
	//unsigned int delta(0);
	//unsigned int current(0);	// カーソルバーみたいな感じ

	//printf("# Track =====================\n");
	//printf("track_datas.size : %d\n", track.tagTrackDatas[no].iSize);
	//printf("track_datas.data\n");
	//for (int j = 0; j<track.tagTrackDatas[no].iSize; j++){   // データ分だけ繰返し

	//	delta = 0;   // 初期化

	//	// デルタタイム計測(0なら多分同時押し、もしくはノートオフとオンが重なったとか)
	//	while (true)
	//	{
	//		//デルタタイムの長さ
	//		delta = ((delta) << 7) | (track.tagTrackDatas[no].cData[j] & 0x7F);

	//		// 7ビット目が0ならデルタタイム終了(デルタタイムは4バイト)
	//		if ((track.tagTrackDatas[no].cData[j++] & 0x80) == 0x00) break;
	//	}

	//	// 先頭からの位置
	//	current += delta;

	//	printf("%d:: \n", delta);   // デルタタイム出力

	//	// ランニングステータスルールに対する処理
	//	if ((track.tagTrackDatas[no].cData[j] & 0x80) == 0x80){
	//		// ランニングステータスルールが適用されない場合は、ステータスバイト用変数を更新。
	//		status = (BYTE)track.tagTrackDatas[no].cData[j];   // ステータスバイトを保持しておく
	//	}
	//	else {
	//		printf("@\n");   // ランニングステータスルール適用のしるし
	//		j--;   // データバイトの直前のバイト（デルタタイムかな？）を指すようにしておく。
	//		// 次の処理でj++するはずなので、そうすればデータバイトにアクセスできる。
	//	}

	//	// データ判別
	//	if ((status & 0xf0) == 0x80){
	//		// ノート・オフ【ボイスメッセージ】
	//		printf("Note Off [%02dch] : \n", (status & 0x0f));
	//		j++;
	//		printf("Note%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
	//		printf("[0x%02x] \n", (BYTE)track.tagTrackDatas[no].cData[j++]);
	//		printf("Velocity=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
	//	}
	//	else if ((status & 0xf0) == 0x90){
	//		// ノート・オン【ボイスメッセージ】
	//		printf("Note On  [%02dch] : \n", (status & 0x0f));
	//		j++;

	//		int lane = (BYTE)track.tagTrackDatas[no].cData[j];
	//		printf("Note%d\n", lane);
	//		j++;

	//		printf("Velocity=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);

	//		event_mng->PushNoteOn(current, lane);
	//	}
	//	else if ((status & 0xf0) == 0xa0){
	//		// ポリフォニック・キー・プレッシャー【ボイスメッセージ】
	//		printf("Polyphonic Key Pressure [%02dch] : \n", (status & 0x0f));
	//		j++;
	//		printf("Note%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
	//		printf("[0x%02x] \n", (BYTE)track.tagTrackDatas[no].cData[j++]);
	//		printf("Pressure=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
	//	}
	//	else if ((status & 0xf0) == 0xb0){
	//		// コントロールチェンジ【ボイスメッセージ】、【モードメッセージ】
	//		printf("Control Change [%02dch] : \n", (status & 0x0f));
	//		j++;
	//		temp = (BYTE)track.tagTrackDatas[no].cData[j++];
	//		if (temp <= 63){   // 連続可変タイプのエフェクトに関するコントロール情報（MSBもLSBも）
	//			// （ホントは「0<=c && c<=63」と書きたいけど、warningが出るので「c<=63」にする）
	//			printf("VariableEffect(\n");
	//			switch (temp){
	//			case 0:    // 上位バイト[MSB]
	//			case 32:   // 下位バイト[LSB]
	//				printf("BankSelect[%s]\n", (temp == 0) ? "MSB\n" : "LSB\n");   // バンク・セレクト
	//				break;
	//			case 1:
	//			case 33:
	//				printf("ModulationDepth[%s]\n", (temp == 1) ? "MSB\n" : "LSB\n");   // モジュレーション・デプス
	//				break;
	//			case 2:
	//			case 34:
	//				printf("BreathType[%s]\n", (temp == 2) ? "MSB\n" : "LSB\n");   // ブレス・タイプ
	//				break;
	//			case 4:
	//			case 36:
	//				printf("FootType[%s]\n", (temp == 4) ? "MSB\n" : "LSB\n");   // フット・タイプ
	//				break;
	//			case 5:
	//			case 37:
	//				printf("PortamentoTime[%s]\n", (temp == 5) ? "MSB\n" : "LSB\n");   // ポルタメント・タイム
	//				break;
	//			case 6:
	//			case 38:
	//				printf("DataEntry[%s]\n", (temp == 6) ? "MSB\n" : "LSB\n");   // データ・エントリー
	//				break;
	//			case 7:
	//			case 39:
	//				printf("MainVolume[%s]\n", (temp == 7) ? "MSB\n" : "LSB\n");   // メイン・ボリューム
	//				break;
	//			case 8:
	//			case 40:
	//				printf("BalanceControl[%s]\n", (temp == 8) ? "MSB\n" : "LSB\n");   // バランス・コントロール
	//				break;
	//			case 10:
	//			case 42:
	//				printf("Panpot[%s]\n", (temp == 10) ? "MSB\n" : "LSB\n");   // パンポット
	//				break;
	//			case 11:
	//			case 43:
	//				printf("Expression[%s]\n", (temp == 11) ? "MSB\n" : "LSB\n");   // エクスプレッション
	//				break;
	//			case 16:
	//			case 17:
	//			case 18:
	//			case 19:
	//			case 48:
	//			case 49:
	//			case 50:
	//			case 51:
	//				printf("SomethingElse_No_%d[%s]\n", temp, (temp == 16) ? "MSB\n" : "LSB\n");   // 汎用操作子
	//				break;
	//			default:
	//				printf("##UndefinedType_No_%d[%s]", (temp<32) ? temp : temp - 32, (temp<32) ? "MSB\n" : "LSB\n");   // よくわからないコントロール
	//			}

	//			printf(")=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
	//		}
	//		else if (120 <= temp && temp <= 127){   // モード・メッセージ
	//			printf("ModeMessage(\n");
	//			switch (temp){
	//			case 120:
	//				printf("AllSoundOff\n");   // オール・サウンド・オフ
	//				break;
	//			case 121:
	//				printf("ResetAllController\n");   // リセット・オール・コントローラー
	//				break;
	//			case 122:
	//				printf("LocalControl\n");   // ローカル・コントロール
	//				break;
	//			case 123:
	//				printf("AllNoteOff\n");   // オール・ノート・オフ
	//				break;
	//			case 124:
	//				printf("OmniOn\n");   // オムニ・オン
	//				break;
	//			case 125:
	//				printf("OmniOff\n");   // オムニ・オフ
	//				break;
	//			case 126:
	//				printf("MonoModeOn\n");   // モノモード・オン（ポリモード・オフ）
	//				break;
	//			case 127:
	//				printf("PolyModeOn\n");   // ポリモード・オン（モノモード・オフ）
	//				break;
	//			default:
	//				printf("##UndefinedType_No_%d\n", temp);   // よくわからないコントロール
	//			}
	//			printf(")=%d\n", temp, (BYTE)track.tagTrackDatas[no].cData[j]);
	//		}
	//		printf("\n");
	//	}
	//	else if ((status & 0xf0) == 0xc0){
	//		// プログラム・チェンジ【ボイスメッセージ】
	//		printf("Program Change [%02dch] : \n", (status & 0x0f));
	//		j++;
	//		printf("ToneNo=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
	//	}
	//	else if ((status & 0xf0) == 0xd0){
	//		// チャンネル・プレッシャー【ボイスメッセージ】
	//		printf("Channel Pressure [%02dch] : \n", (status & 0x0f));
	//		j++;
	//		printf("Pressure=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
	//	}
	//	else if ((status & 0xf0) == 0xe0){
	//		// ピッチ・ベンド・チェンジ【ボイスメッセージ】
	//		printf("Pitch Bend Change [%02dch] : \n", (status & 0x0f));
	//		j++;
	//		printf(" (LSB:%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
	//		printf(", MSB:%d)\n", (BYTE)track.tagTrackDatas[no].cData[j]);
	//	}
	//	else if ((status & 0xf0) == 0xf0){
	//		// 【システム・メッセージ】
	//		switch (status & 0x0f){
	//		case 0x00:   // エクスクルーシブ・メッセージ
	//			printf("F0 Exclusive Message : \n");
	//			j++;

	//			// SysExのデータ長を取得
	//			cnt = 0;   // 初期化
	//			while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){   // フラグビットが1の間はループ
	//				cnt = cnt | temp;   // 合成
	//				cnt <<= 7;   // 7bit左シフト
	//			}
	//			cnt = cnt | temp;   // 合成
	//			printf(" Length=%u\n", (unsigned int)cnt);   // SysExのデータ長を取得

	//			for (int k = 1; k <= cnt; k++){   // 長さの分だけデータ取得
	//				printf("[%02x]\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
	//			}
	//			j--;   // 行き過ぎた分をデクリメント

	//			break;
	//		case 0x01:   // MIDIタイムコード
	//			j++;
	//			break;
	//		case 0x02:   // ソング・ポジション・ポインタ
	//			j++;
	//			break;
	//		case 0x03:   // ソング・セレクト
	//			j++;
	//			break;
	//		case 0x07:   // エンド・オブ・エクスクルーシブでもあるけども...
	//			// F7ステータスの場合のエクスクルーシブ・メッセージ
	//			//printf("@End of Exclusive");
	//			printf("F7 Exclusive Message : \n");
	//			j++;

	//			// SysExのデータ長を取得
	//			cnt = 0;   // 初期化
	//			while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){   // フラグビットが1の間はループ
	//				cnt = cnt | temp;   // 合成
	//				cnt <<= 7;   // 7bit左シフト
	//			}
	//			cnt = cnt | temp;   // 合成
	//			printf(" Length=%u\n", (unsigned int)cnt);   // SysExのデータ長を取得

	//			for (int k = 1; k <= cnt; k++){   // 長さの分だけデータ取得
	//				printf("[%02x]\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
	//			}
	//			j--;   // 行き過ぎた分をデクリメント

	//			break;
	//		case 0x0F:   // メタイベント
	//			printf("Meta Ivent : \n");
	//			j++;
	//			switch ((BYTE)track.tagTrackDatas[no].cData[j]){
	//			case 0x00:   // シーケンス番号
	//				printf("Sequence Number=\n");
	//				j += 2;   // データ長の分を通り越す
	//				cnt = (BYTE)track.tagTrackDatas[no].cData[j++];
	//				cnt <<= 8;   // 8bit左シフト
	//				cnt = cnt | (BYTE)track.tagTrackDatas[no].cData[j];
	//				printf("%d\n", cnt);
	//				break;
	//			case 0x01:   // テキスト[可変長]
	//				printf("Text=\n");
	//				cnt = 0;
	//				j += 1;
	//				while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
	//					cnt = cnt | (temp & 0x7F);   // 合成
	//					cnt <<= 7;   // 7bit左シフト
	//				}
	//				cnt = cnt | (temp & 0x7F);   // 合成
	//				for (int k = 1; k <= cnt; k++){
	//					printf("%c", track.tagTrackDatas[no].cData[j++]);
	//				}
	//				j--;   // 行き過ぎた分をデクリメント
	//				break;
	//			case 0x02:   // 著作権表示[可変長]
	//				printf("Copyright=\n");
	//				cnt = 0;
	//				j += 1;
	//				while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
	//					cnt = cnt | (temp & 0x7F);   // 合成
	//					cnt <<= 7;   // 7bit左シフト
	//				}
	//				cnt = cnt | (temp & 0x7F);   // 合成
	//				for (int k = 1; k <= cnt; k++){
	//					printf("%c", track.tagTrackDatas[no].cData[j++]);
	//				}
	//				j--;   // 行き過ぎた分をデクリメント
	//				break;
	//			case 0x03:   // シーケンス名（曲タイトル）・トラック名[可変長]
	//				printf("Title=\n");
	//				cnt = 0;
	//				j += 1;
	//				while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
	//					cnt = cnt | (temp & 0x7F);   // 合成
	//					cnt <<= 7;   // 7bit左シフト
	//				}
	//				cnt = cnt | (temp & 0x7F);   // 合成
	//				for (int k = 1; k <= cnt; k++){
	//					printf("%c", track.tagTrackDatas[no].cData[j++]);
	//				}
	//				j--;   // 行き過ぎた分をデクリメント
	//				break;
	//			case 0x04:   // 楽器名[可変長]
	//				printf("InstName=\n");
	//				cnt = 0;
	//				j += 1;
	//				while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
	//					cnt = cnt | (temp & 0x7F);   // 合成
	//					cnt <<= 7;   // 7bit左シフト
	//				}
	//				cnt = cnt | (temp & 0x7F);   // 合成
	//				for (int k = 1; k <= cnt; k++){
	//					printf("%c", track.tagTrackDatas[no].cData[j++]);
	//				}
	//				j--;   // 行き過ぎた分をデクリメント
	//				break;
	//			case 0x05:   // 歌詞[可変長]
	//				printf("Words=\n");
	//				cnt = 0;
	//				j += 1;
	//				while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
	//					cnt = cnt | (temp & 0x7F);   // 合成
	//					cnt <<= 7;   // 7bit左シフト
	//				}
	//				cnt = cnt | (temp & 0x7F);   // 合成
	//				for (int k = 1; k <= cnt; k++){
	//					printf("%c", track.tagTrackDatas[no].cData[j++]);
	//				}
	//				j--;   // 行き過ぎた分をデクリメント
	//				break;
	//			case 0x06:   // マーカー[可変長]
	//				printf("Marker=\n");
	//				cnt = 0;
	//				j += 1;
	//				while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
	//					cnt = cnt | (temp & 0x7F);   // 合成
	//					cnt <<= 7;   // 7bit左シフト
	//				}
	//				cnt = cnt | (temp & 0x7F);   // 合成
	//				for (int k = 1; k <= cnt; k++){
	//					printf("%c", track.tagTrackDatas[no].cData[j++]);
	//				}
	//				j--;   // 行き過ぎた分をデクリメント
	//				break;
	//			case 0x07:   // キューポイント[可変長]
	//				printf("CuePoint=\n");
	//				cnt = 0;
	//				j += 1;
	//				while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
	//					cnt = cnt | (temp & 0x7F);   // 合成
	//					cnt <<= 7;   // 7bit左シフト
	//				}
	//				cnt = cnt | (temp & 0x7F);   // 合成
	//				for (int k = 1; k <= cnt; k++){
	//					printf("%c", track.tagTrackDatas[no].cData[j++]);
	//				}
	//				j--;   // 行き過ぎた分をデクリメント
	//				break;
	//			case 0x08:   // プログラム名（音色名）[可変長]
	//				printf("ProgramName=\n");
	//				cnt = 0;
	//				j += 1;
	//				while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
	//					cnt = cnt | (temp & 0x7F);   // 合成
	//					cnt <<= 7;   // 7bit左シフト
	//				}
	//				cnt = cnt | (temp & 0x7F);   // 合成
	//				for (int k = 1; k <= cnt; k++){
	//					printf("%c", track.tagTrackDatas[no].cData[j++]);
	//				}
	//				j--;   // 行き過ぎた分をデクリメント
	//				break;
	//			case 0x09:   // デバイス名（音源名）[可変長]
	//				printf("DeviceName=\n");
	//				cnt = 0;
	//				j += 1;
	//				while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
	//					cnt = cnt | (temp & 0x7F);   // 合成
	//					cnt <<= 7;   // 7bit左シフト
	//				}
	//				cnt = cnt | (temp & 0x7F);   // 合成
	//				for (int k = 1; k <= cnt; k++){
	//					printf("%c", track.tagTrackDatas[no].cData[j++]);
	//				}
	//				j--;   // 行き過ぎた分をデクリメント
	//				break;
	//			case 0x20:   // MIDIチャンネルプリフィックス[1byte]
	//				printf("MidiChannelPrefix=\n");
	//				j += 2;   // データ長の分を通り越す
	//				cnt = (BYTE)track.tagTrackDatas[no].cData[j];
	//				printf("%d\n", cnt);
	//				break;
	//			case 0x21:   // ポート指定[1byte]
	//				printf("Port=\n");
	//				j += 2;   // データ長の分を通り越す
	//				cnt = (BYTE)track.tagTrackDatas[no].cData[j];
	//				printf("%d\n", cnt);
	//				break;
	//			case 0x2F:   // トラック終端[0byte]
	//				printf("End of Track\n");
	//				j += 1;   // データ長の分を通り越す
	//				break;
	//			case 0x51:   // テンポ設定[3byte]
	//				printf("Temp=\n");
	//				j += 2;   // データ長の分を通り越す
	//				cnt = (BYTE)track.tagTrackDatas[no].cData[j++];
	//				cnt <<= 8;   // 8bit左シフト
	//				cnt = cnt | (BYTE)track.tagTrackDatas[no].cData[j++];
	//				cnt <<= 8;   // 8bit左シフト
	//				cnt = cnt | (BYTE)track.tagTrackDatas[no].cData[j];
	//				printf("%d\n", cnt);

	//				{
	//					// BPM割り出し
	//					float bpm = 60000000 / (float)cnt;

	//					// 小数点第1で切り捨て処理(10にすると第一位、100にすると第2位まで切り捨てられる)
	//					bpm = floorf(bpm * 10) / 10;

	//					// リストにぶちこむ
	//					event_mng->PushBPM(current, bpm);
	//				}

	//				break;
	//			case 0x54:   // SMPTEオフセット[5byte]
	//				printf("SMPTE_Offset=\n");
	//				j += 2;   // データ長の分を通り越す
	//				cnt = (BYTE)track.tagTrackDatas[no].cData[j++];
	//				switch (cnt & 0xC0){   // フレーム速度
	//				case 0x00:
	//					printf("24fps\n");
	//					break;
	//				case 0x01:
	//					printf("25fps\n");
	//					break;
	//				case 0x10:
	//					printf("30fps(DropFrame)\n");
	//					break;
	//				case 0x11:
	//					printf("30fps\n");
	//					break;
	//				}
	//				printf(",Hour:%d\n", (cnt & 0x3F));   // 時間
	//				printf(",Min:%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);   // 分
	//				printf(",Sec:%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);   // 秒
	//				printf(",Frame:%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);   // フレーム
	//				printf(",SubFrame:%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);   // サブフレーム
	//				break;
	//			case 0x58:   // 拍子設定[4byte]
	//				printf("Rhythm=\n");
	//				j += 2;   // データ長の分を通り越す
	//				printf("%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
	//				cnt = 1;
	//				for (int k = 0; k <= (int)track.tagTrackDatas[no].cData[j]; k++){   // 拍子の分母を算出する
	//					cnt *= 2;   // 2の累乗
	//				}
	//				printf("/%d\n", cnt);
	//				j++;
	//				printf(" ClockPerBeat=%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
	//				printf(" 32NotePer4Note=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
	//				break;
	//			case 0x59:   // 調設定[2byte]
	//				printf("Key=\n");
	//				j += 2;   // データ長の分を通り越す
	//				cnt = (int)track.tagTrackDatas[no].cData[j++];
	//				if (temp > 0){
	//					printf("Sharp[%d]\n", temp);
	//				}
	//				else if (temp == 0){
	//					printf("C\n");
	//				}
	//				else {
	//					printf("Flat[%d]\n", temp);
	//				}
	//				cnt = (int)track.tagTrackDatas[no].cData[j];
	//				if (temp == 0){
	//					printf("_Major\n");
	//				}
	//				else {
	//					printf("_Minor\n");
	//				}

	//				break;
	//			case 0x7F:   // シーケンサ特定メタイベント
	//				printf("SpecialIvent=\n");
	//				cnt = 0;
	//				j += 1;
	//				while ((temp = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
	//					cnt = cnt | (temp & 0x7F);   // 合成
	//					cnt <<= 7;   // 7bit左シフト
	//				}
	//				cnt = cnt | (temp & 0x7F);   // 合成
	//				for (int k = 1; k <= cnt; k++){
	//					printf("[%02x]\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
	//				}
	//				j--;   // 行き過ぎた分をデクリメント
	//				break;
	//			}
	//			break;
	//		}
	//	}
	//}

}

void MIDI_Loader::LoadTrackALL(MIDILoadData &track, int no)
{
	BYTE c(0);			 // イベント解析の際に使用する一時保存用変数
	BYTE status(0);   // ステータスバイト用の一時変数
	int cnt(0);
	unsigned int delta(0);   // 初期化

	printf("# Track[%02d] =====================\n", no + 1);
	printf("track_datas[%d].size : %d\n", no, track.tagTrackDatas[no].iSize);
	printf("track_datas[%d].data\n", no);
	for (int j = 0; j<track.tagTrackDatas[no].iSize; j++){   // データ分だけ繰返し

		delta = 0;   // 初期化
		while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
			delta = delta | (c & 0x7F);   // 合成
			delta <<= 7;   // 7bit左シフト
		}
		delta = delta | c;   // 合成
		printf("%d:: \n", delta);   // デルタタイム出力

		// ランニングステータスルールに対する処理
		if ((track.tagTrackDatas[no].cData[j] & 0x80) == 0x80){
			// ランニングステータスルールが適用されない場合は、ステータスバイト用変数を更新。
			status = (BYTE)track.tagTrackDatas[no].cData[j];   // ステータスバイトを保持しておく
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
			printf("Note%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
			printf("[0x%02x] \n", (BYTE)track.tagTrackDatas[no].cData[j++]);
			printf("Velocity=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
		}
		else if ((status & 0xf0) == 0x90){
			// ノート・オン【ボイスメッセージ】
			printf("Note On  [%02dch] : \n", (status & 0x0f));
			j++;
			printf("Note%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
			printf("[0x%02x] \n", (BYTE)track.tagTrackDatas[no].cData[j++]);
			printf("Velocity=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
		}
		else if ((status & 0xf0) == 0xa0){
			// ポリフォニック・キー・プレッシャー【ボイスメッセージ】
			printf("Polyphonic Key Pressure [%02dch] : \n", (status & 0x0f));
			j++;
			printf("Note%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
			printf("[0x%02x] \n", (BYTE)track.tagTrackDatas[no].cData[j++]);
			printf("Pressure=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
		}
		else if ((status & 0xf0) == 0xb0){
			// コントロールチェンジ【ボイスメッセージ】、【モードメッセージ】
			printf("Control Change [%02dch] : \n", (status & 0x0f));
			j++;
			c = (BYTE)track.tagTrackDatas[no].cData[j++];
			if (c <= 63){   // 連続可変タイプのエフェクトに関するコントロール情報（MSBもLSBも）
				// （ホントは「0<=c && c<=63」と書きたいけど、warningが出るので「c<=63」にする）
				printf("VariableEffect(\n");
				switch (c){
				case 0:    // 上位バイト[MSB]
				case 32:   // 下位バイト[LSB]
					printf("BankSelect[%s]\n", (c == 0) ? "MSB\n" : "LSB\n");   // バンク・セレクト
					break;
				case 1:
				case 33:
					printf("ModulationDepth[%s]\n", (c == 1) ? "MSB\n" : "LSB\n");   // モジュレーション・デプス
					break;
				case 2:
				case 34:
					printf("BreathType[%s]\n", (c == 2) ? "MSB\n" : "LSB\n");   // ブレス・タイプ
					break;
				case 4:
				case 36:
					printf("FootType[%s]\n", (c == 4) ? "MSB\n" : "LSB\n");   // フット・タイプ
					break;
				case 5:
				case 37:
					printf("PortamentoTime[%s]\n", (c == 5) ? "MSB\n" : "LSB\n");   // ポルタメント・タイム
					break;
				case 6:
				case 38:
					printf("DataEntry[%s]\n", (c == 6) ? "MSB\n" : "LSB\n");   // データ・エントリー
					break;
				case 7:
				case 39:
					printf("MainVolume[%s]\n", (c == 7) ? "MSB\n" : "LSB\n");   // メイン・ボリューム
					break;
				case 8:
				case 40:
					printf("BalanceControl[%s]\n", (c == 8) ? "MSB\n" : "LSB\n");   // バランス・コントロール
					break;
				case 10:
				case 42:
					printf("Panpot[%s]\n", (c == 10) ? "MSB\n" : "LSB\n");   // パンポット
					break;
				case 11:
				case 43:
					printf("Expression[%s]\n", (c == 11) ? "MSB\n" : "LSB\n");   // エクスプレッション
					break;
				case 16:
				case 17:
				case 18:
				case 19:
				case 48:
				case 49:
				case 50:
				case 51:
					printf("SomethingElse_No_%d[%s]\n", c, (c == 16) ? "MSB\n" : "LSB\n");   // 汎用操作子
					break;
				default:
					printf("##UndefinedType_No_%d[%s]", (c<32) ? c : c - 32, (c<32) ? "MSB\n" : "LSB\n");   // よくわからないコントロール
				}

				printf(")=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
			}
			else if (64 <= c && c <= 95){   // 連続可変でないタイプのエフェクトに関するコントロール情報
				printf("InvariableEffect(\n");
				switch (c){
				case 64:
					printf("Hold1(Damper)\n");   // ホールド１（ダンパー）
					break;
				case 65:
					printf("Portamento\n");   // ポルタメント
					break;
				case 66:
					printf("Sostenuto\n");   // ソステヌート
					break;
				case 67:
					printf("SoftPedal\n");   // ソフト・ペダル
					break;
				case 69:
					printf("Hold2(Freeze)\n");   // ホールド２（フリーズ）
					break;
				case 71:
					printf("HarmonicIntensity\n");   // ハーモニック・インテンシティ
					break;
				case 72:
					printf("ReleaseTime\n");   // リリース・タイム
					break;
				case 73:
					printf("AttackTime\n");   // アタック・タイム
					break;
				case 74:
					printf("Brightness\n");   // ブライトネス
					break;
				case 80:
				case 81:
				case 82:
				case 83:
					printf("SomethingElse_No_%d\n", c);   // 汎用操作子
					break;
				case 91:
					printf("ExternalEffect\n");   // 外部エフェクト
					break;
				case 92:
					printf("Tremol\no");   // トレモロ
					break;
				case 93:
					printf("Chorus\n");   // コーラス
					break;
				case 94:
					printf("Celeste\n");   // セレステ
					break;
				case 95:
					printf("Phaser\n");   // フェイザー
					break;
				default:
					printf("##UndefinedType_No_%d\n", c);   // よくわからないコントロール
				}
				printf(")=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
			}
			else if (96 <= c && c <= 119){   // 特殊な情報
				printf("SpecialPurpose(\n");
				switch (c){
				case 96:
					printf("DataIncrement\n");   // データ・インクリメント
					break;
				case 97:
					printf("DataDecrement\n");   // デクリメント
					break;
				case 98:
					printf("NRPN[LSB]\n");   // NRPNのLSB
					break;
				case 99:
					printf("NRPN[MSB]\n");   // NRPNのMSB
					break;
				case 100:
					printf("RPN[LSB]\n");   // RPNのLSB
					break;
				case 101:
					printf("RPN[MSB]\n");   // RPNのMSB
					break;
				default:
					printf("##UndefinedType_No_%d\n", c);   // よくわからないコントロール
				}
				printf(")=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
			}
			else if (120 <= c && c <= 127){   // モード・メッセージ
				printf("ModeMessage(\n");
				switch (c){
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
					printf("##UndefinedType_No_%d\n", c);   // よくわからないコントロール
				}
				//printf(")=%d\n", c, (BYTE)track.tagTrackDatas[no].cData[j]);
			}
			printf("\n");
		}
		else if ((status & 0xf0) == 0xc0){
			// プログラム・チェンジ【ボイスメッセージ】
			printf("Program Change [%02dch] : \n", (status & 0x0f));
			j++;
			printf("ToneNo=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
		}
		else if ((status & 0xf0) == 0xd0){
			// チャンネル・プレッシャー【ボイスメッセージ】
			printf("Channel Pressure [%02dch] : \n", (status & 0x0f));
			j++;
			printf("Pressure=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
		}
		else if ((status & 0xf0) == 0xe0){
			// ピッチ・ベンド・チェンジ【ボイスメッセージ】
			printf("Pitch Bend Change [%02dch] : \n", (status & 0x0f));
			j++;
			printf(" (LSB:%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
			printf(", MSB:%d)\n", (BYTE)track.tagTrackDatas[no].cData[j]);
		}
		else if ((status & 0xf0) == 0xf0){
			// 【システム・メッセージ】
			switch (status & 0x0f){
			case 0x00:   // エクスクルーシブ・メッセージ
				printf("F0 Exclusive Message : \n");
				j++;

				// SysExのデータ長を取得
				cnt = 0;   // 初期化
				while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){   // フラグビットが1の間はループ
					cnt = cnt | c;   // 合成
					cnt <<= 7;   // 7bit左シフト
				}
				cnt = cnt | c;   // 合成
				printf(" Length=%u\n", (unsigned int)cnt);   // SysExのデータ長を取得

				for (int k = 1; k <= cnt; k++){   // 長さの分だけデータ取得
					printf("[%02x]\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
				}
				j--;   // 行き過ぎた分をデクリメント

				break;
			case 0x01:   // MIDIタイムコード
				printf("MIDI Time Code : \n");
				j++;
				printf("(frame/sec/min/hour)=%d", (BYTE)track.tagTrackDatas[no].cData[j]);
				break;
			case 0x02:   // ソング・ポジション・ポインタ
				printf("Song Position Pointer : \n");
				j++;
				printf(" (LSB:%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
				printf(", MSB:%d)\n", (BYTE)track.tagTrackDatas[no].cData[j]);
				break;
			case 0x03:   // ソング・セレクト
				printf("Song Select : \n");
				j++;
				printf("SelectNo=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
				break;
			case 0x06:   // チューン・リクエスト
				printf("Tune Request\n");
				break;
			case 0x07:   // エンド・オブ・エクスクルーシブでもあるけども...
				// F7ステータスの場合のエクスクルーシブ・メッセージ
				//printf("@End of Exclusive");
				printf("F7 Exclusive Message : \n");
				j++;

				// SysExのデータ長を取得
				cnt = 0;   // 初期化
				while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){   // フラグビットが1の間はループ
					cnt = cnt | c;   // 合成
					cnt <<= 7;   // 7bit左シフト
				}
				cnt = cnt | c;   // 合成
				printf(" Length=%u\n", (unsigned int)cnt);   // SysExのデータ長を取得

				for (int k = 1; k <= cnt; k++){   // 長さの分だけデータ取得
					printf("[%02x]\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
				}
				j--;   // 行き過ぎた分をデクリメント

				break;
			case 0x08:   // タイミング・クロック
				printf("Timing Clock\n");
				break;
			case 0x0A:   // スタート
				printf("Start\n");
				break;
			case 0x0B:   // コンティニュー
				printf("Continue\n");
				break;
			case 0x0C:   // ストップ
				printf("Stop\n");
				break;
			case 0x0E:   // アクティブ・センシング
				printf("Active Sensing\n");
				break;
				//case 0x0F:   // システムリセット（何か間違っている気がする。。。）
				//    printf("System Reset");
			case 0x0F:   // メタイベント
				printf("Meta Ivent : \n");
				j++;
				switch ((BYTE)track.tagTrackDatas[no].cData[j]){
				case 0x00:   // シーケンス番号
					printf("Sequence Number=\n");
					j += 2;   // データ長の分を通り越す
					cnt = (BYTE)track.tagTrackDatas[no].cData[j++];
					cnt <<= 8;   // 8bit左シフト
					cnt = cnt | (BYTE)track.tagTrackDatas[no].cData[j];
					printf("%d\n", cnt);
					break;
				case 0x01:   // テキスト[可変長]
					printf("Text=\n");
					cnt = 0;
					j += 1;
					while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
						cnt = cnt | (c & 0x7F);   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | (c & 0x7F);   // 合成
					for (int k = 1; k <= cnt; k++){
						printf("%c", track.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント
					break;
				case 0x02:   // 著作権表示[可変長]
					printf("Copyright=\n");
					cnt = 0;
					j += 1;
					while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
						cnt = cnt | (c & 0x7F);   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | (c & 0x7F);   // 合成
					for (int k = 1; k <= cnt; k++){
						printf("%c", track.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント
					break;
				case 0x03:   // シーケンス名（曲タイトル）・トラック名[可変長]
					printf("Title=\n");
					cnt = 0;
					j += 1;
					while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
						cnt = cnt | (c & 0x7F);   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | (c & 0x7F);   // 合成
					for (int k = 1; k <= cnt; k++){
						printf("%c", track.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント
					break;
				case 0x04:   // 楽器名[可変長]
					printf("InstName=\n");
					cnt = 0;
					j += 1;
					while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
						cnt = cnt | (c & 0x7F);   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | (c & 0x7F);   // 合成
					for (int k = 1; k <= cnt; k++){
						printf("%c", track.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント
					break;
				case 0x05:   // 歌詞[可変長]
					printf("Words=\n");
					cnt = 0;
					j += 1;
					while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
						cnt = cnt | (c & 0x7F);   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | (c & 0x7F);   // 合成
					for (int k = 1; k <= cnt; k++){
						printf("%c", track.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント
					break;
				case 0x06:   // マーカー[可変長]
					printf("Marker=\n");
					cnt = 0;
					j += 1;
					while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
						cnt = cnt | (c & 0x7F);   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | (c & 0x7F);   // 合成
					for (int k = 1; k <= cnt; k++){
						printf("%c", track.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント
					break;
				case 0x07:   // キューポイント[可変長]
					printf("CuePoint=\n");
					cnt = 0;
					j += 1;
					while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
						cnt = cnt | (c & 0x7F);   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | (c & 0x7F);   // 合成
					for (int k = 1; k <= cnt; k++){
						printf("%c", track.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント
					break;
				case 0x08:   // プログラム名（音色名）[可変長]
					printf("ProgramName=\n");
					cnt = 0;
					j += 1;
					while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
						cnt = cnt | (c & 0x7F);   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | (c & 0x7F);   // 合成
					for (int k = 1; k <= cnt; k++){
						printf("%c", track.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント
					break;
				case 0x09:   // デバイス名（音源名）[可変長]
					printf("DeviceName=\n");
					cnt = 0;
					j += 1;
					while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
						cnt = cnt | (c & 0x7F);   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | (c & 0x7F);   // 合成
					for (int k = 1; k <= cnt; k++){
						printf("%c", track.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント
					break;
				case 0x20:   // MIDIチャンネルプリフィックス[1byte]
					printf("MidiChannelPrefix=\n");
					j += 2;   // データ長の分を通り越す
					cnt = (BYTE)track.tagTrackDatas[no].cData[j];
					printf("%d\n", cnt);
					break;
				case 0x21:   // ポート指定[1byte]
					printf("Port=\n");
					j += 2;   // データ長の分を通り越す
					cnt = (BYTE)track.tagTrackDatas[no].cData[j];
					printf("%d\n", cnt);
					break;
				case 0x2F:   // トラック終端[0byte]
					printf("End of Track\n");
					j += 1;   // データ長の分を通り越す
					break;
				case 0x51:   // テンポ設定[3byte]
					printf("Temp=\n");
					j += 2;   // データ長の分を通り越す
					cnt = (BYTE)track.tagTrackDatas[no].cData[j++];
					cnt <<= 8;   // 8bit左シフト
					cnt = cnt | (BYTE)track.tagTrackDatas[no].cData[j++];
					cnt <<= 8;   // 8bit左シフト
					cnt = cnt | (BYTE)track.tagTrackDatas[no].cData[j];
					printf("%d\n", cnt);
					break;
				case 0x54:   // SMPTEオフセット[5byte]
					printf("SMPTE_Offset=\n");
					j += 2;   // データ長の分を通り越す
					cnt = (BYTE)track.tagTrackDatas[no].cData[j++];
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
					printf(",Min:%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);   // 分
					printf(",Sec:%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);   // 秒
					printf(",Frame:%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);   // フレーム
					printf(",SubFrame:%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);   // サブフレーム
					break;
				case 0x58:   // 拍子設定[4byte]
					printf("Rhythm=\n");
					j += 2;   // データ長の分を通り越す
					printf("%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
					cnt = 1;
					for (int k = 0; k <= (int)track.tagTrackDatas[no].cData[j]; k++){   // 拍子の分母を算出する
						cnt *= 2;   // 2の累乗
					}
					printf("/%d\n", cnt);
					j++;
					printf(" ClockPerBeat=%d\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
					printf(" 32NotePer4Note=%d\n", (BYTE)track.tagTrackDatas[no].cData[j]);
					break;
				case 0x59:   // 調設定[2byte]
					printf("Key=\n");
					j += 2;   // データ長の分を通り越す
					cnt = (int)track.tagTrackDatas[no].cData[j++];
					if (c > 0){
						printf("Sharp[%d]\n", c);
					}
					else if (c == 0){
						printf("C\n");
					}
					else {
						printf("Flat[%d]\n", c);
					}
					cnt = (int)track.tagTrackDatas[no].cData[j];
					if (c == 0){
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
					while ((c = (BYTE)track.tagTrackDatas[no].cData[j++]) & 0x80){
						cnt = cnt | (c & 0x7F);   // 合成
						cnt <<= 7;   // 7bit左シフト
					}
					cnt = cnt | (c & 0x7F);   // 合成
					for (int k = 1; k <= cnt; k++){
						printf("[%02x]\n", (BYTE)track.tagTrackDatas[no].cData[j++]);
					}
					j--;   // 行き過ぎた分をデクリメント
					break;
				default:
					;
				}
				break;
			default:
				printf("# SysEx (Something else...[Status:%02x])", status);   // 見知らぬステータスなら
			}
			printf("");
		}
		else {
			printf("## Something else...[Status:%02x]", status);   // 見知らぬステータスなら
		}
	}
	printf("");
}