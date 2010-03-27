// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// StdAir
#include <stdair/basic/BasConst_General.hpp>
#include <stdair/bom/BookingClass.hpp>
// AirInv
#include <airinv/bom/BookingClassStruct.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  BookingClassStruct_T::BookingClassStruct_T () {
  }
  
  // //////////////////////////////////////////////////////////////////////
  const std::string BookingClassStruct_T::describe() const {
    std::ostringstream ostr;
    ostr << "            " << _classCode << _subclassCode
         << " " << _cumulatedProtection << ":" << _protection
         << " " << _noShowPercentage << ":" << _overbookingPercentage
         << " " << _nbOfBookings << ":" << _groupNbOfBookings
         << ":" << _groupPendingNbOfBookings << ":" << _staffNbOfBookings
         << ":" << _wlNbOfBookings << ":" << _etb
         << " " << _netClassAvailability  << ":" << _segmentAvailability
         << ":" << _netRevenueAvailability
         << std::endl;
    return ostr.str();
  }

  // //////////////////////////////////////////////////////////////////////
  void BookingClassStruct_T::fill (stdair::BookingClass& ioBookingClass) const {
    // Set the Yield Range Upper Value
    // ioBookingClass.setYieldRangeValue (_yieldRangeUpperValue);

    // Set the Availability
    // ioBookingClass.setAvailability (_availability);

    // Set the number of seats
    // ioBookingClass.setNbOfSeats (_nbOfSeats);
      
    // Set the Seat Index
    // ioBookingClass.setSeatIndex (_seatIndex);
  }

}
