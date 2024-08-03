#include "TextFile.h"

#include <iomanip>

namespace gamelib
{
	TextFile::TextFile(const std::string& fileName)
	{
		Create(fileName);
	}

	TextFile::~TextFile()
	{
		Flush();
		Close();
	}

	void TextFile::Append(const std::string& line)
	{
		buffer << GetTimeString() << " " << line;
		Flush();
	}

	
	void TextFile::Create(const std::string& fileName)
	{
		file.open(fileName.c_str());
	}

	void TextFile::Flush()
	{
		file << buffer.str();
		file.flush();

		// Reset buffer
		buffer.str("");
	}

	void TextFile::Close()
	{
		file.close();
	}

	std::string TextFile::GetTimeString()
	{
		std::stringstream timeString;
		tm newTime{};
		const time_t now = time(nullptr);

		localtime_s(&newTime, &now);  // NOLINT(cert-err33-c)

		timeString << std::setw(2) << std::setfill('0') << newTime.tm_mday << "-";
		timeString << std::setw(2) << std::setfill('0') << newTime.tm_mon << "-";
		timeString << std::setw(2) << std::setfill('0') << 1900 + newTime.tm_year << " ";
		timeString << std::setw(2) << std::setfill('0') << newTime.tm_hour << ":";
		timeString << std::setw(2) << std::setfill('0') << newTime.tm_min << ":";
		timeString << std::setw(2) << std::setfill('0') << newTime.tm_sec;

		return timeString.str();
	}
}
