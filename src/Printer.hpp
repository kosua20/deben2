#pragma once

#include "Common.hpp"
#include "system/System.hpp"
#include "Listing.hpp"

std::string toString(double amount);

class Printer {
public:

	void printList(const std::vector<Operation> & operations, long totalCount);

	static void printTotals(const Totals & totals, bool leadingNewline = true);

private:

	std::string monthHeader(const Date & date, int pad, int length);

	std::string totalsFooter(const Totals & totals, int length);

	std::string operationString(const Operation & op, long index, int pad, int shift);

	// Update separating strings.
	std::string _extSep = "";
	std::string _intSep = "";
	std::string _verSep = " ";

};
