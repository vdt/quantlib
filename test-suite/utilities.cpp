
/*
 Copyright (C) 2003, 2004 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include "utilities.hpp"
#include <ql/Instruments/payoffs.hpp>
#include <ql/TermStructures/flatforward.hpp>
#include <ql/Volatilities/blackconstantvol.hpp>

#define CHECK_DOWNCAST(Derived,Description) { \
    boost::shared_ptr<Derived> hd = boost::dynamic_pointer_cast<Derived>(h); \
    if (hd) \
        return Description; \
}

namespace QuantLib {

    std::string payoffTypeToString(const boost::shared_ptr<Payoff>& h) {

        CHECK_DOWNCAST(PlainVanillaPayoff, "plain-vanilla");
        CHECK_DOWNCAST(CashOrNothingPayoff, "cash-or-nothing");
        CHECK_DOWNCAST(AssetOrNothingPayoff, "asset-or-nothing");
        CHECK_DOWNCAST(SuperSharePayoff, "super-share");
        CHECK_DOWNCAST(GapPayoff, "gap");

        QL_FAIL("unknown payoff type");
    }


    std::string exerciseTypeToString(const boost::shared_ptr<Exercise>& h) {

        CHECK_DOWNCAST(EuropeanExercise, "European");
        CHECK_DOWNCAST(AmericanExercise, "American");
        CHECK_DOWNCAST(BermudanExercise, "Bermudan");

        QL_FAIL("unknown exercise type");
    }


    boost::shared_ptr<TermStructure> 
    flatRate(const Date& today,
             const boost::shared_ptr<Quote>& forward,
             const DayCounter& dc) {
        return boost::shared_ptr<TermStructure>(
                       new FlatForward(today, today, 
                                       RelinkableHandle<Quote>(forward), dc));
    }

    boost::shared_ptr<TermStructure> 
    flatRate(const Date& today, Rate forward, const DayCounter& dc) {
        return flatRate(
               today, boost::shared_ptr<Quote>(new SimpleQuote(forward)), dc);
    }

    boost::shared_ptr<BlackVolTermStructure> 
    flatVol(const Date& today,
            const boost::shared_ptr<Quote>& vol,
            const DayCounter& dc) {
        return boost::shared_ptr<BlackVolTermStructure>(
                      new BlackConstantVol(today, 
                                           RelinkableHandle<Quote>(vol), dc));
    }

    boost::shared_ptr<BlackVolTermStructure> 
    flatVol(const Date& today, Volatility vol, const DayCounter& dc) {
        return flatVol(
                   today, boost::shared_ptr<Quote>(new SimpleQuote(vol)), dc);
    }

    Real relativeError(Real x1, Real x2, Real reference) {
        if (reference != 0.0)
            return QL_FABS(x1-x2)/reference;
        else
            // fall back to absolute error
            return QL_FABS(x1-x2);
    }

}
