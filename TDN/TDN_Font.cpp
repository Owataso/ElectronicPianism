#include "TDNLIB.h"
#include<mbstring.h>

/************************/
//		tdnFont
/************************/

/****** 前方宣言 *******/

//	管理用パラメータ									
bool tdnFont::m_IsInitialized = false;
tdnFont::CacheDesc		tdnFont::m_CacheDesc = { 0 }; //キャッシュの色々

// 絵文字
LPCSTR tdnFont::m_lpcsPictGraphStrs[(int)PICTOGRAPH::MAX]
{
	"∇"		// 初心者マーク
};

tdn2DObj* tdnFont::m_pPictograph = nullptr;

/***********************/


/***********************************************/
//		初期化・解放
/***********************************************/
void	tdnFont::Initialize()
{
	//	通常文字キャッシュの初期化
	{
		//	すでに確保されている領域がある場合は破棄
		if (m_CacheDesc.textureCacheList){ delete[] m_CacheDesc.textureCacheList; m_CacheDesc.textureCacheList = nullptr; }		//	文字テクスチャのリスト
		if (m_CacheDesc.referenceDataList){ delete[] m_CacheDesc.referenceDataList; m_CacheDesc.referenceDataList = nullptr; }	//	キャッシュの照合用データリスト
															//	キャッシュを保存する領域を確保
		m_CacheDesc.textureCacheList = new tdn2DObj[CacheQty];  // 文字テクスチャのリスト
		m_CacheDesc.referenceDataList = new	RefData[CacheQty];	// キャッシュの照合用データのリスト
																//	キャッシュ分初期化
		for (UINT i = 0; i < CacheQty; i++)
		{
			m_CacheDesc.referenceDataList[i].isEnable = false;// 有効フラグをfalse
		}

		m_CacheDesc.nextUseCacheNum = 0;//	次に使用するキャッシュの番号
	}

	// 絵文字用
	m_pPictograph = new tdn2DObj("Data/System/Pictograph.png");

	//	初期化フラグを立てる
	m_IsInitialized = true;
}

void	tdnFont::Release()
{
	//	通常キャッシュ
	{
		if (m_CacheDesc.textureCacheList){ delete[] m_CacheDesc.textureCacheList; m_CacheDesc.textureCacheList = nullptr; }		//	文字テクスチャのリスト
		if (m_CacheDesc.referenceDataList){ delete[] m_CacheDesc.referenceDataList; m_CacheDesc.referenceDataList = nullptr; }	//	キャッシュの照合用データリスト
		m_CacheDesc.nextUseCacheNum = 0;
	}

	// 絵文字
	SAFE_DELETE(m_pPictograph);
	
	//	初期化フラグをリセット
	m_IsInitialized = false;
}

/***********************************************/
//		文字描画
/***********************************************/

//文字列描画
void	tdnFont::RenderString(LPCSTR string, int fontSize, int drawX, int  drawY, COLOR dwColor, LPCSTR fontName, u32 dwFlags)
{
	//	変数の用意
	int	x = drawX;
	int	y = drawY;
	UINT	myByte = 0;
	//	終端文字までループ
	for (UINT i = 0; string[i] != '\0'; i += myByte)
	{
		//	文字のバイト数を調べる	
		myByte = _mbclen((BYTE*)&string[i]);

		// 改行コードがあれば改行して次の文字へ
		if (string[i] == '\n')
		{
			x = drawX;
			y += (int)fontSize + 8;// 初期の文字サイズ分+幅で下げている
			continue;
		}

		//	文字を描画
		int	drawCoordX = x;
		int	drawCoordY = y;
		Vector2	drawedSize = RenderCharacter(&string[i], fontName, fontSize, drawCoordX, drawCoordY, dwColor, dwFlags);

		//	描画座標をずらす
		x += (int)drawedSize.x;
	}
}

void tdnFont::RenderString(int fontSize, int drawX, int  drawY, COLOR dwColor, const char * _Format, ...)
{
	char str[1024]{};
	va_list list;
	va_start(list, _Format);
	vsprintf(str, _Format, list);
	va_end(list);

	RenderString(str, fontSize, drawX, drawY, dwColor, "メイリオ");
}


// 一文字描画
Vector2	tdnFont::RenderCharacter(LPCSTR character, LPCSTR fontName, int fontSize, int drawX, int  drawY, COLOR dwColor, u32 dwFlags)
{
	//	文字テクスチャ管理パラメータが初期化されているかチェック
	if (!m_IsInitialized)	Initialize();

	//	変数の用意
	tdn2DObj*	pImage;
	UINT	createSize;
	UINT	byteSize = _mbclen((BYTE*)character);

	//	幅と高さを比較して大きいほうを基準にテクスチャを作成
	int		createSizeW = (int)((float)fontSize);
	int		createSizeH = (int)((float)fontSize);
	if (createSizeW > createSizeH)	createSize = createSizeW;
	else							createSize = createSizeH;

	//	使用するテクスチャのアドレス[配列番号]をセット
	pImage = &m_CacheDesc.textureCacheList[SearchCache(character, createSize, fontName)];

	//	空白チェック
	bool	bDraw = true;
	
	switch (byteSize) 
	{
	case	1:
		if (character[0] == ' ') { bDraw = false;	}
		break;
	case	2:// マルチバイト
	{
		char	buffer[3] = { character[0], character[1], '\0' };
		if (strcmp(buffer, "　") == 0) 
		{
			bDraw = false;
		}

		// ★絵文字検索
		bDraw = SearchPictograph(buffer, drawX, drawY, pImage->GetWidth(), dwColor, dwFlags);
	}
	break;
	}

	//	描画
	if (bDraw)
	{
		pImage->SetARGB(dwColor);
		pImage->Render(drawX, drawY, pImage->GetWidth(), pImage->GetHeight(),
			0, 0, pImage->GetWidth(), pImage->GetHeight(), dwFlags);
	}

	//	描画した文字のサイズを返す
	return	Vector2((float)pImage->GetWidth(), (float)pImage->GetHeight());
}


//  簡易
void tdnFont::RenderFont2D(LPCSTR _String, int _FontSize, int _DrawX, int _DrawY, COLOR dwColor)
{
	LPSTR	c_FontName = "HGP創英角ﾎﾟｯﾌﾟ体";

	RenderString(_String, _FontSize,
		_DrawX, _DrawY, dwColor, c_FontName, RS::COPY);
}

// 中央揃へ
void tdnFont::RenderStringCentering(LPCSTR string, int fontSize, int drawX, int  drawY, COLOR dwColor, LPCSTR fontName, u32 dwFlags)
{
	//	変数の用意
	int	x = drawX;
	int	y = drawY;
	UINT	myByte = 0;
	UINT	addByte = 0;
	UINT	prevByte = 0;
	std::string str;
	str = string;

	//	終端文字又は改行までループ
	for (UINT i = 0; string[i] != '\0'; )
	{
		// 更新
		i += myByte;

		//	文字のバイト数を調べる	
		myByte = _mbclen((BYTE*)&string[i]);
		addByte += _mbclen((BYTE*)&string[i]);


		// 改行まできたら一度描画してリセット
		if (string[i] == '\n'|| string[i] == '\0')
		{	
			
			// 中央揃への処理
			x = drawX - (int)(((addByte - prevByte)*(fontSize * 0.5f)) * 0.5f);

			// ここで描画	
			std::string subString = str.substr(prevByte, (addByte - prevByte));
			prevByte = addByte;// 前回のByte数を変更

			// 文字列描画
			RenderString(subString.c_str(), fontSize, x, y, dwColor, fontName, dwFlags);

			// 改行　初期の文字サイズ分+幅で下げている
			y += (int)fontSize + 8;

		}
		
	}
	

	

}


/****************************************/
//	 2DObjから文字を作るサポート関数
/****************************************/
UINT	tdnFont::SearchCache(LPCSTR chara, UINT size, LPCSTR fontName)
{
	//	使用可能なキャッシュを探す
	UINT	sizeAllow = 0;							//	許容サイズ差
	UINT	byteSize = _mbclen((BYTE*)chara);	//	バイト数
												//	キャッシュ数分ループ
	for (UINT cNum = 0; cNum < CacheQty; cNum++) {
		RefData&	refData = m_CacheDesc.referenceDataList[cNum];
		if (!refData.isEnable)									continue;
		//	バイト数判定
		if (refData.byte != byteSize)							continue;
		//	文字判定（バイト数に応じて分岐）
		if (byteSize != 2) {
			if (refData.chara[0] != chara[0])					continue;
		}
		else {
			if (refData.chara[0] != chara[0])					continue;
			if (refData.chara[1] != chara[1])					continue;
		}
		//	サイズ判定
		if (refData.size - sizeAllow > size)					continue;
		if (refData.size + sizeAllow < size)					continue;
		//	フォント判定
		if (strlen(refData.fontName) == 0) {
			if (strlen(fontName) != 0)						continue;
		}
		else {
			if (strlen(fontName) == 0)						continue;
			if (strcmp(refData.fontName, fontName) != 0)	continue;
		}
		//	判定をクリア
		return	cNum;
	}

	//	新しく文字テクスチャを作成
	UINT	useNum = m_CacheDesc.nextUseCacheNum;
	m_CacheDesc.textureCacheList[useNum].LoadFontTexture(chara, size, fontName);
	//	照合用パラメータを更新
	RefData&	refData = m_CacheDesc.referenceDataList[useNum];
	refData.chara[0] = chara[0];
	refData.chara[1] = chara[1];
	refData.size = size;
	refData.byte = byteSize;
	refData.isEnable = true;
	refData.fontName = fontName;

	//	次に書き換えるキャッシュの番号を更新
	if (++m_CacheDesc.nextUseCacheNum == CacheQty)	m_CacheDesc.nextUseCacheNum = 0;

	//	新しくテクスチャを作成した番号を返す
	return	useNum;
}

bool tdnFont::SearchPictograph(char buffer[3], int x, int y, UINT size, DWORD color, DWORD RenderFlag)
{
	// ★絵文字検索


	// 文字の色　(TODO) 今はアルファだけ変えています
	DWORD col = color & 0xff000000;
	col = col | 0x00ffffff;

	m_pPictograph->SetARGB(col);

	FOR((int)PICTOGRAPH::MAX)
	{
		if (strcmp(buffer, m_lpcsPictGraphStrs[i]) == 0)
		{
			m_pPictograph->Render(x, y, size, size, i * 32, 0, 32, 32, RenderFlag);
			return false;
		}
	}


	// 絵文字が見つからなかったので描画してよし
	return true;
}
