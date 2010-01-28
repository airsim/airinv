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
  AIRINV_ServiceContext::
  AIRINV_ServiceContext (const stdair::AirlineCode_T& iAirlineCode,
                         stdair::Inventory& ioInventory)
    : _airlineCode (iAirlineCode), _inventory (ioInventory) {
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
