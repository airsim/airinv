#ifndef __AIRINV_BOM_LEGSTRUCT_HPP
#define __AIRINV_BOM_LEGSTRUCT_HPP

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
#include <airinv/bom/LegCabinStruct.hpp>

// Forward declarations
namespace stdair {
  class LegDate;
}

namespace AIRINV {

  /** Utility Structure for the parsing of Leg structures. */
  struct LegStruct_T : public stdair::StructAbstract {
    // Attributes
    stdair::AirportCode_T _boardingPoint;
    stdair::Date_T _boardingDate;
    stdair::Duration_T _boardingTime;
    stdair::AirportCode_T _offPoint;
    stdair::Date_T _offDate;
    stdair::Duration_T _offTime;
    stdair::Duration_T _elapsed;
    LegCabinStructList_T _cabinList;

    /** Fill the LegDate objects with the attributes of the LegStruct. */
    void fill (stdair::LegDate&) const;
      
    /** Give a description of the structure (for display purposes). */
    const std::string describe() const;

    /** Default Constructor. */
    LegStruct_T();
  };

  /** List of Leg structures. */
  typedef std::vector<LegStruct_T> LegStructList_T;

}
#endif // __AIRINV_BOM_LEGSTRUCT_HPP