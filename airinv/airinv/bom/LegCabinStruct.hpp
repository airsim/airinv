#ifndef __AIRINV_BOM_LEGCABINSTRUCT_HPP
#define __AIRINV_BOM_LEGCABINSTRUCT_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
#include <vector>
// STDAIR
#include <stdair/STDAIR_Types.hpp>
#include <stdair/basic/StructAbstract.hpp>

// Forward declarations
namespace stdair {
  class LegCabin;
}

namespace AIRINV {

  /** Utility Structure for the parsing of LegCabin details. */
  struct LegCabinStruct_T : public stdair::StructAbstract {
    // Attributes
    stdair::CabinCode_T _cabinCode;
    stdair::CabinCapacity_T _capacity;

    /** Fill the LegCabin objects with the attributes of the
        LegCabinStruct. */
    void fill (stdair::LegCabin&, const bool) const;
      
    /** Give a description of the structure (for display purposes). */
    const std::string describe() const;
  };

  /** List of LegCabin-Detail strucutres. */
  typedef std::vector<LegCabinStruct_T> LegCabinStructList_T;

}
#endif // __AIRINV_BOM_LEGCABINSTRUCT_HPP
