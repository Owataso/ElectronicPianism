#pragma once

#include <time.h>

class BenchMark
{
private:
	clock_t m_StartTime, m_EndTime;
	double m_time;
public:
	BenchMark() :m_StartTime(0), m_EndTime(0), m_time(0){}
	void Start(){ m_StartTime = clock(); }
	void End()
	{
		m_EndTime = clock();
		m_time = m_EndTime - m_StartTime;
		m_time /= CLOCKS_PER_SEC;
	}
	double GetTime(){ return m_time; }

};