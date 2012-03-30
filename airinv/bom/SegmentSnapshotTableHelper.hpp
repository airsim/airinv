#ifndef __AIRINV_BOM_SEGMENTSNAPSHOTTABLEHELPER_HPP
#define __AIRINV_BOM_SEGMENTSNAPSHOTTABLEHELPER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// StdAir
#include <stdair/stdair_basic_types.hpp>

// Forward declarations
namespace stdair {
  class SegmentSnapshotTable;
  class SegmentCabin;
}

namespace AIRINV {
  
  /** Class representing the actual business functions for
      an airline inventory. */
  class SegmentSnapshotTableHelper {
  public:
    // ////////// Business Methods /////////
    /** Take inventory snapshots. */
    static void takeSnapshots (stdair::SegmentSnapshotTable&,
                               const stdair::DateTime_T&);
  private:
    // ////////// Helpers for business methods. //////////
    /** Take the snapshots for the given segment-cabin. */
    static void takeSnapshots (stdair::SegmentSnapshotTable&,
                               const stdair::DTD_T&,
                               const stdair::SegmentCabin&,
                               const stdair::SegmentDataID_T);

    /** Register the product and price oriented bookings. */
    static void registerProductAndPriceOrientedBookings 
    (stdair::SegmentSnapshotTable&, const stdair::DTD_T&,
     const stdair::SegmentCabin&, const stdair::SegmentDataID_T);
  };

}
#endif // __AIRINV_BOM_SEGMENTSNAPSHOTTABLEHELPER_HPP
