#pragma once

#include "Common.hpp"
#include "Operation.hpp"
#include "system/System.hpp"

class Listing {
public:

	Listing(const fs::path & path);

	void save(const fs::path & path);

	void removeOperation(long id);

	void addOperation(const std::vector<std::string> & args);

	std::vector<Operation> operations(long last);

	std::vector<Totals> monthTotals(long last);

	Totals totals();

	long count() const;

private:

	std::vector<Operation> _operations;
	std::vector<std::string> _comments;
	bool _modified = false;
	
};
