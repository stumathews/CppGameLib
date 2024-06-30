#pragma once
#ifndef TESTDATA_H
#define TESTDATA_H

#include "net/BitfieldReader.h"
#include "net/BitPacker.h"

namespace TestData
{
	struct TestNetworkPacket
	{
		#define MAX_ELEMENTS 3
	
	    int NumElements;
	    int Elements[MAX_ELEMENTS];

	    void Write(gamelib::BitPacker<uint16_t>& bitPacker) const
	    {
			bitPacker.Pack(BITS_REQUIRED( 0, MAX_ELEMENTS), NumElements);
			for(int i = 0; i < NumElements;i++)
			{
				bitPacker.Pack(BITS_REQUIRED( 0,255 ), Elements[i]);
			}
	    }

	    void Read(gamelib::BitfieldReader<const char>& bitfieldReader)
	    {
	        NumElements = bitfieldReader.ReadNext<int>(BITS_REQUIRED( 0, MAX_ELEMENTS ));
			for(int i = 0; i < NumElements;i++)
			{
				Elements[i] = bitfieldReader.ReadNext<int>(BITS_REQUIRED( 0,255 ));
			}
	    }
	};

}

#endif