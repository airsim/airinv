// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// StdAir
#include <stdair/basic/BasConst_General.hpp>
#include <stdair/service/Logger.hpp>
// AIRINV
#include <airinv/AIRINV_Types.hpp>
#include <airinv/bom/FlightDateStruct.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  FlightDateStruct_T::FlightDateStruct_T ()
    : _flightDate (stdair::DEFAULT_DATE),
      _flightTypeCode (FlightTypeCode::HIDDEN),
      _legAlreadyDefined (false), _itSeconds (0) {
  }

  // //////////////////////////////////////////////////////////////////////
  stdair::Date_T FlightDateStruct_T::getDate() const {
    return stdair::Date_T (_itYear + 2000, _itMonth, _itDay);
  }

  // //////////////////////////////////////////////////////////////////////
  stdair::Duration_T FlightDateStruct_T::getTime() const {
    return boost::posix_time::hours (_itHours)
      + boost::posix_time::minutes (_itMinutes)
      + boost::posix_time::seconds (_itSeconds);
  }
  
  // //////////////////////////////////////////////////////////////////////
  const std::string FlightDateStruct_T::describe() const {
    std::ostringstream ostr;
    ostr << _airlineCode << _flightNumber << ", " << _flightDate
         << " (" << _flightTypeCode << ")" << std::endl;
      
    for (LegStructList_T::const_iterator itLeg = _legList.begin();
         itLeg != _legList.end(); ++itLeg) {
      const LegStruct_T& lLeg = *itLeg;
      ostr << lLeg.describe();
    }

    for (SegmentStructList_T::const_iterator itSegment = _segmentList.begin();
         itSegment != _segmentList.end(); ++itSegment) {
      const SegmentStruct_T& lSegment = *itSegment;
      ostr << lSegment.describe();
    }

    //ostr << "[Debug] - Staging Leg: ";
    //ostr << _itLeg.describe();
    //ostr << "[Debug] - Staging Cabin: ";
    //ostr << _itCabin.describe();

    return ostr.str();
  }

  // //////////////////////////////////////////////////////////////////////
  void FlightDateStruct_T::addAirport (const stdair::AirportCode_T& iAirport) {
    AirportList_T::const_iterator itAirport = _airportList.find (iAirport);
    if (itAirport == _airportList.end()) {
      // Add the airport code to the airport set
      const bool insertSuccessful = _airportList.insert (iAirport).second;

      if (insertSuccessful == false) {
        // TODO: throw an exception
      }
          
      // Add the airport code to the airport vector
      _airportOrderedList.push_back (iAirport);
    }
  }

  // //////////////////////////////////////////////////////////////////////
  void FlightDateStruct_T::buildSegments () {
    // The list of airports encompasses all the airports on which
    // the flight takes off or lands. Moreover, that list is
    // time-ordered: the first airport is the initial departure of
    // the flight, and the last airport is the eventual point of
    // rest of the flight.
    // Be l the size of the ordered list of airports.
    // We want to generate all the segment combinations from the legs
    // and, hence, from all the possible (time-ordered) airport pairs.
    // Thus, we both iterator on i=0...l-1 and j=i+1...l
    assert (_airportOrderedList.size() >= 2);

    _segmentList.clear();
    for (AirportOrderedList_T::const_iterator itAirport_i =
           _airportOrderedList.begin();
         itAirport_i != _airportOrderedList.end()-1; ++itAirport_i) {
      for (AirportOrderedList_T::const_iterator itAirport_j = itAirport_i + 1;
           itAirport_j != _airportOrderedList.end(); ++itAirport_j) {
        SegmentStruct_T lSegmentStruct;
        lSegmentStruct._boardingPoint = *itAirport_i;
        lSegmentStruct._offPoint = *itAirport_j;
          
        _segmentList.push_back (lSegmentStruct);
      }
    }

    // Clear the lists of airports, so that it is ready for the next flight
    _airportList.clear();
    _airportOrderedList.clear();
  }
      
  // //////////////////////////////////////////////////////////////////////
  void FlightDateStruct_T::
  addSegmentCabin (const SegmentStruct_T& iSegment,
                   const SegmentCabinStruct_T& iCabin) {
    // Retrieve the Segment structure corresponding to the (boarding, off) point
    // pair.
    SegmentStructList_T::iterator itSegment = _segmentList.begin();
    for ( ; itSegment != _segmentList.end(); ++itSegment) {
      const SegmentStruct_T& lSegment = *itSegment;

      const stdair::AirportCode_T& lBoardingPoint = iSegment._boardingPoint;
      const stdair::AirportCode_T& lOffPoint = iSegment._offPoint;
      if (lSegment._boardingPoint == lBoardingPoint
          && lSegment._offPoint == lOffPoint) {
        break;
      }
    }

    // If the segment key (airport pair) given in the schedule input file
    // does not correspond to the leg (boarding, off) points, throw an exception
    // so that the user knows the schedule input file is corrupted.
    if (itSegment == _segmentList.end()) {
      STDAIR_LOG_ERROR ("Within the inventory input file, there is a "
                        << "flight for which the airports of segments "
                        << "and those of the legs do not correspond.");
      throw SegmentDateNotFoundException();
    }

    // Add the Cabin structure to the Segment Cabin structure.
    assert (itSegment != _segmentList.end());
    SegmentStruct_T& lSegment = *itSegment;
    lSegment._cabinList.push_back (iCabin);
  }
    
  // //////////////////////////////////////////////////////////////////////
  void FlightDateStruct_T::
  addSegmentCabin (const SegmentCabinStruct_T& iCabin) {
    // Iterate on all the Segment structures (as they get the same cabin
    // definitions)
      
    for (SegmentStructList_T::iterator itSegment = _segmentList.begin();
         itSegment != _segmentList.end(); ++itSegment) {
      SegmentStruct_T& lSegment = *itSegment;

      lSegment._cabinList.push_back (iCabin);
    }
  }

  // //////////////////////////////////////////////////////////////////////
  void FlightDateStruct_T::
  addFareFamily (const SegmentStruct_T& iSegment,
                 const SegmentCabinStruct_T& iCabin,
                 const FareFamilyStruct_T& iFareFamily) {
    // Retrieve the Segment structure corresponding to the (boarding, off) point
    // pair.
    SegmentStructList_T::iterator itSegment = _segmentList.begin();
    for ( ; itSegment != _segmentList.end(); ++itSegment) {
      const SegmentStruct_T& lSegment = *itSegment;

      const stdair::AirportCode_T& lBoardingPoint = iSegment._boardingPoint;
      const stdair::AirportCode_T& lOffPoint = iSegment._offPoint;
      if (lSegment._boardingPoint == lBoardingPoint
          && lSegment._offPoint == lOffPoint) {
        break;
      }
    }

    // If the segment key (airport pair) given in the schedule input file
    // does not correspond to the leg (boarding, off) points, throw an exception
    // so that the user knows the schedule input file is corrupted.
    if (itSegment == _segmentList.end()) {
      STDAIR_LOG_ERROR ("Within the schedule input file, there is a flight "
                          << "for which the airports of segments and "
                          << "those of the legs do not correspond.");
      throw SegmentDateNotFoundException();
    }

    // Add the Cabin structure to the Segment Cabin structure.
    assert (itSegment != _segmentList.end());
    SegmentStruct_T& lSegment = *itSegment;

    // Retrieve the Segment cabin structure given the cabin code
    SegmentCabinStructList_T::iterator itCabin = lSegment._cabinList.begin();
    for ( ; itCabin != lSegment._cabinList.end(); ++itCabin) {
      const SegmentCabinStruct_T& lCabin = *itCabin;

      const stdair::CabinCode_T& lCabinCode = lCabin._cabinCode;
      if (iCabin._cabinCode == lCabinCode) {
        break;
      }
    }
    // If the segmentCabin key (cabin code) given in the schedule input file
    // does not correspond to the stored cabin codes, throw an exception
    // so that the user knows the schedule input file is corrupted.
    if (itCabin == lSegment._cabinList.end()) {
      STDAIR_LOG_ERROR ("Within the schedule input file, there is a flight "
                          << "for which the cabin code does not exist.");
      throw SegmentDateNotFoundException();
    }
    // Add the Cabin structure to the Segment Cabin structure.
    assert (itCabin != lSegment._cabinList.end());
    SegmentCabinStruct_T& lCabin = *itCabin;
    lCabin._fareFamilies.push_back(iFareFamily);
  }
    
  // //////////////////////////////////////////////////////////////////////
  void FlightDateStruct_T::
  addFareFamily (const SegmentCabinStruct_T& iCabin,
                 const FareFamilyStruct_T& iFareFamily) {
    // Iterate on all the Segment structures (as they get the same cabin
    // definitions)
      
    for (SegmentStructList_T::iterator itSegment = _segmentList.begin();
         itSegment != _segmentList.end(); ++itSegment) {
      SegmentStruct_T& lSegment = *itSegment;

      // Retrieve the Segment cabin structure given the cabin code
      SegmentCabinStructList_T::iterator itCabin = lSegment._cabinList.begin();
      for ( ; itCabin != lSegment._cabinList.end(); ++itCabin) {
        const SegmentCabinStruct_T& lCabin = *itCabin;

        const stdair::CabinCode_T& lCabinCode = lCabin._cabinCode;
        if (iCabin._cabinCode == lCabinCode) {
          break;
        }
      }
      // If the segmentCabin key (cabin code) given in the schedule input file
      // does not correspond to the stored cabin codes, throw an exception
      // so that the user knows the schedule input file is corrupted.
      if (itCabin == lSegment._cabinList.end()) {
        STDAIR_LOG_ERROR ("Within the schedule input file, there is a flight"
                            << " for which the cabin code does not exist.");
        throw SegmentDateNotFoundException();
      }
      // Add the Cabin structure to the Segment Cabin structure.
      assert (itCabin != lSegment._cabinList.end());
      SegmentCabinStruct_T& lCabin = *itCabin;
      lCabin._fareFamilies.push_back(iFareFamily);
    }
  }

}
