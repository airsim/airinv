#ifndef __AIRINV_BOM_BUCKETSTRUCT_HPP
#define __AIRINV_BOM_BUCKETSTRUCT_HPP

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
  class Bucket;
}

namespace AIRINV {

  /** Utility Structure for the parsing of Bucket structures. */
  struct BucketStruct_T : public stdair::StructAbstract {
    // Attributes
    stdair::Yield_T _yieldRangeUpperValue;
    stdair::CabinCapacity_T _availability;
    stdair::NbOfSeats_T _nbOfSeats;
    stdair::SeatIndex_T _seatIndex;

    /** Fill the Bucket objects with the attributes of the BucketStruct. */
    void fill (stdair::Bucket&) const;
      
    /** Give a description of the structure (for display purposes). */
    const std::string describe() const;

    /** Default Constructor. */
    BucketStruct_T();
  };

  /** List of Bucket structures. */
  typedef std::vector<BucketStruct_T> BucketStructList_T;

}
#endif // __AIRINV_BOM_BUCKETSTRUCT_HPP
