#ifndef __AIRINV_BOM_LEGCABIN_HPP
#define __AIRINV_BOM_LEGCABIN_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STDAIR
#include <stdair/bom/LegCabin.hpp>

// Forward declarations
namespace stdair {
  class FacBomContent;
}

namespace AIRINV {
  /** Class representing the actual business functions for
      an airline leg-cabin. */
  class LegCabin : public stdair::LegCabin {
    friend class stdair::FacBomContent;

  public:
    /** Definition allowing to retrieve the associated BOM structure type. */
    typedef stdair::LegCabinStructure_T Structure_T;

  private:
    /** Constructors are private so as to force the usage of the Factory
        layer. */
    /** Constructors. */
    LegCabin (const Key_T&, Structure_T&);
    /** Destructor. */
    ~LegCabin();
    /** Default constructors. */
    LegCabin ();
    LegCabin (const LegCabin&);

  };

}
#endif // __AIRINV_BOM_LEGCABIN_HPP
