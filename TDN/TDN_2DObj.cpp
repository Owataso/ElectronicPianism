#include	"TDNLIB.h"
#include<mbstring.h>

// 画像の補間への対処のためのサイズ調整
// テクスチャ上のテクセルはそのど真ん中が切りの良い整数座標になっているため、
//「0.5」というのはテクセルとテクセルの調度境目に相当する位置になる
static const float abjustSize = 0.5f;

/****************************************************/
//	2DObj
/****************************************************/
tdn2DObj::tdn2DObj() :m_bLoadTexture(false)
{
	// 初期化
	m_width = m_height = 0;
	lpSurface = NULL;
	lpTexture = NULL;

	// ステータス初期化
	scale = 1.0f;
	angle = 0.0f;
	color = 0xFFFFFFFF;
	centerX = centerY = 0.0f;
	isTurnOver = false;
	isShiftCenter = false;
}

// ファイルから画像読み込み
tdn2DObj::tdn2DObj(const char* fileName):m_bLoadTexture(true)
{
	// 初期化
	m_width = m_height = 0;
	lpSurface = NULL;
	lpTexture = NULL;
	
	// テクスチャの読み込み
	lpTexture = tdnTexture::Load(fileName);
	if (lpTexture == NULL)
	{
		//MessageBox(0, "ファイルからテクスチャの読み込みに失敗", "2DObj", MB_OK);
		MyAssert(0, "エラー: ファイルからテクスチャの読み込みに失敗。\n%s", fileName);	// エラー出してるファイル名の特定
		return;
	}

	/***************************************/
	// 読み込んだテクスチャからサイズを保存
	D3DSURFACE_DESC sd;						// サーフェイスを記述する。
	lpTexture->GetLevelDesc(0, &sd);		// テクスチャの情報取得
	m_width = sd.Width;						// メンバ変数にサイズ保存 
	m_height = sd.Height;					

	// メインサーフェイスの取得(保存)
	lpTexture->GetSurfaceLevel(0, &lpSurface);

	// ステータス初期化
	scale = 1.0f;
	angle = 0.0f;
	color = 0xFFFFFFFF;
	centerX = centerY = 0.0f;
	isTurnOver = false;
	isShiftCenter = false;

}

// メモリーから画像読み込み
tdn2DObj::tdn2DObj(const char* IDName, const char* pArchiveName) :m_bLoadTexture(true)
{
	// 初期化
	m_width = m_height = 0;
	lpSurface = NULL;
	lpTexture = NULL;

	// テクスチャの読み込み
	lpTexture = tdnTexture::LoadMemory(IDName, pArchiveName);
	if (lpTexture == NULL)
	{
		MessageBox(0, "メモリーからテクスチャの読み込みに失敗\nヒント:大文字小文字の入力の差は関係ない", "2DObj", MB_OK);
		return;
	}

	/***************************************/
	// 読み込んだテクスチャからサイズを保存
	D3DSURFACE_DESC sd;						// サーフェイスを記述する。
	lpTexture->GetLevelDesc(0, &sd);		// テクスチャの情報取得
	m_width = sd.Width;						// メンバ変数にサイズ保存 
	m_height = sd.Height;

	// メインサーフェイスの取得(保存)
	lpTexture->GetSurfaceLevel(0, &lpSurface);

	// ステータス初期化
	scale = 1.0f;
	angle = 0.0f;
	color = 0xFFFFFFFF;
	centerX = centerY = 0.0f;
	isTurnOver = false;
	isShiftCenter = false;

}

// レンダーターゲット作成
tdn2DObj::tdn2DObj(UINT width, UINT height, FMT2D fmtFlag) :m_bLoadTexture(true)
{
	//	情報初期化
	m_width = m_height = 0;
	lpSurface = NULL;
	lpTexture = NULL;
	D3DFORMAT	fmt = D3DFMT_R8G8B8;
	u32			usage = 0;
	D3DPOOL		pool = D3DPOOL_MANAGED;

	
	// fmtFlagでフォーマット指定
	switch (fmtFlag)
	{
	case RENDERTARGET:
		usage = D3DUSAGE_RENDERTARGET;
		fmt = tdnSystem::GetScreenFormat();
		pool = D3DPOOL_DEFAULT;
		break;
	case FLOAT1:
		usage = D3DUSAGE_RENDERTARGET;
		fmt = D3DFMT_R32F;
		pool = D3DPOOL_DEFAULT;
		break;
	case FLOAT2:
		usage = D3DUSAGE_RENDERTARGET;
		fmt = D3DFMT_G32R32F;
		pool = D3DPOOL_DEFAULT;
		break;
	case USEALPHA:
		usage = D3DUSAGE_RENDERTARGET;
		fmt = D3DFMT_A8R8G8B8;
		pool = D3DPOOL_DEFAULT;
		break;
	case SYSTEMMEM:
		fmt = D3DFMT_A8R8G8B8;
		pool = D3DPOOL_SYSTEMMEM;
		break;
	case HDR:
		usage = D3DUSAGE_RENDERTARGET;
		fmt = D3DFMT_A16B16G16R16F;
		pool = D3DPOOL_DEFAULT;
		break;
	case SYSTEMMEM_HDR:
		fmt = D3DFMT_R32F;
		pool = D3DPOOL_SYSTEMMEM;
		break;
	default:
		MessageBox(0, "フォーマットの設定に失敗", "2DObj", MB_OK);
		break;
	}

	// ↑の設定でテクスチャ作成
	D3DXCreateTexture(tdnSystem::GetDevice(), width, height, 0, usage, fmt, pool, &lpTexture);
	if (lpTexture == NULL)
	{
		MessageBox(0, "D3DXCreateTextureの設定に失敗", "2DObj", MB_OK);
		return;
	}

	/***************************************/
	// 読み込んだテクスチャからサイズを保存
	D3DSURFACE_DESC sd;						// サーフェイスを記述する。
	lpTexture->GetLevelDesc(0, &sd);		// テクスチャの情報取得
	m_width = sd.Width;						// メンバ変数にサイズ保存 
	m_height = sd.Height;

	// メインサーフェイスの取得(保存)
	lpTexture->GetSurfaceLevel(0, &lpSurface);

	// ステータス初期化
	scale = 1.0f;
	angle = 0.0f;
	color = 0xFFFFFFFF;
	centerX = centerY = 0.0f;
	isTurnOver = false;
	isShiftCenter = false;

}

// テクスチャ参照型
tdn2DObj::tdn2DObj(Texture2D *texture) : lpTexture(texture), m_bLoadTexture(false), scale(1), angle(0), color(0xffffffff), centerX(0), centerY(0), isTurnOver(false), isShiftCenter(false)
{
	MyAssert(texture, "エラー: tdn2DObjのコンストラクタ、テクスチャが空。");

	/***************************************/
	// 読み込んだテクスチャからサイズを保存
	D3DSURFACE_DESC sd;						// サーフェイスを記述する。
	lpTexture->GetLevelDesc(0, &sd);		// テクスチャの情報取得
	m_width = sd.Width;						// メンバ変数にサイズ保存 
	m_height = sd.Height;

	// メインサーフェイスの取得(保存)
	lpTexture->GetSurfaceLevel(0, &lpSurface);
}

//	文字テクスチャをロード 一文字のみを作ります。
bool tdn2DObj::LoadFontTexture(LPCSTR character, UINT charaSize, LPCSTR fontName)
{
	//	現在保持しているテクスチャを破棄
	if (lpTexture != NULL) {
		lpTexture->Release();
		lpTexture = NULL;
	}

	//	フォントの生成
	int	fontsize = charaSize;
	LOGFONT	lf = { fontsize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS, PROOF_QUALITY, (FIXED_PITCH | FF_MODERN), "ＭＳ 明朝" };
	strcpy_s(lf.lfFaceName, fontName);
	HFONT	hFont;
	if (!(hFont = CreateFontIndirect(&lf))) {
		return FALSE;
	}

	//	デバイスコンテキスト取得
	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	//	文字コード取得
	TCHAR *c = TEXT((LPSTR)character);
	UINT code = 0;
	if (IsDBCSLeadByte(*c))
		code = (BYTE)c[0] << 8 | (BYTE)c[1];
	else
		code = c[0];

	//	フォントビットマップ取得
	TEXTMETRIC	TM;
	GetTextMetrics(hdc, &TM);
	GLYPHMETRICS	GM;
	CONST MAT2	Mat = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
	DWORD		size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
	BYTE		*ptr = new BYTE[size];
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

	// デバイスコンテキストとフォントハンドルの開放
	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hdc);

	// テクスチャ作成
	if (FAILED(tdnSystem::GetDevice()->CreateTexture(GM.gmCellIncX, TM.tmHeight, 1,
		D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
		&lpTexture, NULL)))
	if (FAILED(tdnSystem::GetDevice()->CreateTexture(GM.gmCellIncX, TM.tmHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &lpTexture, NULL)))
	{
		delete[] ptr;	return FALSE;
	}

	//	テクスチャにフォントビットマップ書き込み
	D3DLOCKED_RECT	LockedRect;
	if (FAILED(lpTexture->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD)))
	if (FAILED(lpTexture->LockRect(0, &LockedRect, NULL, 0)))
	{
		delete[] ptr;	return FALSE;
	}

	//	フォント情報の書き込み
	int	iOfs_x = GM.gmptGlyphOrigin.x;
	int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;
	int iBmp_w = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
	int iBmp_h = GM.gmBlackBoxY;
	int Level = 17;
	int	x, y;
	DWORD	Alpha, Color;
	FillMemory(LockedRect.pBits, LockedRect.Pitch * TM.tmHeight, 0);
	for (y = iOfs_y; y < (iOfs_y + iBmp_h); y++) {

		for (x = iOfs_x; x < (int)(iOfs_x + GM.gmBlackBoxX); x++) {

			Alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);

			Color = 0x00ffffff | (Alpha << 24);

			memcpy((BYTE*)LockedRect.pBits + (LockedRect.Pitch * y) + (4 * x), &Color, sizeof(DWORD));

		}

	}
	//	サーフェイスアンロック
	lpTexture->UnlockRect(0);
	delete[] ptr;

	//	パラメータをセット
	UINT	byte = _mbclen((BYTE*)character);
	m_width = GM.gmCellIncX;
	m_height = TM.tmHeight;

	return	TRUE;
}

//	解放
tdn2DObj::~tdn2DObj(){
	if (lpSurface){
		lpSurface->Release();
	}
	if (lpTexture && m_bLoadTexture){
		tdnTexture::Release(lpTexture);
	}
}

/*******************************/
//	レンダーターゲット切り替え
/*******************************/
void tdn2DObj::RenderTarget(int index)
{
	tdnSystem::GetDevice()->SetRenderTarget(index, lpSurface);
}

/*******************************/
//	描画
/*******************************/
void tdn2DObj::Render(int x, int y, u32 dwFlags)
{

	// 頂点情報
	TLVERTEX	v[4];

	// 順番　拡大(S)->回転(R)->移動(T)

	// Zは0
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0.0f;

	// 原点から拡大
	v[0].sx = v[2].sx = (float)(m_width * -0.5f - centerX) * scale;
	v[1].sx = v[3].sx = (float)(m_width * 0.5f - centerX) * scale;
	v[0].sy = v[1].sy = (float)(m_height * -0.5f - centerY) * scale;
	v[2].sy = v[3].sy = (float)(m_height * 0.5f - centerY) * scale;


	//v[0].sx = v[2].sx = (float)x;
	//v[1].sx = v[3].sx = (float)(x + this->m_width) - abjustSize; //0.5fずらす
	//v[0].sy = v[1].sy = (float)y;
	//v[2].sy = v[3].sy = (float)(y + this->m_height) - abjustSize;

	// 原点から回転
	for (DWORD i = 0; i < 4; i++){
		const float xrot = v[i].sx;
		const float yrot = v[i].sy;
		v[i].sx = xrot * cos(angle) + yrot * sin(angle);
		v[i].sy = -xrot * sin(angle) + yrot * cos(angle);
	}


	// 移動
	float transX = 0.0f;
	float transY = 0.0f;
	if (isShiftCenter)
	{
		// そのまま移動
		transX = (float)(x);
		transY = (float)(y);
	}
	else
	{
		// 元の位置に戻して移動
		transX = (float)(x + centerX + (m_width * 0.5f));
		transY = (float)(y + centerY + (m_height * 0.5f));
	}

	v[0].sx += transX;
	v[2].sx += transX;
	v[1].sx += transX - abjustSize; //0.5fずらす
	v[3].sx += transX - abjustSize; //0.5fずらす	
	v[0].sy += transY;
	v[1].sy += transY;
	v[2].sy += transY - abjustSize;
	v[3].sy += transY - abjustSize;


	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v[0].tu = v[2].tu = (float)abjustSize / (float)this->m_width;// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = 1.0;
		else
			v[1].tu = v[3].tu = 1.0f - (float)abjustSize / (float)this->m_width;

	}
	else //反転するなら
	{
		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = 0.0f;//
		else
			v[1].tu = v[3].tu = (float)abjustSize / (float)this->m_width;//

		v[0].tu = v[2].tu = 1.0;
	}
	// 縦のuv座標は変わらず
	v[0].tv = v[1].tv = (float)abjustSize / (float)this->m_height;// 
	// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v[2].tv = v[3].tv = 1.0;
	else
		v[2].tv = v[3].tv = 1.0f - (float)abjustSize / (float)this->m_height;


	// ポリゴンの色
	v[0].color = v[1].color = v[2].color = v[3].color = color;

	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;//除算数?

	// ↑の情報でポリゴン作成!!
	tdnPolygon::Render2D(v, 2, this, dwFlags);

}

void tdn2DObj::Render(int x, int y, tdnShader* shader, char* name)
{

	// 頂点情報
	TLVERTEX	v[4];

	// 順番　拡大(S)->回転(R)->移動(T)

	// Zは0
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0.0f;

	// 原点から拡大
	v[0].sx = v[2].sx = (float)(m_width * -0.5f - centerX) * scale;
	v[1].sx = v[3].sx = (float)(m_width * 0.5f - centerX) * scale;
	v[0].sy = v[1].sy = (float)(m_height * -0.5f - centerY) * scale;
	v[2].sy = v[3].sy = (float)(m_height * 0.5f - centerY) * scale;

	// 原点から回転
	for (DWORD i = 0; i < 4; i++){
		const float xrot = v[i].sx;
		const float yrot = v[i].sy;
		v[i].sx = xrot * cos(angle) + yrot * sin(angle);
		v[i].sy = -xrot * sin(angle) + yrot * cos(angle);
	}


	// 移動
	float transX = 0.0f;
	float transY = 0.0f;
	if (isShiftCenter)
	{
		// そのまま移動
		transX = (float)(x);
		transY = (float)(y);
	}
	else
	{
		// 元の位置に戻して移動
		transX = (float)(x + centerX + (m_width * 0.5f));
		transY = (float)(y + centerY + (m_height * 0.5f));
	}

	v[0].sx += transX;
	v[2].sx += transX;
	v[1].sx += transX - abjustSize; //0.5fずらす
	v[3].sx += transX - abjustSize; //0.5fずらす	
	v[0].sy += transY;
	v[1].sy += transY;
	v[2].sy += transY - abjustSize;
	v[3].sy += transY - abjustSize;


	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v[0].tu = v[2].tu = (float)abjustSize / (float)this->m_width;// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = 1.0;
		else
			v[1].tu = v[3].tu = 1.0f - (float)abjustSize / (float)this->m_width;

	}
	else //反転するなら
	{
		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = 0.0f;//
		else
			v[1].tu = v[3].tu = (float)abjustSize / (float)this->m_width;//

		v[0].tu = v[2].tu = 1.0;
	}
	// 縦のuv座標は変わらず
	v[0].tv = v[1].tv = (float)abjustSize / (float)this->m_height;// 
	// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v[2].tv = v[3].tv = 1.0;
	else
		v[2].tv = v[3].tv = 1.0f - (float)abjustSize / (float)this->m_height;


	// ポリゴンの色
	v[0].color = v[1].color = v[2].color = v[3].color = color;

	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;//除算数?

	// ↑の情報でポリゴン作成!!
	tdnPolygon::Render2D(v, 2, this, shader, name);

}

// サイズ指定の2D描画
void tdn2DObj::Render(int x, int y, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags)
{
	// 頂点情報
	TLVERTEX	v[4];

	// 順番　拡大(S)->回転(R)->移動(T)

	// 原点から拡大
	v[0].sx = v[2].sx = (float)(w * -0.5f - centerX) * scale;
	v[1].sx = v[3].sx = (float)(w * 0.5f - centerX) * scale;
	v[0].sy = v[1].sy = (float)(h * -0.5f - centerY) * scale;
	v[2].sy = v[3].sy = (float)(h * 0.5f - centerY) * scale;

	// 原点から回転
	for (DWORD i = 0; i < 4; i++){
		const float xrot = v[i].sx;
		const float yrot = v[i].sy;
		v[i].sx = xrot * cos(angle) + yrot * sin(angle);
		v[i].sy = -xrot * sin(angle) + yrot * cos(angle);
	}

	// 移動
	float transX = 0.0f;
	float transY = 0.0f;
	if (isShiftCenter)
	{
		// そのまま移動
		transX = (float)(x);
		transY = (float)(y);
	}
	else
	{
		// 元の位置に戻して移動
		transX = (float)(x + centerX + (w * 0.5f));
		transY = (float)(y + centerY + (h * 0.5f));
	}

	v[0].sx += transX;
	v[2].sx += transX;
	v[1].sx += transX - abjustSize; //0.5fずらす
	v[3].sx += transX - abjustSize; //0.5fずらす	
	v[0].sy += transY;
	v[1].sy += transY;
	v[2].sy += transY - abjustSize;
	v[3].sy += transY - abjustSize;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v[0].tu = v[2].tu = (float)(tx + (float)abjustSize) / ((float)this->m_width);// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = (float)(tx + tw) / (float)this->m_width;
		else
			v[1].tu = v[3].tu = (float)((tx + tw) - abjustSize) / (float)this->m_width;

	}
	else //反転するなら
	{
		if (angle == 0.0f)
			v[1].tu = v[3].tu = (float)(tx) / ((float)this->m_width);//
		else
			v[1].tu = v[3].tu = (float)(tx + (float)abjustSize) / ((float)this->m_width);//

		v[0].tu = v[2].tu = (float)((tx + tw) - (float)abjustSize) / ((float)this->m_width);
	}

	// 縦のuv座標は変わらず
	v[0].tv = v[1].tv = (float)(ty + abjustSize) / (float)this->m_height;// 
	/// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v[2].tv = v[3].tv = (float)((ty + th)) / (float)this->m_height;
	else
		v[2].tv = v[3].tv = (float)((ty + th) - abjustSize) / (float)this->m_height;


	// ポリゴンの色
	v[0].color = v[1].color = v[2].color = v[3].color = color;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;//除算数?	
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0.0f;// Zは0

	// ↑の情報でポリゴン作成!!
	tdnPolygon::Render2D(v, 2, this, dwFlags);

}

void tdn2DObj::Render(int x, int y, int w, int h, int tx, int ty, int tw, int th, tdnShader* shader, char* tech)
{
	// 頂点情報
	TLVERTEX	v[4];

	// 順番　拡大(S)->回転(R)->移動(T)

	// 原点から拡大
	v[0].sx = v[2].sx = (float)(w * -0.5f - centerX) * scale;
	v[1].sx = v[3].sx = (float)(w * 0.5f - centerX) * scale;
	v[0].sy = v[1].sy = (float)(h * -0.5f - centerY) * scale;
	v[2].sy = v[3].sy = (float)(h * 0.5f - centerY) * scale;

	// 原点から回転
	for (DWORD i = 0; i < 4; i++){
		const float xrot = v[i].sx;
		const float yrot = v[i].sy;
		v[i].sx = xrot * cos(angle) + yrot * sin(angle);
		v[i].sy = -xrot * sin(angle) + yrot * cos(angle);
	}

	// 移動
	float transX = 0.0f;
	float transY = 0.0f;
	if (isShiftCenter)
	{
		// そのまま移動
		transX = (float)(x);
		transY = (float)(y);
	}
	else
	{
		// 元の位置に戻して移動
		transX = (float)(x + centerX + (w * 0.5f));
		transY = (float)(y + centerY + (h * 0.5f));
	}

	v[0].sx += transX;
	v[2].sx += transX;
	v[1].sx += transX - abjustSize; //0.5fずらす
	v[3].sx += transX - abjustSize; //0.5fずらす	
	v[0].sy += transY;
	v[1].sy += transY;
	v[2].sy += transY - abjustSize;
	v[3].sy += transY - abjustSize;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v[0].tu = v[2].tu = (float)(tx + (float)abjustSize) / ((float)this->m_width);// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v[1].tu = v[3].tu = (float)(tx + tw) / (float)this->m_width;
		else
			v[1].tu = v[3].tu = (float)((tx + tw) - abjustSize) / (float)this->m_width;

	}
	else //反転するなら
	{
		if (angle == 0.0f)
			v[1].tu = v[3].tu = (float)(tx) / ((float)this->m_width);//
		else
			v[1].tu = v[3].tu = (float)(tx + (float)abjustSize) / ((float)this->m_width);//

		v[0].tu = v[2].tu = (float)((tx + tw) - (float)abjustSize) / ((float)this->m_width);
	}

	// 縦のuv座標は変わらず
	v[0].tv = v[1].tv = (float)(ty + abjustSize) / (float)this->m_height;// 
	/// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v[2].tv = v[3].tv = (float)((ty + th)) / (float)this->m_height;
	else
		v[2].tv = v[3].tv = (float)((ty + th) - abjustSize) / (float)this->m_height;


	// ポリゴンの色
	v[0].color = v[1].color = v[2].color = v[3].color = color;
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0f;//除算数?	
	v[0].sz = v[1].sz = v[2].sz = v[3].sz = 0.0f;// Zは0

	// ↑の情報でポリゴン作成!!
	tdnPolygon::Render2D(v, 2, this, shader, tech);


}


// 3D描画
void tdn2DObj::Render3D(float x, float y, float z, u32 dwFlags)
{

	// 順番　拡大(S)->回転(R)->移動(T)

	// 3D用頂点
	LVERTEX v2[4];

	// ゲームの→　と　↑　を指定
	Vector3 right(matView._11, matView._21, matView._31);// +で→　-で←
	Vector3 up(matView._12, matView._22, matView._32);// +で↑　-で↓
	right.Normalize();
	up.Normalize();

	// ビルボードのベクトルを回転
	float cosan(cosf(angle)), sinan(sinf(angle));
	Vector3 turned_right, turned_up;
	turned_right = right * cosan + up * sinan;

	turned_right = right * cos(angle) + up * sin(angle);
	turned_up = -right * sin(angle) + up * cos(angle);

	// 拡大率&幅
	// v[0].sx = v[2].sx = (float)(m_width * -0.5f - centerX) * scale;
	float maxSize;
	(m_width > m_height) ? ((float)(maxSize = (float)m_width)) : ((float)(maxSize = (float)m_height));
	float sizeX = (float)(m_width / maxSize)* scale;
	float sizeY = (float)(m_height / maxSize)* scale;

	// ポジションの初期設定			  
	v2[0].x = -(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//←　行って　↑ = 左上
	v2[0].y = -(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[0].z = -(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[1].x = +(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//→　行って　↑　= 右上
	v2[1].y = +(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[1].z = +(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[2].x = -(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//←　行って　↓　= 右下
	v2[2].y = -(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[2].z = -(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	v2[3].x = +(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//→　行って　↓　= 右下
	v2[3].y = +(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[3].z = +(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	// 回転(R)
	//for (DWORD i = 0; i < 4; i++){
	//	const float xrot = v2[i].x;
	//	const float yrot = v2[i].y;
	//	const float zrot = v2[i].z;

	//	v2[i].x = xrot * cos(angle) + yrot * sin(angle);
	//	v2[i].y = -xrot * sin(angle) + yrot * cos(angle);
	//}

	// 頂点を平行移動(T)	
	v2[0].x += (float)x;
	v2[0].y += (float)y;
	v2[0].z += (float)z;

	v2[1].x += (float)x;
	v2[1].y += (float)y;
	v2[1].z += (float)z;

	v2[2].x += (float)x;
	v2[2].y += (float)y;
	v2[2].z += (float)z;

	v2[3].x += (float)x;
	v2[3].y += (float)y;
	v2[3].z += (float)z;


	// ポリゴンの色
	v2[0].color = v2[1].color = v2[2].color = v2[3].color = color;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v2[0].tu = v2[2].tu = (float)abjustSize / (float)this->m_width;// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = 1.0f;
		else
			v2[1].tu = v2[3].tu = 1.0f - (float)abjustSize / (float)this->m_width;

	}
	else //反転するなら
	{
		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = 0.0f;//
		else
			v2[1].tu = v2[3].tu = (float)abjustSize / (float)this->m_width;//

		v2[0].tu = v2[2].tu = 1.0f;
	}
	// 縦のuv座標は変わらず
	v2[0].tv = v2[1].tv = (float)abjustSize / (float)this->m_height;// 
	// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v2[2].tv = v2[3].tv = 1.0f;
	else
		v2[2].tv = v2[3].tv = 1.0f - (float)abjustSize / (float)this->m_height;


	//描画
	tdnPolygon::Render3D(v2, 2, this, dwFlags);

}

void tdn2DObj::Render3D(Vector3 pos, u32 dwFlags)
{

	// 順番　拡大(S)->回転(R)->移動(T)

	// 3D用頂点
	LVERTEX v2[4];

	// ゲームの→　と　↑　を指定
	Vector3 right(matView._11, matView._21, matView._31);// +で→　-で←
	Vector3 up(matView._12, matView._22, matView._32);// +で↑　-で↓
	right.Normalize();
	up.Normalize();

	// ビルボードのベクトルを回転
	float cosan(cosf(angle)), sinan(sinf(angle));
	Vector3 turned_right, turned_up;
	turned_right = right * cosan + up * sinan;

	turned_right = right * cos(angle) + up * sin(angle);
	turned_up = -right * sin(angle) + up * cos(angle);

	// 拡大率&幅
	// v[0].sx = v[2].sx = (float)(m_width * -0.5f - centerX) * scale;
	float maxSize;
	(m_width > m_height) ? ((float)(maxSize = (float)m_width)) : ((float)(maxSize = (float)m_height));
	float sizeX = (float)(m_width / maxSize)* scale;
	float sizeY = (float)(m_height / maxSize)* scale;

	// ポジションの初期設定			  
	v2[0].x = -(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//←　行って　↑ = 左上
	v2[0].y = -(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[0].z = -(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[1].x = +(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//→　行って　↑　= 右上
	v2[1].y = +(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[1].z = +(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[2].x = -(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//←　行って　↓　= 右下
	v2[2].y = -(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[2].z = -(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	v2[3].x = +(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//→　行って　↓　= 右下
	v2[3].y = +(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[3].z = +(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	// 回転(R)
	//for (DWORD i = 0; i < 4; i++){
	//	const float xrot = v2[i].x;
	//	const float yrot = v2[i].y;
	//	const float zrot = v2[i].z;

	//	v2[i].x = xrot * cos(angle) + yrot * sin(angle);
	//	v2[i].y = -xrot * sin(angle) + yrot * cos(angle);
	//}

	// 頂点を平行移動(T)	
	v2[0].x += (float)pos.x;
	v2[0].y += (float)pos.y;
	v2[0].z += (float)pos.z;

	v2[1].x += (float)pos.x;
	v2[1].y += (float)pos.y;
	v2[1].z += (float)pos.z;

	v2[2].x += (float)pos.x;
	v2[2].y += (float)pos.y;
	v2[2].z += (float)pos.z;

	v2[3].x += (float)pos.x;
	v2[3].y += (float)pos.y;
	v2[3].z += (float)pos.z;


	// ポリゴンの色
	v2[0].color = v2[1].color = v2[2].color = v2[3].color = color;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v2[0].tu = v2[2].tu = (float)abjustSize / (float)this->m_width;// ほんの少しUV座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = 1.0f;
		else
			v2[1].tu = v2[3].tu = 1.0f - (float)abjustSize / (float)this->m_width;

	}
	else //反転するなら
	{
		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = 0.0f;//
		else
			v2[1].tu = v2[3].tu = (float)abjustSize / (float)this->m_width;//

		v2[0].tu = v2[2].tu = 1.0f;
	}
	// 縦のuv座標は変わらず
	v2[0].tv = v2[1].tv = (float)abjustSize / (float)this->m_height;// 
	// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v2[2].tv = v2[3].tv = 1.0f;
	else
		v2[2].tv = v2[3].tv = 1.0f - (float)abjustSize / (float)this->m_height;


	//描画
	tdnPolygon::Render3D(v2, 2, this, dwFlags);

}

void tdn2DObj::Render3D(float x, float y, float z, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags)
{
	// 順番　拡大(S)->回転(R)->移動(T)

	// 3D用頂点
	LVERTEX v2[4];

	// ゲームの→　と　↑　を指定
	Vector3 right(matView._11, matView._21, matView._31);// +で→　-で←
	Vector3 up(matView._12, matView._22, matView._32);// +で↑　-で↓
	right.Normalize();
	up.Normalize();

	// ビルボードのベクトルを回転
	float cosan(cosf(angle)), sinan(sinf(angle));
	Vector3 turned_right, turned_up;
	turned_right = right * cosan + up * sinan;
	turned_right = right * cos(angle) + up * sin(angle);
	turned_up = -right * sin(angle) + up * cos(angle);

	// 拡大率&幅
	float maxSize;
	(m_width > m_height) ? ((float)(maxSize = (float)m_width)) : ((float)(maxSize = (float)m_height));
	float sizeX = (float)(w / maxSize)* scale;
	float sizeY = (float)(h / maxSize)* scale;

	// ポジションの初期設定			  
	v2[0].x = -(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//←　行って　↑ = 左上
	v2[0].y = -(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[0].z = -(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[1].x = +(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//→　行って　↑　= 右上
	v2[1].y = +(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[1].z = +(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[2].x = -(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//←　行って　↓　= 右下
	v2[2].y = -(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[2].z = -(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	v2[3].x = +(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//→　行って　↓　= 右下
	v2[3].y = +(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[3].z = +(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));


	// 頂点を平行移動(T)	
	v2[0].x += (float)x;
	v2[0].y += (float)y;
	v2[0].z += (float)z;

	v2[1].x += (float)x;
	v2[1].y += (float)y;
	v2[1].z += (float)z;

	v2[2].x += (float)x;
	v2[2].y += (float)y;
	v2[2].z += (float)z;

	v2[3].x += (float)x;
	v2[3].y += (float)y;
	v2[3].z += (float)z;


	// ポリゴンの色
	v2[0].color = v2[1].color = v2[2].color = v2[3].color = color;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v2[0].tu = v2[2].tu = (float)(tx + (float)abjustSize) / ((float)this->m_width);// ほんの少しUv2座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = (float)(tx + tw) / (float)this->m_width;
		else
			v2[1].tu = v2[3].tu = (float)((tx + tw) - abjustSize) / (float)this->m_width;

	}
	else //反転するなら
	{
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = (float)(tx) / ((float)this->m_width);//
		else
			v2[1].tu = v2[3].tu = (float)(tx + (float)abjustSize) / ((float)this->m_width);//

		v2[0].tu = v2[2].tu = (float)((tx + tw) - (float)abjustSize) / ((float)this->m_width);
	}

	// 縦のuv2座標は変わらず
	v2[0].tv = v2[1].tv = (float)(ty + abjustSize) / (float)this->m_height;// 
	/// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v2[2].tv = v2[3].tv = (float)((ty + th)) / (float)this->m_height;
	else
		v2[2].tv = v2[3].tv = (float)((ty + th) - abjustSize) / (float)this->m_height;


	//描画
	tdnPolygon::Render3D(v2, 2, this, dwFlags);


}

void tdn2DObj::Render3D(Vector3 pos, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags)
{
	// 順番　拡大(S)->回転(R)->移動(T)

	// 3D用頂点
	LVERTEX v2[4];

	// ゲームの→　と　↑　を指定
	Vector3 right(matView._11, matView._21, matView._31);// +で→　-で←
	Vector3 up(matView._12, matView._22, matView._32);// +で↑　-で↓
	right.Normalize();
	up.Normalize();

	// ビルボードのベクトルを回転
	float cosan(cosf(angle)), sinan(sinf(angle));
	Vector3 turned_right, turned_up;
	turned_right = right * cosan + up * sinan;
	turned_right = right * cos(angle) + up * sin(angle);
	turned_up = -right * sin(angle) + up * cos(angle);

	// 拡大率&幅
	float maxSize;
	(m_width > m_height) ? ((float)(maxSize = (float)m_width)) : ((float)(maxSize = (float)m_height));
	float sizeX = (float)(w / maxSize)* scale;
	float sizeY = (float)(h / maxSize)* scale;

	// ポジションの初期設定			  
	v2[0].x = -(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//←　行って　↑ = 左上
	v2[0].y = -(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[0].z = -(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[1].x = +(turned_right.x*(sizeX)) + (turned_up.x * (sizeY));//→　行って　↑　= 右上
	v2[1].y = +(turned_right.y*(sizeX)) + (turned_up.y * (sizeY));
	v2[1].z = +(turned_right.z*(sizeX)) + (turned_up.z * (sizeY));

	v2[2].x = -(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//←　行って　↓　= 右下
	v2[2].y = -(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[2].z = -(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));

	v2[3].x = +(turned_right.x*(sizeX)) - (turned_up.x * (sizeY));//→　行って　↓　= 右下
	v2[3].y = +(turned_right.y*(sizeX)) - (turned_up.y * (sizeY));
	v2[3].z = +(turned_right.z*(sizeX)) - (turned_up.z * (sizeY));


	// 頂点を平行移動(T)	
	v2[0].x += (float)pos.x;
	v2[0].y += (float)pos.y;
	v2[0].z += (float)pos.z;

	v2[1].x += (float)pos.x;
	v2[1].y += (float)pos.y;
	v2[1].z += (float)pos.z;

	v2[2].x += (float)pos.x;
	v2[2].y += (float)pos.y;
	v2[2].z += (float)pos.z;

	v2[3].x += (float)pos.x;
	v2[3].y += (float)pos.y;
	v2[3].z += (float)pos.z;


	// ポリゴンの色
	v2[0].color = v2[1].color = v2[2].color = v2[3].color = color;

	// テクスチャ内の座標(UV)
	if (isTurnOver == false)
	{
		v2[0].tu = v2[2].tu = (float)(tx + (float)abjustSize) / ((float)this->m_width);// ほんの少しUv2座標をずらす

		// 回転を適用したなら　補間のずれを修正するため少しずらす
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = (float)(tx + tw) / (float)this->m_width;
		else
			v2[1].tu = v2[3].tu = (float)((tx + tw) - abjustSize) / (float)this->m_width;

	}
	else //反転するなら
	{
		if (angle == 0.0f)
			v2[1].tu = v2[3].tu = (float)(tx) / ((float)this->m_width);//
		else
			v2[1].tu = v2[3].tu = (float)(tx + (float)abjustSize) / ((float)this->m_width);//

		v2[0].tu = v2[2].tu = (float)((tx + tw) - (float)abjustSize) / ((float)this->m_width);
	}

	// 縦のuv2座標は変わらず
	v2[0].tv = v2[1].tv = (float)(ty + abjustSize) / (float)this->m_height;// 
	/// 回転を適用したなら　補間のずれを修正するため少しずらす
	if (angle == 0.0f)
		v2[2].tv = v2[3].tv = (float)((ty + th)) / (float)this->m_height;
	else
		v2[2].tv = v2[3].tv = (float)((ty + th) - abjustSize) / (float)this->m_height;


	//描画
	tdnPolygon::Render3D(v2, 2, this, dwFlags);


}




//-----------------------------------
///	情報更新
//-----------------------------------

// 拡大縮小
void tdn2DObj::SetScale(float scale)
{
	this->scale = scale;
}

// 回転
void tdn2DObj::SetAngle(float angle)
{
	this->angle = angle;
}

// 色調整 0~255まで
void tdn2DObj::SetARGB(BYTE A, BYTE R, BYTE G, BYTE B)
{

	// ARGBをchar型四つ分に入れていく
	color = ARGB(A, R, G, B);
	// 各頂点に反映
	//v[0].color = v[1].color = v[2].color = v[3].color = color;

}

// 色調整(int型) 0~255まで
void tdn2DObj::SetARGB(int A, int R, int G, int B)
{
	//ラムダ式Min~Maxの範囲に抑える　0~255
	BYTE a = (BYTE)Math::Clamp((float)A, 0, 255);
	BYTE r = (BYTE)Math::Clamp((float)R, 0, 255);
	BYTE g = (BYTE)Math::Clamp((float)G, 0, 255);
	BYTE b = (BYTE)Math::Clamp((float)B, 0, 255);

	/// ARGBをchar型四つ分に入れていく
	// DWORD color = ARGB(a, r, g, b);
	color = ARGB(a, r, g, b);
	// 各頂点に反映
	//v[0].color = v[1].color = v[2].color = v[3].color = color;

}

// 16進数での色調整
void tdn2DObj::SetARGB(DWORD ARGB)
{
	color = ARGB;
	// 各頂点に反映
	//v[0].color = v[1].color = v[2].color = v[3].color = color;

}

void tdn2DObj::SetAlpha(BYTE A)
{
	DWORD orgColor = (color & 0x00ffffff);
	color = (A << 24) | orgColor;
}

// 
void tdn2DObj::SetAlpha(float A)
{
	assert(A >= 0 && A <= 1);
	SetAlpha((BYTE)(A * 255));
}

void tdn2DObj::SetRGB(BYTE R, BYTE G, BYTE B)
{
	DWORD orgColor = (color & 0xff000000);
	color = (R << 16) | (G << 8) | (B) | orgColor;

}

void tdn2DObj::SetRGB(int R, int G, int B)
{
	BYTE r = (BYTE)Math::Clamp((float)R, 0, 255);
	BYTE g = (BYTE)Math::Clamp((float)G, 0, 255);
	BYTE b = (BYTE)Math::Clamp((float)B, 0, 255);

	DWORD orgColor = (color & 0xff000000);
	color = (r << 16) | (g << 8) | (b) | orgColor;
}

void tdn2DObj::SetTurnOver(bool turnFlag)
{

	isTurnOver = turnFlag;

}

void tdn2DObj::SetShiftCenter(bool ShiftFlag)
{
	//　画像を座標の中心に持っていくか
	isShiftCenter = ShiftFlag;

}

// 回転の中心変更
void tdn2DObj::SetCenter(float x, float y)
{
	centerX = x;
	centerY = y;
}
