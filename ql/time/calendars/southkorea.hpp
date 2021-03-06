/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 FIMAT Group
 Copyright (C) 2007 StatPro Italia srl
 Copyright (C) 2008 Charles Chongseok Hyun

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file southkorea.hpp
    \brief South Korean calendars
*/

#ifndef quantlib_south_korean_calendar_hpp
#define quantlib_south_korean_calendar_hpp

#include <ql/time/calendar.hpp>

namespace QuantLib {

    //! South Korean calendars
    /*! Public holidays:
        <ul>
        <li>Saturdays</li>
        <li>Sundays</li>
        <li>New Year's Day, January 1st</li>
        <li>Independence Day, March 1st</li>
        <li>Arbour Day, April 5th (until 2005)</li>
        <li>Labour Day, May 1st</li>
        <li>Children's Day, May 5th</li>
        <li>Memorial Day, June 6th</li>
        <li>Constitution Day, July 17th (until 2007)</li>
        <li>Liberation Day, August 15th</li>
        <li>National Fondation Day, October 3th</li>
        <li>Christmas Day, December 25th</li>
        </ul>

        Other holidays for which no rule is given
        (data available for 2004-2012 only:)
        <ul>
        <li>Lunar New Year, the last day of the previous lunar year,
            January 1st, 2nd in lunar calendar</li>
        <li>Election Days</li>
        <li>National Assemblies</li>
        <li>Presidency</li>
        <li>Regional Election Days</li>
        <li>Buddha's birthday, April 8th in lunar calendar</li>
        <li>Harvest Moon Day, August 14th, 15th, 16th in lunar calendar</li>
        </ul>

        Holidays for the Korea exchange
        (data from
        <http://eng.krx.co.kr/m8/m8_6/m8_6_1/JHPENG08006_01.jsp> or
        <http://www.dooriworld.com/daishin/holiday/holiday.html>):
        <ul>
        <li>Public holidays as listed above</li>
        <li>Year-end closing</li>
        </ul>

        \ingroup calendars
    */
    class SouthKorea : public Calendar {
      private:
        class SettlementImpl : public Calendar::Impl {
          public:
            std::string name() const { return "South-Korean settlement"; }
            bool isWeekend(Weekday) const;
            bool isBusinessDay(const Date&) const;
        };
        class KrxImpl : public SettlementImpl {
          public:
            std::string name() const { return "South-Korea exchange"; }
            bool isBusinessDay(const Date&) const;
        };
      public:
        enum Market { Settlement,  //!< Public holidays
                      KRX          //!< Korea exchange
        };
        SouthKorea(Market m = KRX);
    };

}


#endif
