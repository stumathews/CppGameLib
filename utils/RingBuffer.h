﻿#pragma once
#include <cstdint>

namespace gamelib
{
	template <class T>
	class RingBuffer
	{
	public:

		explicit RingBuffer(int bufferSize = 1024)
		{
			this->bufferSize = bufferSize;

			sequenceBuffer = new uint32_t[bufferSize];
			sequenceBufferData = std::vector<T>(bufferSize);

		}		
		
		T* Get(const uint16_t sequence)
		{
		    const int index = GetSequenceIndex(sequence);
			if(sequenceBuffer[index] == sequence)
			{
				return sequenceBufferData.data() + index;
			}
		    return nullptr;
		}

		void Put(const uint16_t sequence, T data)
		{
			const int index = GetSequenceIndex(sequence);
			sequenceBuffer[index] = sequence;
			sequenceBufferData[index] = data;
			lastAddedSequence = sequence;
		}

		[[nodiscard]] uint16_t GetLastAddedSequence() const { return lastAddedSequence; }

		[[nodiscard]] uint16_t GetBufferSize() const { return bufferSize; }

		[[nodiscard]] std::vector<T> GetDataBuffer() const { return sequenceBufferData.data(); }

		[[nodiscard]] uint16_t GetSequenceIndex(const uint16_t sequence) const
		{
			return sequence % bufferSize;
		}
		
	private:

		uint16_t sequence = 0;		
		uint16_t lastAddedSequence = 0;		

		int bufferSize{};
		uint32_t* sequenceBuffer = {};
		std::vector<T> sequenceBufferData = {};
	};
}
