/*!
 * \page test_archi_inv_cpp Specific Implementation of an Airline Inventory
 * \code
 */
// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// StdAir
#include <stdair/stdair_inventory_types.hpp>
// Local
#include <test/architecture/MPInventory.hpp>

namespace myprovider {
  
  // ////////////////////////////////////////////////////////////////////
  Inventory::Inventory (const Key_T& iKey) : stdair::Inventory (iKey) {
  }

  // ////////////////////////////////////////////////////////////////////
  Inventory::~Inventory () {
  }

  // ////////////////////////////////////////////////////////////////////
  std::string Inventory::toString() const {
    std::ostringstream oStr;
    oStr << _key.toString();
    return oStr.str();
  }
  
  // ////////////////////////////////////////////////////////////////////
  const std::string Inventory::describeKey() const {
    return _key.toString();
  }
  
}
/*!
 * \endcode
 */