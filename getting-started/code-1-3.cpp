/**
 * @file code-1-3.cpp
 * @author Oliver Ofenloch (57812959+ofenloch@users.noreply.github.com)
 * @brief
 * @version 0.1
 * @date 2022-12-16
 *
 * @copyright Copyright (c) 2022
 * 
  * This is the original version of the code sample 1.3 in
 * https://letyourmoneygrow.com/yetanotherquant/QuantLib/book/BookQuantLib.pdf
 * by Vasily Nekrasov.
 *
 */

#include <ql/quantlib.hpp>
#include <iostream>
#include <math.h>
using namespace QuantLib;
void main() {
    // bond parameters
    Date issueDate(2, October, 2014);
    Natural settlementDays = 2;
    Calendar calendar = Germany(Germany::Settlement);
    BusinessDayConvention paymentConvention = Following;
    BusinessDayConvention accrualConvention = Unadjusted;
    Real faceAmount = 100;
    Real redemption = 100;
    Date maturityDate(2, October, 2016);
    DayCounter dayCounter = ActualActual(ActualActual::ISDA);
    Frequency frequency = Annual;
    // make schedules via constructor
    Schedule schedule(issueDate, maturityDate, Period(frequency), calendar, accrualConvention,
                      accrualConvention, DateGeneration::Backward, false);
    //... and via Named Parameter Idiom helper
    Schedule schedule2 = MakeSchedule()
                             .from(issueDate)
                             .to(maturityDate)
                             .withFrequency(frequency)
                             .withCalendar(calendar)
                             .withConvention(accrualConvention)
                             .withTerminationDateConvention(accrualConvention)
                             .backwards()
                             .endOfMonth(false);
    // check that the schedules are identical
    std::vector<Date> datesSchedule = schedule.dates();
    std::vector<Date> datesSchedule2 = schedule2.dates();
    assert(datesSchedule.size() == datesSchedule2.size());
    for (unsigned int d = 0; d < datesSchedule.size(); d++) {
        assert(datesSchedule[d] == datesSchedule2[d]);
        std::cout << "Schedule Date " << d + 1 << " " << datesSchedule[d] << "\n";
    }
    // coupons
    std::vector<Rate> coupons(datesSchedule.size() - 1, 0.05);
    // bonds
    FixedRateBond couponBond(settlementDays, faceAmount, schedule, coupons, dayCounter,
                             paymentConvention, redemption, issueDate, calendar);
    FixedRateBond couponBond2(settlementDays, calendar, faceAmount, issueDate, maturityDate,
                              Period(frequency), coupons, dayCounter, accrualConvention,
                              paymentConvention, redemption, issueDate, Date(),
                              DateGeneration::Backward, false, calendar);
    // bond cashflow
    Leg bondCashflow = couponBond.cashflows();
    for (unsigned int d = 0; d < bondCashflow.size(); d++)
        std::cout << "Payment " << d + 1 << ": " << (bondCashflow[d])->date() << " "
                  << (bondCashflow[d])->amount() << "\n";
    // check that both bonds pay the same interest
    for (Date d = issueDate; d <= maturityDate; d++) {
        double accuredInterest = couponBond.accruedAmount(d);
        double accuredInterest2 = couponBond2.accruedAmount(d);
        std::cout << "Interest accrued on " << d << " " << accuredInterest << "\n";
        assert(abs(accuredInterest - accuredInterest2) < 1.0e-15);
    }
}