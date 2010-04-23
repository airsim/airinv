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
#include <airinv/command/InventoryBuilder.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildFlightDate (stdair::Inventory& ioInventory,
                   const FlightDateStruct_T& iFlightDateStruct) {
    // Create the FlightDateKey
    stdair::FlightDateKey_T lFlightDateKey (iFlightDateStruct._flightNumber,
                                            iFlightDateStruct._flightDate);

    // Check that the flight-date object is not already existing. If a
    // FlightDate object with the same key has already been created,
    // then just update it, ifnot, create a flight-date and update it.
    stdair::FlightDate* lFlightDate_ptr = 
      ioInventory.getFlightDate (lFlightDateKey);
    if (lFlightDate_ptr == NULL) {
      // Instantiate a fligh-date object for the given key (flight number and
    // flight date)
    lFlightDate_ptr = &stdair::CmdBomManager::createFlightDate (ioInventory,
                                                                lFlightDateKey);
    }
    assert (lFlightDate_ptr != NULL);
    
    
  }
  
}
