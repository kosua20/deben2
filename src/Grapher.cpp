#include "Grapher.hpp"
#include "system/Terminal.hpp"
#include "system/TextUtilities.hpp"

#include <map>
#include <iostream>

void Grapher::graphMonths(const std::vector<Totals> & months, const Totals & totals, int height){

	const size_t mCount = months.size();

	// Compute the total amount evolution over the months, starting from the current total.
	std::vector<Amount> cumulMonths(mCount, Amount(0));
	cumulMonths[mCount - 1] = totals.first + totals.second;

	for(long mid = long(mCount)-2; mid >= 0; --mid){
		const auto & month = months[mid];
		cumulMonths[mid] = cumulMonths[mid+1] - (month.first + month.second);
	}

	// Find the min and max values to display.
	std::pair<Amount, Amount> minMax = { cumulMonths[0], cumulMonths[0]};
	for(size_t mid = 0; mid < mCount; ++mid){
		const Amount currMin = std::min(cumulMonths[mid], std::min(months[mid].first, -months[mid].second));
		const Amount currMax = std::max(cumulMonths[mid], std::max(months[mid].first, -months[mid].second));

		minMax.first = std::min(minMax.first, currMin);
		minMax.second = std::max(minMax.second, currMax);
	}
	// Adjust the bottom of the range.
	minMax.first *= 0.9;

	// We want the max to be N characters high.
	const float totalSegm = float(minMax.second - minMax.first) / height;
	const int graphWidth = int(mCount+1)*3;
	const int graphWidthWithBorders = graphWidth + 2;
	const int labelCount = height / 4;

	// Pad for vertical axis labels.
	const size_t padSize = Operation::amountLength(minMax.second) + 2;
	// Initial padding for lines with no label.
	const std::string padBegin = std::string(padSize, ' ');
	// Build list of label values and their locations.
	std::map<int, Amount> labels;
	for(int lid = 0; lid < labelCount; ++lid){
		const int line = lid * height / (labelCount - 1);
		const Amount val = lid * (minMax.second - minMax.first) / (labelCount - 1) + minMax.first;
		labels[line] = val;
	}

	// Horizontal axis labels will display the month index, we need the first one.
	Date now;
	int firstMonth = now.month() - int(mCount) + 1;
	while(firstMonth < 0){
		firstMonth += 12;
	}

	// Build separators.
	const std::string hSep = "+" + std::string(graphWidthWithBorders, '-') + "+" + "\n";
	const std::string vSep = Terminal::supportsANSI() ? " " : "|";
	// Graph background character.
	const std::string empty = Terminal::blackBg(" ");

	// Start graph top edge.
	std::cout << "\n";
	if(!Terminal::supportsANSI()){
		std::cout << padBegin << hSep;
	}

	// Graph lines: display label if on the right line,
	// then for each column check if we should display
	// the bar, a marker for in/out or the background.
	for(int y = height; y >= 0; --y ){

		// Handle label.
		std::string padLine;
		if(labels.find(y) != labels.end()){
			padLine = Operation::writeAmount(labels[y]);
		} else {
			padLine = padBegin;
		}
		std::cout << Terminal::italic(TextUtilities::padLeft(padLine, padSize, ' '));

		// Vertical opening graph edge + initial spacing.
		std::cout << vSep << empty << empty << empty;

		// Compute value for this line.
		const float value = float(y) * totalSegm + float(minMax.first);
		const float valueUp = float(y + 1) * totalSegm + float(minMax.first);

		// Scan horizontally.
		for(int x = 1; x < graphWidth; ++x){
			const int subPixel = x % 3;
			// Check if we are between two bars.
			if( subPixel == 0 || (!Terminal::supportsANSI() && subPixel == 2)){
				std::cout << empty;
				continue;
			}

			// Which month are we covering.
			const int mid = x/3;
			// If we are too much to the right, just background.
			if(mid >= int(mCount)){
				std::cout << empty;
				continue;
			}

			// Is the current range containing the in value ?
			const bool containsIn = months[mid].first > value && months[mid].first <= valueUp;
			// Is the current range containing the out value ?
			const bool containsOut = -months[mid].second > value && -months[mid].second <= valueUp;
			// If both, display a combined indicator.
			if(containsIn && containsOut){
				std::cout << Terminal::brightYellow(Terminal::brightYellowBg("*"));
			} else if(containsIn){
				std::cout << Terminal::green(Terminal::greenBg("+"));
			} else if(containsOut){
				std::cout << Terminal::red(Terminal::redBg("x"));
			} else if(float(cumulMonths[mid]) > value){
				// Finally check if we are below the top of the bar.
				std::cout << Terminal::brightBlack(Terminal::brightBlackBg("█"));
			} else {
				std::cout << empty;
			}
		}
		// Closing graph edge.
		std::cout << vSep << "\n";
	}

	// Edge below graph.
	if(!Terminal::supportsANSI()){
		std::cout << padBegin << hSep;
	}

	// Horizontal axis labels: months.
	// Initial padding, no bacground this time.
	std::cout << padBegin << vSep << "   ";
	// Cover each 'pixel'
	for(int x = 1; x < graphWidth-1; ++x){
		// Find the current month.
		int mid = x/3;
		// If we are too much to the right, skip.
		if(mid >= int(mCount)){
			std::cout << " ";
			continue;
		}
		// If we are at the same horizontal location as the bar, print the month.
		if(x%3 == 1){
			const std::string monthStr = std::to_string((firstMonth + mid - 1)%12 + 1);
			std::cout << Terminal::bold(TextUtilities::padRight(monthStr, 3, ' '));
		}
	}
	// End of the label line.
	std::cout << vSep << "\n";

	// Separator between labels and legend.
	if(!Terminal::supportsANSI()){
		std::cout << padBegin << hSep;
	} else {
		std::cout << "\n";
	}

	// Width of the graph - width of the legend text.
	const int legendSize = graphWidthWithBorders - 23;
	// Legend text with proper color code.
	std::string legendText = Terminal::brightBlack(Terminal::brightBlackBg("█")) + ": total  ";
	legendText += Terminal::green(Terminal::greenBg("+")) + ": in  ";
	legendText += Terminal::red(Terminal::redBg("x")) + ": out";
	// Padding to center label text.
	const std::string legendPad = legendSize != 0 ? std::string(legendSize/2, ' ') : "";

	// Generate the full line with initial padding and text.
	std::cout << padBegin << vSep;
	std::cout << legendPad << legendText << legendPad;
	// Compensate odd line length alignment.
	if(legendSize % 2 == 1){
		std::cout << " ";
	}
	// End of the line legend.
	std::cout << vSep << "\n";

	// Closing separator.
	if(!Terminal::supportsANSI()){
		std::cout << padBegin << hSep << "\n";
	} else {
		std::cout << "\n";
	}

}
