#ifndef __IT_DEBUG_H__
#define __IT_DEBUG_H__

#define _IT_DEBUG

#ifdef _IT_DEBUG

//*****************************************************************
//newオーバーロード
//*****************************************************************

#include"tdnlib.h"

#define _CRTDBG_MAP_ALLOC

#include <Windows.h>
#include <cstdlib>
#include <cassert>
#include <crtdbg.h>
#include <new>
#include <memory>

//*****************************************************************
//メモリーリーク自動チェック
//*****************************************************************
#define MyMemoryLeakCheck _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)

//*****************************************************************
//		例外によって送出されるクラス
//*****************************************************************

template<class _error_type>
class ErrorHandlerBase
{
	typedef _error_type ERROR_TYPE_VALUE;
private:
	char				str[512];
	ERROR_TYPE_VALUE	data;
public:
	ErrorHandlerBase(
		const char* errstr,				//エラーメッセージ
		const ERROR_TYPE_VALUE& data):	//error値
		data(data)
	{
		str[0] = '\0';
		if (errstr)
		{
			strcpy_s(str, 512, errstr);
		}
	}
	~ErrorHandlerBase()
	{

	}
	ErrorHandlerBase(const ErrorHandlerBase& errh)
	{
		strcpy_s(str, 512, errh.GetStr());
	}
public:
	const char*	GetStr()const
	{
		return str;
	}
	const ERROR_TYPE_VALUE&	GetErrData()const
	{
		return data;
	}
};

//ゲーム強制終了error
typedef ErrorHandlerBase<int> Error_GameExit;


//*****************************************************************
//printfと同じ形式で出力ウィンドウに表示
//*****************************************************************
#define MyDebugString(str,...)\
{\
	TCHAR temp[512]; \
	sprintf_s<512>(temp, str, __VA_ARGS__); \
	OutputDebugString(temp); \
}

//*****************************************************************
//	メッセージボックスをprintfみたいに表示する
//*****************************************************************
#define MessageBoxPlus(str,...)\
{\
	char t[512]; \
	sprintf_s<512>(t, str, __VA_ARGS__);\
	MessageBox(0, t, "MessageBoxPlus", MB_OK);\
}

//*****************************************************************
//printfと同じ形式でアサートに文字列を表示させる
//*****************************************************************
#define MyAssert(expr,str,...)\
{\
if (!(expr)){\
\
	\
	char t[256], ti[600]; \
	sprintf_s<256>(t, str, __VA_ARGS__); \
	sprintf_s<600>(ti, "　%s　の行番号　%d　でエラーが発生しました \n %s \n式(%s) \n実行を続けますか？", __FILE__, __LINE__, t, #expr); \
	MessageBeep(MB_OK); \
if (MessageBox(0, ti, "MyAssertError!!!", MB_YESNO) == IDNO){\
	Error_GameExit err("GameExitError", 0);\
	throw err; \
}\
}\
}

#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)

#else

#define MyDebugString(str,...)
#define MyAssert(expr,str,...) expr
#define MyMemoryLeakCheck

#endif	//_IT_DEBUG

#endif	//eof