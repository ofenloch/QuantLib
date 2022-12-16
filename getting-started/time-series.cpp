/**
 * @file time-series.cpp
 * @author Oliver Ofenloch (57812959+ofenloch@users.noreply.github.com)
 * @brief
 * @version 0.1
 * @date 2022-12-16
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <ql/quantlib.hpp>
#include <boost/algorithm/string.hpp>
#include <exception>
#include <fstream>
#include <istream>
#include <string>
#include <vector>

QuantLib::TimeSeries<double> PriceSeries(char* filename) {
    // Read the file provided via command line
    std::ifstream in(filename);
    std::string line;
    std::vector<std::string> lines;
    while (in >> line)
        lines.push_back(line);

    // Container tools
    std::vector<QuantLib::Date> dates;
    std::vector<double> quotes;

    for (unsigned int i = 0; i < lines.size(); i++) {
        std::vector<std::string> outerArray;

        boost::split(outerArray, lines[i], boost::is_any_of(","));

        std::vector<std::string> innerArray;
        boost::split(innerArray, outerArray[0], boost::is_any_of("-"));

        QuantLib::Year year = (QuantLib::Year)std::stoi(innerArray[0]);
        QuantLib::Month month = (QuantLib::Month)std::stoi(innerArray[1]);
        QuantLib::Day day = (QuantLib::Day)std::stoi(innerArray[2]);

        dates.push_back(QuantLib::Date(day, month, year));
        quotes.push_back(atof(outerArray[6].c_str()));
    }

    // Create a QuantLib::TimeSeries object
    QuantLib::TimeSeries<double> series(dates.begin(), dates.end(), quotes.begin());

    // Return the time series
    return series;
}

int main(int argc, char* argv[]) {

    try {


        // Source file
        char* filename = argv[1];

        // Call to the function
        QuantLib::TimeSeries<double> mySeries = PriceSeries(filename);

        ///////////////////////////////////////////////////////////////
        // Below are implementations of some methods of QL Timeseries//
        ///////////////////////////////////////////////////////////////

        // Is the time series empty?
        std::cout << "Is the series empty? (0 = not empty)\t" << mySeries.empty() << std::endl;

        // Start date of the time series
        std::cout << "Start date of the time series:\t" << mySeries.firstDate() << std::endl;

        // Last date of the time series
        std::cout << "Last date of the time series:\t" << mySeries.lastDate() << std::endl;

        // What was the Adj.close value on November 14th, 2012?
        std::cout << "Adjusted close on November 14th, 2012:\t"
                  << mySeries[QuantLib::Date(14, QuantLib::Nov, 2012)] << std::endl;

        return 0;
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}