#pragma once
#include "Common.hpp"
#include <ctime>

class Date {
public:

	Date();

	Date(const std::string & date);

	std::string toString(const std::string & format, const std::string & local = "") const;

	int day() const;

	int month() const;

	int year() const;

	static Date dateFromTokens(const std::string & tokens);

private:

	std::tm _date;
};

