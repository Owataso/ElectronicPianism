#pragma once

#include "../Music/MusicInfo.h"
#include "../Window/QuestionWindow.h"
#include "../Effect/KeyboardGuide.h"

class tdnMovie;


/********************************************/
//	�ȃZ���N�g�V�[��
/********************************************/
class sceneMusicSelect : public BaseScene
{
public:
	//------------------------------------------------------
	//	�������E�J��
	//------------------------------------------------------
	bool Initialize();
	~sceneMusicSelect();

	//------------------------------------------------------
	//	�X�V�E�`��
	//------------------------------------------------------
	bool Update();
	void Render();

private:

	enum MUSIC_PLAYER{ SELECTION, DECIDE };
	std::unique_ptr<tdnSoundBGM> m_pMusicPlayer;	// �I�ȉ�ʂ̋�

	struct _MusicInfo
	{
		std::string path;
		tdn2DAnim *pMusicTextAnim, *pMusicArtistAnim;
		_MusicInfo(int id) :path("DATA/Musics/EP/No_" + std::to_string(id)), pMusicTextAnim(new tdn2DAnim((char*)(path + "/title.png").c_str())), pMusicArtistAnim(new tdn2DAnim(pMusicTextAnim->GetTexture()))
		{
			// �A�j���[�V�����摜������
			pMusicTextAnim->OrderMoveAppeared(10, 32 - 16, 200 + 16);
			pMusicArtistAnim->OrderMoveAppeared(10, 150 - 16, 296 - 16);
		}
		~_MusicInfo(){ delete pMusicTextAnim; delete pMusicArtistAnim; }
		void UpdateAnim(){ pMusicTextAnim->Update(); pMusicArtistAnim->Update(); }
	};
	//std::string *m_PathList;
	//tdn2DObj **m_pMusicTextImages;	// �������ɏo���Ȗ��̃e�L�X�g
	std::map<int, _MusicInfo*> m_MusicInfoMap;

	int m_LimitTime;						// ��������
	static const int NOLILMIT_TIME = -99;	// �t���[�v���C�Ƃ��������ԂȂ��̂Ƃ�

	enum class FOLDER_TYPE
	{
		BEGINNER,			// ���S�Ҍ���
		ALL,				// �S��
		ELECTRONIC_PIANISM,	// EP����
		LEVEL1_5,			// Lv1~5
		LEVEL6_10,			// Lv6~10
		LEVEL11_15,			// Lv11~15
		LEVEL16_20,			// Lv16~20
		LEVEL21_25,			// Lv21~25
		LEVEL26_30,			// Lv26~30
		LEVEL31,			// Lv31
		ORIGINAL,			// �I���W�i��
		GAME,				// �Q�[�����y
		CLASSIC,			// �N���V�b�N
		VOCALOID,			// �{�J��
		POPS,				// �A�j���Ƃ��Ō��̗�
		OTHER,				// ���̑�
		MAX
	};

	std::map<FOLDER_TYPE, LPCSTR> m_FolderTypeMap;	// enum��������

	// ���[�h
	class Mode
	{
	public:
		class Base
		{
		protected:
			sceneMusicSelect *pMusicSelect;
			static int m_iSelectMusic;			// �I��ł��(�����̋Ȗ��Ƃ��Ŏg������)
		public:
			Base(sceneMusicSelect *me) :pMusicSelect(me){}
			virtual ~Base(){}
			virtual void Update(){}
			virtual void Render(){}
			virtual void ShiftDifficulty(){}	// ��Փx�ύX�{�^�����������Ƃ��A�����̃t�H���_�[�̒��g�̓�Փx��ύX����
		};

		// �t�H���_�[�I��
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
				void Update(){ pos = pos*.75f + NextPos*.25f; }	// ��ԏ���
				void PosAssignNext(){ pos = NextPos; }
			}m_FolderPositions[(int)FOLDER_TYPE::MAX];
			void SetNextFolderPos(int cursor, bool bInit)
			{
				// �s�̐�
				static const int MAX_COLUMN(15);								// �s�̍ő吔
				const int column(min((int)FOLDER_TYPE::MAX, MAX_COLUMN));		// �s�̐�
				const int ColumnCenter((int)(column * .45f));					// �s�̐�����^�񒆂�I�o

				// �s�̕`��
				FOR(column)
				{
					static const int OFFSET_X(780);	// ���O�J�n�n�_��X���W
					static const int OFFSET_Y(345);	// ���O�J�n�n�_��Y���W

					// �����̃J�[�\���ł̑O����擾
					int index(cursor + (i - ColumnCenter));

					// ���ɍs����������(��ʂł͏�)
					if (index < 0)
					{
						// �S�A���t�@�x�b�g���𑫂����Ƃɂ���āA�ߏ蕪�����点��
						index += (int)FOLDER_TYPE::MAX;
					}

					// ��ɍs����������(��ʂł͉�)
					else if (index >= (int)FOLDER_TYPE::MAX)
					{
						// �S�A���t�@�x�b�g���𑫂����Ƃɂ���āA�ߏ蕪�����点��
						index -= ((int)FOLDER_TYPE::MAX);
					}

					const int addPosX((index == cursor) ? -20 : 0);
					const int width((ColumnCenter - i) * -40);	// �s�̐^�񒆂���_�Ƃ��āA�������炸����(�^�񒆂Ȃ�������0)
					const int posY(OFFSET_Y + width);
					m_FolderPositions[index].NextPos.x = (float)(OFFSET_X + addPosX);
					m_FolderPositions[index].NextPos.y = (float)posY/* + addPosY*/;
					
					// �������A�������͉�ʊO��������A��Ԃ��|�����A���̂܂ܑ��
					if (bInit)
					{
						m_FolderPositions[index].PosAssignNext();	// pos=NextPos
					}
					else if (i == column - 1) m_FolderPositions[index].pos.y = column * 42.0f;	// ���̕`�揇�����܂���(���\����1���������Ă�)
				}
			}
		};

		// �ȑI��
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
				static const int OFFSET_X(780);	// ���O�J�n�n�_��X���W
				static const int OFFSET_Y(345);	// ���O�J�n�n�_��Y���W

				// �s�̐�
				const UINT MAX_COLUMN(15);										// �s�̍ő吔
				const int column(min(pMusicSelect->m_pSelectFolderInfo->size(), MAX_COLUMN));	// �s�̐�
				const int ColumnCenter((int)(column * .45f));					// �s�̐�����^�񒆂�I�o

				FOR((int)pMusicSelect->m_pSelectFolderInfo->size())
				{
					// �����̃J�[�\���ł̑O����擾
					int index(cursor + (i - ColumnCenter));

					// ���ɍs����������(��ʂł͏�)
					if (index < 0)
					{
						// �S�A���t�@�x�b�g���𑫂����Ƃɂ���āA�ߏ蕪�����点��
						index += (int)pMusicSelect->m_pSelectFolderInfo->size();
					}

					// ��ɍs����������(��ʂł͉�)
					else if (index >= (int)pMusicSelect->m_pSelectFolderInfo->size())
					{
						// �S�A���t�@�x�b�g���𑫂����Ƃɂ���āA�ߏ蕪�����点��
						index -= ((int)pMusicSelect->m_pSelectFolderInfo->size());
					}

					if (i < column)
					{
						const int addPosX((index == cursor) ? -20 : 0);
						const int width((ColumnCenter - i) * -40);	// �s�̐^�񒆂���_�Ƃ��āA�������炸����(�^�񒆂Ȃ�������0)
						const int posY(OFFSET_Y + width);
						pMusicSelect->m_pSelectFolderInfo->at(index)->NextPos.x = (float)(OFFSET_X + addPosX);
						pMusicSelect->m_pSelectFolderInfo->at(index)->NextPos.y = (float)posY;

						// �������A�������͉�ʊO��������A��Ԃ��|�����A���̂܂ܑ��
						if (bInit)
						{
							pMusicSelect->m_pSelectFolderInfo->at(index)->PosAssignNext();	// pos=NextPos
						}
						else if (i == column - 1) pMusicSelect->m_pSelectFolderInfo->at(index)->pos.y = column * 42.0f;	// ���̕`�揇�����܂���(���\����1���������Ă�)
					}
					else  pMusicSelect->m_pSelectFolderInfo->at(index)->NextPos.y = 240;	// �`��O�̂�͈ꗥ�Ő^�񒆏゠����ɒu��
				}
			}

		private:
			tdnStreamSound *m_pStreamAuditionPlayer;	// �����ȍĐ��p
			int m_AuditionDelayTime;	// �Ȃ�I��ŁA�������������Ȃ�����(�I�񂾏u�Ԃɗ��ꂽ��s���R)
			bool m_bAuditionPlay;		// �ȗ����Ă邩
			int m_iPrevOctave;
		};

		// �I�����I��������
		class DecideMusic : public Base
		{
		public:
			DecideMusic(sceneMusicSelect *me);
			~DecideMusic();
			void Update()override;
			void Render()override;
		private:

			static const int c_RippleTriggerFrame = 180;	// ���b�v����������t���[��
			static const int c_NumRipple = 3;				// ���b�v���d�ˊ|�����鐔
			int m_frame;									// �o�ߎ��Ԍv���p
			std::unique_ptr<tdn2DObj> m_pScreen;			// �`���ʂɑ΂��ă|�X�g�G�t�F�N�g(�g��H)�������邽�߂Ɏg��
			Surface *m_surface;								// �o�b�N�o�b�t�@�p
			std::unique_ptr<tdn2DAnim> m_pScreenRipples[c_NumRipple];		// ��ʂɂ�����g��p

			// �Ȗ�
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
					static const float MaxScale(.5f);						// 1.0f+MaxScale���ő�l(�I�Ȃ����i�K����Ԃł���)
					const float rate((float)frame / c_RippleFrame);			// ���b�v��������1�Ƃ��銄��
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
				tdn2DObj *pImage;		// �Ȗ��e�L�X�g�C���[�W
				tdn2DAnim *pUpRipple;	// �L�����Ă����g��
				tdn2DAnim *pDownRipples[2];	// �k�܂��Ă����g��
				tdn2DAnim *pMoveArtistName;	// �����A�[�e�B�X�g��
			}*m_pMusicTitleAnim;
		};
	};

	enum class MODE{ SELECT_FOLDER, SELECT_MUSIC, DECIDE_MUSIC };

	// ���[�h�Ϗ�
	//Mode::Base *m_pMode;

	// �t�H���_�[��I�񂾎��ɂ��̑I�񂾃t�H���_�[�ɉ����ă\�[�g�E�i�荞�݂���֐������ƁA����̊֐����܂Ƃ߂�֐��|�C���^
	class MusicSort	// namespace�I�Ȉ���
	{
	public:
		class Base
		{
		protected:
			// �����ɍŏ��ƍő��������ă��x���Ń\�[�g����֐�
			void LevelSort(int Min, int Max, sceneMusicSelect *pMusicSelect);

			// �����ɃW��������������ăW���������\�[�g����֐�
			void GenreSort(MUSIC_GENRE genre, sceneMusicSelect *pMusicSelect);
		public:
			// �\�[�g����֐�
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
		FolderInfo *info;			// �ȏ��
		int difficulty;				// ��Փx
		Vector2 pos, NextPos;
		bool bExitFolder;			// �t�H���_�[������p
		SelectMusicInfo(FolderInfo *info, DIFFICULTY dif, bool bExitFolder) :info(info), difficulty((int)dif), bExitFolder(bExitFolder), pos(780, 0), NextPos(780, 0){}
		SelectMusicInfo(){}
		void UpdatePotision(){ pos = pos*.75f + NextPos*.25f; }	// ��ԏ���
		void PosAssignNext(){ pos = NextPos; }
	};

	// �O��̏��ۑ��p
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
		int m_iSelectDifficulty;	// �I��ł��Փx
		int m_iSelectFolder;		// �I��ł�t�H���_�[
		std::vector<SelectMusicInfo*> m_SelectFolderInfo;
		Mode::Base *m_pMode;		// ���[�h�Ϗ�

		SelectManager() :m_iSelectDifficulty(0), m_iSelectFolder(0), m_pMode(nullptr){ m_SelectFolderInfo.clear(); }
		SelectManager(const SelectManager&){}
		SelectManager &operator=(const SelectManager&){}
	};

	// �t�H���_�[�I�񂾎��̒��g�p(�\�[�g������i�荞�񂾂肷��)
	std::vector<SelectMusicInfo*> *m_pSelectFolderInfo;	// (�V���O���g���̃f�[�^���Q�ƁE��������)
	FolderInfo *GetSelectMusicInfo(int SelectMusicNo){ return m_pSelectFolderInfo->at(SelectMusicNo)->info; }
	bool isExitFolder(int index){ return m_pSelectFolderInfo->at(index)->bExitFolder; }
	void Push(FolderInfo *info, DIFFICULTY dif, bool bExitFolder = false);

	int GetFolderHosokuType()
	{
		switch ((FOLDER_TYPE)*m_ipSelectFolder)
		{
		case sceneMusicSelect::FOLDER_TYPE::ALL:
		case sceneMusicSelect::FOLDER_TYPE::ELECTRONIC_PIANISM:
			return 0;	// �o�[�W�����ʑI��
			break;
		case sceneMusicSelect::FOLDER_TYPE::BEGINNER:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL1_5:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL6_10:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL11_15:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL16_20:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL21_25:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL26_30:
		case sceneMusicSelect::FOLDER_TYPE::LEVEL31:
			return 1;	// ���x���ʑI��
			break;
		case sceneMusicSelect::FOLDER_TYPE::ORIGINAL:
		case sceneMusicSelect::FOLDER_TYPE::GAME:
		case sceneMusicSelect::FOLDER_TYPE::CLASSIC:
		case sceneMusicSelect::FOLDER_TYPE::VOCALOID:
		case sceneMusicSelect::FOLDER_TYPE::POPS:
		case sceneMusicSelect::FOLDER_TYPE::OTHER:
			return 2;	// �W�������ʑI��
			break;
		}
		assert(0);
		return 0;
	}
	int GetSelectMusicID(int iSelectMusic)
	{
		return m_pSelectFolderInfo->at(iSelectMusic)->info->byID;
	}

	void ClearFolderInfo();		// �t�H���_�[�̒��g������

	int m_WaitTimer;			// ���̃V�[���ɓ����Ă���A�j���I���܂ł̂��������̑ҋ@����
	int *m_ipSelectDifficulty;	// �I��ł��Փx(�V���O���g���̃f�[�^���Q�ƁE��������)
	float m_DifficultyFrameX;	// �I��ł�Ȃ̓�Փx�̕�ԗp
	int *m_ipSelectFolder;		// �I��ł�t�H���_�[(�V���O���g���̃f�[�^���Q�ƁE��������)
	int m_LampAnimFrame;		// �����v�A�j���[�V�����t���[��
	int m_LampAnimPanel;		// �����v�A�j���[�V�����̃R�}

	bool m_bDecision;			// �Ȍ��肵�Ă���B
	bool m_bOption;				// �I�v�V�������J���Ă邩�ǂ���
	float m_CurrentOptionX, m_NextOptionX;	// �I�v�V�����̍��W
	void UpdateOption();		// �I�v�V�����X�V
	void RenderOption();		// �I�v�V�����`��
	void RenderGuide();			// �K�C�h�`��

	std::unique_ptr<QuestionWindow> m_pQuestionWindow;	// ����g�p
	std::unique_ptr<KeyboardGuide> m_pKeyboardGuide;	// �K�C�h

	enum IMAGE
	{
		BACK,	// �w�i
		MUSIC_FRAME,	// �Ȗ��t���[��
		NUMBER,		// �i���o�[
		NUMBER_L,	// L�T�C�Y�i���o�[
		LAMP,		// �N���A�����v
		BPM_FONT,		// BPM�̃t�H���g
		OPTION,			// �I�v�V�����g
		OPTION_CURSOR,	// �I�v�V�����I���̂��
		OCTAVE_BACK,	// �I�N�^�[�u���摜
		MAX
	};
	enum class ANIM_IMAGE
	{
		MIDASHI,	// MUSIC_SELECT
		STAGE,	// ����stage�I��
		UP_FRAME,	// ��̃t���[��
		SELECT_DIFFICULTY,	// �I��ł��Փx�̃t���[��
		DIF_FRAME,		// ��Փx�t���[��
		FOLDER_TEXT,	// �������ɏo���t�H���_�[���̃e�L�X�g
		FOLDER_HOSOKU,	// �t�H���_�[�̕⑫
		RANK,			// �����N
		SCORE_FRAME,	// �X�R�A�t���[��
		CLEAR_TEXT,		// �N���A�̃e�L�X�g
		OCTAVE,			// �I�N�^�[�u���摜
		MAX
	};
	std::unique_ptr<tdn2DObj>  m_pImages[IMAGE::MAX];				// �摜
	std::unique_ptr<tdn2DAnim> m_pAnimImages[(int)ANIM_IMAGE::MAX];	// �A�j���[�V��������摜

	// �w�i����
	tdnMovie *m_pBackMovie;
};