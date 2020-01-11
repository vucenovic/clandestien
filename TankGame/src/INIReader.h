#pragma once

#include <string>
#include <map>

/*
	A simple INI reader
	INI files must not contain leading whitespaces
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

template<typename T>
inline T INIReader::Get(const std::string & section, const std::string & name, T default_value)
{
	auto a = values.find(section);
	if (a == values.end()) return default_value;
	auto b = a->second.find(name);
	if (b == a->second.end()) return default_value;
	return convert<T>(b->second);
}
