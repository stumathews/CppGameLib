#pragma once
#ifndef TEXT_FILE
#define TEXT_FILE
#include <fstream>
#include <sstream>
#include <string>

namespace gamelib
{
	class TextFile
	{
	public:
		explicit TextFile(const std::string& fileName);
		
		~TextFile();
		void Append(const std::string& line);
		void Create(const std::string& fileName);
		void Flush();
		void Close();

	private:
		std::ofstream file;
		std::stringstream buffer;
		static std::string GetTimeString();
	};
}

#endif

