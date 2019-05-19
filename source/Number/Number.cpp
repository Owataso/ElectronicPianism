#include "TDNLIB.h"
#include "Number.h"

/********************************************/
//	”Žš‰æ‘œ•`‰æ
/********************************************/
void NumberRender(tdn2DObj *image, int x, int y, int width, int height, int val, int digit, bool bTopZero)
{
	if (bTopZero)
	{
		bool zero(false);
		for (int i = digit - 1; i >= 0; i--)
		{
			int work(val);
			for (int j = i - 1; j >= 0; j--) work /= 10;
			const int number = work % 10;

			if (zero)
			{
				image->Render(x - width * i, y, width, height, number * width, 0, width, height);
			}
			else if (number != 0)
			{
				zero = true;
				image->Render(x - width * i, y, width, height, number * width, 0, width, height);
			}
		}
	}
	else
	{
		for (int i = 0; i < digit; i++)
		{
			image->Render(x - width * i, y, width, height, (val % 10) * width, 0, width, height);
			val /= 10;
		}
	}
};