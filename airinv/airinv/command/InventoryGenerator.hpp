#ifndef __AIRINV_CMD_INVENTORYGENERATOR_HPP
#define __AIRINV_CMD_INVENTORYGENERATOR_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// StdAir
#include <stdair/command/CmdAbstract.hpp>
// AirSched
#include <airinv/AIRINV_Types.hpp>

// Forward declarations
namespace stdair {
  class BomRoot;
  class FlightDate;
}

namespace AIRINV {
  // Forward declarations
  struct FlightDateStruct_T;
  namespace InventoryParserHelper {
    struct doEndFlightDate;
  }
  
  /** Class handling the generation / instantiation of the Inventory BOM. */
  class InventoryGenerator : public stdair::CmdAbstract {
    // Only the following class may use methods of InventoryGenerator.
    // Indeed, as those methods build the BOM, it is not good to expose
    // them publicly.
    friend struct InventoryParserHelper::doEndFlightDate;

  private:
    /** Generate the Flight-Date objects corresponding to the given
        FlightDateStruct_T, and add them to the given BomRoot. */
    static void createFlightDate (stdair::BomRoot&,
                                  const FlightDateStruct_T&);


  };

}
#endif // __AIRINV_CMD_INVENTORYGENERATOR_HPP
