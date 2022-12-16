/**
 * @file zero-bond-yield.cpp
 * @author Oliver Ofenloch (57812959+ofenloch@users.noreply.github.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-16
 * 
 * @copyright Copyright (c) 2022
 * 
 * This example is taken from https://letyourmoneygrow.com/yetanotherquant/QuantLib/book/BookQuantLib.pdf
 * 
 * 
 */

#include <ql/quantlib.hpp>
#include <iostream>
#include <math.h>
using namespace QuantLib;
int main(int argc, char* arv[])
{
    // create a zero coupon bond
    Date issueDate(2, October, 2014);
    Natural settlementDays = 2;
    Calendar calendar = Germany(Germany::Settlement);
    BusinessDayConvention paymentConvention = Following;
    Real faceAmount = 100;
    Real redemption = 100;
    Date maturityDate(2, October, 2016);
    // To set up a  zero bond we need the following parameters:
    // 1) issueDate is the date on which the bond is issued
    // 2) settlementDays are days that the backoffice needs to commit the transaction. In other words, if an
    //    investor buys a bond on issueDate, he will get it on (issueDate + settlementDays). In Germany, two
    //    settlement days are common for the bonds. This rule is also known as T+2. Note that the interest starts
    //    accruing from (issueDate + settlementDays), not from the issueDate! The smallest time span for which the
    //    interest is accrued is one day.
    // 3) calendar and paymentConvention. Normally, the interest accrues both for business days and holidays. But
    //    what if a settlement or redemption falls on a holiday (and how we detect it)?! The holidays are defined by
    //    calendar; ql/time/calendars contains the calendars for many countries. In our example we considered the
    //    basic German calendar (Germany::Settlement). Besides, there are special calendars for the most important
    //    German exchanges: FrankfurtStockExchange, Xetra, Eurex, Euwax (have a look at germany.hpp).
    //    The paymentConvention tells us what to do if a payment (or settlement) falls on a holiday. In our case the
    //    convention is "Following", which means that the date will be moved to the nearest following business day.
    //    In our example the issueDate (2.10.2014) is Thursday. Thus T+2, i.e. the settlement date (4.10.2014) falls on
    //    Saturday, which is not a business day. We need two business date for the settlement, so the settlement is,
    //    according to the "Following" rule is 7.10.2014 (Tuesday).
    //    Moreover, the maturity date (2.10.2016) is also a non-business day (Sunday). According to the "Following"
    //    rule the bond should be redeemed on 3.10.2014 ... but it is a holiday according to our calendar
    //    thus the maturity date is shifted to 4.10.2014.
    // 4) faceAmount and redemption are self-explanatory. It is a pretty rare case that they are different but 
    //    it is possible.
    // 5) maturityDate is the date on which the redemption is paid. In case of a coupon bond the last coupon is
    //    usually also paid on maturity date. Note that the interest is usually accrued for the settlement day but not
    //    for the maturity day!
    //    Further parameters we need are not the member variables of the ZeroCouponBond class, yet we need
    //    them to compute the bond yield.
    // 6) cleanPrice is (in case of a zero bond) just its market value. In case of a coupon bond one needs to
    //    distinguish between clean and dirty price due to partly accrued but not yet paid coupon. Clean price is more
    //    comfortable because it does not jump on the coupon payment dates.
    // 7) dayCounter, as its name implies, counts the number of [interest] days and the year fraction between two
    //    dates. On one hand we said that a day is the basic time span for which the interest accrues. On the other
    //    hand a year is the basic time unit for the interest rate arithmetic.
    //    In our cased we used ActualActual::ISDA. It works as follows: first of all it splits the time span by years: in
    //    our example to [7.10.2014, 31.12.2014] + [1.01.2015, 31.12.2015] + [1.01.2016, 4.10.2016). There are 86
    //    days in [7.10.2014, 31.12.2014] and 2014 is not a leap year, so we divide 86 by 365 to get the year fraction.
    //    The year 2015 is completely within the time span, so it gives the year fraction equal to 1.0 (and it does not
    //    matter whether it's a leap year or not). Finally, there are 277 interest days in [1.01.2016, 4.10.2016) and
    //    2016 is a leap year. So we divide 277 by 366 and the whole time span is equal to 86/365 + 1.0 + 277/366 =
    //    1.9924 years.
    // 8) compounding means the interest rate compounding rule. QuantLib supports Simple ,
    //    Compounded , Continuous and SimpleThenCopounded rules. Compounded is the most
    //    common rule in Germany.
    // 9) frequency means something periodical, e.g. how often the coupons are paid. A zero bond has no coupons
    //    but still the frequency may be relevant because by Compounded rule we actually have. See lines 51-53 
    //    in ql/interestrate.cpp:
    //                            case Compounded: return std::pow(1.0+r_/freq_, freq_*t);
    //    Nice to know, since it is not mentioned in ql/compounding.hpp!
    // 10) accuracy and maxEvaluations determine the numeral precision of the bond yield. In Germany it is
    //     required to calculate with 1.0e-6 precision but since in our case we check the yield by means of a closed
    //     form solution, we set very high precision to check whether the numerical iteration method can achieve it (it
    //     can).
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

    return 0;
}