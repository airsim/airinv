// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// Airinv
#include <airinv/basic/BasConst_AIRINV_Service.hpp>
#include <airinv/service/AIRINV_Master_ServiceContext.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Master_ServiceContext::AIRINV_Master_ServiceContext()
    : _ownStdairService (false),
      _ownSEVMGRService (true) {
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Master_ServiceContext::~AIRINV_Master_ServiceContext() {
  }
  
  // //////////////////////////////////////////////////////////////////////
  const std::string AIRINV_Master_ServiceContext::shortDisplay() const {
    std::ostringstream oStr;
    oStr << "AIRINV_Master_ServiceContext -- Owns StdAir service: "
         << _ownStdairService;
    return oStr.str();
  }

  // //////////////////////////////////////////////////////////////////////
  const std::string AIRINV_Master_ServiceContext::display() const {
    std::ostringstream oStr;
    oStr << shortDisplay();
    return oStr.str();
  }

  // //////////////////////////////////////////////////////////////////////
  const std::string AIRINV_Master_ServiceContext::describe() const {
    return shortDisplay();
  }

  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Master_ServiceContext::reset() {
    
    // The shared_ptr<>::reset() method drops the refcount by one.
    // If the count result is dropping to zero, the resource pointed to
    // by the shared_ptr<> will be freed.
    
    // Reset the stdair shared pointer
    _stdairService.reset();
    
    // Reset the sevmgr shared pointer
    _sevmgrService.reset();
    
    // Reset the airinv shared pointer
    _airinvService.reset();
  }

}
