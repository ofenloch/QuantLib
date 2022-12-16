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
#include <ctype.h>
#include <exception>
#include <fstream>
#include <istream>
#include <string>
#include <vector>


#define MAX_LINE_LENGTH 1024

QuantLib::TimeSeries<double> PriceSeries(char* filename) {
    // Read the file provided via command line
    std::ifstream in(filename);
    char line[MAX_LINE_LENGTH];
    std::vector<std::string> lines;
    while (in.getline(line, MAX_LINE_LENGTH, '\n')) {
        lines.push_back(line);
    }
    // Container tools
    std::vector<QuantLib::Date> dates;
    std::vector<double> quotes;

    for (unsigned int i = 0; i < lines.size(); i++) {
        if (!std::isdigit(lines[i].at(0))) {
            // Ignore header and comment lines.
            // The lines look loke this:
            //    2000-01-03,19.900000,20.400000,19.799999,20.000000,14.064997,6400
            // Usually the files has a header line like this:
            //    Date,Open,High,Low,Close,Adj Close,Volume
            // We'd like to be able to comment some lines with '#'.
            //
            // So, at least for now, we simply ignore all lines that do not start with a digit.
            continue;
        }
        std::vector<std::string> outerArray;

        boost::split(outerArray, lines[i], boost::is_any_of(","));

        std::vector<std::string> innerArray;
        boost::split(innerArray, outerArray[0], boost::is_any_of("-"));

        QuantLib::Year year = (QuantLib::Year)std::stoi(innerArray[0]);
        QuantLib::Month month = (QuantLib::Month)std::stoi(innerArray[1]);
        QuantLib::Day day = (QuantLib::Day)std::stoi(innerArray[2]);

        dates.push_back(QuantLib::Date(day, month, year));
        quotes.push_back(atof(outerArray[5].c_str()));
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

        const bool bEmpty = mySeries.empty();
        const size_t nLines = mySeries.size();
        const QuantLib::Date dFirstDate = mySeries.firstDate();
        const QuantLib::Date dLastDate = mySeries.lastDate();
        const QuantLib::Date dBeforeLastDate = mySeries.lastDate() - 1;


        // Is the time series empty?
        if (bEmpty) {
            std::cout << "TimeSeries is empty.\n";
            return 1;
        }
        std::cout << "TimeSeries has " << mySeries.size() << " entries.\n";

        // Start date of the time series
        std::cout << "TimeSeries' first date is " << dFirstDate << '\n';

        // Last date of the time series
        std::cout << "TimeSeries' last date is " << dLastDate << '\n';

        // What was the Adj.close value on November 14th, 2012?
        std::cout << "Adjusted close on " << dBeforeLastDate << " was " << mySeries[dBeforeLastDate]
                  << '\n';

        return 0;
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}