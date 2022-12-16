/**
 * @file code-3-2.cpp
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
 *
 */

#include <ql/quantlib.hpp>
#include <iostream>
#include <math.h>
using namespace QuantLib;
#define LENGTH(a) (sizeof(a) / sizeof(a[0]))
void main() {
    const Size numberOfBonds = 11;
    Real cleanPrices[] = {100.0, 104.2, 110, 103, 105, 100, 103, 96, 103, 92, 98};
    Real lengths[] = {1.0, 1.7, 2.1, 3, 3.8, 4.6, 5.6, 6.5, 7.2, 8, 9};
    Real coupons[] = {0.02, 0.04, 0.05, 0.04, 0.05, 0.04, 0.05, 0.04, 0.05, 0.04, 0.05};
    const double FACE_VALUE = 100.0;
    const double COUPON = 4.0;
    const double MATURITY = 5.4;
    std::vector<boost::shared_ptr<SimpleQuote>> quote;
    for (Size i = 0; i < numberOfBonds; i++) {
        boost::shared_ptr<SimpleQuote> cp(new SimpleQuote(cleanPrices[i]));
        quote.push_back(cp);
    }
    RelinkableHandle<Quote> quoteHandle[numberOfBonds];
    for (Size i = 0; i < numberOfBonds; i++) {
        quoteHandle[i].linkTo(quote[i]);
    }
    Frequency frequency = Annual;
    DayCounter dc = SimpleDayCounter();
    BusinessDayConvention accrualConvention = ModifiedFollowing;
    BusinessDayConvention convention = ModifiedFollowing;
    Real redemption = 100.0;
    Calendar calendar = NullCalendar();
    Date today = calendar.adjust(Date::todaysDate());
    Date origToday = today;
    Settings::instance().evaluationDate() = today;
    Natural bondSettlementDays = 0;
    Natural curveSettlementDays = 0;
    Date bondSettlementDate = calendar.advance(today, bondSettlementDays * Days);
    std::cout << "Today's date: " << today << std::endl;
    std::cout << "Bonds' settlement date: " << bondSettlementDate << std::endl;
    std::vector<boost::shared_ptr<BondHelper>> instrumentsA; // for FittedBondDiscountCurve
    std::vector<boost::shared_ptr<RateHelper>> instrumentsB; // for PiecewiseYieldCurve
    for (Size j = 0; j < LENGTH(lengths); j++) {
        Date maturity = bondSettlementDate;
        while (dc.yearFraction(bondSettlementDate, maturity) < lengths[j]) {
            maturity++;
        }
        Schedule schedule(bondSettlementDate, maturity, Period(frequency), calendar,
                          accrualConvention, accrualConvention, DateGeneration::Backward, false);
        boost::shared_ptr<BondHelper> helperA(
            new FixedRateBondHelper(quoteHandle[j], bondSettlementDays, FACE_VALUE, schedule,
                                    std::vector<Rate>(1, coupons[j]), dc, convention, redemption));
        boost::shared_ptr<RateHelper> helperB(
            new FixedRateBondHelper(quoteHandle[j], bondSettlementDays, FACE_VALUE, schedule,
                                    std::vector<Rate>(1, coupons[j]), dc, convention, redemption));
        instrumentsA.push_back(helperA);
        instrumentsB.push_back(helperB);
    }
    bool constrainAtZero = true;
    Real tolerance = 1.0e-10;
    Size max = 5000;
    //////////////The main solution (in context of the task)///////////////////////
    boost::shared_ptr<YieldTermStructure> ts0(new PiecewiseYieldCurve<ZeroYield, Linear>(
        curveSettlementDays, calendar, instrumentsB, dc));
    double d5_4 = ts0->discount(MATURITY);
    double d4_4 = ts0->discount(MATURITY - 1);
    double d3_4 = ts0->discount(MATURITY - 2);
    double d2_4 = ts0->discount(MATURITY - 3);
    double d1_4 = ts0->discount(MATURITY - 4);
    double d0_4 = ts0->discount(MATURITY - 5);
    double f = MATURITY - 5; // fraction of the 1st coupon
    double bond11PV = COUPON * (d5_4 + d4_4 + d3_4 + d2_4 + d1_4 + d0_4 * f) + FACE_VALUE * d5_4;
    std::cout << "PV Bond11 (<ZeroYield, Linear> interpolation): " << bond11PV << "\n";
    //////////////Alternative Solutions///////////////////////
    //<Discount, LogLinear> is more common in practice than <ZeroYield, Linear>
    boost::shared_ptr<YieldTermStructure> ts1(new PiecewiseYieldCurve<Discount, LogLinear>(
        curveSettlementDays, calendar, instrumentsB, dc));
    d5_4 = ts1->discount(MATURITY);
    d4_4 = ts1->discount(MATURITY - 1);
    d3_4 = ts1->discount(MATURITY - 2);
    d2_4 = ts1->discount(MATURITY - 3);
    d1_4 = ts1->discount(MATURITY - 4);
    d0_4 = ts1->discount(MATURITY - 5);
    f = MATURITY - 5;
    bond11PV = COUPON * (d5_4 + d4_4 + d3_4 + d2_4 + d1_4 + d0_4 * f) + FACE_VALUE * d5_4;
    std::cout << "PV Bond11 (<Discount, LogLinear> interpolation): " << bond11PV << "\n";
    ///////////Svensson Fitting is used by German Central Bank (Bundesbank)////////////
    boost::shared_ptr<FittedBondDiscountCurve> ts2(new FittedBondDiscountCurve(
        curveSettlementDays, calendar, instrumentsA, dc, SvenssonFitting(), tolerance, max));
    d5_4 = ts2->discount(MATURITY);
    d4_4 = ts2->discount(MATURITY - 1);
    d3_4 = ts2->discount(MATURITY - 2);
    d2_4 = ts2->discount(MATURITY - 3);
    d1_4 = ts2->discount(MATURITY - 4);
    d0_4 = ts2->discount(MATURITY - 5);
    f = MATURITY - 5;
    bond11PV = COUPON * (d5_4 + d4_4 + d3_4 + d2_4 + d1_4 + d0_4 * f) + FACE_VALUE * d5_4;
    std::cout << "PV Bond11 (Svensson interpolation): " << bond11PV << "\n";
    //////The same via bond pricing engine//////////
    RelinkableHandle<YieldTermStructure> yieldTermStructure;
    boost::shared_ptr<PricingEngine> bondEngine(new DiscountingBondEngine(yieldTermStructure));
    Date maturity = bondSettlementDate;
    while (dc.yearFraction(bondSettlementDate, maturity) < MATURITY) {
        maturity++;
    }
    Schedule schedule(bondSettlementDate, maturity, Period(frequency), calendar, accrualConvention,
                      accrualConvention, DateGeneration::Backward, false);
    FixedRateBond bond11(bondSettlementDays, FACE_VALUE, schedule,
                         std::vector<Rate>(5, COUPON / 100.0), dc, convention, redemption);
    bond11.setPricingEngine(bondEngine);
    yieldTermStructure.linkTo(ts0);
    std::cout << "PV Bond11 (pricing engine, ts0): " << bond11.NPV() << "\n";
    yieldTermStructure.linkTo(ts1);
    std::cout << "PV Bond11 (pricing engine, ts1): " << bond11.NPV() << "\n";
    yieldTermStructure.linkTo(ts2);
    std::cout << "PV Bond11 (pricing engine, ts2): " << bond11.NPV() << "\n";
}