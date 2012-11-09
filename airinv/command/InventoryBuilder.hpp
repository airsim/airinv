#ifndef __AIRINV_CMD_INVENTORYBUILDER_HPP
#define __AIRINV_CMD_INVENTORYBUILDER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// StdAir
#include <stdair/command/CmdAbstract.hpp>
// AirInv
#include <airinv/AIRINV_Types.hpp>

/// Forward declarations
namespace stdair {
  class BomRoot;
  class Inventory;
  class FlightDate;
  class LegDate;
  class LegCabin;
  class Bucket;
  class SegmentDate;
  class SegmentCabin;
  class FareFamily;
  struct ParsedKey;
  class BookingClass;
}

namespace AIRINV {

  /// Forward declarations
  struct FlightDateStruct;
  struct LegStruct;
  struct LegCabinStruct;
  struct BucketStruct;
  struct SegmentStruct;
  struct SegmentCabinStruct;
  struct FareFamilyStruct;
  struct BookingClassStruct;
  namespace InventoryParserHelper {
    struct doEndFlightDate;
  }
  
  /**
   * @brief Class handling the generation / instantiation of the Inventory BOM.
   */
  class InventoryBuilder : public stdair::CmdAbstract {
    /**
     * Only the following class may use methods of InventoryBuilder.
     * Indeed, as those methods build the BOM, it is not good to expose
     * them publicly.
     */
    friend class AIRINV_Service;
    friend struct InventoryParserHelper::doEndFlightDate;

  private:
    /**
     * Build the inventory object corresponding to the given
     * FlightDateStruct, and add them to the given bom root.
     */
    static void buildInventory (stdair::BomRoot&, const FlightDateStruct&);
    
    /**
     * Build the flight-date objects corresponding to the given
     * FlightDateStruct, and add them to the given nventory.
     */
    static void buildFlightDate (stdair::Inventory&, const FlightDateStruct&);

    /**
     * Build the leg-date objects corresponding to the given
     * LegDateStruct, and add them to the given flight-date.
     */
    static void buildLegDate (stdair::FlightDate&, const LegStruct&); 

    /**
     *
     */
    static void buildRoutingLegKey (stdair::FlightDate&);
    
    /**
     * Build the leg-cabin objects corresponding to the given
     * LegCabinStruct, and add them to the given leg-date.
     */
    static void buildLegCabin (stdair::LegDate&, const LegCabinStruct&);

    /**
     * Build the leg-cabin objects corresponding to the given
     * LegCabinStruct, and add them to the given leg-date.
     */
    static void buildBucket (stdair::LegCabin&, const BucketStruct&);

    /**
     * Build the segment-date objects corresponding to the given
     * SegmentDateStruct, and add them to the given flight-date.
     */
    static void buildSegmentDate (stdair::FlightDate&, const SegmentStruct&);

    /**
     * Build the segment-cabin objects corresponding to the given
     * SegmentCabinStruct, and add them to the given segment-date.
     */
    static void buildSegmentCabin (stdair::SegmentDate&,
                                   const SegmentCabinStruct&);

    /**
     * Build the fare family objects corresponding to the given
     * FareFamilyStruct, and add them to the given segment-cabin.
     */
    static void buildFareFamily (stdair::SegmentCabin&,
                                 const FareFamilyStruct&);

    /**
     * Build the booking class objects corresponding to the given
     * BookingClassStruct, and add them to the given fare family.
     */
    static void buildBookingClass (stdair::FareFamily&,
                                   const BookingClassStruct&);

    /**
     * Build the partner inventory objects.
     */
    static void buildPartnerInventories (stdair::BomRoot&);

    /**
     * Build the partner inventory objects.
     */
    static void buildPartnerInventories (stdair::BomRoot&,
                                         stdair::Inventory&);

    /**
     * Build the partner inventory objects.
     */
    static void buildPartnerInventories (stdair::BomRoot&,
                                         stdair::Inventory&,
                                         stdair::FlightDate&);
    
 
    static void buildInventory (stdair::BomRoot&,
                                stdair::Inventory&,
                                const std::string& iFullKeyStr);

    static void buildFlightDate (stdair::BomRoot&,
                                 stdair::Inventory&,
                                 const std::string& iFullKeyStr);

    /**
     * Recursively clone the children of a flight date.
     *
     * @param const FlightDate& Root of the BOM tree to be cloned. 
     * 
     * @return FlightDate& Root of the cloned BOM tree.
     */
    static stdair::FlightDate& cloneFlightDate (const stdair::FlightDate&);  
 
    /**
     * Recursively clone the children of a leg date.
     *
     * @param const LegDate& Root of the BOM tree to be cloned. 
     * 
     * @return LegDate& Root of the cloned BOM tree.
     */
    static stdair::LegDate& cloneLegDate (const stdair::LegDate&);
  
    /**
     * Recursively clone the children of a leg cabin.
     *
     * @param const LegCabin& Root of the BOM tree to be cloned. 
     * 
     * @return LegCabin& Root of the cloned BOM tree.
     */
    static stdair::LegCabin& cloneLegCabin (const stdair::LegCabin&);
 
    /**
     * Clone a bucket object.
     *
     * @param const Bucket& Bucket to be cloned. 
     * 
     * @return Bucket& Cloned bucket.
     */
    static stdair::Bucket& cloneBucket (const stdair::Bucket&);
 
    /**
     * Recursively clone the children of a segment date.
     *
     * @param const SegmentDate& Root of the BOM tree to be cloned. 
     * 
     * @return SegmentDate& Root of the cloned BOM tree.
     */
    static stdair::SegmentDate& cloneSegmentDate (const stdair::SegmentDate&); 

    /**
     * Link all the booking classes with their corresponding segmentDate and 
     * segmentCabin.
     *
     * @param SegmentDate& Segment date to be linked with booking classes.
     * @param SegmentCabin& Segment cabin to be linked with booking classes.
     */
    static void linkBookingClassesWithSegment (stdair::SegmentDate&,
					       stdair::SegmentCabin&);
 
    /**
     * Recursively clone the children of a segment cabin.
     *
     * @param const SegmentCabin& Root of the BOM tree to be cloned. 
     * 
     * @return SegmentCabin& Root of the cloned BOM tree.
     */
    static stdair::SegmentCabin& cloneSegmentCabin (const stdair::SegmentCabin&);
  
    /**
     * Recursively clone the children of a fare family.
     *
     * @param const FareFamily& Root of the BOM tree to be cloned. 
     * 
     * @return FareFamily& Root of the cloned BOM tree.
     */
    static stdair::FareFamily& cloneFareFamily (const stdair::FareFamily&);
     
    /**
     * Recursively clone the children of a booking class.
     *
     * @param const BookingClass& Root of the BOM tree to be cloned. 
     * 
     * @return BookingClass& Root of the cloned BOM tree.
     */
    static stdair::BookingClass& cloneBookingClass (const stdair::BookingClass&);
    
  };

}
#endif // __AIRINV_CMD_INVENTORYBUILDER_HPP
