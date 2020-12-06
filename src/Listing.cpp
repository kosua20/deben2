#include "Listing.hpp"
#include "system/TextUtilities.hpp"
#include "system/System.hpp"

Listing::Listing(const fs::path & path){
	const std::string file = System::loadStringFromFile(path);
	const auto lines = TextUtilities::split(file, "\n", true);

	for(const auto & lineRaw : lines){
		const std::string line = TextUtilities::trim(lineRaw, "\t ");
		if(line.empty()){
			continue;
		}
		if(line[0] == '#'){
			_comments.push_back(line);
			continue;
		}
		_operations.emplace_back(TextUtilities::split(line, "\t", true));
	}
}

void Listing::save(const fs::path & path){
	if(!_modified){
		return;
	}
	
	// Start by stringifying the operations and sorting them.
	std::vector<std::string> lines(_operations.size());
	for(size_t oid = 0; oid < _operations.size(); ++oid){
		lines[oid] = _operations[oid].toString();
	}
	std::sort(lines.begin(), lines.end());

	// Merge all content.
	std::string content;
	for(const auto & line : _comments){
		content.append(line + "\n");
	}
	for(const auto & line : lines){
		content.append(line + "\n");
	}
	System::writeStringToFile(content, path);
}

void Listing::removeOperation(long id){
	const long opSize = long(_operations.size());
	if(id < 0){
		id = opSize-1;
	}
	if(id >= opSize){
		Log::Warning() << "Operation " << id << " doesn't exist." << std::endl;
		return;
	}
	_operations.erase(_operations.begin() + id);
	_modified = true;
}

void Listing::addOperation(const std::vector<std::string> & args){
	if(args.empty()){
		Log::Warning() << "No operation to add." << std::endl;
		return;
	}
	_modified = true;

	// Get amount.
	const std::string amountStr = TextUtilities::trim(args[0], "\t ");
	// Amount is positive only if + sign present.
	const double sgn = amountStr[0] == '+' ? 1.0 : -1.0;
	const double amount = sgn * std::abs(TextUtilities::parseDouble(amountStr));

	// Extract date if present.
	Date date;
	size_t lastLabelToken = args.size()-1;

	if(args.size() > 2){
		// Check if last argument is a valid date.
		const std::string & dateStr = args[args.size()-1];
		const std::string::size_type pos = dateStr.find_first_not_of("0123456789/");
		if(pos == std::string::npos){
			date = Date::dateFromTokens(dateStr);
			--lastLabelToken;
		}
	}

	// Remaining tokens are the label, merge them.
	std::string label;
	for(size_t lid = 1; lid <= lastLabelToken; ++lid){
		if(lid != 1){
			label.append(" ");
		}
		label.append(args[lid]);
	}
	if(label.empty()){
		label = "Unknown";
	}
	_operations.emplace_back(amount, label, date);
}

std::vector<Operation> Listing::operations(long last){
	if(last <= 0){
		return _operations;
	}
	const long opSize = long(_operations.size());
	const size_t begin = std::max(opSize - last, 0l);
	std::vector<Operation> select;
	select.reserve(last);

	for(long oid = begin; oid < opSize; ++oid){
		select.push_back(_operations[oid]);
	}
	return select;
}

Totals Listing::totals(){
	Totals totals = {0.0, 0.0};
	for(const auto & ope : _operations){
		if(ope.type() == Operation::Type::IN){
			totals.first += ope.amount();
		} else {
			totals.second += ope.amount();
		}
	}
	return totals;
}

long Listing::count() const {
	return long(_operations.size());
}

