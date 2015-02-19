#include<iostream>

enum LogLevel {
	ERROR,
	WARNING,
	DEBUG,
	VERBOSE
};

extern constexpr LogLevel logLevel = DEBUG;

constexpr bool isLoggable(LogLevel level)
{
	return level <= logLevel;
}

void log(const LogLevel level, const char *str)
{
	if (isLoggable(level))
		std::cout << str << std::endl;
}

int main ()
{
	log(VERBOSE, "overly verbose message");
	log(DEBUG, "debug message");
}
