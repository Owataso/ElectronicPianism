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

		printf_s("\n[スペースキー]:計測開始\n[1キー]:計測終了\n[2キー]:プログラムを閉じる\n");
		while (true)
		{
			if ((m_1 = (GetKeyState(0x31) & 0x80) ? m_1 + 1 : 0) == 1) Update();		// 再帰(意味不明な参考演算はトリガーを取っている)
			if (GetKeyState(0x32) & 0x80) return;										// 抜ける

			SpaceFrame = (GetKeyState(VK_SPACE) & 0x80) ? SpaceFrame + 1 : 0;

			if (SpaceFrame == 1)
			{
				if (keisokujikan == 0)
				{
					keisokujikan = clock();
					printf_s("計測開始！\n\n");
				}
				else
				{
					const float delta = (clock() - keisokujikan) * 0.001f;	// ミリ秒→秒
					keisokujikan = clock();
					const float bpm = 60 / delta;
					AverageBPM = ((SumBPM += bpm) / ++PushCount);

					printf_s("計測:%.2f   予測BPM:%.2f\n", bpm, AverageBPM);
				}
			}
		}
	}
private:
	int m_1;
};

void main()
{
	printf_s("*ＢＰＭチェッカー*\n\n曲のリズムに合わせて[スペースキー]を押すとＢＰＭを計測してくれるよ\n※計測に終わりはないので、だいたいこのぐらいかなと思った所で止める。\n");

	Checker checker;
	checker.Update();
}