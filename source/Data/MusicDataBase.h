#pragma once

//*****************************************************************************
//
//		全ての曲情報を管理するデータベース<Singleton>
//
//*****************************************************************************

class FolderInfo;
class EditFolderInfo;

/********************************************/
//	全ての曲情報を管理するデータベース<Singleton>
/********************************************/
class MusicDataBase
{
public:
	//===============================================
	//	実体取得
	//===============================================
	static MusicDataBase *GetInstance(){ static MusicDataBase instance; return &instance; }

	//===============================================
	//	コンストラクタ・デストラクタ
	//===============================================
	MusicDataBase();
	~MusicDataBase();

	//===============================================
	//	曲の情報の取得
	//===============================================
	FolderInfo *GetEPContentsData(int no);
	EditFolderInfo *GetEditContentsData(int no);


	//===============================================
	//	ディレクトリの取得
	//===============================================
	DirectoryInfo *GetEPDirectory(){ return m_EPDirectory; }
	DirectoryInfo *GetEditDirectory(){ return m_EditDirectry; }


	//===============================================
	//	曲数の取得
	//===============================================
	int GetNumEPContents();
	int GetNumEditContents();

private:
	//===============================================
	//	ElectronicPianism収録曲データ
	//===============================================
	FolderInfo **m_EPContentsData;
	DirectoryInfo *m_EPDirectory;
	void LoadEPContents();

	//===============================================
	//	Editモードで作られた曲データ
	//===============================================
	EditFolderInfo **m_EditContentsData;
	DirectoryInfo *m_EditDirectry;
	void LoadEditContents();

};

/********************************************/
//	インスタンス簡略化
/********************************************/
#define MUSIC_DATA_BASE (MusicDataBase::GetInstance())

/*
ソートする関数
int cmp(const void *x, const void *y){
return(strcmp(*(char**)x,*(char**)y));
}
void main(void){
char *data[]={ "AKIRA","AKIKO","AIKOSANN" };
char *wk;
int i;

printf("before:\n");
for(i=0;i<3;i++)
printf("%s\n",data[i]);
qsort(data,sizeof(data)/sizeof(char*),sizeof(char*),cmp);
printf("after:\n");
for(i=0;i<3;i++)
printf("%s\n",data[i]);
}
*/