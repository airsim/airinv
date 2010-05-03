#ifndef __AIRINV_BOM_SEGMENTDATE_HPP
#define __AIRINV_BOM_SEGMENTDATE_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STDAIR
#include <stdair/bom/SegmentDate.hpp>

// Forward declarations
namespace stdair {
  class FacBomContent;
}

namespace AIRINV {
  /** Class representing the actual business functions for
      an airline segment-date. */
  class SegmentDate : public stdair::SegmentDate {
    friend class stdair::FacBomContent;

  public:
    /** Definition allowing to retrieve the associated BOM structure type. */
    typedef stdair::SegmentDateStructure_T Structure_T;

  public:
    // ////////// Business Methods /////////

  private:
    /** Constructors are private so as to force the usage of the Factory
        layer. */
    /** Constructors. */
    SegmentDate (const Key_T&, Structure_T&);
    /** Destructor. */
    ~SegmentDate();
    /** Default constructors. */
    SegmentDate ();
    SegmentDate (const SegmentDate&);

  };

}
#endif // __AIRINV_BOM_SEGMENTDATE_HPP
