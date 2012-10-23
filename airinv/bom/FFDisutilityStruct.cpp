// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// StdAir
#include <stdair/service/Logger.hpp>
// AIRINV
#include <airinv/bom/FFDisutilityStruct.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  FFDisutilityStruct::FFDisutilityStruct() {
  }

  // ////////////////////////////////////////////////////////////////////
  FFDisutilityStruct::~FFDisutilityStruct() {
  }
  
  // ////////////////////////////////////////////////////////////////////
  const std::string FFDisutilityStruct::describe() const {
    std::ostringstream oStr;
    oStr << _key << "; ";
    for (stdair::FFDisutilityCurve_T::const_reverse_iterator itFFDisutility =
           _curve.rbegin(); itFFDisutility != _curve.rend(); ++itFFDisutility) {
      const stdair::DTD_T& lDTD = itFFDisutility->first;
      const double& lFFDisutility = itFFDisutility->second;
      oStr << lDTD << ":" << lFFDisutility << ";";
    }
    
    return oStr.str();
  }

}
