#ifndef __AIRINV_CMD_INVENTORYPARSER_HPP
#define __AIRINV_CMD_INVENTORYPARSER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// StdAir
#include <stdair/STDAIR_Types.hpp>
#include <stdair/command/CmdAbstract.hpp>

// Forward declarations.
namespace stdair {
  class Inventory;
}

namespace AIRINV {
  
  /** Class wrapping the parser entry point. */
  class InventoryParser : public stdair::CmdAbstract {
  public:
    /** Parses the CSV file describing an airline inventory, and generates
        the corresponding data model in memory. It can then be used,
        for instance in a simulator.
        @param const stdair::Filename_T& The file-name of the CSV-formatted
               inventory input file.
        @param stdair::BomRoot& Root of the BOM tree. */
    static void buildInventory (const stdair::Filename_T&, stdair::Inventory&);
  };
}
#endif // __AIRINV_CMD_INVENTORYPARSER_HPP