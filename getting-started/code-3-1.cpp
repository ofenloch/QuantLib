/**
 * @file code-3-1.cpp
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

#include <iostream>
#include <math.h>
void main() {
    // market data
    double coupons[] = {4.0, 5.0, 5.0, 4.0, 5.0, 6.0, 6.0, 6.0, 6.0, 5.0};
    double pvs[] = {101.5, 103.0, 102.5, 98.0, 101.0, 105.0, 104.5, 104.0, 104.0, 96.0};
    const double FACE_VALUE = 100.0;
    // iteratively calculate yields by bootstrapping algorithm
    const int n = sizeof(coupons) / sizeof(coupons[0]);
    double yields[n];
    yields[0] = (FACE_VALUE + coupons[0]) / pvs[0] - 1;
    std::cout << "yield 1: " << yields[0] << "\n";
    for (int i = 1; i < n; i++) {
        double couponsPV = 0.0;
        for (int c = 1; c <= i; c++) {
            couponsPV += coupons[c] / pow((1.0 + yields[c - 1]), c);
        }
        yields[i] = exp(log((FACE_VALUE + coupons[i]) / (pvs[i] - couponsPV)) / (i + 1)) - 1;
        std::cout << "yield " << i + 1 << ": " << yields[i] << "\n";
    }
    // answer the question
    const double bond11Coupon = 3.0;
    const int bond11Maturity = 9;
    double bond11PV = 0.0;
    for (int i = 1; i <= bond11Maturity; i++) {
        bond11PV += bond11Coupon / pow((1.0 + yields[i - 1]), i);
    }
    bond11PV += FACE_VALUE / pow((1.0 + yields[bond11Maturity - 1]), bond11Maturity);
    std::cout << "PV of the Bond11 is " << bond11PV << "\n";
}