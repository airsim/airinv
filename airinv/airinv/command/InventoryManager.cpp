// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <exception>
// StdAir
#include <stdair/service/Logger.hpp>
#include <stdair/bom/Inventory.hpp>
// AirInv
#include <airinv/AIRINV_Types.hpp>
#include <airinv/bom/Inventory.hpp>
#include <airinv/command/InventoryManager.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  void InventoryManager::
  sell (stdair::Inventory& ioInventory,
        const stdair::TravelSolutionStruct& iTravelSolution, 
        const stdair::PartySize_T& iPartySize) {

    try {

      // Make the sale within the inventory.
      Inventory::sell (ioInventory, iTravelSolution, iPartySize);
      
    } catch (const std::exception& lStdError) {
      STDAIR_LOG_ERROR ("Error: " << lStdError.what());
      throw BookingException();
    }
  }

}
