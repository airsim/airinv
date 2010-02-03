#ifndef __AIRINV_CMD_INDEXBUILDER_HPP
#define __AIRINV_CMD_INDEXBUILDER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STDAIR
#include <stdair/STDAIR_Types.hpp>

// Forward declarations
namespace stdair {
  class Inventory;
  struct TravelSolutionStruct;
}

namespace AIRINV {

  /** Command wrapping the travel request process. */
  class InventoryManager {
    friend class AIRINV_Service;
  private:

    /** Register a booking (segment sell). */
    static void sell (stdair::Inventory&, const stdair::TravelSolutionStruct&,
                      const stdair::PartySize_T&);

  private:
    /** Constructors. */
    InventoryManager() {}
    InventoryManager(const InventoryManager&) {}
    /** Destructor. */
    ~InventoryManager() {}
  };

}
#endif // __AIRINV_CMD_INDEXBUILDER_HPP
