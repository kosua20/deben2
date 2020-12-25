#include "Date.hpp"
#include "system/TextUtilities.hpp"

#include <iomanip>

Date::Date(){
	// Initialize the date at the current time.
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);
	_date = std::tm(*now);
}

Date::Date(const std::string & date) {
	// Initialize the date at the current time.
	std::time_t t = std::time(0);
	std::tm* now = std::localtime(&t);
	_date = std::tm(*now);

	// Process the string.
	auto tokens = TextUtilities::split(date, "/", true);
	// We expect a full YYYY/MM/DD date.
	if(tokens.size() < 3){
		Log::Error() << "Unable to fill full date, expected YYYY/MM/DD." << std::endl;
	} else {
		_date.tm_year = std::stoi(tokens[0]) - 1900;
		_date.tm_mon = std::stoi(tokens[1]) - 1;
		_date.tm_mday = std::stoi(tokens[2]);
	}
}

Date Date::dateFromTokens(const std::string & toks) {
	Date date;
	// Process the string.
	auto tokens = TextUtilities::split(toks, "/", true);
	// Tokens are in DD/MM/YYYY order, each optional.
	if(tokens.size() > 0){
		date._date.tm_mday = std::stoi(tokens[0]);
	}
	if(tokens.size() > 1){
		date._date.tm_mon = std::stoi(tokens[1]) - 1;
	}
	if(tokens.size() > 2){
		date._date.tm_year = std::stoi(tokens[2]) - 1900;
	}
	return date;
}

std::string Date::toString(const std::string & format, const std::string & locale) const {
	std::stringstream str;
	if(!locale.empty()){
		str.imbue(std::locale(locale));
	}
	str << std::put_time(&_date, format.c_str());
	return str.str();
}

int Date::day() const {
	return _date.tm_mday;
}

int Date::month() const {
	return _date.tm_mon + 1;
}

int Date::year() const {
	return _date.tm_year + 1900;
}
