#include "Common.hpp"
#include "Strings.hpp"
#include "Listing.hpp"
#include "Printer.hpp"
#include "Grapher.hpp"

#include "system/Config.hpp"
#include "system/System.hpp"
#include "system/TextUtilities.hpp"
#include "system/Terminal.hpp"


#include <ctime>
#include <iomanip>
#include <chrono>

enum class Action {
	ADD, REMOVE, LIST, TOTAL, GRAPH
};

class DebenConfig : public Config {
public:
	
	explicit DebenConfig(const std::vector<std::string> & argv) : Config(argv) {
		// Defaut file path is coming from the environment.
		char* defaultPath = std::getenv("DEBEN_FILE");
		if(defaultPath){
			path = std::string(defaultPath);
		}

		// Process arguments.
		for(const auto & arg : arguments()) {
			// Config path.
			if(arg.key == "path" || arg.key == "p"){
				if(!arg.values.empty()){
					path = arg.values[0];
				}

			}
			if(arg.key == "version" || arg.key == "v") {
				version = true;
			}
			if(arg.key == "license") {
				license = true;
			}
			if(arg.key == "money") {
				bonus = true;
			}
			if(arg.key == "no-color" || arg.key == "nc") {
				ascii = true;
			}

			if(arg.key == "delete" || arg.key == "d") {
				action = Action::REMOVE;
				if(!arg.values.empty()){
					index = stol(arg.values[0]);
				}
			}
			if(arg.key == "list" || arg.key == "l") {
				action = Action::LIST;
				if(!arg.values.empty()){
					count = stol(arg.values[0]);
				}
			}

			if(arg.key == "graph" || arg.key == "g"){
				action = Action::GRAPH;
				if(!arg.values.empty()){
					months = stol(arg.values[0]);
				}
				if(arg.values.size() > 1){
					height = stol(arg.values[1]);
				}
			}

			if(arg.key == "add" || arg.key == "a" ) {
				action = Action::ADD;
				rawOp = arg.values;
			}
			// Default "add" action.
			if(TextUtilities::isNumber(arg.key)){
				action = Action::ADD;
				rawOp = arg.values;
				rawOp.insert(rawOp.begin(), arg.key);
			}
		}


		registerArgument("path", "p", "Listing file to use ($DEBEN_FILE by default)", "path to file");

		registerSection("Operations");
		registerArgument("add", "a", "Add an operation (--add is optional)", "[+,-]amount 'label' dd[/mm[/YYYY]]");
		registerArgument("delete", "d", "Remove operation at index i (the last one by default)", "i");

		registerSection("Display");
		registerArgument("list", "l", "List the last n operations (40 by default)", "n");
		registerArgument("graph", "g", "Display a plot of the last n months (12 by default) on a graph of m lines", "n [m]");
		registerArgument("no-color", "nc", "Skip text decorations", "n");

		registerSection("Infos");
		registerArgument("version", "v", "Displays the current Deben version.");
		registerArgument("license", "", "Display the license message.");
		
	}

	std::vector<std::string> rawOp;
	std::string path = "";
	Action action = Action::TOTAL;
	long index = -1;
	long count = 40;
	long months = 12;
	long height = 24;
	bool ascii = false;
	// Messages.
	bool version = false;
	bool license = false;
	bool bonus = false;
};


int main(int argc, char** argv){
	setlocale(LC_ALL, "");
	
	DebenConfig config(std::vector<std::string>(argv, argv+argc));
	if(config.version){
		Log::Info() << versionMessage << std::endl;
		return 0;
	} else if(config.license){
		Log::Info() << licenseMessage << std::endl;
		return 0;
	} else if(config.bonus){
		Log::Info() << bonusMessage << std::endl;
		return 0;
	} else if(config.showHelp(config.path.empty())){
		return 0;
	}
	if(config.ascii){
		Terminal::disableANSI();
	}

	const fs::path path(config.path);

	Listing list(path);

	if(config.action == Action::LIST){
		auto ops = list.operations(config.count);
		auto totals = list.totals();
		Printer::printList(ops, list.count());
		Printer::printTotals(totals, false);
	}
	if(config.action == Action::GRAPH){
		auto months = list.monthTotals(config.months);
		Grapher::graphMonths(months, list.totals(), config.height);
	}
	if(config.action == Action::REMOVE){
		list.removeOperation(config.index);
		Printer::printTotals(list.totals());
	}
	if(config.action == Action::ADD){
		list.addOperation(config.rawOp);
		Printer::printTotals(list.totals());
	}
	if(config.action == Action::TOTAL){
		Printer::printTotals(list.totals());
	}

	list.save(path);
	return 0;
}
