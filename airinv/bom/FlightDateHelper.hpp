#ifndef __AIRINV_BOM_FLIGHTDATEHELPER_HPP
#define __AIRINV_BOM_FLIGHTDATEHELPER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// StdAir
#include <stdair/stdair_basic_types.hpp>

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
    /** Fill the attributes derived from the routing legs (e.g., board
        and off dates). */
    static void fillFromRouting (const stdair::FlightDate&);

    /** Update the availability of all classes after a reservation. */
    static void updateAvailability (const stdair::FlightDate&,
                                   const stdair::SegmentCabin&,
                                   const stdair::PartySize_T& iNbOfBookings);

    /** Update the availability pool of all the segment-cabins after a
        reservation. */
    static void updateAvailabilityPool (const stdair::FlightDate&,
                                       const stdair::CabinCode_T&);

    /** Recalculate the availability of all the segment-cabins after a
        reservation. */
    static void recalculateAvailability (const stdair::FlightDate&,
                                         const stdair::CabinCode_T&);

    /** Update booking controls after optimisation. */
    static void updateBookingControls (stdair::FlightDate&);

    /** Recalculate the availability of all the segment-cabins after an
        optimisation. */
    static void recalculateAvailability (const stdair::FlightDate&);
  };

}
#endif // __AIRINV_BOM_FLIGHTDATEHELPER_HPP
