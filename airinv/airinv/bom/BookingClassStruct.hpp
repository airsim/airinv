#ifndef __AIRINV_BOM_BOOKINGCLASSSTRUCT_HPP
#define __AIRINV_BOM_BOOKINGCLASSSTRUCT_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
#include <vector>
// STDAIR
#include <stdair/STDAIR_Types.hpp>
#include <stdair/basic/StructAbstract.hpp>
// AIRINV
#include <airinv/AIRINV_Types.hpp>

// Forward declarations
namespace stdair {
  class BookingClass;
}

namespace AIRINV {

  /** Utility Structure for the parsing of BookingClass structures. */
  struct BookingClassStruct_T : public stdair::StructAbstract {
    // Attributes
    stdair::ClassCode_T _classCode;
    stdair::SubclassCode_T _subclassCode;
    stdair::AuthorizationLevel_T _cumulatedProtection;
    stdair::AuthorizationLevel_T _protection;
    stdair::OverbookingRate_T _noShowPercentage;
    stdair::OverbookingRate_T _overbookingPercentage;
    stdair::NbOfBookings_T _nbOfBookings;
    stdair::NbOfBookings_T _groupNbOfBookings;
    stdair::NbOfBookings_T _groupPendingNbOfBookings;
    stdair::NbOfBookings_T _staffNbOfBookings;
    stdair::NbOfBookings_T _wlNbOfBookings;
    stdair::NbOfBookings_T _etb;
    stdair::Availability_T _netClassAvailability;
    stdair::Availability_T _segmentAvailability;
    stdair::Availability_T _netRevenueAvailability;
    
    /** Fill the BookingClass objects with the attributes of the
        BookingClassStruct. */
    void fill (stdair::BookingClass&) const;
      
    /** Give a description of the structure (for display purposes). */
    const std::string describe() const;

    /** Default Constructor. */
    BookingClassStruct_T();
  };

  /** List of BookingClass structures. */
  typedef std::vector<BookingClassStruct_T> BookingClassStructList_T;

}
#endif // __AIRINV_BOM_BOOKINGCLASSSTRUCT_HPP
