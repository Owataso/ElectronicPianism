#include	"TDNLIB.h"

/*****************/
/// アーカイブ
/*****************/
// アーカイブファイルを作成する
bool tdnArchiver::CreateArchiveFile(const char* pHeaderFilename, const char* pDataFilename)
{
	// ヘッダー部作成
	headerOfs.open(pHeaderFilename, std::ios::binary);
	if (headerOfs.fail())
	{
		return false;
	}
	// データ部作成
	dataOfs.open(pDataFilename, std::ios::binary);;
	if (dataOfs.fail())
	{
		return false;
	}

	ZeroMemory(nameID, sizeof(char[64]));
	wroteArchiveFileSize = 0;

	// 最大ヘッダー数
	HEAD_MAX = 0;

	return true;
}

// データファイルをアーカイブファイルに追加する
bool tdnArchiver::AddData(const char* pDataFileName)
{
	// ヘッダーファイルを作成済みか
	if (!headerOfs.is_open())
	{
		return false;
	}

	// データファイルを読み込む
	std::ifstream ifs; // input file stream
	ifs.open(pDataFileName, std::ios::binary);
	if (ifs.fail())
	{
		return false;
	}

	//------------------------------------------------
	// データファイルのサイズを取得
	//------------------------------------------------
	// ファイル末尾に移動
	ifs.seekg(0, std::fstream::end);
	// EOF(End of File)つまりファイル末尾の位置を取得
	auto EOFPosition = ifs.tellg();

	// 作法として一度Clearをかける。fstreamの内部フラグをリセット。
	ifs.clear();

	// ファイル先頭に移動
	ifs.seekg(0, std::fstream::beg);
	// ファイル先頭の位置を取得
	auto BeginPosition = ifs.tellg();

	// サイズを算出する
	uint32_t dataFileSize = (uint32_t)(EOFPosition - BeginPosition);

	// アーカイブファイルの最大サイズを超えているならエラー
	if (eMaxArchiveFileSize - wroteArchiveFileSize <
		sizeof(uint32_t)+											// ヘッダーの最大数 
		sizeof(char[64]) + sizeof(uint32_t)+sizeof(uint32_t)+	// ヘッダー部
		dataFileSize +												// データ部
		sizeof(uint32_t)+sizeof(uint32_t))						// ダミーヘッダー部
	{
		ifs.close();
		return false;
	}

	//------------------------------------------------
	// メモリにデータファイルの内容を展開する
	//------------------------------------------------
	// データファイルからデータを読み込むためのバイト配列をnew
	char* pData = new char[dataFileSize];
	// 0初期化
	memset(pData, 0, dataFileSize);

	// データを読み込む
	ifs.read(pData, dataFileSize);
	// ifsを閉じておく
	ifs.close();


	//------------------------------------------------
	// ヘッダー部の数をカウント
	//------------------------------------------------
	HEAD_MAX++;// 追加したデータの数

	//------------------------------------------------
	// ヘッダー部作成
	//------------------------------------------------

	// 毎回0に初期化
	ZeroMemory(nameID, sizeof(char[64]));
	memcpy(nameID, pDataFileName, sizeof(char[64]));// 送られてきた名前を書き込む！
	headerOfs.write((char*)&nameID, sizeof(char[64])); // キャラ型64分の情報を送る
	headerOfs.write((char*)&wroteArchiveFileSize, 4); // シークするために追加
	headerOfs.write((char*)&dataFileSize, 4); // unsigned int(32bitつまり4byte)を書き込む

	//------------------------------------------------
	// データ部作成
	//------------------------------------------------
	dataOfs.write(pData, dataFileSize);// データ部をデータを描きます

	//------------------------------------------------
	// （ほんとは）ここで圧縮したり暗号化したり
	//------------------------------------------------

	//------------------------------------------------
	// 後処理
	//------------------------------------------------
	delete[] pData;
	//dataID++;
	//wroteArchiveFileSize += sizeof(char[64]) + sizeof(uint32_t)+sizeof(uint32_t);
	wroteArchiveFileSize += dataFileSize;

	return true;
}

// アーカイブ終了処理(ダミーヘッダーの追加)
bool tdnArchiver::CloseArchiveFile(const char* pArchiveFilename, const char* pHeaderFilename, const char* pDataFilename)
{
	if (!headerOfs.is_open())
	{
		return false;
	}

	//------------------------------------------------
	// ダミーヘッダー作成
	//------------------------------------------------
	uint32_t dummy = -1;
	headerOfs.write((char*)&dummy, sizeof(char[64]));
	headerOfs.write((char*)&dummy, 4);// unsigned int なので　-1をいれると貫通する？
	headerOfs.write((char*)&dummy, 4);

	//------------------------------------------------
	// ファイルクローズ
	//------------------------------------------------
	headerOfs.close();
	dataOfs.close();


	//--------------------------------------------------------
	// 最後に　ヘッダー数　ヘッダー＆ダミー　と　データ　をくっつける 
	//--------------------------------------------------------
	// ヘッダー部作成
	archiverOfs.open(pArchiveFilename, std::ios::binary);
	if (archiverOfs.fail())
	{
		return false;
	}


	/***************************************************/
	// データファイルを読み込む
	std::ifstream ifs; // input file stream
	ifs.open(pHeaderFilename, std::ios::binary);
	if (ifs.fail())
	{
		return false;
	}
	//------------------------------------------------
	// データファイルのサイズを取得
	//------------------------------------------------
	// ファイル末尾に移動
	ifs.seekg(0, std::fstream::end);
	// EOF(End of File)つまりファイル末尾の位置を取得
	auto EOFPosition = ifs.tellg();
	// 作法として一度Clearをかける。fstreamの内部フラグをリセット。
	ifs.clear();
	// ファイル先頭に移動
	ifs.seekg(0, std::fstream::beg);
	// ファイル先頭の位置を取得
	auto BeginPosition = ifs.tellg();
	// サイズを算出する
	uint32_t dataFileSize = (uint32_t)(EOFPosition - BeginPosition);
	//------------------------------------------------
	// メモリにデータファイルの内容を展開する
	//------------------------------------------------
	// データファイルからデータを読み込むためのバイト配列をnew
	char* pData = new char[dataFileSize];
	// 0初期化
	memset(pData, 0, dataFileSize);
	// データを読み込む
	ifs.read(pData, dataFileSize);
	// ifsを閉じておく
	ifs.close();
	/*******************************************************/

	/*******************************************************/
	// データファイルを読み込む
	std::ifstream ifs2; // input file stream
	ifs2.open(pDataFilename, std::ios::binary);
	if (ifs2.fail())
	{
		return false;
	}
	//------------------------------------------------
	// データファイルのサイズを取得
	//------------------------------------------------
	// ファイル末尾に移動
	ifs2.seekg(0, std::fstream::end);
	// EOF(End of File)つまりファイル末尾の位置を取得
	auto EOFPosition2 = ifs2.tellg();
	// 作法として一度Clearをかける。fstreamの内部フラグをリセット。
	ifs2.clear();
	// ファイル先頭に移動
	ifs2.seekg(0, std::fstream::beg);
	// ファイル先頭の位置を取得
	auto BeginPosition2 = ifs2.tellg();
	// サイズを算出する
	uint32_t dataFileSize2 = (uint32_t)(EOFPosition2 - BeginPosition2);
	//------------------------------------------------
	// メモリにデータファイルの内容を展開する
	//------------------------------------------------
	char* pData2 = new char[dataFileSize2];
	memset(pData2, 0, dataFileSize2);
	ifs2.read(pData2, dataFileSize2);
	ifs2.close();

	/*******************************************************/


	//------------------------------------------------
	// ヘッダー最大数作成
	//------------------------------------------------
	archiverOfs.write((char*)&HEAD_MAX, 4);

	//------------------------------------------------
	// ヘッダー部作成
	//------------------------------------------------
	archiverOfs.write(pData, dataFileSize);

	//------------------------------------------------
	// データ部作成
	//------------------------------------------------
	archiverOfs.write(pData2, dataFileSize2);

	// ファイルクローズ
	archiverOfs.close();


	//------------------------------------------------
	// 後処理
	//------------------------------------------------
	delete[] pData;
	delete[] pData2;


	return true;
}


/********************/
/// アーカイブ解凍
/********************/

//// 宣言
//tdnUnArchiver* tdnUnArchiver::pInstance = nullptr;
//
//// 実体
//tdnUnArchiver& tdnUnArchiver::GetInstance()
//{
//	if (!pInstance)
//	{
//		pInstance = new tdnUnArchiver;
//	}
//	return *pInstance;
//}
//
//// 解放
//void tdnUnArchiver::Release()
//{
//	if (pInstance)
//	{
//		delete pInstance;
//		pInstance = nullptr;
//	}
//}

// アーカイブファイルからデータを取得
char* tdnUnArchiver::OpenArchiveFile(const char* pArchiveFilename, const char* pName)
{
	// アーカイブファイルを読み込む
	std::ifstream archiveIfs; // input file stream
	archiveIfs.open(pArchiveFilename, std::ios::binary);
	if (archiveIfs.fail())
	{
		return false;
	}

	// ファイル先頭に移動
	//ifs.seekg(0, std::fstream::beg);

	// ★まず先頭のヘッダー最大数を取る
	uint32_t HEAD_MAX = 0;
	archiveIfs.read((char*)&HEAD_MAX, 4);

	// 情報を取り出す
	for (int i = 0;; i++)
	{
		// ヘッダー部読み込み
		ArchiveHeaderBlock headerBlock;
		size_t size = sizeof(ArchiveHeaderBlock);
		archiveIfs.read((char*)&headerBlock, sizeof(ArchiveHeaderBlock));

		// 『大文字と小文字を区別せず』にIDの名前と同じなら
		if (lstrcmpi(headerBlock.nameID, pName) == 0)// 比較した結果が同じなら
		{
			// データ部読み込み
			//ArchiveDataBlock dataBlock;

			//------------------------------------------------
			// メモリにデータファイルの内容を展開する
			//------------------------------------------------
			// データファイルからデータを読み込むためのバイト配列をnew
			// ※ここでnewしたメモリは外部でデリートするしかないという地獄。
			char* pData = new char[headerBlock.dataFileSize];
			// 0初期化
			//memset(pData, 0, headerBlock.dataFileSize);

			// データ部からを情報読み込む前にそのデータのあるとこまでシーク
			// ヘッダー部の最大値の値 +　開始位置から描きこんだ分＋ヘッダー全部 + ダミーヘッダー　まで移動！
			archiveIfs.seekg(
				4 +												// 最初の最大値
				(sizeof(ArchiveHeaderBlock)*HEAD_MAX) +			// ヘッダー全部
				(sizeof(ArchiveHeaderBlock)) +					// ダミーヘッダー 
				headerBlock.wroteDataFileSize +					// 開始位置から描きこんだ分
				std::fstream::beg);


			// シークしたデータ部から情報読み込む
			archiveIfs.read(pData, headerBlock.dataFileSize);// ファイルサイズ分読み込む
			// ↑のpDataにファイルの情報が詰まりました。

			// ifsを閉じておく
			archiveIfs.close();

			return pData;
		}

		// ヘッダー部が-1(ダミー)ならば読み込み終了
		if (headerBlock.dataFileSize == -1)
		{
			// ifsを閉じておく
			archiveIfs.close();
			return false;
		}

	}

}


// まずアーカイブファイルを読み込む
int tdnUnArchiver::GetSize(const char* pArchiveFilename, const char* pName)
{

	// アーカイブファイルを読み込む
	std::ifstream archiveIfs; // input file stream
	archiveIfs.open(pArchiveFilename, std::ios::binary);
	if (archiveIfs.fail())
	{
		return false;
	}

	// ファイル先頭に移動
	//ifs.seekg(0, std::fstream::beg);

	// ★まず先頭のヘッダー最大数を取る
	uint32_t HEAD_MAX = 0;
	archiveIfs.read((char*)&HEAD_MAX, 4);

	// 情報を取り出す
	for (int i = 0;; i++)
	{
		// ヘッダー部読み込み
		ArchiveHeaderBlock headerBlock;
		size_t size = sizeof(ArchiveHeaderBlock);
		archiveIfs.read((char*)&headerBlock, sizeof(ArchiveHeaderBlock));

		// 『大文字と小文字を区別せず』にIDの名前と同じなら
		if (lstrcmpi(headerBlock.nameID, pName) == 0)// 比較した結果が同じなら
		{
			// ifsを閉じておく
			archiveIfs.close();
			return headerBlock.dataFileSize;
		}

		// ヘッダー部が-1(ダミー)ならば読み込み終了
		if (headerBlock.dataFileSize == -1)
		{
			// ifsを閉じておく
			archiveIfs.close();
			return false;
		}

	}


}

// アーカイブを開く＋メモリサイズを取得
char* tdnUnArchiver::OpenArchiveFile(const char* pArchiveFilename, const char* pName, int& outMemorySize)
{
	// メモリサイズを初期化
	outMemorySize = 0;

	// アーカイブファイルを読み込む
	std::ifstream archiveIfs; // input file stream
	archiveIfs.open(pArchiveFilename, std::ios::binary);
	if (archiveIfs.fail())
	{
		return false;
	}

	// ファイル先頭に移動
	//ifs.seekg(0, std::fstream::beg);

	// ★まず先頭のヘッダー最大数を取る
	uint32_t HEAD_MAX = 0;
	archiveIfs.read((char*)&HEAD_MAX, 4);

	// 情報を取り出す
	for (int i = 0;; i++)
	{
		// ヘッダー部読み込み
		ArchiveHeaderBlock headerBlock;
		size_t size = sizeof(ArchiveHeaderBlock);
		archiveIfs.read((char*)&headerBlock, sizeof(ArchiveHeaderBlock));

		// 『大文字と小文字を区別せず』にIDの名前と同じなら
		if (lstrcmpi(headerBlock.nameID, pName) == 0)// 比較した結果が同じなら
		{
			// データ部読み込み
			//ArchiveDataBlock dataBlock;

			//------------------------------------------------
			// メモリにデータファイルの内容を展開する
			//------------------------------------------------
			// データファイルからデータを読み込むためのバイト配列をnew
			char* pData = new char[headerBlock.dataFileSize];
			// 0初期化
			//memset(pData, 0, headerBlock.dataFileSize);

			// データ部からを情報読み込む前にそのデータのあるとこまでシーク
			// ヘッダー部の最大値の値 +　開始位置から描きこんだ分＋ヘッダー全部 + ダミーヘッダー　まで移動！
			archiveIfs.seekg(
				4 +												// 最初の最大値
				(sizeof(ArchiveHeaderBlock)*HEAD_MAX) +			// ヘッダー全部
				(sizeof(ArchiveHeaderBlock)) +					// ダミーヘッダー 
				headerBlock.wroteDataFileSize +					// 開始位置から描きこんだ分
				std::fstream::beg);


			// シークしたデータ部から情報読み込む
			archiveIfs.read(pData, headerBlock.dataFileSize);// ファイルサイズ分読み込む
			// ↑のpDataにファイルの情報が詰まりました。

			// ifsを閉じておく
			archiveIfs.close();


			outMemorySize = headerBlock.dataFileSize;
			return pData;
		}

		// ヘッダー部が-1(ダミー)ならば読み込み終了
		if (headerBlock.dataFileSize == -1)
		{
			// ifsを閉じておく
			archiveIfs.close();
			return false;
		}

	}

}