/**
 * @file code-1-2.cpp
 * @author Oliver Ofenloch (57812959+ofenloch@users.noreply.github.com)
 * @brief
 * @version 0.1
 * @date 2022-12-16
 *
 * @copyright Copyright (c) 2022
 *
 * This is the original version of the code sample 1.2 in
 * https://letyourmoneygrow.com/yetanotherquant/QuantLib/book/BookQuantLib.pdf
 * by Vasily Nekrasov.
 * 
 */

#include <ql/quantlib.hpp>
#include <iostream>
#include <math.h>
using namespace QuantLib;
void main() {
    // create a zero coupon bond
    Date issueDate(2, October, 2014);
    Natural settlementDays = 2;
    Calendar calendar = Germany(Germany::Settlement);
    BusinessDayConvention paymentConvention = Following;
    Real faceAmount = 100;
    Real redemption = 100;
    Date maturityDate(2, October, 2016);
    ZeroCouponBond zeroBond(settlementDays, calendar, faceAmount, maturityDate, paymentConvention,
                            redemption, issueDate);
    // calculate its yield
    Real cleanPrice = 97.5;
    DayCounter dayCounter = ActualActual(ActualActual::ISDA);
    Compounding compounding = Compounded;
    Frequency frequency = Annual;
    Real accuracy = 1.0e-15;
    Size maxEvaluations = 10000;
    Rate yield = zeroBond.yield(cleanPrice, dayCounter, compounding, frequency,
                                zeroBond.settlementDate(issueDate), accuracy, maxEvaluations);
    std::cout << "yield from QuantLib " << (double)yield << "\n";
    // and check the results manually
    double yearFraction2014 = (Date(31, December, 2014) - Date(7, October, 2014) + 1);
    yearFraction2014 /= 365;
    double yearFraction2016 = (Date(4, October, 2016) - Date(1, January, 2016));
    yearFraction2016 /= 366;
    double t = yearFraction2014 + 1.0 + yearFraction2016;
    double tDC = dayCounter.yearFraction(Date(7, October, 2014), Date(4, October, 2016));
    assert(abs(t - tDC) < accuracy);
    double yield2 = exp((log(redemption) - log(cleanPrice)) / t) - 1;
    std::cout << "yield from manual check " << yield2 << "\n";
    assert(abs((double)yield - yield2) < accuracy);
}