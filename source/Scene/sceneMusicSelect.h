#pragma once

#include "../Music/MusicInfo.h"
#include "../Window/QuestionWindow.h"
#include "../Effect/KeyboardGuide.h"

class tdnMovie;


/********************************************/
//	曲セレクトシーン
/********************************************/
class sceneMusicSelect : public BaseScene
{
public:
	//------------------------------------------------------
	//	初期化・開放
	//------------------------------------------------------
	bool Initialize();
	~sceneMusicSelect();

	//------------------------------------------------------
	//	更新・描画
	//------------------------------------------------------
	bool Update();
	void Render();

private:

	enum MUSIC_PLAYER{ SELECTION, DECIDE };
	std::unique_ptr<tdnSoundBGM> m_pMusicPlayer;	// 選曲画面の曲

	struct _MusicInfo
	{
		std::string path;
		tdn2DAnim *pMusicTextAnim, *pMusicArtistAnim;
		_MusicInfo(int id) :path("DATA/Musics/EP/No_" + std::to_string(id)), pMusicTextAnim(new tdn2DAnim((char*)(path + "/title.png").c_str())), pMusicArtistAnim(new tdn2DAnim(pMusicTextAnim->GetTexture()))
		{
			// アニメーション画像初期化
			pMusicTextAnim->OrderMoveAppeared(10, 32 - 16, 200 + 16);
			pMusicArtistAnim->OrderMoveAppeared(10, 150 - 16, 296 - 16);
		}
		~_MusicInfo(){ delete pMusicTextAnim; delete pMusicArtistAnim; }
		void UpdateAnim(){ pMusicTextAnim->Update(); pMusicArtistAnim->Update(); }
	};
	//std::string *m_PathList;
	//tdn2DObj **m_pMusicTextImages;	// 左半分に出す曲名のテキスト
	std::map<int, _MusicInfo*> m_MusicInfoMap;

	int m_LimitTime;						// 制限時間
	static const int NOLILMIT_TIME = -99;	// フリープレイとか制限時間なしのとき

	enum class FOLDER_TYPE
	{
		BEGINNER,			// 初心者向け
		ALL,				// 全曲
		ELECTRONIC_PIANISM,	// EP無印
		LEVEL1_5,			// Lv1~5
		LEVEL6_10,			// Lv6~10
		LEVEL11_15,			// Lv11~15
		LEVEL16_20,			// Lv16~20
		LEVEL21_25,			// Lv21~25
		LEVEL26_30,			// Lv26~30
		LEVEL31,			// Lv31
		ORIGINAL,			// オリジナル
		GAME,				// ゲーム音楽
		CLASSIC,			// クラシック
		VOCALOID,			// ボカロ
		POPS,				// アニメとか版権の類
		OTHER,				// その他
		MAX
	};

	std::map<FOLDER_TYPE, LPCSTR> m_FolderTypeMap;	// enum→文字列

	// モード
	class Mode
	{
	public:
		class Base
		{
		protected:
			sceneMusicSelect *pMusicSelect;
			static int m_iSelectMusic;			// 選んでる曲(決定後の曲名とかで使うから)
		public:
			Base(sceneMusicSelect *me) :pMusicSelect(me){}
			virtual ~Base(){}
			virtual void Update(){}
			virtual void Render(){}
			virtual void ShiftDifficulty(){}	// 難易度変更ボタンを押したとき、自分のフォルダーの中身の難易度を変更する
		};

		// フォルダー選択
		class FolderSelect : public Base
		{
		public:
			FolderSelect(sceneMusicSelect *me) :Base(me){ SetNextFolderPos(*me->m_ipSelectFolder, true); }
			void Update()override;
			void Render()override;
			void ShiftDifficulty()override;
		private:
			struct
			{
				Vector2 pos, NextPos;
				void Update(){ pos = pos*.75f + NextPos*.25f; }	// 補間処理
				void PosAssignNext(){ pos = NextPos; }
			}m_FolderPositions[(int)FOLDER_TYPE::MAX];
			void SetNextFolderPos(int cursor, bool bInit)
			{
				// 行の数
				static const int MAX_COLUMN(15);								// 行の最大数
				const int column(min((int)FOLDER_TYPE::MAX, MAX_COLUMN));		// 行の数
				const int ColumnCenter((int)(column * .45f));					// 行の数から真ん中を選出

				// 行の描画
				FOR(column)
				{
					static const int OFFSET_X(780);	// 名前開始地点のX座標
					static const int OFFSET_Y(345);	// 名前開始地点のY座標

					// 自分のカーソルでの前後を取得
					int index(cursor + (i - ColumnCenter));

					// 下に行きすぎたら(画面では上)
					if (index < 0)
					{
						// 全アルファベット分を足すことによって、過剰分をずらせる
						index += (int)FOLDER_TYPE::MAX;
					}

					// 上に行きすぎたら(画面では下)
					else if (index >= (int)FOLDER_TYPE::MAX)
					{
						// 全アルファベット分を足すことによって、過剰分をずらせる
						index -= ((int)FOLDER_TYPE::MAX);
					}

					const int addPosX((index == cursor) ? -20 : 0);
					const int width((ColumnCenter - i) * -40);	// 行の真ん中を基準点として、そこからずれる量(真ん中ならもちろん0)
					const int posY(OFFSET_Y + width);
					m_FolderPositions[index].NextPos.x = (float)(OFFSET_X + addPosX);
					m_FolderPositions[index].NextPos.y = (float)posY/* + addPosY*/;
					
					// 初期化、もしくは画面外だったら、補間を掛けず、そのまま代入
					if (bInit)
					{
						m_FolderPositions[index].PosAssignNext();	// pos=NextPos
					}
					else if (i == column - 1) m_FolderPositions[index].pos.y = column * 42.0f;	// 下の描画順をごまかす(結構この1文が効いてる)
				}
			}
		};

		// 曲選択
		class MusicSelect : public Base
		{
		public:
			MusicSelect(sceneMusicSelect *me);
			~MusicSelect();
			void Update()override;
			void Render()override;
			void RenderMusicTitle(int id);
			void RenderBPM();
			void RenderScore(int id);
			void RenderMusicList();
			void ShiftDifficulty()override;
			void SetNextMusicPos(int cursor, bool bInit)
			{
				static const int OFFSET_X(780);	// 名前開始地点のX座標
				static const int OFFSET_Y(345);	// 名前開始地点のY座標

				// 行の数
				const UINT MAX_COLUMN(15);										// 行の最大数
				const int column(min(pMusicSelect->m_pSelectFolderInfo->size(), MAX_COLUMN));	// 行の数
				const int ColumnCenter((int)(column * .45f));					// 行の数から真ん中を選出

				FOR((int)pMusicSelect->m_pSelectFolderInfo->size())
				{
					// 自分のカーソルでの前後を取得
					int index(cursor + (i - ColumnCenter));

					// 下に行きすぎたら(画面では上)
					if (index < 0)
					{
						// 全アルファベット分を足すことによって、過剰分をずらせる
						index += (int)pMusicSelect->m_pSelectFolderInfo->size();
					}

					// 上に行きすぎたら(画面では下)
					else if (index >= (int)pMusicSelect->m_pSelectFolderInfo->size())
					{
						// 全アルファベット分を足すことによって、過剰分をずらせる
						index -= ((int)pMusicSelect->m_pSelectFolderInfo->size());
					}

					if (i < column)
					{
						const int addPosX((index == cursor) ? -20 : 0);
						const int width((ColumnCenter - i) * -40);	// 行の真ん中を基準点として、そこからずれる量(真ん中ならもちろん0)
						const int posY(OFFSET_Y + width);
						pMusicSelect->m_pSelectFolderInfo->at(index)->NextPos.x = (float)(OFFSET_X + addPosX);
						pMusicSelect->m_pSelectFolderInfo->at(index)->NextPos.y = (float)posY;

						// 初期化、もしくは画面外だったら、補間を掛けず、そのまま代入
						if (bInit)
						{
							pMusicSelect->m_pSelectFolderInfo->at(index)->PosAssignNext();	// pos=NextPos
						}
						else if (i == column - 1) pMusicSelect->m_pSelectFolderInfo->at(index)->pos.y = column * 42.0f;	// 下の描画順をごまかす(結構この1文が効いてる)
					}
					else  pMusicSelect->m_pSelectFolderInfo->at(index)->NextPos.y = 240;	// 描画外のやつは一律で真ん中上あたりに置く
				}
			}

		private:
			tdnStreamSound *m_pStreamAuditionPlayer;	// 試聴曲再生用
			int m_AuditionDelayTime;	// 曲を選んで、少しだけ流さない時間(選んだ瞬間に流れたら不自然)
			bool m_bAuditionPlay;		// 曲流してるか
			int m_iPrevOctave;
		};

		// 選択し終わったやつ
		class DecideMusic : public Base
		{
		public:
			DecideMusic(sceneMusicSelect *me);
			~DecideMusic();
			void Update()override;
			void Render()override;
		private:

			static const int c_RippleTriggerFrame = 180;	// リップル発動するフレーム
			static const int c_NumRipple = 3;				// リップル重ね掛けする数
			int m_frame;									// 経過時間計測用
			std::unique_ptr<tdn2DObj> m_pScreen;			// 描画画面に対してポストエフェクト(波紋？)をかけるために使う
			Surface *m_surface;								// バックバッファ用
			std::unique_ptr<tdn2DAnim> m_pScreenRipples[c_NumRipple];		// 画面にかける波紋用

			// 曲名
			class MusicTitleAnim
			{
			public:
				MusicTitleAnim(Texture2D *texture) :pImage(new tdn2DObj(texture)), pUpRipple(new tdn2DAnim(texture)), pMoveArtistName(new tdn2DAnim(texture))
				{
					FOR(2)
					{
						pDownRipples[i] = new tdn2DAnim(texture);
						pDownRipples[i]->OrderRipple(c_RippleFrame, 2.5f - i*.25f, -.03f - i*.01f);
						pDownRipples[i]->Action(5 - i * 2);
					}
					pUpRipple->OrderRipple(c_RippleFrame, 1.0f, .035f);
					pUpRipple->Action();
					pMoveArtistName->OrderMoveAppeared(20, 290, 426);
					pMoveArtistName->Action(c_RippleFrame);
				}
				~MusicTitleAnim(){ delete pImage; delete pUpRipple; POINTER_ARRAY_DELETE(pDownRipples,2) delete pMoveArtistName; }
				void Update(int frame)
				{
					frame = min(c_RippleFrame, frame);
					static const float MaxScale(.5f);						// 1.0f+MaxScaleが最大値(選曲した段階が一番でかい)
					const float rate((float)frame / c_RippleFrame);			// リップル発動を1とする割合
					pImage->SetScale(1.0f + (MaxScale - rate * MaxScale));
					FOR(2)pDownRipples[i]->Update();
					pUpRipple->Update();
					pMoveArtistName->Update();
				}
				void Render()
				{
					pMoveArtistName->Render(290, 300 + 110, 700, 38, 0, 90, 700, 38);
					pImage->Render(290, 300, 700, 90, 0, 0, 700, 90);
					FOR(2)pDownRipples[i]->Render(290, 300, 700, 90, 0, 0, 700, 90, RS::ADD);
					pUpRipple->Render(290, 300, 700, 90, 0, 0, 700, 90, RS::ADD);
				}
			private:
				static const int c_RippleFrame = 40;
				tdn2DObj *pImage;		// 曲名テキストイメージ
				tdn2DAnim *pUpRipple;	// 広がっていく波紋
				tdn2DAnim *pDownRipples[2];	// 縮まっていく波紋
				tdn2DAnim *pMoveArtistName;	// 動くアーティスト名
			}*m_pMusicTitleAnim;
		};
	};

	enum class MODE{ SELECT_FOLDER, SELECT_MUSIC, DECIDE_MUSIC };

	// モード委譲
	//Mode::Base *m_pMode;

	// フォルダーを選んだ時にその選んだフォルダーに応じてソート・絞り込みする関数たちと、それの関数をまとめる関数ポインタ
	class MusicSort	// namespace的な扱い
	{
	public:
		class Base
		{
		protected:
			// 引数に最少と最大をもらってレベルでソートする関数
			void LevelSort(int Min, int Max, sceneMusicSelect *pMusicSelect);

			// 引数にジャンルをもらってジャンルをソートする関数
			void GenreSort(MUSIC_GENRE genre, sceneMusicSelect *pMusicSelect);
		public:
			// ソートする関数
			virtual void Funk(sceneMusicSelect *pMusicSelect) = 0;
		};

		class Beginner : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class All : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};
		
		class ElectronicPianism : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Level1_5 : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Level6_10 : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Level11_15 : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Level16_20 : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Level21_25 : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Level26_30 : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Level31 : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Original : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Game : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Classic : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Vocaloid : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Pops : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};

		class Other : public Base
		{
		public:
			void Funk(sceneMusicSelect *pMusicSelect);
		};
	};
	std::unique_ptr<MusicSort::Base> m_pSort[(int)FOLDER_TYPE::MAX];

	struct SelectMusicInfo
	{
		FolderInfo *info;			// 曲情報
		int difficulty;				// 難易度
		Vector2 pos, NextPos;
		bool bExitFolder;			// フォルダー抜ける用
		SelectMusicInfo(FolderInfo *info, DIFFICULTY dif, bool bExitFolder) :info(info), difficulty((int)dif), bExitFolder(bExitFolder), pos(780, 0), NextPos(780, 0){}
		SelectMusicInfo(){}
		void UpdatePotision(){ pos = pos*.75f + NextPos*.25f; }	// 補間処理
		void PosAssignNext(){ pos = NextPos; }
	};

	// 前回の情報保存用
	class SelectManager
	{
	public:
		static SelectManager *GetInstance(){ static SelectManager i; return &i; }
		~SelectManager()
		{
			for (auto it : m_SelectFolderInfo) delete it;
			SAFE_DELETE(m_pMode);
		}
		void ModeUpdate(){ m_pMode->Update(); }
		void ModeRender(){ m_pMode->Render(); }

		void ShiftDifficulty(){ m_pMode->ShiftDifficulty(); }
		int *GetSelectDifAddress(){ return &m_iSelectDifficulty; }
		int *GetSelectFolderAddress(){ return &m_iSelectFolder; }
		std::vector<SelectMusicInfo*> *GetSelectFolderInfoAddress(){ return &m_SelectFolderInfo; }

		bool isEmpty(){ return m_SelectFolderInfo.empty(); }

		void ChangeMode(MODE m, sceneMusicSelect *pMusicSelect)
		{
			if (m_pMode) delete m_pMode;
			switch (m)
			{
			case MODE::SELECT_FOLDER:
				m_pMode = new Mode::FolderSelect(pMusicSelect);
				break;

			case MODE::SELECT_MUSIC:
				m_pMode = new Mode::MusicSelect(pMusicSelect);
				break;

			case MODE::DECIDE_MUSIC:
				m_pMode = new Mode::DecideMusic(pMusicSelect);
				break;
			}
		}
		void ModeRelease(){ SAFE_DELETE(m_pMode); }
	private:
		int m_iSelectDifficulty;	// 選んでる難易度
		int m_iSelectFolder;		// 選んでるフォルダー
		std::vector<SelectMusicInfo*> m_SelectFolderInfo;
		Mode::Base *m_pMode;		// モード委譲

		SelectManager() :m_iSelectDifficulty(0), m_iSelectFolder(0), m_pMode(nullptr){ m_SelectFolderInfo.clear(); }
		SelectManager(const SelectManager&){}
		SelectManager &operator=(const SelectManager&){}
	};

	// フォルダー選んだ時の中身用(ソートしたり絞り込んだりする)
	std::vector<SelectMusicInfo*> *m_pSelectFolderInfo;	// (シングルトンのデータを参照・書き換え)
	FolderInfo *GetSelectMusicInfo(int SelectMusicNo){ return m_pSelectFolderInfo->at(SelectMusicNo)->info; }
	bool isExitFolder(int index){ return m_pSelectFolderInfo->at(index)->bExitFolder; }
	void Push(FolderInfo *info, DIFFICULTY dif, bool bExitFolder = false);

	int GetFolderHosokuType()
	{
		switch ((FOLDER_TYPE)*m_ipSelectFolder)
		{
		case sceneMusicSelect::FOLDER_TYPE::ALL:
		case sceneMusicSelect::FOLDER_TYPE::ELECTRONIC_PIANISM:
			return 0;	// バージョン別選択
			break;
		case sceneMusicSelect::FOLDER_TYPE::BEGINNER:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL1_5:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL6_10:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL11_15:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL16_20:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL21_25:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL26_30:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL31:
			return 1;	// レベル別選択
			break;
		case sceneMusicSelect::FOLDER_TYPE::ORIGINAL:
		case sceneMusicSelect::FOLDER_TYPE::GAME:
		case sceneMusicSelect::FOLDER_TYPE::CLASSIC:
		case sceneMusicSelect::FOLDER_TYPE::VOCALOID:
		case sceneMusicSelect::FOLDER_TYPE::POPS:
		case sceneMusicSelect::FOLDER_TYPE::OTHER:
			return 2;	// ジャンル別選択
			break;
		}
		assert(0);
		return 0;
	}
	int GetSelectMusicID(int iSelectMusic)
	{
		return m_pSelectFolderInfo->at(iSelectMusic)->info->byID;
	}

	void ClearFolderInfo();		// フォルダーの中身情報消去

	int m_WaitTimer;			// このシーンに入ってからアニメ終わるまでのだいたいの待機時間
	int *m_ipSelectDifficulty;	// 選んでる難易度(シングルトンのデータを参照・書き換え)
	float m_DifficultyFrameX;	// 選んでる曲の難易度の補間用
	int *m_ipSelectFolder;		// 選んでるフォルダー(シングルトンのデータを参照・書き換え)
	int m_LampAnimFrame;		// ランプアニメーションフレーム
	int m_LampAnimPanel;		// ランプアニメーションのコマ

	bool m_bDecision;			// 曲決定している。
	bool m_bOption;				// オプションを開いてるかどうか
	float m_CurrentOptionX, m_NextOptionX;	// オプションの座標
	void UpdateOption();		// オプション更新
	void RenderOption();		// オプション描画
	void RenderGuide();			// ガイド描画

	std::unique_ptr<QuestionWindow> m_pQuestionWindow;	// 質問枠用
	std::unique_ptr<KeyboardGuide> m_pKeyboardGuide;	// ガイド

	enum IMAGE
	{
		BACK,	// 背景
		MUSIC_FRAME,	// 曲名フレーム
		NUMBER,		// ナンバー
		NUMBER_L,	// Lサイズナンバー
		LAMP,		// クリアランプ
		BPM_FONT,		// BPMのフォント
		OPTION,			// オプション枠
		OPTION_CURSOR,	// オプション選択のやつ
		OCTAVE_BACK,	// オクターブ幅画像
		MAX
	};
	enum class ANIM_IMAGE
	{
		MIDASHI,	// MUSIC_SELECT
		STAGE,	// ○○stage的な
		UP_FRAME,	// 上のフレーム
		SELECT_DIFFICULTY,	// 選んでる難易度のフレーム
		DIF_FRAME,		// 難易度フレーム
		FOLDER_TEXT,	// 左半分に出すフォルダー情報のテキスト
		FOLDER_HOSOKU,	// フォルダーの補足
		RANK,			// ランク
		SCORE_FRAME,	// スコアフレーム
		CLEAR_TEXT,		// クリアのテキスト
		OCTAVE,			// オクターブ幅画像
		MAX
	};
	std::unique_ptr<tdn2DObj>  m_pImages[IMAGE::MAX];				// 画像
	std::unique_ptr<tdn2DAnim> m_pAnimImages[(int)ANIM_IMAGE::MAX];	// アニメーションする画像

	// 背景動画
	tdnMovie *m_pBackMovie;
};