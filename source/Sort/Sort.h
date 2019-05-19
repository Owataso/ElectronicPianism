#pragma once

#include "TDNLIB.h"

namespace Sort
{
	template <typename T>
	class Base
	{
	public:
		virtual void Function(const std::vector<T>& v, bool asc = true) = 0;
	protected:
		void Swap(const T &t1, const T &t2)
		{
			T temp = t1;
			t1 = t2;
			t2 = temp;
		}
	};

	// バブルソート
	template <typename T>
	class BubbleSort : public Base<T>
	{
	public:
		void Function(const std::vector<T>& v, bool asc = true)
		{
			for (UINT i = 0; i < v.size() - 1; i++)
			{
				for (UINT j = i + 1; j < v.size(); j++)
				{
					if (asc ? (v[i] > v[j]) : (v[i] < v[j])) Swap(p1, p2);
				}
			}
		}
	};
}