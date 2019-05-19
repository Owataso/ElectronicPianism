#include	"TDNLIB.h"

//*****************************************************************************
//
//		テクスチャ関連
//
//*****************************************************************************

Texture2D*	lpLastTexture;										// (?)なにこれ？
struct tdnTexture::TEXINFO tdnTexture::TexInfo[MAX_TEXTURE];	// テクスチャの情報を格納する配列

//	初期化
void	tdnTexture::Initialize()
{
	// IDirect3DTexture9を初期化
	for (int i = 0; i<MAX_TEXTURE; i++)
	{
		if (TexInfo[i].lpTexture) TexInfo[i].lpTexture->Release();	// テクスチャの情報が入っていたらRelease
		TexInfo[i].lpTexture = NULL;								// IDirect3DTexture9を0に
		TexInfo[i].UseCount = 0;									// 使用カウントを0に
	}
}

//	読み込み
Texture2D* tdnTexture::Load(const char* filename, int flag)
{
	int			TexNo;
	HRESULT		hr;
	D3DFORMAT	fmt = D3DFMT_UNKNOWN;

	enum { NEW_TEXTURE = -1 };
	TexNo = NEW_TEXTURE;

	//	すでに読み込まれているテクスチャかを調べる
	for (int i = 0; i<MAX_TEXTURE; i++)
	{
		if (!TexInfo[i].lpTexture) continue;						// テクスチャの中に何も入っていないならcontinueで戻る
		if (lstrcmpi(TexInfo[i].filename, filename) != 0) continue;	// 『大文字と小文字を区別せず』にファイル名の比較をして一致していないならcontinueで戻る
		TexNo = i;													// 一度読み込んだテクスチャがあるならその配列番号を記録してループから出る
		//if (TexInfo[(MAX_TEXTURE - 1)].lpTexture){ MessageBox(0, "保存できる最大のテクスチャ数まで到達しました!", "Texture", MB_OK); }
		break;
	}

	//	NEW_TEXTUREのままだったら新規読み込み 
	if (TexNo == NEW_TEXTURE)
	{
		if (flag == 1) fmt = D3DFMT_R8G8B8;

		//	空いてる配列を検索
		for (TexNo = 0; TexNo<MAX_TEXTURE; TexNo++) if (!TexInfo[TexNo].lpTexture) break;
		
		//	ファイル名保存
		CopyMemory(TexInfo[TexNo].filename, filename, strlen(filename) + 1);

		//　読み込んだ画像の情報をファイルからもらう
		D3DXGetImageInfoFromFile(filename, &TexInfo[TexNo].info);

		//	テクスチャ読み込み
		hr = D3DXCreateTextureFromFileEx(
			tdnSystem::GetDevice(),		// ビデオカードのデバイス 
			filename,					// ファイルの位置
			TexInfo[TexNo].info.Width,	// ↑で取得したサイズを使用
			TexInfo[TexNo].info.Height,	// 
			1,							// ミップマップのレベル
			0,							// テクスチャの性質や確保するメモリの場所を指定する。ミップマップを自動生成にしてる
			TexInfo[TexNo].info.Format,	// 画像のサーフェイスフォーマットを指定　取得したフォーマットを採用
			D3DPOOL_DEFAULT,			// バッファを保持するメモリ
			D3DX_FILTER_POINT,			// 線形補間
			D3DX_FILTER_POINT,			// ミップマップにも線形補間で
			0x00000000,					// 透明にする色を指定できる。しない場合はNULL(0)
			NULL,						// 元の画像の情報を格納するD3DXIMAGE_INFO構造体へのポインタを渡す。↑で保存しているのでいらない
			NULL,						// 色関係？
			&TexInfo[TexNo].lpTexture	// テクスチャへのポインタ
			);

		//　テクスチャがなかったら
		if (FAILED(hr)) return NULL;
	}
	TexInfo[TexNo].UseCount++;			// そのテクスチャを利用しているカウンタを一個増やす
	return TexInfo[TexNo].lpTexture;
}

// メモリからの読み込み
Texture2D* tdnTexture::LoadMemory(const char* filename, const char* pArchiver)
{
	// メンバ変数
	char* saveBuf;	// 削除用
	char* seekBuf;	// シーク用
	int	  bufSize;	// バッファのサイズ

	// アーカイブを解除してデータを受け取る
	saveBuf = tdnUnArchiver::OpenArchiveFile(pArchiver, filename, bufSize);
	seekBuf = saveBuf;	// シークに移す

	int			TexNo;
	HRESULT		hr;
	D3DFORMAT	fmt = D3DFMT_UNKNOWN;

	enum { NEW_TEXTURE = -1 };
	TexNo = NEW_TEXTURE;

	//	すでに読み込まれているテクスチャかを調べる
	for (int i = 0; i<MAX_TEXTURE; i++)
	{
		if (!TexInfo[i].lpTexture) continue;						// テクスチャの中に何も入っていないならcontinueで戻る
		if (lstrcmpi(TexInfo[i].filename, filename) != 0) continue;	// ファイル名の比較をして一致していないならcontinueで戻る
		TexNo = i;													// 一度読み込んだテクスチャがあるならその配列番号を記録してループから出る
		//if (TexInfo[(MAX_TEXTURE - 1)].lpTexture){ MessageBox(0, "保存できる最大のテクスチャ数まで到達しました!", "Texture", MB_OK); }
		break;
	}

	//	NEW_TEXTUREのままだったら新規読み込み 
	if (TexNo == NEW_TEXTURE)
	{

		//	空いてる配列を検索
		for (TexNo = 0; TexNo<MAX_TEXTURE; TexNo++) if (!TexInfo[TexNo].lpTexture) break;

		//	ファイル名保存
		CopyMemory(TexInfo[TexNo].filename, filename, strlen(filename) + 1);

		//　読み込んだ画像の情報を『メモリー』から取得
		D3DXGetImageInfoFromFileInMemory(seekBuf, sizeof(char)*bufSize, &TexInfo[TexNo].info);
		
		//	『メモリー』からテクスチャ読み込み
		hr = D3DXCreateTextureFromFileInMemoryEx(
			tdnSystem::GetDevice(),		// ビデオカードのデバイス 
			seekBuf,					// 読み込むメモリーの位置
			sizeof(char)*bufSize,		// メモリー(バッファ)のサイズ
			TexInfo[TexNo].info.Width,	// ↑で取得したサイズを使用
			TexInfo[TexNo].info.Height,	// 
			1,							// ミップマップのレベル
			0,							// テクスチャの性質や確保するメモリの場所を指定する。ミップマップを自動生成にしてる
			TexInfo[TexNo].info.Format,	// 画像のサーフェイスフォーマットを指定　取得したフォーマットを採用
			D3DPOOL_DEFAULT,			// バッファを保持するメモリ
			D3DX_FILTER_POINT,			// 線形補間
			D3DX_FILTER_POINT,			// ミップマップにも線形補間で
			0x00000000,					// 透明にする色を指定できる。しない場合はNULL(0)
			NULL,						// 元の画像の情報を格納するD3DXIMAGE_INFO構造体へのポインタを渡す。↑で保存しているのでいらない
			NULL,						// 色関係？
			&TexInfo[TexNo].lpTexture	// テクスチャへのポインタ
			);

		//　テクスチャがなかったら
		if (FAILED(hr))
		{
			delete[] saveBuf;			// newしたメモリーをしっかり消す
			return NULL;
		}
	}
	TexInfo[TexNo].UseCount++;			// そのテクスチャを利用しているカウンタを一個増やす
	
	delete[] saveBuf;					// newしたメモリーをしっかり消す
	return TexInfo[TexNo].lpTexture;


}

//	解放
void tdnTexture::Release(Texture2D* lpTexture)
{
	int	TexNo;

	enum { NOT_FIND = -1 };// 見つからなかった
	TexNo = NOT_FIND;
	// 送られてきたテクスチャ名と同じものを検索 
	for (int i = 0; i < MAX_TEXTURE; i++)
	{
		if (!TexInfo[i].lpTexture) continue;				// テクスチャの中に何も入っていないならcontinueで戻る
		if (TexInfo[i].lpTexture != lpTexture) continue;	// テクスチャの情報と一致していないならcontinueで戻る
		TexNo = i;											// 一致したらその配列番号を記録して出る
		break;
	}

	// NOT_FIND以外->見つかった
	if (TexNo != NOT_FIND)
	{
		TexInfo[TexNo].UseCount--;					// そのテクスチャを利用している2DObjのカウンタを減らす	
		if (TexInfo[TexNo].UseCount > 0) return;	// まだこのテクスチャを使っている2DObjがあるのなら残す
		//	テクスチャ解除
		tdnSystem::GetDevice()->SetTexture(0, NULL);
		lpLastTexture = NULL;						// (?)なにこれ？
		//	テクスチャ解放
		if (TexInfo[TexNo].lpTexture->Release() != D3D_OK)
		{
			//	解放失敗 (?) なんかシェーダーを使ったら解放失敗する
			//MessageBox(0, "テクスチャの解放に失敗", "Texture", MB_OK);
		}
		TexInfo[TexNo].lpTexture = NULL;
		TexInfo[TexNo].UseCount = 0;
	}
	else 
	{
		// Load関数でロードされていない場合は見つからないので、リリースのみ実行
		lpTexture->Release();
	}
}

