#include<iostream>
#include<Windows.h>
#include<time.h>

class Checker
{
public:
	Checker() :m_1(0){}
	void Update()
	{
		int SpaceFrame(0);
		time_t keisokujikan(0);
		int PushCount(0);
		float SumBPM(0), AverageBPM(0);

		printf_s("\n[�X�y�[�X�L�[]:�v���J�n\n[1�L�[]:�v���I��\n[2�L�[]:�v���O���������\n");
		while (true)
		{
			if ((m_1 = (GetKeyState(0x31) & 0x80) ? m_1 + 1 : 0) == 1) Update();		// �ċA(�Ӗ��s���ȎQ�l���Z�̓g���K�[������Ă���)
			if (GetKeyState(0x32) & 0x80) return;										// ������

			SpaceFrame = (GetKeyState(VK_SPACE) & 0x80) ? SpaceFrame + 1 : 0;

			if (SpaceFrame == 1)
			{
				if (keisokujikan == 0)
				{
					keisokujikan = clock();
					printf_s("�v���J�n�I\n\n");
				}
				else
				{
					const float delta = (clock() - keisokujikan) * 0.001f;	// �~���b���b
					keisokujikan = clock();
					const float bpm = 60 / delta;
					AverageBPM = ((SumBPM += bpm) / ++PushCount);

					printf_s("�v��:%.2f   �\��BPM:%.2f\n", bpm, AverageBPM);
				}
			}
		}
	}
private:
	int m_1;
};

void main()
{
	printf_s("*�a�o�l�`�F�b�J�[*\n\n�Ȃ̃��Y���ɍ��킹��[�X�y�[�X�L�[]�������Ƃa�o�l���v�����Ă�����\n���v���ɏI���͂Ȃ��̂ŁA�����������̂��炢���ȂƎv�������Ŏ~�߂�B\n");

	Checker checker;
	checker.Update();
}