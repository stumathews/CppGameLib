#pragma once
#include "BitfieldReader.h"
#include "BitPacker.h"
#include "BitPackingTypes.h"
#include "Fish.h"
#ifndef PING_MESSAGE_H

namespace  gamelib
{
	class PingMessage
	{
	public:
		PingMessage(): Type(""), IsHappy(false), EventType(0) {  }
		std::string Type;
		bool IsHappy;
		int EventType;
		std::vector<std::string> Names;
		std::vector<int> Ages;
		Fish Fish;

		// Pack into bitPacker's associated buffer
		void Write(BitPacker<uint32_t>& bitPacker) const
		{
			/*
			 <PingMessage>
			    <Type>ping</Type>
			    <IsHappy>true</IsHappy>
			    <EventType>1015</EventType>
			    <Names itemCount="3">
			        <Name>Stuart</Name>
			        <Name>Jenny</Name>
			        <Name>bruce</Name>
			    </Names>
			    <Ages itemCount="3">
			        <Age>1</Age>
			        <Age>2</Age>
			        <Age>3</Age>
			    </Ages>
			    <Fish Name="Neemo" Surname="Mathews"/>
			</PingMessage>
			 */


			bit_packing_types::String<uint32_t> type(Type);			
			bit_packing_types::ArrayOfStrings<uint32_t> names(Names.data(), Names.size());

			auto constexpr eventTypeBitsRequired = BitPacker<uint32_t>::BitsRequired(0, 2000);
			auto constexpr bitsRequiredFor3Ints = BitPacker<uint32_t>::BitsRequired(0, 3);

			type.Write(bitPacker);
			bitPacker.Pack(1, IsHappy);
			bitPacker.Pack(eventTypeBitsRequired, EventType);
			names.Write(bitPacker);

			// manually pack array of ints
			bitPacker.Pack(bitsRequiredFor3Ints, Names.size());
			bitPacker.Pack(bitsRequiredFor3Ints, Ages[0]);
			bitPacker.Pack(bitsRequiredFor3Ints, Ages[1]);
			bitPacker.Pack(bitsRequiredFor3Ints, Ages[2]);

			bitPacker.Finish();
			
			// pack fish manually
			const bit_packing_types::String<uint32_t> fishName(Fish.Name);
			const bit_packing_types::String<uint32_t> fishSurname(Fish.Surname);

			fishName.Write(bitPacker);
			fishSurname.Write(bitPacker);

			bitPacker.Finish();
		}

		// Unpack into bitfieldReader's associated buffer
		void Read(BitfieldReader<uint32_t>& bitfieldReader)
		{
			bit_packing_types::String<uint32_t> type;			
			bit_packing_types::ArrayOfStrings<uint32_t> names;

			auto constexpr eventTypeBitsRequired = BitPacker<uint32_t>::BitsRequired(0, 2000);
			
			auto constexpr bitsRequiredFor3Ints = BitPacker<uint32_t>::BitsRequired(0, 3);

			type.Read(bitfieldReader);
			Type = std::string(type.c_str());

			IsHappy = bitfieldReader.ReadNext<bool>(1);
			EventType = bitfieldReader.ReadNext<int>(eventTypeBitsRequired);
			names.Read(bitfieldReader);
			for(int i = 0; i < names.NumElements(); i++)
			{
				Names.emplace_back(names[i].c_str());
			}

			const auto count = bitfieldReader.ReadNext<int>(bitsRequiredFor3Ints);
			for(int i = 0; i < count; i++)
			{
				Ages.push_back(bitfieldReader.ReadNext<int>(bitsRequiredFor3Ints));
			}

			bitfieldReader.Finish();

			bit_packing_types::String<uint32_t> fishName;			
			bit_packing_types::String<uint32_t> fishSurname;

			fishName.Read(bitfieldReader);
			Fish.Name = fishName.c_str();
			fishSurname.Read(bitfieldReader);
			Fish.Surname = fishSurname.c_str();

		}
	};
}

#endif