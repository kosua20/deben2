#include "Listing.hpp"
#include "system/TextUtilities.hpp"
#include "system/System.hpp"

Listing::Listing(const fs::path & path){
	std::string file = System::loadStringFromFile(path);
	TextUtilities::replace( file, "\r\n", "\n" );
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
		const auto toks = TextUtilities::split( line, "\t", true );
		if( toks.empty() ) {
			continue;
		}
		_operations.emplace_back( toks );
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
	const Amount amount = Operation::parseAmount(args[0]);

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
	const long begin = std::max(opSize - last, 0l);
	std::vector<Operation> select;
	select.reserve(last);

	for(long oid = begin; oid < opSize; ++oid){
		select.push_back(_operations[oid]);
	}
	return select;
}


std::vector<Totals> Listing::monthTotals(long last){
	// We need unique comparison of months.
	const auto hashDate = [](const Date & date){
		return long(date.year()) * 12 + long(date.month());
	};

	// Get current month.
	const Date now;
	const long currentMonth = hashDate(now);
	const long earliestMonth = std::max(currentMonth - last + 1, long(0));
	// We need to find the earliest record from this month.
	const auto firstOp = std::find_if(_operations.begin(), _operations.end(), [earliestMonth, &hashDate](const Operation & op){
		return hashDate(op.date()) >= earliestMonth;
	});

	std::vector<Totals> totals = { {Amount(0), Amount(0)}};

	long ongoingMonth = earliestMonth;

	for(auto op = firstOp; op != _operations.end(); ++op ){
		const long opMonth = hashDate(op->date());
		if(opMonth != ongoingMonth){
			for(long mid = ongoingMonth + 1; mid <= opMonth; ++mid){
				totals.emplace_back(Amount(0), Amount(0));
			}
			ongoingMonth = opMonth;
		}

		auto & tots = totals.back();
		if(op->type() == Operation::In){
			tots.first += op->amount();
		} else {
			tots.second += op->amount();
		}
	}
	// Handle missing months after last record.
	for(long mid = ongoingMonth + 1; mid <= currentMonth; ++mid){
		totals.emplace_back(Amount(0), Amount(0));
	}

	return totals;
}

Totals Listing::totals(){
	Totals totals = {Amount(0), Amount(0)};
	for(const auto & ope : _operations){
		if(ope.type() == Operation::Type::In){
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

