
#include "Operation.hpp"

#include "Printer.hpp"
#include "system/TextUtilities.hpp"

Operation::Operation(float amount, const std::string & label, const Date & date):
	_date(date), _label(label), _amount(amount), _type(amount > 0.0f ? IN : OUT){

}

Operation::Operation(const std::vector<std::string> & strs){
	
	_date = Date(strs[0]);
	// manual double parsing
	_amount = TextUtilities::parseDouble(strs[1]);
	_type = _amount > 0.0 ? Type::IN : Type::OUT;

	bool first = true;
	for(size_t sid = 2; sid < strs.size(); ++sid){
		if(strs[sid].empty()){
			continue;
		}
		_label += (first ? "" : " ") + strs[sid];
		first = false;
	}

}

std::string Operation::toString() const {
	const std::string dateStr = _date.toString("%Y/%m/%d");
	const std::string signStr = (_type == Type::IN ? "+" : "-");
	const std::string amountStr = ::toString(std::abs(_amount));
	return dateStr + "\t" + signStr + amountStr + "\t" + _label;
}

const std::string & Operation::label() const {
	return _label;
}

Operation::Type Operation::type() const {
	return _type;
}

double Operation::amount() const {
	return _amount;
}

const Date & Operation::date() const {
	return _date;
}
