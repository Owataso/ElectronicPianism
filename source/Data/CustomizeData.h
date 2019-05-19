#pragma once


//===============================================
//	カスタマイズの定数
//===============================================
enum class CUSTOMIZE_TYPE
{
	SKIN,
	NOTE,
	EXPLOSION,
	SUDDEN,
	VOICE,
	SELECT_MUSIC,
	MAX
};

/********************************************/
//	カスタマイズ管理クラス<Singleton>
/********************************************/
class CustomizeDataManager
{
public:
	//===============================================
	//	実体取得
	//===============================================
	static CustomizeDataManager *GetInstance(){ static CustomizeDataManager instance; return &instance; }


	//===============================================
	//	スキンデータの取得
	//===============================================
	std::string GetSkin(CUSTOMIZE_TYPE type, int no);	// 引数番号のスキン取得
	std::string GetSetedSkin(CUSTOMIZE_TYPE type);		// セットされたスキンを取得
	std::string GetSkinName(CUSTOMIZE_TYPE type, int no);
	UINT GetNumSkin(CUSTOMIZE_TYPE type){ return m_vDataList[(int)type].size(); }

	//===============================================
	//	デストラクタ
	//===============================================
	~CustomizeDataManager();

private:
	//===============================================
	//	カスタマイズのデータ
	//===============================================
	struct CustomizeData
	{
		char path[64];		// ファイル名
		char ID[64];		// 名前
	};


	//===============================================
	//	カスタマイズデータのリスト
	//===============================================
	std::vector<CustomizeData*> m_vDataList[(int)CUSTOMIZE_TYPE::MAX];


	//===============================================
	//	データのロード
	//===============================================
	void LoadSettingData();
	void LoadSkin(CUSTOMIZE_TYPE eSkin);


	//===============================================
	//	シングルトンの作法
	//===============================================
	CustomizeDataManager();
	CustomizeDataManager(const CustomizeDataManager&){}
	CustomizeDataManager &operator =(const CustomizeDataManager&);
};

/********************************************/
//	インスタンス簡略化
/********************************************/
#define CustomizeDataMgr CustomizeDataManager::GetInstance()