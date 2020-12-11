#pragma once

#include "Common.hpp"
#include "Operation.hpp"
#include "system/System.hpp"


class Printer {
public:

	static void printList(const std::vector<Operation> & operations, long totalCount);

	static void printTotals(const Totals & totals, bool leadingNewline = true);

private:

	static std::string monthHeader(const Date & date, int pad, int length, const std::string & verSep, const std::string & intSep);

	static std::string totalsFooter(const Totals & totals, int length, const std::string & verSep, const std::string & intSep);

	static std::string operationString(const Operation & op, long index, int pad, int shift, const std::string & verSep);

};
