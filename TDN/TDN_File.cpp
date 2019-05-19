
#include	"TDNLIB.h"
#include<iostream>

// フォルダを作るために必要なヘッダ
#include <direct.h>

// c++11にて追加されたファイル関係の便利なライブラリ
#include <filesystem> // std::tr2::sys::path etc.
namespace sys = std::tr2::sys;

// 静的メンバ変数の宣言
char tdnFile::strFile[256] = "";		// ダイアログとかでのパス


										//*****************************************************
										//		ファイル関係で便利になりそうなのをまとめるクラス
										//*****************************************************

int tdnFile::CreateFolder(LPCSTR path)
{
	// ディレクトリ作成&結果を返す(-1なら失敗、0なら成功)
	return _mkdir(path);
}

void tdnFile::EnumDirectory(LPCSTR path, DirectoryInfo *out, bool bExt)
{
	sys::path p(path); // 列挙の起点

					   // for_eachとラムダによるループでは、スコープ外の変数にアクセスできないので不採用
					   //std::for_each(sys::directory_iterator(p), sys::directory_iterator(),[](const sys::path& p)
					   //{
					   //	if (sys::is_regular_file(p)) { // ファイルなら...
					   //		std::cout << "file: " << p.filename() << std::endl;
					   //	}
					   //	else if (sys::is_directory(p)) { // ディレクトリなら...
					   //		std::cout << "dir.: " << p.string() << std::endl;
					   //	}
					   //});

					   // for文で回して列挙する
	for (std::tr2::sys::directory_iterator it(p), end; it != end; ++it)
	{
		// ファイルだったら
		if (sys::is_regular_file(it->path()))
		{
			// ファイル名の配列に格納
#ifdef _V140_
			out->FileNames.push_back((bExt) ? it->path().filename().string() : it->path().stem().string());	// 三項演算子で拡張子付きかそうでないかで分けている
#else
			out->FileNames.push_back((bExt) ? it->path().filename() : it->path().stem());	// 三項演算子で拡張子付きかそうでないかで分けている	
#endif
		}

		// フォルダーだったら
		else if (sys::is_directory(it->path()))
		{
			// フォルダ名の配列に格納
			out->FolderNames.push_back(it->path().string());
		}
	}
}

std::string tdnFile::GetFileExtention(LPCSTR path)
{
	sys::path p(path);

	// 拡張子を返す
#ifdef _V140_
	return p.extension().string();
#else
	return p.extension();
#endif
}

std::string tdnFile::GetFileName(LPCSTR path, bool bExt)
{
	sys::path p(path);

	// trueなら拡張子付き、falseなら拡張子なしのファイル名を返す
#ifdef _V140_
	return (bExt) ? p.filename().string() : p.stem().string();
#else
	return (bExt) ? p.filename() : p.stem();
#endif
}

std::string tdnFile::OpenFileDialog(LPCSTR filter)
{
	LPCTSTR str_filter = TEXT(filter);

	// ★★★★★★重要なこと
	// ダイアログを開く関数(GetOpenFileName)を使った後にifstream関連の処理を行うともれなく失敗してしまうことが判明、
	// 調べた結果、GetCurrentDirectryとダイアログ後のSetCurrentDirectryを使うと無事解消された
	// あまりに嬉しかったのでURL貼るhttp://stackoverflow.com/questions/20158334/get-open-filename-is-killing-ifstream
	char buffer[255];
	GetCurrentDirectoryA(255, buffer);

	OPENFILENAME of;
	memset(&of, 0, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	// ダイアログボックスを所有するウィンドウへのハンドル(開いてる最中にそのウィンドウをいじられないようにする)
	of.hwndOwner = tdnSystem::GetWindow();
	of.lpstrFilter = TEXT(str_filter);
	// ファイル名を格納したバッファのアドレス
	of.lpstrFile = (LPTSTR)strFile;
	// lpstrFileメンバで指定されるバッファのサイズ
	of.nMaxFile = MAX_PATH;
	// ダイアログボックスの初期化に使用されるビットフラグ
	// OFN_PATHMUSTEXIST:ユーザーが有効なパス
	//  およびファイル名のみを入力できるように指定
	of.Flags = OFN_PATHMUSTEXIST;
	// デフォルトの拡張子を格納したバッファのアドレス
	of.lpstrDefExt = TEXT("txt");
	// コモンダイアログの表示
	if (GetOpenFileName(&of))	// FALSEが返ったらキャンセルが押された
	{
		// ★★★
		SetCurrentDirectory(buffer);

		// strFileにダイアログで選んだパスが格納されている
		return std::string(strFile);
	}

	// ファイルオープンに失敗したので、空の文字列
	return std::string("");
}

std::string tdnFile::SaveFileDialog(LPCSTR filter)
{
	LPCTSTR str_filter = TEXT(filter);

	char buffer[255];
	GetCurrentDirectoryA(255, buffer);

	OPENFILENAME of;
	memset(&of, 0, sizeof(OPENFILENAME));
	of.lStructSize = sizeof(OPENFILENAME);
	// ダイアログボックスを所有するウィンドウへのハンドル(開いてる最中にそのウィンドウをいじられないようにする)
	of.hwndOwner = tdnSystem::GetWindow();
	of.lpstrFilter = TEXT(str_filter);
	// ファイル名を格納したバッファのアドレス
	of.lpstrFile = (LPTSTR)strFile;
	// lpstrFileメンバで指定されるバッファのサイズ
	of.nMaxFile = MAX_PATH;
	// ダイアログボックスの初期化に使用されるビットフラグ
	// OFN_PATHMUSTEXIST:ユーザーが有効なパス
	//  およびファイル名のみを入力できるように指定
	of.Flags = OFN_PATHMUSTEXIST;
	// デフォルトの拡張子を格納したバッファのアドレス
	of.lpstrDefExt = TEXT("txt");
	// コモンダイアログの表示
	if (GetSaveFileName(&of))	// FALSEが返ったらキャンセルが押された
	{
		SetCurrentDirectory(buffer);

		// strFileにダイアログで選んだパスが格納されている
		return std::string(strFile);
	}

	// ファイルオープンに失敗したので、空の文字列
	return std::string("");
}