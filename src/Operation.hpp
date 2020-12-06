#pragma once

#include "Common.hpp"
#include "Date.hpp"

using Amount = long long;

class Operation {
public:
	enum Type {
	 IN, OUT
	};

	Operation(Amount amount, const std::string & label, const Date & date);

	Operation(const std::vector<std::string> & strs);
	
	const std::string & label() const;

	Type type() const;

	Amount amount() const;

	std::string toString() const;

	const Date & date() const;

	static Amount parseAmount(const std::string & s);

	static std::string writeAmount(const Amount & a, bool showPlusSign = false);
	
private:
	Date _date;
	std::string _label;
	Amount _amount;
	Type _type;
};
