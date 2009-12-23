// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <iosfwd>
#include <string>
#include <vector>
#include <exception>
// AirInv
#include <airinv/command/InventoryManager.hpp>
#include <airinv/service/Logger.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  void InventoryManager::
  sell (const AirlineCode_T& iAirlineCode, const PartySize_T& iPartySize) {

    try {

      // DEBUG
      AIRINV_LOG_DEBUG ("A booking has been made for the airline "
                        << iAirlineCode << " for " << iPartySize
                        << " passengers.");
    
    } catch (const std::exception& lStdError) {
      AIRINV_LOG_ERROR ("Error: " << lStdError.what());
      throw BookingException();
    }
  }

}
