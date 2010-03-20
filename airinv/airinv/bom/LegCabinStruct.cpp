// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// STDAIR
#include <stdair/bom/LegCabin.hpp>
// AIRINV
#include <airinv/bom/LegCabinStruct.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  const std::string LegCabinStruct_T::describe() const {
    std::ostringstream ostr;
    ostr << "        " << _cabinCode << " " << _capacity << ", ";
    return ostr.str();
  }

  // //////////////////////////////////////////////////////////////////////
  void LegCabinStruct_T::fill (stdair::LegCabin& ioLegCabin,
                               const bool iAnalysisStatus) const {
    // Set the Capacity
    ioLegCabin.setCapacity (_capacity, iAnalysisStatus);
  }

}
