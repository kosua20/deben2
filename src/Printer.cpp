#include "Printer.hpp"
#include "system/TextUtilities.hpp"
#include "system/Terminal.hpp"
#include <iostream>

void Printer::printTotals(const Totals & totals, bool leadingNewline){
	std::string tPos = Operation::writeAmount(totals.first);
	std::string tNeg = Operation::writeAmount(totals.second);
	std::string tTot = Operation::writeAmount(totals.first + totals.second);

	const size_t digitCount = std::max(std::max(tPos.size(), tNeg.size()), tTot.size());

	tPos = TextUtilities::padLeft(tPos, digitCount, ' ');
	tNeg = TextUtilities::padLeft(tNeg, digitCount, ' ');
	tTot = TextUtilities::padLeft(tTot, digitCount, ' ');

	const std::string vertT = Terminal::supportsANSI() ? " " : "|";
	const std::string sepaT = Terminal::supportsANSI() ? "" : ("+" + std::string(9 + digitCount, '-') + "+\n");

	const std::string posStr = " In.:   " + tPos + " ";
	const std::string negStr = " Out.:  " + tNeg + " ";
	const std::string totStr = " Total: " + tTot + " ";

	std::cout << (leadingNewline ? "\n" : "") << sepaT;
	std::cout << vertT << Terminal::blackBg(Terminal::bold(Terminal::green(posStr))) << vertT << "\n";
	std::cout << vertT << Terminal::blackBg(Terminal::bold(Terminal::red(negStr))) << vertT << "\n";
	std::cout << vertT << Terminal::blackBg(Terminal::bold(Terminal::white(totStr))) << vertT << "\n";
	std::cout << sepaT << "\n";
}

void Printer::printList(const std::vector<Operation> & operations, long totalCount){
	if(operations.empty()) {
		std::cout << Terminal::italic("Empty list") << std::endl;
		return;
	}

	// Compute various needed lengths.
	const std::string tCountStr = std::to_string(totalCount);
	const size_t maxIndexSize = tCountStr.size();
	size_t maxDescSize = 0;
	for(const auto & op : operations){
		maxDescSize = std::max(maxDescSize, TextUtilities::count(op.label()));
	}
	const size_t maxLineSize = maxIndexSize + 27 + maxDescSize;

	// Update separating strings.
	_extSep = "";
	_intSep = "";
	_verSep = " ";
	if(!Terminal::supportsANSI()){
		_extSep = "+" + std::string(maxLineSize - 2, '-') + "+";
		_intSep = "+" + std::string(maxIndexSize, '-') + "+" + std::string(10, '-') + "+" + std::string(10, '-') + "+" + std::string( maxDescSize + 2, '-') + "+";
		_verSep = "|";
	}

	// Initial list header.
	std::string fullStr = "\n ";
	fullStr += Terminal::inverse("Operations: " + std::to_string(operations.size()) + "/" + tCountStr + " entries.");

	// Initial values for months header and footers.
	const Date & initDate = operations[0].date();
	int currentMonth = initDate.month();
	Totals localTotals = {Amount(0), Amount(0)};

	// First month header.
	fullStr += "\n" + _extSep + "\n" + monthHeader(initDate, maxIndexSize, maxLineSize);


	long i = totalCount - long(operations.size());
	for(const auto & op : operations) {
		// If new month, insert a footer then a header.
		if(op.date().month() != currentMonth){
			fullStr += "\n" + totalsFooter(localTotals, maxLineSize);
			fullStr += "\n" + _extSep;
			fullStr += "\n" + monthHeader(op.date(), maxIndexSize, maxLineSize);
			// Reset values.
			currentMonth = op.date().month();
			localTotals = {Amount(0), Amount(0)};
		}
		// Add current op amount.
		if (op.type() == Operation::IN) {
			localTotals.first += op.amount();
		} else {
			localTotals.second += op.amount();
		}

		// Add current operation.
		const std::string opStr = operationString(op, i, maxIndexSize, maxDescSize );
		fullStr += "\n" + opStr;
		i += 1;
	}

	// Add final footer and separator.
	fullStr += "\n" + totalsFooter(localTotals,  maxLineSize);
	fullStr += "\n" + _extSep;
	// Print the result.
	std::cout << fullStr << "\n";
}

std::string Printer::monthHeader(const Date & date, int pad, int length) {
	static const std::vector<std::string> months = {
		"January", "February", "Mars", "April", "May", "June", "July", "August", "Septembre", "Octobre", "Novembre", "Decembre"
	};

	std::string monthStr = months[date.month()-1] + " " + std::to_string(date.year());
	monthStr = std::string(pad+2, ' ') + monthStr;
	monthStr = TextUtilities::padRight(monthStr, length-2, ' ');

	const std::string vSep = Terminal::supportsANSI() ? " " : "|";
	return _verSep + Terminal::inverse(monthStr) + _verSep + (Terminal::supportsANSI() ? "" : "\n" + _intSep);
}

std::string Printer::totalsFooter(const Totals & totals, int length) {
	const std::string str0 = Operation::writeAmount( totals.first );
	const std::string str1 = Operation::writeAmount( -totals.second );
	const std::string str2 = Operation::writeAmount( totals.first + totals.second );

	std::string total = "Total: " + Terminal::green(str0) + " - " + Terminal::red(str1) + " = " + str2;
	total = TextUtilities::padRight(total, length - 2 + (Terminal::supportsANSI()? 20 : 0), ' ');

	return ( Terminal::supportsANSI() ? "" : _intSep + "\n" ) + _verSep + Terminal::brightBlackBg(Terminal::bold(total)) + _verSep;
}


 std::string Printer::operationString(const Operation & op, long index, int pad, int shift) {

	 const std::string dateStr = op.date().toString("%d/%m/%y");
	 const std::string labelStr = TextUtilities::padRight(op.label(), shift+1, ' ');
	 const std::string amountStr = Operation::writeAmount(op.amount(), true);

	 const std::string localStr = Terminal::bold(TextUtilities::padLeft(amountStr, 9, ' ')) + " " + _verSep + " " + dateStr + " " + _verSep + " " + Terminal::italic(labelStr);
	 const std::string indexStr = TextUtilities::padLeft(std::to_string(index), pad, ' ');
	 return _verSep + Terminal::dim(indexStr) + _verSep + localStr + _verSep;
 }
