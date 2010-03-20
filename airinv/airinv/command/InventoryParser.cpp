// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <string>
// StdAir
#include <stdair/basic/DictionaryManager.hpp>
#include <stdair/bom/Inventory.hpp>
#include <stdair/bom/Network.hpp>
// Airinv
#include <airinv/command/InventoryParserHelper.hpp>
#include <airinv/command/InventoryParser.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  void InventoryParser::
  generateInventory (const stdair::Filename_T& iFilename,
                  stdair::BomRoot& ioBomRoot) {
    // Initialise the inventory file parser.
    InventoryFileParser lInventoryParser (ioBomRoot, iFilename);

    // Parse the CSV-formatted inventory input file, and generate the
    // corresponding InventoryCharacteristic objects.
    lInventoryParser.generateInventory ();
  }

}
