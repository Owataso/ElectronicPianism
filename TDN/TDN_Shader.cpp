#include	"TDNLIB.h"

/********************************************/
//	tdnShader	
/********************************************/

// (∵)後で名前と TransMat N S H などを　GetParameterByNameで指定して置く

// ID3DXEffect
// https://msdn.microsoft.com/ja-jp/library/cc372622.aspx

/**
*	■D3DXCreateEffectFromFile
*	1.デバイス　2.ファイル名のポインタ　
*   3.プリプロセッサマクロの定義　4.オプションインターフェイスポインタ　
*	5.Flag(コンパイルオプション)　6.Poolの引数、NULLにすると共有されない
*　 7.コンパイルされたエフェクトが格納されたバッファへのポインタを返す　8.コンパイルエラー		
*	
**/

/* 初期化・解放 */
tdnShader::tdnShader(char* filename)
{
	LPDEVICE lpDevice = tdnSystem::GetDevice();

	// ファイル名チェック
	char fileName[256];
	if (filename[strlen(filename) - 3] != '.') sprintf(fileName, "%s.fx", filename);
	else sprintf(fileName, "%s", filename);

	// シェーダーの読み込み
	HRESULT	hr;					// エラー報告
	LPD3DXBUFFER pErr = NULL;	// コンパイルエラーの詳細を格納
	hr = D3DXCreateEffectFromFile(lpDevice, fileName, NULL, NULL, 0, NULL, &pShader, &pErr);
	if (FAILED(hr))
	{
		// 出力にエラー表示
		char	szBuffer[512];
		sprintf(szBuffer, "\terrors: %s\n", (char*)pErr->GetBufferPointer());
		OutputDebugString(szBuffer);
		return;
	}

	// 基礎変数の読み込み
	hmWVP = pShader->GetParameterByName(NULL, "Projection");	//親構造体パラメータのハンドルNULLで最上階　パラメータ名 
	htexDecale = pShader->GetParameterByName(NULL, "Texture");
}

tdnShader::~tdnShader()
{
	pShader->Release();
}


/*********************/
// パラメーター設定
/*********************/

void tdnShader::SetWVPMatrix(Matrix* wvpMat){ pShader->SetMatrix(hmWVP, wvpMat); };
void tdnShader::SetWVPMatrix(Matrix& wvpMat){ pShader->SetMatrix(hmWVP, &wvpMat); };

void tdnShader::SetDecaleTexture(Texture2D* decaleTex){ pShader->SetTexture(htexDecale, decaleTex); }
void tdnShader::SetDecaleTexture(Texture2D& decaleTex){ pShader->SetTexture(htexDecale, &decaleTex); }

void tdnShader::SetValue(char* name, Texture2D* tex){ pShader->SetTexture(name, tex); }
void tdnShader::SetValue(char* name, Texture2D& tex){ pShader->SetTexture(name, &tex); }

void tdnShader::SetValue(char* name, Matrix* mat){ pShader->SetMatrix(name, mat); }
void tdnShader::SetValue(char* name, Matrix& mat){ pShader->SetMatrix(name, &mat); }

void tdnShader::SetValue(char* name, D3DXVECTOR4* v){ pShader->SetVector(name, v); }
void tdnShader::SetValue(char* name, D3DXVECTOR4& v){ pShader->SetVector(name, &v); }

void tdnShader::SetValue(char* name, Vector3* v){ pShader->SetFloatArray(name, (float*)v, 3); }
void tdnShader::SetValue(char* name, Vector3& v){ pShader->SetFloatArray(name, (float*)&v, 3); }

void tdnShader::SetValue(char* name, tdn2DObj* tex){ pShader->SetTexture(name, tex->GetTexture()); }
void tdnShader::SetValue(char* name, tdn2DObj& tex){ pShader->SetTexture(name, tex.GetTexture()); }

void tdnShader::SetValue(char* name, float f){ pShader->SetFloat(name, f); }
// void tdnShader::SetValue(char* name, float2 f){ pShader->SetFloatArray(name, f, 2); }// Float2
void tdnShader::SetValue(char* name, int d){ pShader->SetInt(name, d); }
void tdnShader::SetValue(char* name, DWORD d){ pShader->SetValue(name, &d, 4); }






