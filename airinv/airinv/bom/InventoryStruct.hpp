#ifndef __AIRINV_BOM_INVENTORYSTRUCT_HPP
#define __AIRINV_BOM_INVENTORYSTRUCT_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// StdAir
#include <stdair/STDAIR_Types.hpp>
#include <stdair/basic/StructAbstract.hpp>
// Airinv

namespace AIRINV {

  /** Utility Structure for the parsing of Inventory structures. */
  struct InventoryStruct_T : public stdair::StructAbstract {
    
    /** Get the date from the staging details. */
    stdair::Date_T getFlightDate() const;

    // /////////////// Display Support Methods ///////////////
    /** Give a description of the structure (for display purposes). */
    const std::string describe() const;

    // /////////////// Constructors and Destructors ///////////////
    /** Constructor. */
    InventoryStruct_T ();
    
    // /////////////// Attributes ///////////////
    stdair::Date_T _flightDate;
    
    /** Staging Date. */
    unsigned int _itYear;
    unsigned int _itMonth;
    unsigned int _itDay;
      
  };

}
#endif // __AIRINV_BOM_INVENTORYSTRUCT_HPP
