#ifndef __AIRINV_BOM_FLIGHTDATEHELPER_HPP
#define __AIRINV_BOM_FLIGHTDATEHELPER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STDAIR
#include <stdair/STDAIR_Types.hpp>

// Forward declarations
namespace stdair {
  class FlightDate;
}

namespace AIRINV {
  /** Class representing the actual business functions for
      an airline flight-date. */
  class FlightDateHelper {
  public:
    // ////////// Business Methods /////////
    /** Update the availability pool of all the segment-cabins after a
        reservation. */
    static void updateAvailablityPool (const stdair::FlightDate&,
                                       const stdair::CabinCode_T&);
  };

}
#endif // __AIRINV_BOM_FLIGHTDATEHELPER_HPP