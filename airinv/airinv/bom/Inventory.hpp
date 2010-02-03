#ifndef __AIRINV_BOM_INVENTORY_HPP
#define __AIRINV_BOM_INVENTORY_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STDAIR
#include <stdair/STDAIR_Types.hpp>
#include <stdair/bom/Inventory.hpp>

// Forward declarations
namespace stdair {
  struct TravelSolutionStruct;
  class Inventory;
  class FacBomContent;
}

namespace AIRINV {
  /** Class representing the actual business functions for
      an airline inventory. */
  class Inventory : public stdair::Inventory {
    friend class stdair::FacBomContent;

  public:
    /** Definition allowing to retrieve the associated BOM structure type. */
    typedef stdair::InventoryStructure_T BomStructure_T;
    
  public:
    // ////////// Business Methods /////////
    /** Make a sale with the given travel solution. */
    static void sell (stdair::Inventory&, const stdair::TravelSolutionStruct&,
                      const stdair::PartySize_T&);
    
  private:
    /** Constructors are private so as to force the usage of the Factory
        layer. */
    /** Default constructors. */
    Inventory ();
    Inventory (const Inventory&);
    Inventory (const BomKey_T&, BomStructure_T&);

    /** Destructor. */
    virtual ~Inventory();

  };

}
#endif // __AIRINV_BOM_INVENTORY_HPP
