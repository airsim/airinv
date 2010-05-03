#ifndef __AIRINV_BOM_FLIGHTDATE_HPP
#define __AIRINV_BOM_FLIGHTDATE_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STDAIR
#include <stdair/bom/FlightDate.hpp>

// Forward declarations
namespace stdair {
  class FacBomContent;
}

namespace AIRINV {
  /** Class representing the actual business functions for
      an airline flight-date. */
  class FlightDate : public stdair::FlightDate {
    friend class stdair::FacBomContent;

  public:
    /** Definition allowing to retrieve the associated BOM structure type. */
    typedef stdair::FlightDateStructure_T Structure_T;

  public:
    // ////////// Business Methods /////////
    
  private:
    /** Constructors are private so as to force the usage of the Factory
        layer. */
    /** Constructors. */
    FlightDate (const Key_T&, Structure_T&);
    /** Destructor. */
    ~FlightDate();
    /** Default constructors. */
    FlightDate ();
    FlightDate (const FlightDate&);

  };

}
#endif // __AIRINV_BOM_FLIGHTDATE_HPP
