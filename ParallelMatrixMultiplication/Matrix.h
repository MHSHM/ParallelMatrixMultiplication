#pragma once

#include <cstdint>
#include <random>
#include "ThreadPool.h"

template<uint32_t N>
class Matrix
{
private:
	std::vector<std::vector<int>> data;

private:
	int RowColMulti(const std::vector<int>& row, const std::vector<int>& col)
	{
		int sum = 0;

		for (int i = 0; i < N; ++i)
		{
			sum += row[i] * col[i];
		}

		return sum;
	}

private:
	void GetRow(int i, std::vector<int>& target)
	{
		for (int j = 0; j < N; ++j)
		{
			target[j] = data[i][j];
		}
	}

	void GetCol(int i, std::vector<int>& target)
	{
		for (int j = 0; j < N; ++j)
		{
			target[j] = data[j][i];
		}
	}

public:
	friend std::ostream& operator<< (std::ostream& stream, const Matrix& matrix)
	{
		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				stream << matrix.data[i][j] << ' ';
			}

			stream << '\n';
		}

		return stream;
	}

public:
	Matrix()
	{
		data.resize(N);

		for (int i = 0; i < N; i++)
		{
			data[i].resize(N);
		}
	}


	void InitializeWithRandom()
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist6(1, 10);

		for (int i = 0; i < N; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				data[i][j] = dist6(rng);
			}
		}

	}

	void Initialize(const std::vector<int>& data)
	{

	}

	void Multiply(const Matrix& matrix, Matrix& result)
	{
		for (int i = 0; i < N; ++i)
		{
			std::vector<int> row(N, 0);

			GetRow(i, row);

			for (int j = 0; j < N; ++j)
			{
				std::vector<int> col(N, 0);

				GetCol(j, col);

				uint32_t sum = RowColMulti(row, col);

				result.data[i][j] = sum;
			}
		}
	}

	void ParallelMultiply(const Matrix& matrix, Matrix& result, ThreadPool& pool) 
	{
		for (int i = 0; i < N; ++i)
		{
			std::vector<int> row(N, 0);
			GetRow(i, row);

			for (int j = 0; j < N; ++j)
			{
				Task t([this, i, j, row, &result]() {
						std::vector<int> col(N, 0);
						GetCol(j, col);
						uint32_t sum;
						sum = RowColMulti(row, col);
						result.data[i][j] = sum;
					});

				pool.Schedule(t);
			}
		}
	}
};