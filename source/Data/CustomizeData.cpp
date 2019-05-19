#include "TDNLIB.h"
#include "PlayerData.h"
#include "CustomizeData.h"


//------------------------------------------------------
//	初期化
//------------------------------------------------------
CustomizeDataManager::CustomizeDataManager()
{
	FOR((int)CUSTOMIZE_TYPE::MAX)
	{
		m_vDataList[i].clear();
		LoadSkin((CUSTOMIZE_TYPE)i);		// スキン情報全部ロード
	}

	// ユーザーの設定したスキンに設定
	//LoadSettingData();	// プレイヤーデーたでやる。

	//bChange = false;
}


//------------------------------------------------------
//	解放
//------------------------------------------------------
CustomizeDataManager::~CustomizeDataManager()
{
	// ユーザーの設定したスキンに上書き
	//if(bChange)SaveSettingData("DATA/Customize/Setting.txt");

	for (int i = 0; i < (int)CUSTOMIZE_TYPE::MAX; i++)
	{
		for (unsigned int j = 0; j < m_vDataList[i].size(); j++) delete m_vDataList[i][j];
		m_vDataList[i].clear();
	}
}

//------------------------------------------------------
//	セッティングデータ読み込み
//------------------------------------------------------
void CustomizeDataManager::LoadSettingData()
{
	// 読み込みくん
	//std::ifstream ifs(filename);
	//
	//for (int i = 0; i < (int)CUSTOMIZE_TYPE::MAX; i++)
	//{
	//	char work[64];
	//	ifs >> work;	// 読み飛ばし
	//
	//	// ユーザーの設定したID
	//	char ID[64];
	//	ifs >> ID;
	//
	//	for (unsigned int no = 0; no < m_vDataList[i].size(); no++)
	//	{
	//		// 同一IDチェック
	//		if (strcmp(m_vDataList[i][no]->ID, ID) == 0)
	//		{
	//			skin_no[i] = no;
	//			break;
	//		}
	//	}
	//
	//}

	//FOR((int)CUSTOMIZE_TYPE::MAX)
	//{
	//	skin_no[i] = PlayerDataMgr->m_CustomizeRecords[i]->no;
	//}
}

void CustomizeDataManager::LoadSkin(CUSTOMIZE_TYPE skin)
{
	static const char *info_path[(int)CUSTOMIZE_TYPE::MAX] =
	{
		"DATA/Customize/Skin/",
		"DATA/Customize/Note/",
		"DATA/Customize/Explosion/",
		"DATA/Customize/SUDDEN/",
		"DATA/Customize/Voice/",
		"DATA/Customize/SelectMusic/"
	};

	// 情報書いてるテキストへのファイルパス作成
	char file_path[64];
	sprintf_s(file_path, 64, "%s%s", info_path[(int)skin], "Info.txt");

	// 読み込みくん
	std::ifstream ifs2(file_path);

	while (!ifs2.eof())
	{
		CustomizeData *set = new CustomizeData();
		ifs2 >> set->ID;

		// スキンの情報ロード
		char name[64];
		ifs2 >> name;
		sprintf_s(set->path, 64, "%s%s", info_path[(int)skin], name);

		//if (skin == CUSTOMIZE_TYPE::EXPLOSION)
		//{
		//	// 爆発エフェクト独自の情報を読み込み
		//	ifs2 >> set->val1;
		//}

		// プッシュ
		m_vDataList[(int)skin].push_back(set);
	}
}



//------------------------------------------------------
//	情報取得
//------------------------------------------------------
std::string CustomizeDataManager::GetSkin(CUSTOMIZE_TYPE type, int no)
{
	return std::string(m_vDataList[(int)type][no]->path);
}
std::string CustomizeDataManager::GetSetedSkin(CUSTOMIZE_TYPE type)
{
	return std::string(m_vDataList[(int)type][PlayerDataMgr->m_CustomizeRecords[(int)type]->no]->path);
}
std::string CustomizeDataManager::GetSkinName(CUSTOMIZE_TYPE type, int no)
{
	return std::string(m_vDataList[(int)type][no]->ID);
}