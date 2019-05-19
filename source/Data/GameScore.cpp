#include "tdnlib.h"
#include "GameScore.h"


void GameScore::ComputeScore(int NumNorts)
{
	// スコア=比率÷総ノート数×100,000(小数点以下切捨)
	this->score = (int)(
		(this->JudgeCount[(int)JUDGE_TYPE::SUPER] * 1
		+ this->JudgeCount[(int)JUDGE_TYPE::GREAT] * .65f
		+ this->JudgeCount[(int)JUDGE_TYPE::GOOD] * .25f) / NumNorts * MAX_SCORE);
}