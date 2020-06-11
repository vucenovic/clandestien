#pragma once

#include <string>
#include <map>

/*
	A simple INI reader
*/
class INIReader
{
private:
	std::map<std::string, std::map<std::string, std::string>> values;

	static char * FindFirstOccurance(char * start, char * end, const char & val);
	static char * FindFirstNonOccurance(char * start, char * end, const char & val);

	template <typename T> T convert(const std::string & str) = delete;
public:
	INIReader(std::string filePath);

	template <typename T> T Get(const std::string & section, const std::string & name, T default_value);
};

template<>
inline double INIReader::convert(const std::string & str)
{
	return std::stod(str);
}

template<>
inline float INIReader::convert(const std::string & str)
{
	return std::stof(str);
}

template<>
inline long INIReader::convert(const std::string & str)
{
	return std::stol(str);
}

template<>
inline int INIReader::convert(const std::string & str)
{
	return std::stoi(str);
}

template<>
inline bool INIReader::convert(const std::string & str)
{
	return str == "true";
}

template<>
inline std::string INIReader::convert(const std::string & str)
{
	return str;
}

template<>
inline char INIReader::convert(const std::string & str) 
{
	return str[0];
}

template<typename T>
inline T INIReader::Get(const std::string & section, const std::string & name, T default_value)
{
	auto a = values.find(section);
	if (a == values.end()) return default_value;
	auto b = a->second.find(name);
	if (b == a->second.end()) return default_value;
	return convert<T>(b->second);
}

#include "FileUtils.h"

char * INIReader::FindFirstOccurance(char * ptr, char * end, const char & val)
{
	while (ptr != end)
	{
		if (*ptr == val) { return ptr; }
		ptr += 1;
	}
	return end;
}

char * INIReader::FindFirstNonOccurance(char * ptr, char * end, const char & val)
{
	while (ptr != end)
	{
		if (*ptr != val) { return ptr; }
		ptr += 1;
	}
	return end;
}

INIReader::INIReader(std::string filePath)
{
	std::ifstream file(filePath, std::ios::in | std::ios::ate);

	std::map<std::string, std::string> * currentSection = &values[""];

	if (file.is_open()) {
		size_t fileLength = file.tellg();
		file.seekg(0, std::ios::beg);

		lineReader lr = lineReader(64, file, fileLength);
		char * mid_ptr, * start_ptr;
		while (lr.getline())
		{
			start_ptr = FindFirstNonOccurance(lr.line, lr.LineEnd(), ' ');
			switch (start_ptr[0])
			{
			case '[': //Section
				mid_ptr = FindFirstOccurance(start_ptr, lr.LineEnd(), ']');
				currentSection = &values[std::string(lr.line + 1, mid_ptr)];
				break;
			default: //Value
			{
				char * ptr1;
				mid_ptr = FindFirstOccurance(start_ptr, lr.LineEnd(), '=');
				ptr1 = FindFirstOccurance(start_ptr, mid_ptr, ' ');
				std::string name = std::string(start_ptr, ptr1);
				mid_ptr = FindFirstNonOccurance(mid_ptr + 1, lr.LineEnd(), ' ');
				ptr1 = FindFirstOccurance(mid_ptr, lr.LineEnd() - 1, ' ');
				std::string value = std::string(mid_ptr, ptr1);
				auto & tmp = (*currentSection); //I dont know why but writing it to a temp and then using it fixes some weird bug that triggered a stack based overflow error
				tmp[name] = value;
			}
			break;
			case ';': //Comment
			case '\n'://quickfix
			case ' ':
				break;
			}
		}
		file.close();
	}
	//Clear empty sections;


}