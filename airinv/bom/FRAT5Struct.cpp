// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// StdAir
#include <stdair/service/Logger.hpp>
// AIRINV
#include <airinv/bom/FRAT5Struct.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  FRAT5Struct::FRAT5Struct() {
  }

  // ////////////////////////////////////////////////////////////////////
  FRAT5Struct::~FRAT5Struct() {
  }
  
  // ////////////////////////////////////////////////////////////////////
  const std::string FRAT5Struct::describe() const {
    std::ostringstream oStr;
    oStr << _key << "; ";
    for (stdair::FRAT5Curve_T::const_reverse_iterator itFRAT5 = _curve.rbegin();
         itFRAT5 != _curve.rend(); ++itFRAT5) {
      const stdair::DTD_T& lDTD = itFRAT5->first;
      const double& lFRAT5 = itFRAT5->second;
      oStr << lDTD << ":" << lFRAT5 << ";";
    }
    
    return oStr.str();
  }

}
