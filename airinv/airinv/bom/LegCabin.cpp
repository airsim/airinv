// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// AIRINV
#include <airinv/bom/LegCabin.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  LegCabin::LegCabin (const Key_T& iKey, Structure_T& ioLegCabinStructure)
    : stdair::LegCabin (iKey, ioLegCabinStructure) {
  }
  
  // ////////////////////////////////////////////////////////////////////
  LegCabin::~LegCabin () {
  }

}