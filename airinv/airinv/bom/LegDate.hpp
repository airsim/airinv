#ifndef __AIRINV_BOM_LEGDATE_HPP
#define __AIRINV_BOM_LEGDATE_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STDAIR
#include <stdair/bom/LegDate.hpp>

// Forward declarations
namespace stdair {
  class FacBomContent;
}

namespace AIRINV {
  /** Class representing the actual business functions for
      an airline leg-date. */
  class LegDate : public stdair::LegDate {
    friend class stdair::FacBomContent;

  public:
    /** Definition allowing to retrieve the associated BOM structure type. */
    typedef stdair::LegDateStructure_T Structure_T;

  private:
    /** Constructors are private so as to force the usage of the Factory
        layer. */
    /** Constructors. */
    LegDate (const Key_T&, Structure_T&);
    /** Destructor. */
    ~LegDate();
    /** Default constructors. */
    LegDate ();
    LegDate (const LegDate&);

  };

}
#endif // __AIRINV_BOM_LEGDATE_HPP
