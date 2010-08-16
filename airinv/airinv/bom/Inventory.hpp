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
      an airline inventory.
      <br>IMPORTANT NOTE: That class derives from stdair::Inventory, and in some
      cases, only that latter is built/instantiated by the Factory
      (FacBomContent). In particular, it means that it is useless, in those
      cases, to define any attribute (or non-static method) in that class
      (AIRINV::Inventory), as they would not be taken into account.
      <br>To benefit from the attributes and specific non-static methods defined
      here, the FacBomContent has to create specifically an AIRINV::Inventory
      object. */
  class Inventory : public stdair::Inventory {
    friend class stdair::FacBomContent;

  public:
    /** Definition allowing to retrieve the associated BOM structure type. */
    typedef stdair::InventoryStructure_T Structure_T;
    
  public:
    // ////////// Business Methods /////////
    /** Make a sale with the given travel solution. */
    static bool sell (stdair::Inventory&, const std::string& iSegmentDateKey,
                      const stdair::ClassCode_T&, const stdair::PartySize_T&);
    bool sell (const std::string& iSegmentDateKey,
               const stdair::ClassCode_T&, const stdair::PartySize_T&);
    
  private:
    /** Constructors are private so as to force the usage of the Factory
        layer. */
    /** Constructors. */
    Inventory (const Key_T&, Structure_T&);
    /** Destructor. */
    ~Inventory();
    /** Default constructors. */
    Inventory ();
    Inventory (const Inventory&);
  };

}
#endif // __AIRINV_BOM_INVENTORY_HPP
