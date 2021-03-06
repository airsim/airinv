#ifndef __AIRINV_BOM_DCPEVENTSTRUCT_HPP
#define __AIRINV_BOM_DCPEVENTSTRUCT_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
#include <vector>
// StdAir
#include <stdair/stdair_demand_types.hpp>
#include <stdair/stdair_inventory_types.hpp>
#include <stdair/basic/StructAbstract.hpp>
#include <stdair/basic/BasParserTypes.hpp>
// AirInv
#include <airinv/AIRINV_Types.hpp>

namespace AIRINV {

  /** Utility Structure for the parsing of Flight-Period structures. */
  struct DCPEventStruct : public stdair::StructAbstract {
  public:
 
    /** Default constructor. */
    DCPEventStruct ();
    
    /** Get the date from the staging details. */
    stdair::Date_T getDate() const;

    /** Get the time from the staging details. */
    stdair::Duration_T getTime() const;    

    /** Display of the structure. */
    const std::string describe() const;  

    /** Get the size of the airline code list. */
    const unsigned int getAirlineListSize () const {
      return _airlineCodeList.size();
    }

    /** Get the size of the class code list. */
    const unsigned int getClassCodeListSize () const {
      return _classCodeList.size();
    }
  
    /** Get the first airline code. */
    const stdair::AirlineCode_T& getFirstAirlineCode () const;

    /** Initialise the internal iterators on airline code:
	The current iterator is set on the first airline code,
	the next iterator is set on the second one. */
    void beginAirline ();

    /** States whether or not the end of the (airline code)
        list has been reached. */
    bool hasNotReachedEndAirline () const;

    /** Get the current element (airline code). */
    stdair::AirlineCode_T getCurrentAirlineCode () const;

    /** Iterate for one element (airline code): 
        increment both internal iterators on Buckets. */
    void iterateAirline ();
    
    /** Get the first class code list as a string. */
    const std::string& getFirstClassCode () const;

    /** Initialise the internal iterators on class code:
	The current iterator is set on the first class code,
	the next iterator is set on the second one. */
    void beginClassCode ();

    /** States whether or not the end of the (class code)
        list has been reached. */
    bool hasNotReachedEndClassCode () const;

    /** Get the current element (class code). */
    std::string getCurrentClassCode () const;

    /** Iterate for one element (classCode): 
        increment both internal iterators on Buckets. */
    void iterateClassCode ();

  public:
    // ////////////////// Attributes /////////////////
    /** Staging Date. */
    stdair::year_t _itYear;
    stdair::month_t _itMonth;
    stdair::day_t _itDay;
      
    /** Staging Time. */
    //long _itHours;
    stdair::hour_t _itHours;
    stdair::minute_t _itMinutes;
    stdair::second_t _itSeconds;   

    /** Iterator for the current airline code list. */
    stdair::AirlineCodeList_T::iterator _itCurrentAirlineCode;

    /** Iterator for the current class code. */
    stdair::ClassList_StringList_T::iterator _itCurrentClassCode;
    
    /** Origin. */
    stdair::AirportCode_T _origin;

    /** Destination. */
    stdair::AirportCode_T _destination;

    /** Start Range date available for this DCP event.*/
    stdair::Date_T _dateRangeStart;

    /** Start Range date available for this DCP event.*/
    stdair::Date_T _dateRangeEnd;
    
    /** Start time from the time range available for this DCP event.*/
    stdair::Duration_T _timeRangeStart;
     
    /** End time from the time range available for this DCP event.*/
    stdair::Duration_T _timeRangeEnd;

    /** Cabin code. */
    stdair::CabinCode_T _cabinCode;
    
    /** Point-of-sale. */
    stdair::CityCode_T _pos;
    
    /** Channel distribution. */
    stdair::ChannelLabel_T _channel;
    
    /** Number of days that the ticket is sold before the  flightDate.*/
    stdair::DayDuration_T _advancePurchase;
    
    /** Boolean saying whether a saturday is considered during the stay .*/
    stdair::SaturdayStay_T _saturdayStay;

    /** Boolean saying whether the change fees option is requested or not.*/
    stdair::ChangeFees_T _changeFees;

    /** Boolean saying whether the refundable option is requested or not.*/
    stdair::NonRefundable_T _nonRefundable;
     
    /** Number of days that the customer spent into the destination city.*/
    stdair::DayDuration_T _minimumStay;
    
    /** Price.*/
    stdair::PriceValue_T _DCP;

    /** Airline code */
    stdair::AirlineCode_T _airlineCode;

    /** Code */
    stdair::ClassCode_T _classCode;

    /** Airline Code List*/
    stdair::AirlineCodeList_T _airlineCodeList;

    /** Numbers of different Airline Codes*/
    //unsigned long int _nbOfAirlines;

    /** Class Code List*/
    stdair::ClassList_StringList_T _classCodeList;

  };

}
#endif // __AIRINV_BOM_DCPEVENTSTRUCT_HPP
