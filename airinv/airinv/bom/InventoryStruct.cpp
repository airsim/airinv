// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// StdAir
#include <stdair/basic/BasConst_General.hpp>
#include <stdair/service/Logger.hpp>
// AIRINV
#include <airinv/AIRINV_Types.hpp>
#include <airinv/bom/InventoryStruct.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  InventoryStruct_T::InventoryStruct_T ()
    : _flightDate (stdair::DEFAULT_DATE) {
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::Date_T InventoryStruct_T::getFlightDate() const {
    return stdair::Date_T (_itYear, _itMonth, _itDay);
  }

  // ////////////////////////////////////////////////////////////////////
  const std::string InventoryStruct_T::describe() const {
    std::ostringstream ostr;
    ostr << _flightDate << " ";

    return ostr.str();
  }

}
