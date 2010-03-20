// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// STDAIR
#include <stdair/bom/SegmentCabin.hpp>
// AIRINV
#include <airinv/bom/SegmentCabinStruct.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  const std::string SegmentCabinStruct_T::describe() const {
    std::ostringstream ostr;
    ostr << "        " << _cabinCode << " " << _classes;
    return ostr.str();
  }

  // //////////////////////////////////////////////////////////////////////
  void SegmentCabinStruct_T::fill (stdair::SegmentCabin& ioSegmentCabin) const {  
  }

}
