
#include "Operation.hpp"

#include "Printer.hpp"
#include "system/TextUtilities.hpp"

Operation::Operation(Amount amount, const std::string & label, const Date & date):
	_date(date), _label(label), _amount(amount), _type(amount > Amount(0) ? In : Out){

}

Operation::Operation(const std::vector<std::string> & strs){
	
	_date = Date(strs[0]);
	// manual amount parsing
	_amount = Operation::parseAmount(strs[1]);
	_type = _amount > Amount(0) ? Type::In : Type::Out;

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
	const std::string signStr = (_type == Type::In ? "+" : "-");
	const std::string amountStr = Operation::writeAmount(std::abs(_amount), false);
	return dateStr + "\t" + signStr + amountStr + "\t" + _label;
}

const std::string & Operation::label() const {
	return _label;
}

Operation::Type Operation::type() const {
	return _type;
}

Amount Operation::amount() const {
	return _amount;
}

const Date & Operation::date() const {
	return _date;
}

Amount Operation::parseAmount(const std::string & s){
	const std::string ns = TextUtilities::trim(s, "\t ");
	if(ns.empty()){
		return 0;
	}

	// We store in fixed point (+-)61.2 with the extra convention
	// that positive numbers have a mandatory + prefix sign.
	const long long sgn = ns[0] == '+' ? 1 : -1;

	// Find decimal point.
	const auto pos = ns.find_last_of(".,");

	const std::string stru = ns.substr(0, pos);
	const long long unts = stru.empty() ? 0 : std::abs(std::stoll(stru));

	// If no decimal digits, integer * 100.
	if(pos == std::string::npos || pos == (ns.size()-1)){
		return sgn * unts * 100;
	}
	// Else get the first two decimals.
	const std::string strd = ns.substr(pos+1);
	long long decs = 0;
	if(strd.size() == 1){
		// We only have a tenth digit.
		decs = 10 * std::abs(std::stoll(strd));
	} else if(strd.size() > 1){
		// We only want the highest two digits.
		const std::string strd2 = strd.substr(0, 2);
		decs = std::abs(std::stoll(strd2));
	}

	return sgn * (unts * 100 + decs);
}

std::string Operation::writeAmount(const Amount & a, bool showPlusSign){
	// We store in fixed point (+-)61.2
	const long long unts = std::abs(a) / 100;
	const long long decs = std::abs(a) % 100;
	const std::string sgn = a >= 0 ? (showPlusSign ? "+" : "") : "-";

	std::string stru = std::to_string(unts);
	if( stru.size() > 0 && stru[0] == '+' ) {
		stru = stru.substr( 1 );
	}
	const std::string strd = TextUtilities::padLeft(std::to_string(decs), 2, '0');

	return sgn + stru + "." + strd ;
}

size_t Operation::amountLength(const Amount & a){
	return size_t(std::ceil(std::log10(a))+1 + (a < 0 ? 1: 0));
}

