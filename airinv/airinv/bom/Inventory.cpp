// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// STDAIR
#include <stdair/bom/BomList.hpp>
#include <stdair/bom/Inventory.hpp>
#include <stdair/bom/TravelSolutionStruct.hpp>
#include <stdair/bom/SegmentDateTypes.hpp>
#include <stdair/bom/BookingClass.hpp>
#include <stdair/service/Logger.hpp>
// AIRINV
#include <airinv/bom/Inventory.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  Inventory::Inventory (const Key_T& iKey,
                        Structure_T& ioInventoryStructure)
    : stdair::Inventory (iKey, ioInventoryStructure) {
  }
  
  // ////////////////////////////////////////////////////////////////////
  Inventory::~Inventory () {
  }

  // ////////////////////////////////////////////////////////////////////
  void Inventory::sell (stdair::Inventory& ioInventory,
                        const stdair::TravelSolutionStruct& iTravelSolution,
                        const stdair::PartySize_T& iPartySize) {
    
  }

  // ////////////////////////////////////////////////////////////////////
  void Inventory::sell (const stdair::TravelSolutionStruct& iTravelSolution,
                        const stdair::PartySize_T& iPartySize) {
    // Forward the business the the corresponding static function.
    sell (*this, iTravelSolution, iPartySize);
  }
}
