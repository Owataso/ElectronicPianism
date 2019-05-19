#pragma once
#define _CRT_SECURE_NO_WARNINGS// (?)警告を無効にする
#define	_CRT_SECURE_NO_DEPRECATE
#define	DIRECTINPUT_VERSION	0x0800 
//#define NO_WARN_MBCS_MFC_DEPRECATION

//#define _V120_	// Visual Studio 2013 でビルドした場合(現在は手動で分岐する必要がある。どうしたらいいものか)
#define _V140_	// Visual Studio 2015 でビルドした場合

// スタティックリンク
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "winmm.lib" )	// 経過時間を扱うライブラリ
#pragma comment (lib,"dinput8.lib")
#pragma comment (lib, "dsound.lib" )
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "msacm32.lib")	// データの変換を行う(ここではMP3→WAVの変換に用いる)

// インクルード 
#include	<windows.h>		// Windowsアプリケーションを作成
#include	<d3dx9.h>		// DirectXを扱うためのヘッダー
#include	<process.h>		// スレッドを扱うためのヘッダー
#include	<dinput.h>		// DirectInputを扱うためのヘッダー
#include	<dsound.h>		// DirectSoundを扱うためのヘッダー
#include	<mmsystem.h>	// 処理時間を計算するために使う関数が入っている
#include	<stdarg.h>		// 可変長引数を処理するマクロが定義されています
#include	<time.h>		// 主に時間を扱うための関数が定義されています
#include	<math.h>		// 計算に役に立つ関数やマクロが定義されています
#include	<vector>		// 動的配列を扱うヘッダー
#include	<stack>			// 
#include	<map>			// 連想配列を扱うヘッダー
#include	<queue>
#include	<fstream>		// ファイルを扱う関数などが定義されています
#include	<assert.h>		// アサートを扱うヘッダー
#include	<memory>
#include	<random>		// メルセンヌ・ツイスターなどを扱うためのヘッダー
#include	<list>			// 双方向リストを扱うへっだー
#include	<string>		// std::string型を扱うために必要なヘッダー
#include	<memory>		// スマートポインタを扱うためのヘッダー
#include	<thread>		// C++11にて追加されたstd::threadを扱うためのヘッダー

#include	"../source/system/ItDebug.h"

/********************************************/
//	定数	
/********************************************/
#define VECTOR_ZERO Vector3(0.0f,0.0f,0.0f)
#define	PI	((FLOAT)  3.141592654f)

/********************************************/
//	マクロ	
/********************************************/
#define SAFE_DELETE(p) if(p){ delete p;p=nullptr;}
#define SAFE_RELEASE(p) { if(p){ (p)->Release(); (p)=nullptr; } }
#define POINTER_ARRAY_DELETE(p,num) for(int i=0;i<num;i++){if(p[i]){delete p[i];p[i]=nullptr;}}
#define DOUBLE_POINTER_DELETE(p,num) if(p){for(int i=0;i<num;i++){ if(p[i]){delete p[i];} }delete[] p;p=nullptr;}
#define FOR(len) for (int i = 0; i < len; i++)
#define ARGB(a,r,g,b)	((DWORD)( (a<<24) | (r<<16) | (g<<8) | (b) ) )


/********************************************/
//	変数型定義
/********************************************/
typedef signed char		s8;			// 8bit
typedef signed short	s16;
typedef signed long		s32;
// unsigned 
typedef unsigned char	u8;		
typedef unsigned short	u16;
typedef unsigned long	u32;		//32ビットだと4GB

typedef float	f32;
typedef double	f64;


#define COLOR DWORD
#define null NULL

typedef D3DXVECTOR4		Vector4;
typedef D3DXMATRIX		Matrix;
typedef IDirect3DTexture9 Texture2D;		// IDirect3DTexture9の略
typedef IDirect3DSurface9 Surface;			// IDirect3DSurface9の略
typedef IDirect3DDevice9 DEVICE, *LPDEVICE;	// IDirect3DDevice9の略

// ハンガリアン記法
//
//	p または lp ->	ポインタ型
//	m_			->	メンバ変数
//	g_			->	グローバル変数
//	dw			->	double word型　
//	tag			->	構造体
//	by			->	バイナリ
//

// Doxygenの記法
//
//   *@brief		説明文
//	 *@author		作者名
//	 *@return 		返り値
//	 *@param[in]	引数
//	 *@param[out]	値を変える引数
//	 *@note			メモ
//	 !<@brief		変数の後ろ(右)に説明文を付ける時には
//


/********************************************/
//	構造体関連
/********************************************/

/**
*@brief		float二つの構造体
*@author	Nishida
*/
typedef struct float2
{
	float x, y;
	float2() : x(0), y(0){}
}float2;


/**
*@brief		float三つの構造体（基本ベクトル）
*@author	Nishida
*/
typedef struct float3
{
	float x, y, z;//!<@brief	変数.
	float3() :x(0), y(0), z(0){}
} float3, Vector;


/**
*@brief		2二次元ベクトル
*@author	Nishida
*/
typedef struct Vector2 :public float2
{
	/**
	*@brief		コンストラクタ
	*@note		値を0で初期化する
	*/
	Vector2() :float2(){};

	/**
	*@brief			コンストラクタ
	*@param[in]	x	生成するベクトルのx成分
	*@param[in]	y	生成するベクトルのy成分
	*/
	inline Vector2(float x, float y){ this->x = x, this->y = y; }

	/**
	*@brief			コンストラクタ
	*@param[in]	v	生成するベクトルのxy成分
	*/
	inline Vector2(CONST float2& v){ this->x = v.x, this->y = v.y; }

	/**
	*@brief		ベクトルの長さを取得する
	*@return	ベクトルの長さ
	*/
	inline float Length(){ return sqrtf(x*x + y*y); }

	/**
	*@brief		ベクトルの2乗の長さを取得する
	*@return	ベクトルの2乗の長さ
	*/
	inline float LengthSq(){ return (x*x + y*y); }

	/**
	*@brief 正規化
	*@note ベクトルの正規化した値に変更
	*/
	void Normalize()
	{
		float length = Length();
		if (length != 0.0f){ x /= length, y /= length; }
	}

	// オペレーター
	inline Vector2& operator = (CONST Vector2& v){ x = v.x; y = v.y; return *this; }
	inline Vector2& operator = (CONST float2& v){ x = v.x; y = v.y; return *this; }
	inline Vector2& operator += (CONST Vector2& v){ x += v.x; y += v.y; return *this; }
	inline Vector2& operator -= (CONST Vector2& v){ x -= v.x; y -= v.y; return *this; }
	inline Vector2& operator *= (f32 v){ x *= v; y *= v; return *this; }
	inline Vector2& operator /= (f32 v){ x /= v; y /= v; return *this; }

	inline Vector2 operator + () const { Vector2 ret(x, y); return ret; }
	inline Vector2 operator - () const { Vector2 ret(-x, -y); return ret; }

	inline Vector2 operator + (CONST Vector2& v) const { return Vector2(x + v.x, y + v.y); }
	inline Vector2 operator - (CONST Vector2& v) const { return Vector2(x - v.x, y - v.y); }
	inline Vector2 operator * (f32 v) const { Vector2 ret(x * v, y * v); return ret; }
	inline Vector2 operator / (f32 v) const { Vector2 ret(x / v, y / v); return ret; }

	BOOL operator == (CONST Vector2& v) const { return (x == v.x) && (y == v.y); }
	BOOL operator != (CONST Vector2& v) const { return (x != v.x) || (y != v.y); }


} Vector2, *LPVECTOR2;


/**
*@brief		3次元ベクトル構造体
*@author		Nishida
*/
typedef struct Vector3 : public Vector
{
public:

	/**
	*@brief	コンストラクタ
	*@note	値は0で初期化される
	*/
	Vector3() :Vector(){};

	/**
	*@brief				コンストラクタ
	*@param[in]	x	生成するベクトルのx成分
	*@param[in]	y	生成するベクトルのy成分
	*@param[in]	z	生成するベクトルのz成分
	*/
	inline Vector3(float x, float y, float z){ this->x = x, this->y = y, this->z = z; }
	
	/**
	*@brief				コンストラクタ
	*@param[in]	v	生成するベクトルの各種成分
	*/
	inline Vector3(CONST Vector& v){ this->x = v.x, this->y = v.y, this->z = v.z; }

	/**
	*@brief		ベクトルの長さを取得する
	*@return	ベクトルの長さ
	*/
	inline float Length(){ return sqrtf(x*x + y*y + z*z); }

	/**
	*@brief	ベクトルの長さの2乗を取得する
	*@return	ベクトルの長さの2乗
	*/
	inline float LengthSq(){ return x*x + y*y + z*z; }

	/**
	*@brief ベクトルの正規化
	*/
	void Normalize()
	{
		float l = Length();
		if (l != .0f){ x /= l; y /= l; z /= l; }
	}

	
	//	オペレーター

	// this+=vector
	inline Vector3& operator = (CONST Vector& v){ x = v.x; y = v.y; z = v.z; return *this; }
	// this+=Vector3
	inline Vector3& operator += (CONST Vector3& v){ x += v.x; y += v.y; z += v.z; return *this; }
	// this-=Vector3
	inline Vector3& operator -= (CONST Vector3& v){ x -= v.x; y -= v.y; z -= v.z; return *this; }
	// this*=Vector3
	//inline Vector3& operator *= (CONST Vector3& v){ x *= v.x; y *= v.y; z *= v.z; return *this; }
	// this*=flaot
	inline Vector3& operator *= (FLOAT v){ x *= v; y *= v; z *= v; return *this; }
	// this/=flaot
	inline Vector3& operator /= (FLOAT v){ x /= v; y /= v; z /= v; return *this; }

	inline Vector3 operator + () const { Vector3 ret(x, y, z); return ret; }
	inline Vector3 operator - () const { Vector3 ret(-x, -y, -z); return ret; }

	// Vector3 = Vector3 + Vector3
	inline Vector3 operator + (CONST Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
	// Vector3 = Vector3 - Vector3
	inline Vector3 operator - (CONST Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
	// Vector3 = Vector3 * float
	inline Vector3 operator * (FLOAT v) const { Vector3 ret(x*v, y*v, z*v); return ret; }
	// Vector3 = Vector3 / float
	inline Vector3 operator / (FLOAT v) const { Vector3 ret(x / v, y / v, z / v); return ret; }

	//追加分
	//inline Vector operator *(const float& f){ Vector a = { x * f, y * f, z * f }; return a; }

	// if( Vector3 == Vector3 )
	BOOL operator == (CONST Vector3& v) const { return (x == v.x) && (y == v.y) && (z == v.z); }
	// if( Vector3 != Vector3 )
	BOOL operator != (CONST Vector3& v) const { return (x != v.x) || (y != v.y) || (z != v.z); }


	inline void Set(float x, float y, float z){ this->x = x, this->y = y, this->z = z; }

} Vector3, *LPVECTOR3;

/**
*@brief		ベクトルとベクトルの内積
*@return	ベクトルの2乗の長さ
*/
inline float Vector3Dot(Vector& v1, Vector& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

/**
*@brief		ベクトルとベクトルの外積
*@param[out]	out	外積の結果
*@param[in]		v1	1つ目のベクトル
*@param[in]		v2	2つ目のベクトル
*/
inline void Vector3Cross(Vector& out, Vector& v1, Vector& v2)
{
	out.x = v1.y*v2.z - v1.z*v2.y;
	out.y = v1.z*v2.x - v1.x*v2.z;
	out.z = v1.x*v2.y - v1.y*v2.x;
}

/**
*@brief		ベクトルとベクトルの外積
*@param[in]		v1	1つ目のベクトル
*@param[in]		v2	2つ目のベクトル
*@return	外積の結果
*/
inline Vector Vector3Cross(Vector& v1, Vector& v2)
{
	Vector3 out;
	out.x = v1.y*v2.z - v1.z*v2.y;
	out.y = v1.z*v2.x - v1.x*v2.z;
	out.z = v1.x*v2.y - v1.y*v2.x;
	return out;
}

// クォータニオン
typedef struct  Quaternion : public D3DXQUATERNION
{
public:
	//------------------------------------------------------
	//	コンストラクタ
	//------------------------------------------------------
	Quaternion(){}
	Quaternion(const D3DXQUATERNION& in) :D3DXQUATERNION(in)
	{}
	Quaternion( float sx, float sy, float sz, float sw ) : D3DXQUATERNION( sx, sy, sz, sw ){}

	//------------------------------------------------------
	//	生成
	//------------------------------------------------------
	//	単位クォータニオン生成
	void Identity(){ x = y = z = 0; w = 1; }

	//	正規化
	inline void normalize()
	{
		float legnth = getLength();
		if (legnth == .0f) return;
		float invL = 1.0f / legnth;
		(*this) *= invL;
	}

	//------------------------------------------------------
	//	情報取得
	//------------------------------------------------------
	inline float getLength() const{ return sqrtf(x*x + y*y + z*z + w*w); }
	void toMatrix(Matrix& m);

	//------------------------------------------------------
	//	オペレーター
	//------------------------------------------------------
	inline Quaternion operator + () const { Quaternion ret(x, y, z, w); return ret; }
	inline Quaternion operator - () const { Quaternion ret(-x, -y, -z, -w); return ret; }

	//	VS クォータニオン
	inline Quaternion& operator +=(const Quaternion& v){ x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
	inline Quaternion& operator -=(const Quaternion& v){ x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
	inline Quaternion& operator *=(const Quaternion& v)
	{
		x = y * v.z - z * v.y + x * v.w + w * v.x;
		y = z * v.x - x * v.z + y * v.w + w * v.y;
		z = x * v.y - y * v.x + z * v.w + w * v.z;
		w = w * v.w - x * v.x - y * v.y - z * v.z;
		return *this;
	}

	inline Quaternion operator +(const Quaternion& v) const{ return Quaternion(x + v.x, y + v.y, z + v.z, w + v.w); }
	inline Quaternion operator -(const Quaternion& v) const{ return Quaternion(x - v.x, y - v.y, z - v.z, w - v.w); }
	inline Quaternion operator *(const Quaternion& v) const
	{
		return Quaternion(
			y * v.z - z * v.y + x * v.w + w * v.x,
			z * v.x - x * v.z + y * v.w + w * v.y,
			x * v.y - y * v.x + z * v.w + w * v.z,
			w * v.w - x * v.x - y * v.y - z * v.z
			);
	}

	//	VS 値
	inline Quaternion& operator *=(float v){ x *= v; y *= v; z *= v; w *= v; return *this; }
	inline Quaternion& operator /=(float v){ x /= v; y /= v; z /= v; w /= v; return *this; }

	inline Quaternion operator *(float v) const{ return Quaternion(x*v, y*v, z*v, w*v); }
	inline Quaternion operator /(float v) const{ return Quaternion(x / v, y / v, z / v, w / v); }

} Quaternion;

/**
*@brief					回転・移動変換行列の生成
*@param[in]	q			Quaternion
*@param[in]	r			Quaternion
*@param[in]	a			float
*@return	Quaternion	頑張った
*/
Quaternion QuaternionSlerp(Quaternion& q, Quaternion& r, float a);

/*************************************************/
//	行列やベクトルに関する計算のプロトタイプ宣言
/*************************************************/
//**************************************
//								Math
//**************************************

/**
*@brief		数学系処理をまとめた名前空間
*@author		nishida
*/
namespace Math
{
	// 変換行列

	/**
	*@brief					回転・移動変換行列の生成
	*@param[out]	Mat	変換行列の受け取り先
	*@param[in]	x		x成分の移動量
	*@param[in]	y		y成分の移動量
	*@param[in]	z		z成分の移動量
	*@param[in]	ax		x軸を基準とした回転値(ラジアン角)
	*@param[in]	ay		y軸を基準とした回転値(ラジアン角)
	*@param[in]	az		z軸を基準とした回転値(ラジアン角)
	*@note					回転行列を掛ける順番がXYZとは異なり,フライトシュミレーションなどはこちらが向いている
	*/
	void	SetTransformMatrixZXY(Matrix *Mat, float posX, float posY, float posZ, float angleX, float angleY, float angleZ);

	/**
	*@brief					回転・移動変換行列の生成
	*@param[out]	Mat		変換行列の受け取り先
	*@param[in]	posX		x成分の移動量
	*@param[in]	posY		y成分の移動量
	*@param[in]	posZ		z成分の移動量
	*@param[in]	angleX		x軸を基準とした回転値(ラジアン角)
	*@param[in]	angleY		y軸を基準とした回転値(ラジアン角)
	*@param[in]	angleZ		z軸を基準とした回転値(ラジアン角)
	*/
	void	SetTransformMatrixXYZ(Matrix *Mat, float posX, float posY, float posZ, float angleX, float angleY, float angleZ);


	/**
	*@brief						左手座標系ビュー変換行列の生成
	*@param[out]		mat	変換行列の受け取り先
	*@param[in]		Eye	視点の位置
	*@param[in]		At		ターゲットの位置
	*@param[in]		Up	カメラの上向きベクトル
	*/
	void	LookAtLH(Matrix& mat, Vector& Eye, Vector& At, Vector& Up);
	
	// 投影変換表列
	/**
	*@brief							 投影変換表列の生成
	*@param[out]		mat		変換行列の受け取り先
	*@param[in]		fovY		視野角(Field of View)
	*@param[in]		aspect	アスペクト比
	*@param[in]		zn			最も近いz値
	*@param[in]		zf			最も遠いz値
	*/
	void	PerspectiveLH(Matrix& mat, float fovY, float aspect, float zn, float zf);

	/**
	*@brief						平行投影変換行列の生成
	*@param[out]		mat	変換行列の受け取り先
	*@param[in]		w		横幅
	*@param[in]		h		縦幅
	*@param[in]		zn		最も近いz値
	*@param[in]		zf		最も遠いz値
	*/
	void	OlthoLH(Matrix& mat, float w, float h, float zn, float zf);
		
	/**
	*@brief					距離計算
	*@param[in]		xA		点Aのx軸
	*@param[in]		yA		点Aのy軸
	*@param[in]		xB		点Bのx軸
	*@param[in]		yB		点Bのy軸
	*@return	AとBの距離を返す
	*/
	inline float Length(float xA, float yA, float xB, float yB);
	
	/**
	*@brief					距離計算
	*@param[in]		PosA	点Aの位置
	*@param[in]		PosB	点Bの位置
	*@return		AとBの距離を返す
	*/
	inline float Length(Vector2 PosA, Vector2 PosB);
	
	/**
	*@brief					距離計算
	*@param[in]		PosA	点Aの位置
	*@param[in]		PosB	点Bの位置
	*@return		AとBの距離を返す
	*/
	inline float Length(Vector3 PosA, Vector3 PosB);

	/**
	*@brief						座標変換
	*@param[in]		WorldPos	ワールド座標
	*@return		ワールド座標から変換されたスクリーン座標を返す
	*/
	Vector2 WorldToScreen(const Vector3 &WorldPos);

	/**
	*@brief								座標変換
	*@param[in]		ScreenPos			スクリーン座標
	*@param[in]		ProjectiveSpaceZ	カメラのプロジェクションのNearとFarのパーセンテージ(0.0～1.0)
	*@return		スクリーン座標から変換されたワールド座標を返す
	*/
	Vector3 ScreenToWorld(const Vector2 &ScreenPos, float ProjectiveSpaceZ);

	/**
	*@brief								座標変換
	*@param[in]		ScreenPos			スクリーン座標
	*@param[in]		PlateNormal			平面の法線(基本的に上方向)	
	*@param[in]		Shift				原点からのPlateNormal方向の移動量
	*@return		スクリーン座標からのNearとFarのベクトルによるRayを飛ばし、平面による交点の座標を返す
	*/
	Vector3 ScreenToWorldPlate(const Vector2 &ScreenPos, Vector3 &PlateNormal = Vector3(0, 1, 0), float Shift = .0f);

	/**
	*@brief									ベジエ計算
	*@param[in]		out						計算結果吐き出し
	*@param[in]		pos_array				座標の配列(複数の点)
	*@param[in]		num_elements_array		配列の個数
	*@param[in]		percentage				どれぐらいの割合の位置か(0.0～1.0)
	*@return		ベジエ計算を行い、outに吐き出す関数
	*/
	void Bezier(Vector3 *out, Vector3 pos_array[], int num_elements_array, float percentage);

	/**
	*@brief									最小値と最大値を範囲内に値を収める
	*@param[in]		val						範囲内に収める値
	*@param[in]		Min						クランプ最少値
	*@param[in]		Max						クランプ最大値
	*/
	float Clamp(float val, float Min = 0, float Max = 1);

	/**
	*@brief									0.0f~1.0fで補間されたレートを元に数値をブレンド
	*@param[in]		Rate					0.0f~1.0fで補間されたレート
	*@param[in]		MinRate					最少値の時に強く出る数値
	*@param[in]		MaxRate					最大値の時に強く出る数値
	*@return								0.0f~1.0fで補間されたレートを元にブレンドした数値を返す
	*/
	float Blend(float Rate, float MinNum, float MaxNum);
}


//***********************************
//						Collision
//***********************************
/**
*@brief		当たり判定をまとめた名前空間
*@author		nishida
*/
namespace Collision
{

	bool SphereAndSphere(Vector3 PosA, float RadiusA, Vector3 PosB, float RadiusB);



}



/********************************************/
//	前方宣言
/********************************************/
//class tdnRenderState;
//typedef tdnRenderState RS;// (?) 書き方わからん
class tdnShader;
class tdn2DObj;

/********************************************/
//	tdnSystem
/********************************************/
// スクリーンモード
#define	SCREEN800x450	0
#define	SCREEN1280x720	1
#define	SCREEN1920x1080	2
#define	SCREEN800x600	3

// エンディアン
enum ENDIAN
{
	BIG_ENDIAN,			// ビッグエンディアン
	LITTLE_ENDIAN		// リトルエンディアン
};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);			 // ウィンドウプロシージャ
//BOOL InitApp(HINSTANCE hInst, int nCmdShow);												 // ゲーム起動時の最初の初期化
//BOOL EndApp();																			 // ゲームの一番最後の後処理

extern	Matrix	matView;		//	カメラ行列
extern	Matrix	matProjection;	//	投影変換行列

/**
*@brief		DirectXやディスプレイの基本制御を行うクラス
*@author		Nishida
*/
class tdnSystem
{
public:
	// 初期化・解放
	static BOOL InitWindow(HINSTANCE hinst, int nCmdShow, char* AppTitle, DWORD ScreenMode, BOOL isFullScreen);

	static void Release();
private:
	// お約束
	static HINSTANCE hinst;				// インスタンスハンドル	
	static HWND	Window;					// ウィンドウハンドル
	static BOOL WindowActive;			// ウィンドウの状態
private:
	static BOOL InitD3D();				// D3D初期化。Privateへ移行
	static LPDIRECT3D9	lpD3D;			// DirectX9にアクセスするためのポインタ			
	static LPDEVICE	Device;				// ビデオカードにアクセスするための3Dデバイス		
	static D3DPRESENT_PARAMETERS d3dpp;	// ディスプレイパラメーター
	static ID3DXLine* pLine;			// 線描画
	// スクリーンのサイズ
	static RECT	 ScreenSize;			// スクリーンのサイズ
	static D3DFORMAT ScreenFormat;		// スクリーンのフォーマット
	static BOOL FullScreen;				// フルスクリーンのフラグ

	static RECT DisplaySize;			// ディスプレイのサイズ

	// エンディアン(システム初期化時にどちらかを判定します)
	static ENDIAN endian;


public:
	// (?)なぜ値が変わったらまずいデータがグローバルに？	

	// パラメーター取得関数
	static inline HINSTANCE GetHinst(){ return hinst; }					// インスタンスハンドルのポインタ
	static inline HWND GetWindow(){ return Window; }					// ウィンドウハンドルのポインタ
	static inline LPDIRECT3D9 GetD3D9(){ return lpD3D; };				// DirectX9にアクセスするポインタ
	static inline LPDEVICE GetDevice(){ return Device; };				// ビデオカードにアクセスするデバイスのポインタ
	static inline ID3DXLine* GetLine(){ return pLine; };				// 線描画用のポインタ
	static inline RECT GetScreenSize(){ return ScreenSize; }			// 画面のサイズ
	static inline D3DFORMAT GetScreenFormat(){ return ScreenFormat; }	// 画面のフォーマット
	static inline BOOL GetWindowActive(){ return WindowActive; };		// ウィンドウが今動いているか
	static inline BOOL GetFullScreen(){ return FullScreen; };			// フルスクリーンか
	static inline RECT GetDisplaySize() { return DisplaySize; };		// ディスプレイのサイズ

	// ウィンドウメッセージ処理関数
	//static BOOL Message();

	// スクリーンのサイズを取得
	static RECT GetScreenRect(DWORD mode);
	//	シーン開始・終了
	static void BeginScene(){ Device->BeginScene(); }		// 描画開始!
	static void EndScene()									// 描画終了!
	{
		Device->EndScene();
		if (FAILED(Device->Present(NULL, NULL, NULL, NULL))) Device->Reset(&d3dpp);
	}
	// ウィンドウが今動いているかをセットする関数
	static void SetWindowActive(BOOL active){ WindowActive = active; }

	//	デバッグウィンドウ
	static void OpenDebugWindow();
	static void CloseDebugWindow();
	static void printf(const char* format, ...);

	/**
	*@brief					リトルエンディアンかビッグエンディアンかのゲッター
	*@return				エンディアンの定数を返す
	*/
	static ENDIAN GetEndian(){ return endian; }

	/**
	*@brief					エンディアンの変換を行う
	*@param[in]		*input	変換したい値(型は何でもいい)参照で書き換える
	*@param[in]		size	第一引数inputのsizeof
	*/
	static void ConvertEndian(void *input, size_t size){
		char *temp = new char[size];
		for (size_t i = 0; i < size; i++) temp[i] = ((char *)input)[i];
		for (size_t i = 1; i <= size; i++) ((char *)input)[i - 1] = temp[size - i];
		delete temp;
	};

	static void ScreenShot()
	{
		Surface* pBackBuffer;

		// BackBufferに保存
		tdnSystem::GetDevice()->GetRenderTarget(0, &pBackBuffer);


		time_t now = time(NULL);
		struct tm *pnow = localtime(&now);

		char buff[128] = "";
		sprintf(buff, "ScreenShot/%d年%d月%d日%d時%d分%d秒.png", pnow->tm_year + 1900, pnow->tm_mon + 1, pnow->tm_mday, pnow->tm_hour, pnow->tm_min, pnow->tm_sec);


		D3DXSaveSurfaceToFile(buff, D3DXIFF_PNG, pBackBuffer, NULL, NULL);

		//Surfaceの解放	
		SAFE_RELEASE(pBackBuffer);
	}
};


/********************************************/
//				tdnView				     
/********************************************/

/**
*@brief		カメラの空間を制御するクラス
*@author		Nishida
*/
class tdnView
{
public:
	// 初期化
	static void Init()
	{
		SetViewport();
		SetProjection(D3DX_PI / 3.0f, 0.1f, 3000.0f);
	}

	//	視界クリア
	static void Clear(DWORD color = 0, bool bClearZ = true);
	static void ClearZ();

	//	視点設定
	static void Set(const Vector& pos, const Vector& target);
	static void Set(float x, float y, float z, float ax, float ay, float az);

	//	投影平面設定
	static void SetViewport();
	static void SetViewport(int x, int y, int w, int h);

	//	投影設定
	static void SetProjection(float FovY, float Near, float Far);
	static void SetProjection(float FovY, float Near, float Far, float asp);

	// 行列・ビューポート更新
	static void Activate();

protected:
	static Vector  Pos, Target;			// 始点と目線	
	// ビューポート
	static D3DVIEWPORT9	Viewport;		// Window画面のどの領域まで3Dを描画するか
	static float FovY;					// 視野角
	static float Near, Far;				// 投影の手前の位置と一番奥の位置 
	static float Aspect;				// 


};


/********************************************/
//				tdnRenderState				     
/********************************************/
typedef enum RS
{
	COPY = 1,
	COPY_NOZ = 2,
	ADD = 3,
	SUB = 4,
	MUL = 5,
	NEGA = 6,
	INVERT = 7
}RM;

/**
*@brief		レンダーステートを指定するクラス
*@author		Nishida
*/
class tdnRenderState
{
public:
	//static const unsigned long COPY = 0;
	//static const unsigned long COPY_NOZ = 32;
	//static const unsigned long ADD = 1;
	//static const unsigned long SUB = 2;
	//static const unsigned long MUL = 4;
	//static const unsigned long NEGA = 8;
	//static const unsigned long INVERT = 16;

	static void Initialize();
	static void Set(DWORD state, D3DMATERIAL9* lpMaterial, Texture2D* lpTexture);
	static void	Filter(BOOL bFilter);												//	サンプラーステート　TRUE：LINER(補間)　FLASE:POINT(処理は早い)　
};

/********************************************/
//				tdnArchive				     
/********************************************/

/**
*@brief		各種ファイルをアーカイブするクラス
*@author		Nishida
*/
class tdnArchiver
{
public:

	// 二つアーカイブファイルを作成する
	bool CreateArchiveFile(const char* pHeaderFilename, const char* pDataFilename);

	// データファイルをアーカイブファイルに追加する
	bool AddData(const char* pDataFileName);

	// アーカイブ終了処理(ダミーヘッダーの追加)
	bool CloseArchiveFile(const char* pArchiveFilename, const char* pHeaderFilename, const char* pDataFilename);

private:
	// 二つに分けてみた。みるしかなかった.. (TODO)誰か分けずに作れるようにしてください
	std::ofstream headerOfs;	// ヘッダー部
	std::ofstream dataOfs;		// データ部
	std::ofstream archiverOfs;	// 最終出力


	// 現在追加したファイルの数(ヘッダー部のID値として使用)
	char nameID[64];

	// 頭の最大
	uint32_t HEAD_MAX;

	// アーカイブファイルの最大サイズ
	enum : uint32_t{ eMaxArchiveFileSize = 4000000000 };
	// 現在書き込んでいるアーカイブファイルの総サイズ
	uint32_t wroteArchiveFileSize;
};


// ヘッダー部構造
class ArchiveHeaderBlock
{
public:
	// データネームID
	char nameID[64];
	// 開始場所データのサイズ
	uint32_t wroteDataFileSize;
	// データブロックのサイズ
	uint32_t dataFileSize;

};

/**
*@brief		アーカイブされたデータを解凍するクラス
*@author		Nishida
*/
class tdnUnArchiver
{
public:	
	// アーカイブを開く
	static char* OpenArchiveFile(const char* pArchiveFilename, const char* pName);
	// メモリのサイズ取得
	static int GetSize(const char* pArchiveFilename, const char* pName);
	// アーカイブを開く＋メモリサイズを取得
	static char* OpenArchiveFile(const char* pArchiveFilename, const char* pName, int& outMemorySize);
};

// シングルトンにした(理由)解除は一つで十分。
//class tdnUnArchiver
//{
//private:
//	static tdnUnArchiver* pInstance;
//	// 封印
//	tdnUnArchiver(){};
//	~tdnUnArchiver(){};
//public:
//	static tdnUnArchiver& GetInstance();
//	static void Release();
//
//public:	// 機能
//	// アーカイブを開く
//	char* OpenArchiveFile(const char* pArchiveFilename, const char* pName);
//	// メモリのサイズ取得
//	int GetSize(const char* pArchiveFilename, const char* pName);
//	// アーカイブを開く＋メモリサイズを取得
//	char* OpenArchiveFile(const char* pArchiveFilename, const char* pName, int& outMemorySize);
//
//};
//#define UNArchive	(tdnUnArchiver::GetInstance())


/********************************************/
//				tdnTexture				     
/********************************************/
/**
*@brief		テクスチャーを管理するクラス
*@author		Nishida
*/
class tdnTexture
{
private:
	static const int MAX_TEXTURE = 512;

	struct TEXINFO {
		char		filename[64];	//	ファイルパス
		int			UseCount;		//	このテクスチャ使用しているカウント
		Texture2D*	lpTexture;		//	テクスチャ	
		D3DXIMAGE_INFO info;		//  (追加)サイズの情報取得
	};
	static struct TEXINFO TexInfo[MAX_TEXTURE];

public:
	//	初期化
	static void Initialize();
	//	読み込み・解放
	static Texture2D* Load(const char* filename, int flag = 0);
	static Texture2D* LoadMemory(const char* filename, const char* pArchiver);

	static void	Release(Texture2D* lpTexture);

};
/********************************************/
//				tdnLight				     
/********************************************/
class tdnLight
{
public:
private:

};


/********************************************/
//				tdnShader				     
/********************************************/
/**
*@brief		シェーダーを管理するクラス
*@author		Nishida
*/
class tdnShader
{

public:
	// 初期化・解放
	tdnShader(char* filename);
	~tdnShader();

	// シェーダー利用開始・終了
	inline UINT Begin(char* name)		// シェーダ開始
	{
		UINT pass;
		// シェーダーの適用
		pShader->SetTechnique(name);	// テクニック名を設定する
		pShader->Begin(&pass, 0);	
		return pass;
	}
	inline void End(){ pShader->End(); }// シェーダ終了

	/*	パスの開始・終了  */	
	inline void BeginPass(int pass){ pShader->BeginPass(pass); }	// アクティブなテクニック内でパスを開始します。
	inline void EndPass(){ pShader->EndPass(); }					// パス終り
	inline void CommitChanges(){ pShader->CommitChanges(); }		// BeginPass～EndPass内でセッターを通して値を変更した際に呼び出す必要がある関数です

	// パラメーター設定
	void SetWVPMatrix(Matrix* wvpMat);
	void SetWVPMatrix(Matrix& wvpMat);

	void SetDecaleTexture(Texture2D* decaleTex);
	void SetDecaleTexture(Texture2D& decaleTex);

	void SetValue(char* name, Texture2D* tex);
	void SetValue(char* name, Texture2D& tex);
	void SetValue(char* name, Matrix* mat);
	void SetValue(char* name, Matrix& mat);
	void SetValue(char* name, D3DXVECTOR4* v);
	void SetValue(char* name, D3DXVECTOR4& v);
	void SetValue(char* name, Vector3* v);
	void SetValue(char* name, Vector3& v);
	void SetValue(char* name, tdn2DObj* tex);
	void SetValue(char* name, tdn2DObj& tex);
	void SetValue(char* name, float f);
	void SetValue(char* name, int d);
	void SetValue(char* name, DWORD d);

private:
	LPD3DXEFFECT pShader;	// エフェクトファイルへのアクセスポインタ
	D3DXHANDLE	hmWVP;		// 射影変換行列　パラメーターを参照するための効率的な手段を提供します。
	D3DXHANDLE  htexDecale;	// テクスチャ

};

/********************************************/
//				tdn2DObj				     
/********************************************/
/**
*@brief		レンダーターゲットのフォーマット
*@author		Nishida
*/
typedef enum FMT2D
{
	RENDERTARGET = 1,
	FLOAT1 = 2,
	FLOAT2 = 3,
	USEALPHA = 4,
	SYSTEMMEM = 5,
	HDR = 6,
	SYSTEMMEM_HDR = 7
}tdn2D;

/**
*@brief		2Dオブジェクトを制御するクラス
*@author		Nishida
*/
class tdn2DObj
{
public:
	tdn2DObj(const char* fileName);							// ファイルから画像読み込み
	tdn2DObj(const char* IDName, const char* pArchiveName);	// メモリーから画像読み込み
	tdn2DObj(UINT width, UINT height, FMT2D fmtFlag);		// レンダーターゲット作成
	tdn2DObj(Texture2D *texture);							// テクスチャ参照型(主に動画で使う)
	tdn2DObj();
	~tdn2DObj();											// 画像orレンダーターゲット解放

	bool LoadFontTexture(LPCSTR character, UINT size, LPCSTR fontName = "メイリオ"); // 文字テクスチャ作成

	// 情報取得
	void RenderTarget(int index = 0);						// レンダーターゲット設定
	inline Surface*	GetSurface(){ return lpSurface; };		// 板
	inline Texture2D*	GetTexture(){ return lpTexture; };	// テクスチャ

	// 描画
	void Render(int x, int y, u32 dwFlags = RS::COPY);
	void Render(int x, int y, tdnShader* shader, char* name);// shader適用
	void Render(int x, int y, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY);
	void Render(int x, int y, int w, int h, int tx, int ty, int tw, int th, tdnShader* shader, char* name);

	// 3D描画
	void Render3D(float x, float y, float z, u32 dwFlags = RS::COPY);
	void Render3D(Vector3 pos, u32 dwFlags = RS::COPY);
	void Render3D(float x, float y, float z, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY);
	void Render3D(Vector3 pos, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY);

	//	情報更新
	void SetScale(float scale);
	void SetAngle(float angle);
	void SetARGB(BYTE A, BYTE R, BYTE G, BYTE B);
	void SetARGB(int A, int R, int G, int B);
	void SetARGB(DWORD ARGB);
	void SetAlpha(BYTE A);
	void SetAlpha(float A);
	void SetRGB(BYTE R, BYTE G, BYTE B);
	void SetRGB(int R, int G, int B);
	void SetTurnOver(bool turnFlag);
	void SetShiftCenter(bool ShiftFlag);
	void SetCenter(float x, float y);

	// 情報取得
	inline float GetScale(){ return scale; }
	inline float GetAngle(){ return angle; }
	inline float GetCenterX(){ return centerX; }
	inline float GetCenterY(){ return centerY; }
	inline bool GetTurnOver(){ return isTurnOver; };
	inline bool GetShiftCenter(){ return isShiftCenter; };
	inline UINT GetWidth(){ return m_width; }
	inline UINT GetHeight(){ return m_height; }
private:

	// unsigned intの理由はGetLevelDescから受け取る情報などがUINTだから
	UINT		m_width;				// 幅
	UINT		m_height;				// 高さ

	// テクスチャと画像情報
	Surface*	lpSurface;			// 板
	Texture2D*	lpTexture;			// テクスチャ

	// ステータス
	float scale, angle;
	DWORD color;
	float centerX, centerY;
	bool isTurnOver;
	bool isShiftCenter;				//xyを画像の中心に
	bool m_bLoadTexture;		// ファイルからテクスチャをロードしたかどうか	
};

/********************************************/
//				tdn2DAnim				     
/********************************************/
/**
*@brief		2Dイラストを用いた特殊なエフェクト効果をまとめた名前空間
*@author		Nishida
*/
namespace AnimAction
{
	/**
	*@brief		2Dエフェクトの基礎（継承元）
	*@author		Nishida
	*/
	class Base
	{
	public:
		Base() :m_bActionFlag(false), m_bEndFlag(false), m_iDelayFlame(0){};
		~Base() {};

		virtual void Update(tdn2DObj* pic) {};// tdn2DObjの実態を中へ	
		// 基本的にアニメの始動		

		virtual void Action(tdn2DObj* pic, int delay) {
			m_iDelayFlame = delay;
			m_bActionFlag = true; /* 実行フラグOn */
			m_bEndFlag = false;	// エンドフラグ
		};
		virtual void Stop(tdn2DObj* pic) { m_bActionFlag = false;/* 実行フラグOff */ };

		/******************************************/
		// アニメ用描画
		/******************************************/
		virtual void Render(tdn2DObj* pic, int x, int y, u32 dwFlags = RS::COPY);// { pic->Render(x, y, dwFlags); }
		virtual void Render(tdn2DObj* pic, int x, int y, tdnShader* shader, char* name);// shader適用
		virtual void Render(tdn2DObj* pic, int x, int y, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY);
		virtual void Render(tdn2DObj* pic, int x, int y, int w, int h, int tx, int ty, int tw, int th, tdnShader* shader, char* name);

		// 3D描画
		virtual void Render3D(tdn2DObj* pic, float x, float y, float z, u32 dwFlags = RS::COPY);
		virtual void Render3D(tdn2DObj* pic, Vector3 pos, u32 dwFlags = RS::COPY);
		virtual void Render3D(tdn2DObj* pic, float x, float y, float z, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY);
		virtual void Render3D(tdn2DObj* pic, Vector3 pos, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY);

		// アクションチェック
		bool ActionCheck();

		// Get
		bool IsAction() { return m_bActionFlag; }
		bool IsEnd() { return m_bEndFlag; }
		int GetDelayFrame(){ return m_iDelayFlame; }
	protected:
		// アクションフラグ	 
		bool m_bActionFlag;
		// 終了フラグ	 
		bool m_bEndFlag;
		// ディレイ
		int m_iDelayFlame;
	};

	/**
	*@brief		波紋エフェクト
	*@author		Nishida
	*/
	class Ripple :public Base
	{
	public:
		// 引数で設定
		Ripple(int endFlame, float startScale, float moveScale);
		~Ripple();

		void Update(tdn2DObj* pic);// tdn2DObjの実態を中へ
		void Action(tdn2DObj* pic, int delay);// 基本的にアニメの始動

	private:
		int m_nowFlame;
		int m_endFlame;

		float m_startscale;
		float m_movescale;

	};

	/**
	*@brief		移動しながら登場
	*@author		Nishida
	*/
	class MoveAppeared :public Base
	{
	public:
		// 引数で設定
		MoveAppeared(int endflame, int startX, int startY);
		~MoveAppeared();

		void Update(tdn2DObj* pic);// tdn2DObjの実態を中へ
		void Action(tdn2DObj* pic, int delay);// 基本的にアニメの始動

		// 描画
		void Render(tdn2DObj* pic, int x, int y, u32 dwFlags = RS::COPY);// { pic->Render(x, y, dwFlags); }
		void Render(tdn2DObj* pic, int x, int y, tdnShader* shader, char* name);// shader適用
		void Render(tdn2DObj* pic, int x, int y, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY);
		void Render(tdn2DObj* pic, int x, int y, int w, int h, int tx, int ty, int tw, int th, tdnShader* shader, char* name);

	private:
		int m_nowFlame;
		int m_endFlame;

		int m_startX;
		int m_startY;

		float m_rate;
	};

	/**
	*@brief		絵を揺らす
	*@author		Nishida
	*/
	class Shake :public Base
	{
	public:
		// 引数で設定
		Shake(int endFlame, float shakeX, float shakeY, int cycle);
		~Shake();

		void Update(tdn2DObj* pic);// tdn2DObjの実態を中へ
		void Action(tdn2DObj* pic, int delay);// 基本的にアニメの始動

		// 描画
		void Render(tdn2DObj* pic, int x, int y, u32 dwFlags = RS::COPY);// { pic->Render(x, y, dwFlags); }
		void Render(tdn2DObj* pic, int x, int y, tdnShader* shader, char* name);// shader適用
		void Render(tdn2DObj* pic, int x, int y, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY);
		void Render(tdn2DObj* pic, int x, int y, int w, int h, int tx, int ty, int tw, int th, tdnShader* shader, char* name);

	private:
		int m_nowFlame;
		int m_endFlame;

		float m_powerX;
		float m_powerY;

		float m_shakeX;
		float m_shakeY;

		float m_orgX;
		float m_orgY;

		int m_cycle;

		bool m_bTrunOver;
	};

	/**
	*@brief		飛び出る
	*@author		Nishida
	*/
	class Jump :public Base
	{
	public:
		// 引数で設定
		Jump(int endFlame, float startScale, float addScale);
		~Jump();

		void Update(tdn2DObj* pic);// tdn2DObjの実態を中へ
		void Action(tdn2DObj* pic, int delay);// 基本的にアニメの始動

		// 描画
		void Render(tdn2DObj* pic, int x, int y, u32 dwFlags = RS::COPY);// { pic->Render(x, y, dwFlags); }
		void Render(tdn2DObj* pic, int x, int y, tdnShader* shader, char* name);// shader適用
		void Render(tdn2DObj* pic, int x, int y, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY);
		void Render(tdn2DObj* pic, int x, int y, int w, int h, int tx, int ty, int tw, int th, tdnShader* shader, char* name);

	private:
		int m_nowFlame;
		int m_endFlame;

		float m_startscale;
		float m_maxScale;

		float m_moveScale;

		float m_orgAlpha;
		float m_alpha;
	};

	/**
	*@brief		集まる用に出現
	*@author		Nishida
	*/
	class Shrink :public Base
	{
	public:
		// 引数で設定
		Shrink(int endFlame, float startScale, float maxScale);
		~Shrink();

		void Update(tdn2DObj* pic);// tdn2DObjの実態を中へ
		void Action(tdn2DObj* pic, int delay);// 基本的にアニメの始動

	private:
		int m_nowFlame;
		int m_endFlame;

		float m_startScale;
		float m_maxScale;

		float m_addScale;
		float m_nowScale;	// 現在のスケール

		float m_orgAlpha;
		float m_alpha;
	};

	/**
	*@brief		大きくなるエフェクト
	*@author		Nishida
	*/
	class Grow :public Base
	{
	public:
		// 引数で設定
		Grow(int endFlame, float startScale, float moveScale);
		~Grow();

		void Update(tdn2DObj* pic);// tdn2DObjの実態を中へ
		void Action(tdn2DObj* pic, int delay);// 基本的にアニメの始動

	private:
		int m_nowFlame;
		int m_endFlame;

		float m_startscale;
		float m_movescale;

	};

	/**
	*@brief		引き伸ばされるように元のサイズに戻るエフェクト
	*@author	Yamagoe
	*/
	class Stretch :public Base
	{
	public:
		// 引数で設定
		Stretch(int endFrame, float StartScaleX, float StartScaleY, float EndScaleX, float EndScaleY, UINT width, UINT height) :Base(), m_width(width), m_height(height), m_nowFrame(0), m_endFrame(endFrame), m_StartScale(StartScaleX, StartScaleY), m_EndScale(EndScaleX, EndScaleY), m_CurrentScale(m_StartScale){}
		~Stretch();

		void Update(tdn2DObj* pic);// tdn2DObjの実態を中へ
		void Action(tdn2DObj* pic, int delay);// 基本的にアニメの始動

		// 描画
		void Render(tdn2DObj* pic, int x, int y, u32 dwFlags = RS::COPY);// { pic->Render(x, y, dwFlags); }
		void Render(tdn2DObj* pic, int x, int y, tdnShader* shader, char* name);// shader適用
		void Render(tdn2DObj* pic, int x, int y, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY);
		void Render(tdn2DObj* pic, int x, int y, int w, int h, int tx, int ty, int tw, int th, tdnShader* shader, char* name);

	private:
		int m_nowFrame;
		int m_endFrame;
		Vector2 m_StartScale;
		Vector2 m_EndScale;
		Vector2 m_CurrentScale;

		const UINT m_width, m_height;	// 画像のサイズ保存用
	};


	/**
	*@brief		透明度アニメ(線形)
	*@author	Yamagoe
	*/
	class Alpha :public Base
	{
	public:
		// 引数で設定
		Alpha(int iEndFrame, float fStartAlpha, float fEndAlpha) :Base(), m_iCurrentFrame(0), m_iEndFrame(iEndFrame), m_fStartAlpha(fStartAlpha), m_fEndAlpha(fEndAlpha){}
		~Alpha();

		void Update(tdn2DObj* pic);// tdn2DObjの実態を中へ
		void Action(tdn2DObj* pic, int delay);// 基本的にアニメの始動
	private:
		int m_iCurrentFrame;
		int m_iEndFrame;
		float m_fStartAlpha;
		float m_fEndAlpha;
	};
}

/**
*@brief		2DObjをラップして様々な特殊機能をつけた
*@author		Nishida
*/
class tdn2DAnim /*:public tdn2DObj*/
{
private:
	// 画像
	tdn2DObj* m_obj;
	// 特殊なエフェクト効果の種
	AnimAction::Base *m_pAction;
public:
	tdn2DAnim(const char* fileName) :m_obj(new tdn2DObj(fileName)) { m_pAction = nullptr; }
	tdn2DAnim(Texture2D *texture) : m_obj(new tdn2DObj(texture)), m_pAction(nullptr){}
	tdn2DAnim(const char* IDName, const char* pArchiveName) :m_obj(new tdn2DObj(IDName, pArchiveName)){ m_pAction = nullptr; }

	~tdn2DAnim() { SAFE_DELETE(m_pAction); SAFE_DELETE(m_obj); }

	// 注文
	void OrderRipple(int endFlame, float startScale, float addScale);
	void OrderMoveAppeared(int endFlame, int startX, int startY);
	void OrderShake(int endFlame, float shakeX, float shakeY, int cycle);
	void OrderJump(int endFlame, float startScale, float addScale);
	void OrderShrink(int endFlame, float startScale, float maxScale);
	void OrderGrow(int endFlame, float startScale, float addScale);

	/**
	*@brief							引き伸ばしアニメ注文(最初は小さくて、元のサイズに引き伸ばされる感覚)
	*@author		Yamagoe
	*@param[in]		endFrame		終了フレーム(完全に元のサイズに戻るフレーム)
	*@param[in]		startScaleX		開始時のXスケール
	*@param[in]		startScaleY		開始時のYスケール
	*@param[in]		startScaleX		終了時のXスケール
	*@param[in]		startScaleY		終了時のYスケール
	*/
	void OrderStretch(int endFrame, float StartScaleX, float StartScaleY, float EndScaleX=1, float EndScaleY=1)
	{
		if (m_pAction) delete m_pAction;// 既存のバッファの開放
		m_pAction = new AnimAction::Stretch(endFrame, StartScaleX, StartScaleY, EndScaleX, EndScaleY, m_obj->GetWidth(), m_obj->GetHeight());
	}

	/**
	*@brief							透明度アニメ注文
	*@author		Yamagoe
	*@param[in]		iEndFrame		終了フレーム(完全に元のサイズに戻るフレーム)
	*@param[in]		fStartAlpha		開始時のアルファ値
	*@param[in]		fEndAlpha		終了時のアルファ値
	*/
	void OrderAlpha(int iEndFrame, float fStartAlpha, float fEndAlpha)
	{
		if (m_pAction) delete m_pAction;// 既存のバッファの開放
		m_pAction = new AnimAction::Alpha(iEndFrame, fStartAlpha, fEndAlpha);
		SetAlpha(fStartAlpha);
	}

	// 実行・始動
	void Update()
	{
		MyAssert((m_pAction != nullptr), "特殊エフェクトがセットされていない!\n[Order関数を呼びましょう]");
		m_pAction->Update(m_obj);
	}

	void Action(int delayTimer = 0)
	{
		MyAssert((m_pAction != nullptr), "特殊エフェクトがセットされていない!\n[Order関数を呼びましょう]");
		m_pAction->Action(m_obj, delayTimer);
	}

	void Stop()
	{
		m_pAction->Stop(m_obj);
	}

	// アクセサ
	tdn2DObj* GetObj() { return m_obj; }
	AnimAction::Base* GetAction() { return m_pAction; }

	//	情報更新
	void SetScale(float scale) { m_obj->SetScale(scale); };
	void SetAngle(float angle) { m_obj->SetAngle(angle); };
	void SetARGB(BYTE A, BYTE R, BYTE G, BYTE B) { m_obj->SetARGB(A, R, G, B); };
	void SetARGB(int A, int R, int G, int B) { m_obj->SetARGB(A, R, G, B); };
	void SetARGB(DWORD ARGB) { m_obj->SetARGB(ARGB); };
	void SetAlpha(float A){ m_obj->SetAlpha(A); }
	void SetRGB(BYTE R, BYTE G, BYTE B) { m_obj->SetRGB(R, G, B); };

	void SetTurnOver(bool turnFlag) { m_obj->SetTurnOver(turnFlag); };
	void SetShiftCenter(bool ShiftFlag) { m_obj->SetShiftCenter(ShiftFlag); };
	void SetCenter(float x, float y) { m_obj->SetCenter(x, y); };

	float GetScale() { return m_obj->GetScale(); }
	float GetAngle() { return m_obj->GetAngle(); }
	float GetCenterX() { return m_obj->GetCenterX(); }
	float GetCenterY() { return m_obj->GetCenterY(); }
	bool GetTurnOver() { return m_obj->GetTurnOver(); };
	bool GetShiftCenter() { return m_obj->GetShiftCenter(); };
	UINT GetWidth() { return m_obj->GetWidth(); }
	UINT GetHeight() { return m_obj->GetHeight(); }
	Texture2D *GetTexture(){ return m_obj->GetTexture(); }

	/******************************************/
	// 描画をアニメ用に加工
	/******************************************/
	// 描画
	void Render(int x, int y, u32 dwFlags = RS::COPY)
	{
		MyAssert((m_pAction != nullptr), "特殊エフェクトがセットされていない!\n[Order関数を呼びましょう]");
		m_pAction->Render(m_obj, x, y, dwFlags);
	}
	void Render(int x, int y, tdnShader* shader, char* name)
	{
		MyAssert((m_pAction != nullptr), "特殊エフェクトがセットされていない!\n[Order関数を呼びましょう]");
		m_pAction->Render(m_obj, x, y, shader, name);
	}
	void Render(int x, int y, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY)
	{
		MyAssert((m_pAction != nullptr), "特殊エフェクトがセットされていない!\n[Order関数を呼びましょう]");
		m_pAction->Render(m_obj, x, y, w, h, tx, ty, tw, th, dwFlags);
	}
	void Render(int x, int y, int w, int h, int tx, int ty, int tw, int th, tdnShader* shader, char* name)
	{
		MyAssert((m_pAction != nullptr), "特殊エフェクトがセットされていない!\n[Order関数を呼びましょう]");
		m_pAction->Render(m_obj, x, y, w, h, tx, ty, tw, th, shader, name);
	}

	// 3D描画
	void Render3D(float x, float y, float z, u32 dwFlags = RS::COPY)
	{
		MyAssert((m_pAction != nullptr), "特殊エフェクトがセットされていない!\n[Order関数を呼びましょう]");
		m_pAction->Render3D(m_obj, x, y, z, dwFlags);
	}
	void Render3D(Vector3 pos, u32 dwFlags = RS::COPY)
	{
		MyAssert((m_pAction != nullptr), "特殊エフェクトがセットされていない!\n[Order関数を呼びましょう]");
		m_pAction->Render3D(m_obj, pos, dwFlags);
	}
	void Render3D(float x, float y, float z, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY)
	{
		MyAssert((m_pAction != nullptr), "特殊エフェクトがセットされていない!\n[Order関数を呼びましょう]");
		m_pAction->Render3D(m_obj, x, y, z, w, h, tx, ty, tw, th, dwFlags);
	}
	void Render3D(Vector3 pos, int w, int h, int tx, int ty, int tw, int th, u32 dwFlags = RS::COPY)
	{
		MyAssert((m_pAction != nullptr), "特殊エフェクトがセットされていない!\n[Order関数を呼びましょう]");
		m_pAction->Render3D(m_obj, pos, w, h, tx, ty, tw, th, dwFlags);
	}

private:


};

/********************************************/
//				tdnPolygon	
/********************************************/

//************************************************************
//	■D3DFVF組み合わせ頂点
//	オリジナルFVFマクロとその設定にあった構造体を送ることで
//	自由な頂点データを送れる。
//***********************************************************

//	２Ｄ用頂点（ライティング＆トランスフォーム済み）
#define D3DFVF_TLVERTEX		( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )//　←D3DFVF_XYZRHW(sx, sy, sz, rhw)と D3DFVF_DIFFUSE(color)とD3DFVF_TEX1(uv)の組み合わせ
typedef struct tagTLVERTEX {
	float	sx, sy, sz;
	float	rhw;
	COLOR	color;
	float	tu, tv;
} TLVERTEX, *LPTLVERTEX;

//	２Ｄ用頂点（ライティング＆トランスフォーム済み、色なし）
#define D3DFVF_TLVERTEX2	( D3DFVF_XYZRHW | D3DFVF_TEX1 )
typedef struct tagTLVERTEX2 {
	float	sx, sy, sz;
	float	rhw;
	float	tu, tv;
} TLVERTEX2, *LPTLVERTEX2;

//	３Ｄ用頂点(法線なし）
#define D3DFVF_VERTEX		( D3DFVF_XYZ | D3DFVF_TEX1 )
typedef struct tagVERTEX {
	float	x, y, z;
	float	tu, tv;
} VERTEX, *LPVERTEX;

//	３Ｄ用頂点(ライティング済み）
#define D3DFVF_LVERTEX		( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 ) // 3つのオブジェクト形式マクロを合わせた
typedef struct tagLVERTEX {
	float	x, y, z;	// 頂点座標
	COLOR	color;		// 頂点の色
	float	tu, tv;		// テクスチャの座標
} LVERTEX, *LPLVERTEX;

//	３Ｄメッシュ用頂点
#define D3DFVF_MESHVERTEX		( D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1 )
typedef struct tagMESHVERTEX {
	float	x, y, z;
	float	nx, ny, nz;
	float	tu, tv;
} MESHVERTEX, *LPMESHVERTEX;

//	３Ｄ用頂点(ライティング済み）
#define D3DFVF_MESHVERTEX2		( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
typedef struct tagLNVERTEX {
	float	x, y, z;
	float	nx, ny, nz;
	COLOR	color;
	float	tu, tv;
} MESHVERTEX2, *LPMESHVERTEX2;

// ライン用の頂点
struct VECTOR_LINE
{
	float x, y, z; // 頂点 
	DWORD color;
};

//	２Ｄオブジェクト
//typedef tdn2DObj *LPtdn2DObj;

/**
*@brief		ポリゴンを生成するクラス
*@author		Nishida
*/
class tdnPolygon
{
public:
	// 3D
	static void Render3D(LPLVERTEX lpVertex, int Num, tdn2DObj* lpObj, u32 dwFlags);										// 3D固定
	static void Render3D(LPLVERTEX lpVertex, int Num, tdn2DObj* lpObj, u32 dwFlags, D3DXMATRIX mat);// 追加
	static void Render3D(LPLVERTEX lpVertex, int Num, tdn2DObj* lpObj, tdnShader* shader, char* name);					// 3Dシェーダ
	// 2D
	static void Render2D(LPTLVERTEX lpVertex, int Num, tdn2DObj* lpObj, u32 dwFlags);										// 2D固定
	static void Render2D(LPTLVERTEX lpVertex, int Num, tdn2DObj* lpObj, tdnShader* shader, char* name);					// 2Dシェーダ	
	// 矩形
	static void Rect(int DstX, int DstY, int DstW, int DstH, u32 dwFlags, COLOR color, float z = .0f);
	static void Rect(int DstX, int DstY, int DstW, int DstH, tdnShader* shader, char* name, COLOR color, float z = .0f);
	// 線
	static void DrawLine(float x1, float y1, float x2, float y2, DWORD color = 0xFFFFFFFF, float size = 1.0f);
	static void DrawLine3D(Vector3 pos1, Vector3 pos2, DWORD color = 0xFFFFFFFF, float size = 1.0f, u32 dwFlag = RS::COPY);
	// 円(2D)
	static void Circle2D(int cx, int cy, float r, u32 dwFlags, COLOR color, float z = .0f);
	static void Circle2D(int cx, int cy, float r, tdnShader* shader, char* name, COLOR color, float z = .0f);

	/*
	*@brief			3角形描画(2D)
	*@param[in]		DstX		開始 X点
	*@param[in]		DstY		開始 Y点
	*@param[in]		DstW		横幅
	*@param[in]		DstH		縦幅
	*@param[in]		angle		三角形の回転角度(radian)
	*/
	static void Triangle(int DstX, int DstY, int DstW, int DstH, COLOR color, float angle, u32 dwFlags = RS::COPY, float z = .0f);
private:

};


// 3Dオブジェクト
class tdnMesh
{
public:
	tdnMesh();
	~tdnMesh();

	/********/
	/* 作成 */
	/********/

	struct CreateData
	{
		unsigned int      numVertexes;  // 頂点の数
		unsigned int      vertexSize;   // 頂点構造体のバイト数
		void              *vertexArray; // 頂点情報（頂点毎）配列

		unsigned int      numIndexes;   // インデックスの数
		DWORD             *indexArray;  // インデックスの配列
		
		unsigned int      numStream;    // ストリーム構造体の数
		unsigned int      streamSize;   // ストリーム構造体のバイト数
		void              *streamArray; // 頂点情報（インデックス毎）の配列

		D3DVERTEXELEMENT9 *decl;        // シェーダーに送る頂点構造体の定義
	};
	bool Create( const CreateData &data );
	bool CreateVertex(
		unsigned int numVertex,    // 頂点数
		unsigned int vertexSize,   // 頂点構造体のバイト数
		void *vertexArray );       // 頂点配列
	bool CreateIndexes(
		unsigned int numIndexes,   // インデックスの数
		const DWORD *indexArray ); // インデックス配列
	bool CreateStream( unsigned int numData, unsigned int dataSize, void *dataArray );
	bool CreateDeclaration(
		unsigned int declArySize,  // 頂点構造体のバイト数
		D3DVERTEXELEMENT9 *decl ); // シェーダー上での頂点構造体の宣言

	// xy平面に正三角形作成
	bool CreateTriangle(
		float radius,    // 外接円の半径
		DWORD color ); 
	// xy平面に長方形作成
	bool CreateRectangle(
		float width,
		float height,
		DWORD color );
	// 正四面体作成
	bool CreateTriangular(
		float radius, // 外接円の半径
		DWORD color );
	// 直方体作成
	bool CreateCube(
		float width,
		float height,
		float depth,
		DWORD color,
		Vector3 *posList = nullptr,
		unsigned int numPos = 0 );

	/************/
	/* 読み込み */
	/************/

	bool LoadMqo( char *filename );

	/********/
	/* 更新 */
	/********/

	// pos, scale, rot からworldMatrix作成
	void UpdateWorldMatrix();

	/********/
	/* 描画 */
	/********/

	void Render( tdnShader *shader, char *technique );

	/*********************/
	/* セッター ゲッター */
	/*********************/

	const Vector3& Pos();
	void Pos( const Vector3& in );
	const Vector3& Scale();
	void Scale( const Vector3& in );
	const Quaternion& Rot();
	void Rot( const Quaternion& in );
	const Matrix& WorldMatrix();
	void WorldMatrix( const Matrix& in );

private:
	IDirect3DVertexDeclaration9* decl;         // 頂点デコレーション（FVF）
	unsigned int                 declSize;     // 頂点構造体のバイト数

	IDirect3DVertexBuffer9*      vertexBuffer;
	unsigned int                 numVertexes;

	IDirect3DIndexBuffer9*       indexBuffer;
	unsigned int                 numIndexes;

	IDirect3DVertexBuffer9*      streamBuffer; // オブジェクト毎の情報
	unsigned int                 streamSize;   // streamBuffer の一つのデータのバイト数
	unsigned int                 numStream;   // streamBuffer の個数

	unsigned int                 numFaces;     // 三角ポリゴン数

	Vector3    pos;
	Vector3    scale;
	Quaternion rot;
	Matrix     worldMatrix;
	public:
	Texture2D *texture;
};

//*****************************************************************************************************************************
//		tdnInput
//*****************************************************************************************************************************
//-----------------------------------------------------------------------------
//		入力関連の定数
//-----------------------------------------------------------------------------
namespace tdnInputEnum
{
	static const int INPUT_DEVICE_MAX = 4;
	static const int STICK_WIDTH = 1000;
	static const int DEFAULT_KEY_CONFIG = -1;
	static const float MIN_MOVE_STICK = .35f;
}


//-----------------------------------------------------------------------------
//		入力デバイス管理
//-----------------------------------------------------------------------------
/**
*@brief		入力デバイスを管理するクラス
*@author		Yamagoe
*/
class tdnInputManager
{
private:
	static LPDIRECTINPUT8 m_lpDI;
	static int m_NumDevice;
	static DIDEVICEINSTANCE	m_DeviceInstances[tdnInputEnum::INPUT_DEVICE_MAX];
	static char m_GroupID[tdnInputEnum::INPUT_DEVICE_MAX][32];

	static BOOL CALLBACK EnumDeviceCallback(const DIDEVICEINSTANCE* pdidi, VOID* pContext);
	static BOOL CALLBACK EnumAxes(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

public:
	static void Initialize();
	static void Release()
	{
		if (m_lpDI)
		{
			delete m_lpDI;
			m_lpDI = nullptr;
		}
	}
	static LPDIRECTINPUTDEVICE8 GetDevice(int no);
	static LPSTR GetGroupID(int no){ return m_GroupID[no]; }
	static LPSTR GetDeviceInstanceName(int no){ return m_DeviceInstances[no].tszInstanceName; }
};

enum KEYCODE
{
	KEY_UP = 0,
	KEY_DOWN = 1,
	KEY_LEFT = 2,
	KEY_RIGHT = 3,

	KEY_A = 4,
	KEY_B = 5,
	KEY_C = 6,
	KEY_D = 7,
	KEY_X = 6,
	KEY_Y = 7,

	KEY_L = 8,
	KEY_L1 = 8,
	KEY_L2 = 10,
	KEY_L3 = 12,

	KEY_R = 9,
	KEY_R1 = 9,
	KEY_R2 = 11,
	KEY_R3 = 13,

	KEY_START = 14,
	KEY_SELECT = 15,

	KEY_ENTER = 14,
	KEY_SPACE = 15,

	AXIS_X = 0,
	AXIS_Y = 1,
	AXIS_Z = 2,
	AXIS_RX = 3,
	AXIS_RY = 4,
	AXIS_RZ = 5
};

typedef struct tagKEYSET
{
	u8	up, down, left, right;
	u8	A, B, X, Y;
	u8	L1, L2, L3;
	u8	R1, R2, R3;
	u8	START, SELECT;
} KEYSET, *LPKEYSET;

typedef struct tagPADSET
{
	u8	lx, ly, rx, ry;
	u8	A, B, X, Y;
	u8	L1, L2, L3;
	u8	R1, R2, R3;
	u8	START, SELECT;
} PADSET, *LPPADSET;

/**
*@brief		ユーザーからの入力情報を管理するクラス
*@author		Yamagoe
*/
class tdnInputDevice
{
private:
	LPDIRECTINPUTDEVICE8 lpDevice;
	LPDIRECTINPUTEFFECT	 pEffect;

	// デフォルトのキー配置
	static const int default_key_map[20];
	static const int default_joy_map[20];

	// 入力に応じた値が格納される(==3とかの3の値が入ってる)
	u8	key_info[20], joy_info[20];

	int key_map[20];
	int joy_map[20];
	int	pad_axisX, pad_axisY;
	int	pad_axisX2, pad_axisY2;
public:
	tdnInputDevice(int n);
	~tdnInputDevice();

	void Update();
	void PadAsign(const PADSET &padset);
	int tdnInputDevice::Get(KEYCODE key){ return key_info[key]; }
	int tdnInputDevice::GetAxisX(){ return (pad_axisX*pad_axisX > tdnInputEnum::MIN_MOVE_STICK*(tdnInputEnum::STICK_WIDTH*tdnInputEnum::STICK_WIDTH)) ? pad_axisX : 0; }
	int tdnInputDevice::GetAxisY(){ return (pad_axisY*pad_axisY > tdnInputEnum::MIN_MOVE_STICK*(tdnInputEnum::STICK_WIDTH*tdnInputEnum::STICK_WIDTH)) ? pad_axisY : 0; }
	int tdnInputDevice::GetAxisX2(){ return (pad_axisX2*pad_axisX2 > tdnInputEnum::MIN_MOVE_STICK*(tdnInputEnum::STICK_WIDTH*tdnInputEnum::STICK_WIDTH)) ? pad_axisX2 : 0; }
	int tdnInputDevice::GetAxisY2(){ return (pad_axisY2*pad_axisY2 > tdnInputEnum::MIN_MOVE_STICK*(tdnInputEnum::STICK_WIDTH*tdnInputEnum::STICK_WIDTH)) ? pad_axisY2 : 0; }
};

/**
*@brief		ゲームを作る人が実際に入力情報の取得で使用するスタティッカークラス
*@author		Yamagoe
*/
class tdnInput
{
private:
	static tdnInputDevice *device[tdnInputEnum::INPUT_DEVICE_MAX];

public:
	static void Initialize();
	static void Release();
	static void Update();
	static void PadAsign(LPSTR config, int no = 0);

	static int KeyGet(KEYCODE key, int no = 0){ return device[no]->Get(key); }
	static int GetAxisX(int no = 0) { return device[no]->GetAxisX(); }
	static int GetAxisY(int no = 0) { return device[no]->GetAxisY(); }
	static int GetAxisX2(int no = 0){ return device[no]->GetAxisX2(); }
	static int GetAxisY2(int no = 0){ return device[no]->GetAxisY2(); }
	static void GetAxisXYf(float *outX, float *outY, int no = 0);
	static void GetAxisXY2f(float *outX, float *outY, int no = 0);
};

#define KEY(x,n) ( tdnInput::KeyGet(x,n) )
#define KEY_TRG(x,n) ( tdnInput::KeyGet(x,n)==3 )

//-----------------------------------------------------------------------------
//		OKB
//-----------------------------------------------------------------------------
enum VK_CODE
{
	//=============================
	// 基本キーボード
	KB_A = 0x41,
	KB_B = 0x42,
	KB_C = 0x43,
	KB_D = 0x44,
	KB_E = 0x45,
	KB_F = 0x46,
	KB_G = 0x47,
	KB_H = 0x48,
	KB_I = 0x49,
	KB_J = 0x4A,
	KB_K = 0x4B,
	KB_L = 0x4C,
	KB_M = 0x4D,
	KB_N = 0x4E,
	KB_O = 0x4F,
	KB_P = 0x50,
	KB_Q = 0x51,
	KB_R = 0x52,
	KB_S = 0x53,
	KB_T = 0x54,
	KB_U = 0x55,
	KB_V = 0x56,
	KB_W = 0x57,
	KB_X = 0x58,
	KB_Y = 0x59,
	KB_Z = 0x5A,
	KB_ENTER = 0x0D,
	KB_SPACE = 0x20,
	KB_LEFT = 0x25,	/* ← */
	KB_UP = 0x26,	/* ↑ */
	KB_RIGHT = 0x27,	/* → */
	KB_DOWN = 0x28,	/* ↓ */

	KB_0 = 0x30,
	KB_1 = 0x31,
	KB_2 = 0x32,
	KB_3 = 0x33,
	KB_4 = 0x34,
	KB_5 = 0x35,
	KB_6 = 0x36,
	KB_7 = 0x37,
	KB_8 = 0x38,
	KB_9 = 0x39,

	KB_COLON = 0xBA,	/* : */
	KB_SEMICORON = 0xBB,	/* ; */
	KB_COMMA = 0xBC,	/* , */
	KB_HYPHEN = 0xBD,	/* - */
	KB_DOT = 0xBE,	/* . */
	KB_SLASH = 0xBF,	/* / */
	KB_AT_SIGN = 0xC0,	/* @ */
	KB_SQUARE_L = 0xDB,	/* [ */
	KB_YEN = 0xDC,	/* \ */
	KB_SQUARE_R = 0xDD,	/* ] */
	KB_CARET = 0xDE,	/* ^ */
	KB_UNDER_BAR = 0xE2,	/* _ */


	//=============================
	// テンキー関連
	KB_NUMPAD0 = 0x60,	/* 0 */
	KB_NUMPAD1 = 0x61,	/* 1 */
	KB_NUMPAD2 = 0x62,	/* 2 */
	KB_NUMPAD3 = 0x63,	/* 3 */
	KB_NUMPAD4 = 0x64,	/* 4 */
	KB_NUMPAD5 = 0x65,	/* 5 */
	KB_NUMPAD6 = 0x66,	/* 6 */
	KB_NUMPAD7 = 0x67,	/* 7 */
	KB_NUMPAD8 = 0x68,	/* 8 */
	KB_NUMPAD9 = 0x69,	/* 9 */
	KB_MULT = 0x6A,	/* * */
	KB_ADD = 0x6B,	/* + */
	KB_SUBTRACT = 0x6D,	/* - */
	KB_DECIMAL = 0x6E,	/* . */
	KB_DIVIDE = 0x6F,	/* / */



	//=============================
	// あまり使うことはないかも…
	KB_BACK = 0x08,
	KB_TAB = 0x09,
	KB_CLEAR = 0x0c,

	KB_SHIFT = 0x10,
	KB_CTRL = 0x11,
	KB_ALT = 0x12,
	KB_PAUSE = 0x13,
	KB_CAPITAL = 0x14,	/* Shift + CapsLock */
	KB_ESC = 0x1B,
	KB_NON_CONV = 0x1D,	/* 無変換 */

	KB_PGUP = 0x21,
	KB_PGDN = 0x22,
	KB_END = 0x23,
	KB_HOME = 0x24,
	KB_PRT_SC = 0x2C,
	KB_INSERT = 0x2D,
	KB_DELETE = 0x2E,

	//=============================
	// マウス(キーではないが)
	MOUSE_LEFT = 0x01,
	MOUSE_RIGHT = 0x02,
	MOUSE_CENTAR = 0x04		/* ホイールクリック(ぐるぐるではない) */

};

/**
*@brief		キーボードの入力情報を管理するクラス
*@author		Yamagoe
*/
class OwatasoKeyBoard
{
public:
	OwatasoKeyBoard();

	// キーボードの状態を格納
	BYTE info[256];

	// キーボードの押してる時間
	UINT on_frame[256];

	void Update();

	BYTE AnyTRG();
};


//====================================
//		初期化と解放
void OKB_Init();
void OKB_Release();
//====================================
//		キーボード情報更新
void OKB_Update();

// 対応しているキーが押されているフレーム数を返す( 押しっぱなし判定にどうぞ )
UINT KeyBoard(BYTE KeyCode);

// キーのフレームが引数frameと同じになったらtrue、それ以外だとfalse( 押した瞬間判定にどうぞ )
bool KeyBoardTRG(BYTE KeyCode, UINT frame = 1);

// 何かキーが押されたとき、1フレームだけそのキーのキーコードが返ってくる( タイピングゲームとかで使えそう )
BYTE KeyBoardAnyTRG();


//-----------------------------------------------------------------------------
//		マウス入力
//-----------------------------------------------------------------------------

enum class WHEEL_FLAG
{
	NONE, UP, DOWN
};

class tdnMouse
{
private:
	static POINT m_CurrentPoint;
	static POINT m_PrevPoint;
	static RECT m_Rc;

	// ホイール
	static int m_PrevWheel;
	static int m_CurrentWheel;
	static WHEEL_FLAG m_FlagW;

	static Vector2 m_Axis;
	static Vector2 m_Pos;
	static int m_FlagRight, m_FlagLeft;

public:

	static void SetWheel(WHEEL_FLAG f)
	{
		m_CurrentWheel = (f == WHEEL_FLAG::DOWN) ? m_CurrentWheel - 1 : m_CurrentWheel + 1;
	}

	static void Initialize(BOOL show);
	static void Update();
	static Vector2 &GetPos(){ return m_Pos; }
	static int GetLeft(){ return m_FlagLeft; }									// 左クリックしたかどうか(iexみたいにおしっぱ1、押した瞬間3が返る)
	static int GetRight(){ return m_FlagRight; }								// 右クリックしたかどうか(iexみたいにおしっぱ1、押した瞬間3が返る)
	static WHEEL_FLAG GetWheelFlag(){ return m_FlagW; }							// ホイールの状態取得
	static bool isPushCenter(){ return ((GetKeyState(0x04) & 0x80) != 0); }		// 真ん中ボタンをクリックしたか
	static float GetMoveDist()
	{
		float vx = (float)(m_CurrentPoint.x - m_PrevPoint.x), vy = (float)(m_CurrentPoint.y - m_PrevPoint.y);
		return sqrtf(vx*vx + vy*vy);
	}
};



//*****************************************************************************
//		tdnAudio
//*****************************************************************************

#include "ogg/vorbisfile.h"

#define	STR_NORMAL	0
#define	STR_FADEIN	1
#define	STR_FADEOUT	2

#define	TYPE_WAV	0
#define	TYPE_OGG	1

#define TDNSOUND_PLAY_NONE -1		// 再生が失敗したときの戻り値となる、または再生した番号を受け取る変数の初期値として与える

// OWD用の構造体
typedef struct tagOWDInfo
{
	WAVEFORMATEX	wfx;	// WAVフォーマット
	DWORD			size;	// 音源データのサイズ
	LPBYTE			data;	// 音源データ
}OWDInfo;

// サウンドエフェクト定数
enum class DXA_FX
{
	OFF = 0,		// エフェクトオフ
	CHORUS = 1 << 0,	// コーラス
	COMPRESSOR = 1 << 1,	// コンプレッサ
	DISTORTION = 1 << 2,	// ディスト―ション
	ECHO = 1 << 3,	// エコー
	FLANGER = 1 << 4,	// フランジ
	GARGLE = 1 << 5,	// ガーグル
	ENVREVERB = 1 << 6,	// 環境リバーブ
	PARAMEQ = 1 << 7,	// パラメトリックイコライザ
	WAVESREVERB = 1 << 8,	// ミュージックリバーブ(16bitのオーディオフォーマットのみ)
	MAX = 1 << 9
};

/**
*@brief		音源情報を設定し、再生・停止等の制御を行うクラス(複数の音をだすため、このクラスを複数管理するクラスを用意する)
*@author		Yamagoe
*/
class tdnSoundBuffer
{
protected:
	LPDIRECTSOUNDBUFFER8	lpBuf;
	LPDIRECTSOUND3DBUFFER8	lpBuf3D;
	LPBYTE LoadFile(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx);
	LPBYTE LoadWAV(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx);
	LPBYTE LoadOWD(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx);	// Owataso Wave Data
	LPBYTE LoadMP3(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx);
	LPBYTE LoadOGG(LPSTR fname, LPDWORD size, LPWAVEFORMATEX wfx);

	DWORD PlayCursor;
	DWORD BufferSize;
	WAVEFORMATEX format;

	// Loadするときに使う変数(ローカル⇒メンバ変数)
	WAVEFORMATEX	wfx;
	LPBYTE			lpWBuf;
	DWORD			size;

	char			wav_file_path[128];	/*	WAVEファイルへのパス*/
	bool			loop_flag;

	static const int READBYTE;

public:
	tdnSoundBuffer(LPDIRECTSOUND8 lpDS, char* filename, bool b3D);
	void Initialize(LPDIRECTSOUND8 lpDS, unsigned char* data, DWORD size, LPWAVEFORMATEX fmt, bool b3D);
	tdnSoundBuffer(){}
	~tdnSoundBuffer();

	// 同じのを複数読み込みたいときに使う
	static void Create_and_copy(LPDIRECTSOUND8 lpDS, char* filename, bool b3D, tdnSoundBuffer **buffers, int dst, int count);

	LPDIRECTSOUNDBUFFER8 GetBuf(){ return lpBuf; }
	void Play(bool loop, DWORD cursor = 0);
	void Stop();
	void Pause();
	void PauseOff();

	void	SetVolume(int volume);
	int		GetVolume();
	void	SetPan(int pan);
	int		GetPan();
	void	SetFrequency(int pitch);
	int		GetFrequency();
	bool	isPlay();
	void	SetSpeed(float speed);
	float	GetSpeed();

	DWORD GetPlayCursor();
	void SetPlayCursor(DWORD new_position);
	DWORD GetPlayFrame();
	int GetPlaySecond();
	void SetPlaySecond(int sec);
	DWORD GetPlayMSecond();
	DWORD GetSize();
	int GetLengthSecond();

	void Get_file_path(char out[]){ sprintf_s(out, sizeof(out), "%s", wav_file_path); }


	//------------------------------------------------------
	//	サウンドエフェクト
	//------------------------------------------------------
	void SetFX(DXA_FX flag);

	//------------------------------------------------------
	//	3D関係
	//------------------------------------------------------
	void SetDist(float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);	// 最大超えるともう聞こえない、最少は普通に1とかでいいと思う
	void SetPos(const Vector3 &pos);																	// 音が発生する座標
	void SetFront(const Vector3 &front);																// 音の向き
	void SetRange(int degreeIn = DS3D_DEFAULTCONEANGLE);												// 音の範囲内角度の設定(度)
	void SetOutRange_volume(int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);								// 角度範囲外の場合の音(-10000(最少)～0(最大))
	void SetMove(const Vector3 &move);																	// 普通は動かないけどドップラー効果で使う
	void SetAll3D(float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};

class tdnStreamSound
{
private:
	int		rate;
	LPDIRECTSOUNDBUFFER	lpStream;		// ストリーム用二次バッファ
	DSBPOSITIONNOTIFY	pn[3];

	HANDLE	hStrThread;
	unsigned long dwThreadId;
	unsigned long dwThrdParam;

	FILE*	hStrFile;
	OggVorbis_File	vf;
	DWORD StrSize;
	DWORD StrPos;
	DWORD LoopPtr;

	BYTE mode;
	int	param;
	int volume;
	bool m_bLoop;
	bool m_bEnd;

	void Initialize(LPDIRECTSOUND lpDS, int rate);

	BOOL	OGGRead(LPBYTE dst, unsigned long size);

	DWORD m_AvgBytesPerSec;

public:
	static const int STRSECOND = 1;

	BYTE	type;
	HANDLE	hEvent[3];
	LPDIRECTSOUNDNOTIFY	lpStrNotify;


	tdnStreamSound(LPDIRECTSOUND lpDS, LPSTR filename, BYTE mode, int param, bool bLoop);
	~tdnStreamSound();

	BOOL	SetBlockOGG(int block);
	BOOL	SetBlockWAV(int block);

	BOOL	SetWAV(LPDIRECTSOUND lpDS, char* filename);
	BOOL	SetOGG(LPDIRECTSOUND lpDS, char* filename);

	bool	isEnd()
	{
		return m_bEnd;
	}
	bool isPlay();
	void	Stop();
	void	SetVolume(int volume);
	int		GetVolume(){ return volume; }
	int		GetRate(){ return rate; }
	void	SetSpeed(float speed){ lpStream->SetFrequency((DWORD)(rate*speed)); }

	DWORD GetBlockMSecond()
	{
		/* 再生位置(ミリ秒換算) */
		DWORD pos;
		lpStream->GetCurrentPosition(&pos, nullptr);
		return (int)(pos / (m_AvgBytesPerSec * .001f));
	}

	DWORD GetPlayCursor()
	{
		return m_PlayMSecond + GetBlockMSecond();
	}

	static DWORD m_PlayMSecond;

	void SetMode(BYTE mode, int param);
	BYTE GetMode(){ return mode; }
	int GetParam(){ return param; }

};

typedef tdnStreamSound tdn_DSSTREAM, *tdn_LPDSSTREAM;




/**
*@brief		サウンド管理の基底(継承で使うだけで、ゲーム作る人は使用しない)
*@author		Yamagoe
*/
class tdnSoundBase
{
protected:
	static const int WavNum = 128;	// バッファ最大値

	HWND	hWndWAV;

	LPDIRECTSOUND8			lpDS;		// DirectSoundオブジェクト
	LPDIRECTSOUNDBUFFER	lpPrimary;		// 一次バッファ

	LPDIRECTSOUND3DLISTENER8 lp3DListener;	// 3Dサウンド上で、音源と聞こえる人との関係でいうと聞こえる人の情報

public:
	//------------------------------------------------------
	//	初期化・解放
	//------------------------------------------------------
	tdnSoundBase();
	virtual ~tdnSoundBase();

	//------------------------------------------------------
	// ※3Dサウンド限定 リスナー情報設定
	//------------------------------------------------------
	void SetListenerPos(const Vector3 &pos);
	void SetListenerOrientation(const Vector3 &fornt, const Vector3 &up);
	void SetListenerMove(const Vector3 &move);
	void SetListenerAll(const Vector3 &pos, const Vector3 &fornt, const Vector3 &up, const Vector3 &velocity);
	void UpdateListener();	// 3Dサウンドを使用する場合、毎回これを呼び出してください
};


/**
*@brief		SE特化のサウンドバッファ管理クラス(同じ音の多重再生を可能にしている)
*@author		Yamagoe
*/
class tdnSoundSE : public tdnSoundBase
{
private:
	struct SEData
	{
		bool b3D;					// 3Dフラグ
		tdnSoundBuffer* buffer;	// サウンドバッファ
	};
	std::vector<SEData*> data[WavNum];

public:
	tdnSoundSE();
	~tdnSoundSE();

	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//	引数…ID:番号、 num_of_play_simultaneously:同時再生可能数(あんまり多いとメモリ食うのでほどほどに。5～10前後)、　ファイル名、　3Dサウンドフラグ
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	/*
	*@brief					音源情報の設定
	*@param[in]		ID							その音のID
	*@param[in]		NumOfPlaySimultaneously		同時再生可能数(あんまり多いとメモリ食うのでほどほどに。5～10前後。)
	*@param[in]		filename					音源ファイルへのファイルパス
	*@param[in]		b3D							3Dサウンドフラグ
	*/
	void Set(int ID, int NumOfPlaySimultaneously, char* filename, bool b3D = false);

	/* 再生 */
	int Play(int ID, bool loop = false);

	/* 停止:noにはPlayで返ってきた値を自分で入れる */
	void Stop(int ID, int no);
	void AllStop();

	/* 各サウンドパラメータ設定・取得関数 */
	void	SetVolume(int ID, int volume);
	void	SetVolume(int ID, float volume);	// 0～1
	int		GetVolume(int ID);
	void	SetPan(int ID, int pan);
	int		GetPan(int ID);
	void	SetFrequency(int ID, int pitch);
	void	SetFrequency(int ID, int no, int pitch);
	int		GetFrequency(int ID, int no);
	void	SetSpeed(int ID, float speed);
	void	SetSpeed(int ID, int no, float speed);
	bool	isPlay(int ID, int no);
	void	SetFX(DXA_FX flag);
	void	SetFX(int ID, DXA_FX flag);


	//---------------------------------------------------------------
	// ※以下の関数を呼ぶにはSetの引数のb3Dをtrueにする必要があります
	//---------------------------------------------------------------
	int Play(int ID, const Vector3 &pos, const Vector3 &front = Vector3(0, 0, -1), const Vector3 &move = Vector3(0, 0, 0), bool loop = false);// 音源情報設定しつつ再生
	int Play(int ID, const Vector3 &pos, const Vector3 &move, bool loop);															// ちょっと簡易版
	void SetDist(int ID, int no, float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);				// 最大超えるともう聞こえない、最少は普通に0とかでいいと思う
	void SetPos(int ID, int no, const Vector3 &pos);																				// 音が発生する座標
	void SetFront(int ID, int no, const Vector3 &front);																			// 音の向き
	void SetRange(int ID, int no, int degreeIn = DS3D_DEFAULTCONEANGLE);															// 音の範囲内角度の設定(度)
	void SetOutRange_volume(int ID, int no, int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);											// 角度範囲外の場合の音(-10000(最少)～0(最大))
	void SetMove(int ID, int no, const Vector3 &move);																				// 普通は動かないけどドップラー効果で使う
	void SetAll3D(int ID, int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};



/**
*@brief		BGM特化用のサウンド管理クラス(主にフェード関係が実装されている)
*@author		Yamagoe
*/
class tdnSoundBGM : public tdnSoundBase
{
private:
	static const int MinVolume = -2500;	// 音量最少値

	enum class MODE
	{
		NONE, FADE_IN, FADE_OUT, MAX
	};
	void None(int no);
	void In(int no);
	void Out(int no);
	void(tdnSoundBGM::*Fade_funk[(int)MODE::MAX])(int);

	struct tagBGMData
	{
		bool b3D;			// 3Dサウンドフラグ
		MODE fade_mode;		// フェードインかアウトか何もしないか
		float volume;		// 音量(0～1)
		float fade_speed;		// フェード速度
		tdnSoundBuffer* buffer;
	};
	std::vector<tagBGMData*> data;

public:
	tdnSoundBGM();
	~tdnSoundBGM();

	//////////////////////////////////////////////
	/* ※毎フレームUpdateを呼び出してください！ */
	void Update();
	//////////////////////////////////////////////

	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//	引数…ID:番号、ファイル名、3Dサウンドフラグ(BGMの場合、使う機会はほぼない)
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	void Set(int ID, char* filename, bool b3D = false);
	void Set(int ID, unsigned char *data, unsigned long size, LPWAVEFORMATEX wfx, bool b3D = false);

	/* 再生 */
	void Play(int ID, bool loop = true, DWORD cursor = 0);

	/* 停止 */
	void Stop(int ID);
	void AllStop();
	void Pause(int ID);
	void PauseOff(int ID, float FadeSpeed = 1);

	/* フェード関係(BGMとSEで分けた要因の1つ) */
	void FadeOut(int ID, float fade_speed);	// fade_speedは0～1の範囲。0.01とかの値になる
	void FadeIn(int ID, float fade_speed, bool loop = true, DWORD cursor = 0);

	enum class CROSS_FADE_TYPE
	{
		NORMAL,			// 普通に最初から再生
		END_OF_ETERNITY	// EoE形式(インさせるBGMをアウトさせるBGMと同じカーソルから再生させる)
	};
	void CrossFade(int inID, int outID, float fade_speed, CROSS_FADE_TYPE type = CROSS_FADE_TYPE::NORMAL, bool loop = true);
	void CrossFade(int inID, int outID, float in_speed, float out_speed, CROSS_FADE_TYPE type, bool loop);

	/* 各サウンドパラメータ設定・取得関数 */
	void	SetVolume(int ID, int volume);
	void	SetVolume(int ID, float volume);	// 0～1
	int		GetVolume(int ID);
	void	SetPan(int ID, int pan);
	int		GetPan(int ID);
	void	SetFrequency(int ID, int pitch);
	int		GetFrequency(int ID, int no);
	void	SetSpeed(int ID, float speed);
	void	SetSpeed(int ID, int no, float speed);
	bool	isPlay(int ID);
	DWORD	GetPlayCursor(int ID);
	DWORD	GetPlayFrame(int ID);
	int 	GetPlaySecond(int ID);
	DWORD	GetPlayMSecond(int ID);
	void	SetPlaySecond(int ID, int sec);
	DWORD	GetSize(int ID);
	int		GetLengthSecond(int ID);
	void	SetFX(DXA_FX flag);
	void	SetFX(int ID, DXA_FX flag);

	tdnStreamSound* PlayStream(char* filename, bool bLoop = true);
	tdnStreamSound* PlayStream(char* filename, BYTE mode, int param, bool bLoop);


	//---------------------------------------------------------------
	// ※以下の関数を呼ぶにはSetの引数のb3Dをtrueにする必要があります
	//---------------------------------------------------------------
	void Play(int ID, const Vector3 &pos, const Vector3 &front = Vector3(0, 0, -1), const Vector3 &move = Vector3(0, 0, 0), bool loop = true);// 音源情報設定しつつ再生
	void SetDist(int ID, int no, float max_dist = DS3D_DEFAULTMAXDISTANCE, float min_dist = DS3D_DEFAULTMINDISTANCE);				// 最大超えるともう聞こえない、最少は普通に0とかでいいと思う
	void SetPos(int ID, int no, const Vector3 &pos);																				// 音が発生する座標
	void SetFront(int ID, int no, const Vector3 &front);																			// 音の向き
	void SetRange(int ID, int no, int degreeIn = DS3D_DEFAULTCONEANGLE);															// 音の範囲内角度の設定(度)
	void SetOutRange_volume(int ID, int no, int out_vol = DS3D_DEFAULTCONEOUTSIDEVOLUME);											// 角度範囲外の場合の音(-10000(最少)～0(最大))
	void SetMove(int ID, int no, const Vector3 &move);																				// 普通は動かないけどドップラー効果で使う
	void SetAll3D(int ID, int no, float max_dist, float min_dist, const Vector3 &pos, const Vector3 &front, int degreeIn, int out_vol, const Vector3 &move);
};


// わぶよみこみ
LPBYTE LoadWavData(LPSTR filename, LPDWORD size, LPWAVEFORMATEX wfx);

// おわたそウェーブデータ
void WriteOWD(char* ReadPath, char *WritePath);

/********************************************/
//				tdnText				     
/********************************************/
/**
*@brief		デバッグテキスト描画
*@author		Nishida
*/
class tdnText{
private:
	static LPD3DXFONT font;
public:
	static void Init();
	static void CleanUpModule();
	static void Draw(int x, int y, DWORD color, const char * _Format, ...);

	/**
	*@brief					引数の文字列を、全角半角問わず1文字ずつカウントして返す関数
	*@author		Yamagoe
	*@param[in]		str		カウントしたい文字列
	*/
	static int StrCount(char *str);
};

/**
*@brief		上記のテキストがスタティッカーなのに対し、こちらは、自分でポインタを持って使う
*@author		Yamagoe
*/
class tdnText2{
private:
	LPD3DXFONT m_font;
	int m_size;			// 中央ぞろえ描画に使用
public:
	tdnText2(){ Init(); }

	/**
	*@brief					引数付きコンストラクタ
	*@param[in]		size	テキストのサイズ
	*@param[in]		*font	フォント名	example "メイリオ" "ＭＳ ゴシック"
	*/
	tdnText2(int size, char *font){ Init(size, font); }

	~tdnText2(){ CleanUpModule(); }
	void Init(int size = 32, char *font = "ＭＳ ゴシック");
	void CleanUpModule();
	void Draw(int x, int y, DWORD color, const char * _Format, ...);
	void DrawCenter(int cx, int y, DWORD color, const char * _Format, ...);
};

/************************/
//		tdnFont
/************************/

const	UINT	CacheQty = 256;			//	文字のキャッシュの数

class tdnFont
{
private:
	tdnFont() {};
	~tdnFont() {};

public:
	//	初期化・解放
	static	void	Initialize();
	static	void	Release();

	// 文字列描画
	static	void	RenderString(LPCSTR string, int fontSize, int drawX, int  drawY, COLOR dwColor=0xffffffff, LPCSTR fontName = "ＭＳ Ｐゴシック", u32 dwFlags = RS::COPY);
	static	void	RenderString(int fontSize, int drawX, int  drawY, COLOR dwColor, const char * _Format, ...);

	// 一文字描画
	static	Vector2	RenderCharacter(LPCSTR character, LPCSTR fontName, int fontSize, int drawX, int  drawY, COLOR dwColor, u32 dwFlags = RS::COPY);

	//  簡易
	static void RenderFont2D(LPCSTR _String, int _FontSize, int _DrawX, int _DrawY, COLOR dwColor);

	// 文字列中央揃へ描画
	static	void RenderStringCentering(LPCSTR string, int fontSize, int drawX, int  drawY, COLOR dwColor=0xffffffff, LPCSTR fontName = "ＭＳ Ｐゴシック", u32 dwFlags = RS::COPY);

	// ピクトグラフ
	enum class PICTOGRAPH
	{
		BEGINNER,
		MAX,
	};

	static LPCSTR GetPictGraphStr(PICTOGRAPH type){ return m_lpcsPictGraphStrs[(int)type]; }
private:

	//	照合用データ
	typedef	struct	ReferenceData {
		bool	isEnable;	//	有効化フラグ
		char	chara[2];	//	文字
		UINT	byte;		//	文字のバイト数
		UINT	size;		//	サイズ
		LPCSTR	fontName;	//	フォント名
	}	RefData;

	//	キャッシュ処理用デスク
	struct	CacheDesc {
		UINT		nextUseCacheNum;		//	次に使用するキャッシュの番号
		tdn2DObj*	textureCacheList;		//	文字テクスチャのリスト
		RefData*	referenceDataList;		//	キャッシュの照合用データリスト
	};

	//	管理パラメータ
	static	bool		m_IsInitialized;		//	初期化済みフラグ
	static	CacheDesc	m_CacheDesc;			//	キャッシュ処理用デスク


	/****************************************/
	//	 2DObjから文字を作るサポート関数
	/****************************************/
	static	UINT	SearchCache(LPCSTR chara, UINT size, LPCSTR fontName);


	/****************************************/
	//	 真夏のファイト4の絵文字サポート関数
	//***************************************/
	static tdn2DObj* m_pPictograph;
	static bool SearchPictograph(char buffer[3], int x, int y, UINT size,DWORD color, DWORD RenderFlag);
	static LPCSTR m_lpcsPictGraphStrs[(int)PICTOGRAPH::MAX];

};



/********************************************/
//				tdnRandom			     
/********************************************/
/**
*@brief		メルセンヌ・ツイスタ法での乱数生成クラス
*@author		Hidaka
*/
class tdnRandom{
public:
	static std::mt19937 engine;

	static void Initialize();

	static std::uniform_int_distribution<int> Make_distribution(int min_num, int max_num);
	static std::uniform_real_distribution<float> Make_distribution(float min_num, float max_num);

	static int Get(int min_num, int max_num);
	static float Get(float min_num, float max_num);

	typedef std::uniform_int_distribution < int > INT;
	typedef std::uniform_int_distribution < float > FLOAT;
};


/********************************************/
//				tdnFile			     
/********************************************/
/**
*@brief		ファイル関係の便利な関数をまとめたクラス
*@author		Yamagoe
*/

// ファイルディレクトリ列挙に使用される構造体
typedef struct tagDirectoryInfo
{
	// newとかめんどくさくなりそうなのでvector使う
	//int NumFile;				// ファイルの個数
	//std::string *FileNames;		// ファイルの名前(可変長)
	//int NumFolder;				// フォルダの個数
	//std::string *FolderNames;	// フォルダの名前(可変長)

	// string型での動的配列で、ファイル名とフォルダー名を格納
	std::vector<std::string> FileNames;		// 列挙されたファイル名
	std::vector<std::string> FolderNames;	// 列挙されたフォルダ名

	tagDirectoryInfo(){ FileNames.clear(), FolderNames.clear(); }
	~tagDirectoryInfo(){ FileNames.clear(), FolderNames.clear(); }
}DirectoryInfo;

class tdnFile
{
public:

	/**
	*@brief			フォルダ(ディレクトリ)作成
	*@param[in]		*path	example…"DATA/CHR/Airou"ならCHRフォルダにAirouというフォルダが作られる
	*@return		成功したら「0」フォルダが既に作られていたりして失敗したら「-1」が返る
	*/
	static int CreateFolder(LPCSTR path);

	/**
	*@brief					ディレクトリの列挙
	*@param[in]		*path	ディレクトリパス("DATA/CHR"ならCHRフォルダの中が列挙される)
	*@param[in]		*out	列挙したファイル名をフォルダー名を格納する構造体へのアドレス
	*@param[in]		bExt	ファイル名の場合、拡張子をつけるかつけないか
	*/
	static void EnumDirectory(LPCSTR path, DirectoryInfo *out, bool bExt = true);

	/**
	*@brief					ファイルの拡張子を返す("tdn.txt"なら.txtが返る)
	*@param[in]		*path	ファイルパス
	*/
	static std::string GetFileExtention(LPCSTR path);

	/**
	*@brief					ファイルパスからファイル名を返す("DATA/Text/tdn.txt"ならtdn.txtが返る)
	*@param[in]		*path	ファイルパス
	*@param[in]		bExt	拡張子をくっつけるか　[true: return tdn.txt]　[false: return tdn]
	*/
	static std::string GetFileName(LPCSTR path, bool bExt = true);

	/**
	*@brief					ファイルを開くダイアログを作成
	*@param[in]		*filter	拡張子フィルター example:"TEXT DATA(*.txt)\0 *.txt\0\0"
	*@return				成功したらダイアログで選択された絶対パスを返す、失敗したら""(空の文字列)
	*/
	static std::string OpenFileDialog(LPCSTR filter = "すべてのファイル(*.*)\0 * .*\0\0");

	/**
	*@brief					ファイルを保存するダイアログを作成
	*@param[in]		*filter	拡張子フィルター example:"TEXT DATA(*.txt)\0 *.txt\0\0"
	*@return				成功したらダイアログで選択された絶対パスを返す、失敗したら""(空の文字列)
	*/
	static std::string SaveFileDialog(LPCSTR filter = "すべてのファイル(*.*)\0 * .*\0\0");
private:
	static char strFile[256];			// ダイアログを開くときに、前のパスを残したいときとかに
};

// bool型のバイトが処理系依存とかいうらしい(知ったか)なので、1ビットでの変数を自分で作る
struct Mybool
{
	unsigned int val : 1;	// 1ビット(真の0 or 1)
	Mybool() : val(0){}
	Mybool(int i) :val(i & 0x1){}
};


/**
*@brief		主にデバッグに役立つ関数をまとめるクラス
*@author		Yamagoe
*/
class tdnDebug
{
public:

	/**
	*@brief			引数の文字列をテキストファイル(DATA/System/log.txt)に吐き出す。主にデバッグが効かないときに。
	*@param[in]		*format	とりあえずprintf的な感じにしてみる
	*/
	static void OutPutLog(const char *format, ...)
	{
		char str[65536];

		// printfの引数を文字列に変換
		{
			// 可変長引数を操作するためのもの
			va_list ptr;
			va_start(ptr, format);
			// このvsprintfでstrに可変長引数込みのformatを格納させる(sprintfの可変長対応verみたいな感じ)
			vsprintf_s(str, sizeof(str), format, ptr);
			va_end(ptr);
		}

		// ログテキストに追加で書き出し(上書きはしない)
		std::ofstream ofs("DATA/System/log.txt", std::ios::out | std::ios::ate | std::ios::app);
		ofs << str;
	}

	/**
	*@brief					ログテキストファイル(DATA/System/log.txt)を白紙にする
	*/
	static void ClearLog()
	{
		// ログテキストを開いて何もしない
		std::ofstream ofs("tdnLog.txt");
	}
private:
};