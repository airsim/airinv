// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <string>
// StdAir
#include <stdair/bom/Inventory.hpp>
// Airinv
#include <airinv/command/InventoryParserHelper.hpp>
#include <airinv/command/InventoryParser.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  void InventoryParser::
  buildInventory (const stdair::Filename_T& iFilename,
                  stdair::Inventory& ioInventory) {
    // Initialise the inventory file parser.
    InventoryFileParser lInventoryParser (ioInventory, iFilename);

    // Parse the CSV-formatted inventory input file, and generate the
    // corresponding Inventory-related objects.
    lInventoryParser.buildInventory ();
  }

}