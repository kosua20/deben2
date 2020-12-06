#pragma once

#include "Common.hpp"
#include "Date.hpp"


class Operation {
public:
	enum Type {
	 IN, OUT
	};

	Operation(float amount, const std::string & label, const Date & date);

	Operation(const std::vector<std::string> & strs);
	
	const std::string & label() const;

	Type type() const;

	double amount() const;

	std::string toString() const;

	const Date & date() const;

private:
	Date _date;
	std::string _label;
	double _amount;
	Type _type;
};
