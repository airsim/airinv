#ifndef __AIRINV_CMD_INVENTORYBUILDER_HPP
#define __AIRINV_CMD_INVENTORYBUILDER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// StdAir
#include <stdair/command/CmdAbstract.hpp>
// AirSched
#include <airinv/AIRINV_Types.hpp>

// Forward declarations
namespace stdair {
  class Inventory;
  class FlightDate;
  class LegDate;
  class SegmentDate;
  class SegmentCabin;
}

namespace AIRINV {
  // Forward declarations
  struct FlightDateStruct_T;
  struct LegStruct_T;
  struct LegCabinStruct_T;
  struct SegmentStruct_T;
  struct SegmentCabinStruct_T;
  struct BookingClassStruct_T;
  namespace InventoryParserHelper {
    struct doEndFlightDate;
  }
  
  /** Class handling the generation / instantiation of the Inventory BOM. */
  class InventoryBuilder : public stdair::CmdAbstract {
    // Only the following class may use methods of InventoryBuilder.
    // Indeed, as those methods build the BOM, it is not good to expose
    // them publicly.
    friend struct InventoryParserHelper::doEndFlightDate;

  private:
    /** Build the flight-date objects corresponding to the given
        FlightDateStruct_T, and add them to the given nventory. */
    static void buildFlightDate (const stdair::Inventory&,
                                 const FlightDateStruct_T&);

    /** Build the leg-date objects corresponding to the given
        LegDateStruct_T, and add them to the given flight-date. */
    static void buildLegDate (const stdair::FlightDate&,
                              const LegStruct_T&);
    
    /** Build the leg-cabin objects corresponding to the given
        LegCabinStruct_T, and add them to the given leg-date. */
    static void buildLegCabin (const stdair::LegDate&,
                               const LegCabinStruct_T&);

    /** Build the segment-date objects corresponding to the given
        SegmentDateStruct_T, and add them to the given flight-date. */
    static void buildSegmentDate (const stdair::FlightDate&,
                                  const SegmentStruct_T&);

    /** Build the segment-cabin objects corresponding to the given
        SegmentCabinStruct_T, and add them to the given segment-date. */
    static void buildSegmentCabin (const stdair::SegmentDate&,
                                   const SegmentCabinStruct_T&);

    /** Build the booking class objects corresponding to the given
        BookingClassStruct_T, and add them to the given segment-cabin. */
    static void buildBookingClass (const stdair::SegmentCabin&,
                                   const BookingClassStruct_T&);

  };

}
#endif // __AIRINV_CMD_INVENTORYBUILDER_HPP
