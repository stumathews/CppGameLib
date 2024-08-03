#pragma once
#ifndef STATISTICS_H
#define STATISTICS_H
#include <vector>

namespace gamelib
{
	struct Statistics
	{
		/**
		 * \brief Smooth Moving Average (SMA)
		 * \tparam T type of data points
		 * \param k last k entries of data set
		 * \param data data points
		 * \return moving average over last k entries in data
		 */
		template <typename T>
		static float SMA(int k, std::vector<T> data)
		{
			const auto n = data.size();
			T sum = 0;

			for(int i = 0; i < k;i++)
			{
				sum += data[n-k+i];
			}
			const float result = (1.0/k) * sum;
			return result;
		}
	};

}

#endif