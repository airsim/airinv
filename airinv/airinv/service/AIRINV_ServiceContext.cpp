// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <iostream>
#include <sstream>
// Airinv
#include <airinv/basic/BasConst_AIRINV_Service.hpp>
#include <airinv/service/AIRINV_ServiceContext.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  AIRINV_ServiceContext::AIRINV_ServiceContext ()
    : _airlineCode (DEFAULT_AIRLINE_CODE) {
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_ServiceContext::
  AIRINV_ServiceContext (const AirlineCode_T& iAirlineCode)
    : _airlineCode (iAirlineCode) {
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_ServiceContext::~AIRINV_ServiceContext() {
  }
  
  // //////////////////////////////////////////////////////////////////////
  const std::string AIRINV_ServiceContext::shortDisplay() const {
    std::ostringstream oStr;
    oStr << "AIRINV_ServiceContext: " << std::endl
         << "Airline code: " << _airlineCode
         << std::endl;
    return oStr.str();
  }

  // //////////////////////////////////////////////////////////////////////
  const std::string AIRINV_ServiceContext::display() const {
    std::ostringstream oStr;
    oStr << shortDisplay();
    return oStr.str();
  }

}
