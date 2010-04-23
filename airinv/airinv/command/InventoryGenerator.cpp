// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// BOOST
#include <boost/date_time/date_iterator.hpp>
// StdAir
#include <stdair/STDAIR_Types.hpp>
#include <stdair/basic/BasConst_BookingClass.hpp>
#include <stdair/basic/BasConst_Yield.hpp>
#include <stdair/basic/BasConst_Inventory.hpp>
#include <stdair/bom/BomSource.hpp>
#include <stdair/factory/FacBomContent.hpp>
#include <stdair/command/CmdBomManager.hpp>
#include <stdair/service/Logger.hpp>
// AIRINV
#include <airinv/bom/FlightDateStruct.hpp>
#include <airinv/command/InventoryGenerator.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  void InventoryGenerator::
  createFlightDate (stdair::BomRoot& ioBomRoot,
                    const FlightDateStruct_T& iFlightDateStruct) {

  }
  
}
