#ifndef __AIRINV_CMD_INVENTORYMANAGER_HPP
#define __AIRINV_CMD_INVENTORYMANAGER_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// STDAIR
#include <stdair/stdair_basic_types.hpp>
#include <stdair/bom/RMEventTypes.hpp>
#include <stdair/bom/BomIDTypes.hpp>
// SEvMgr
#include <sevmgr/SEVMGR_Types.hpp>

// Forward declarations
namespace stdair {
  class BomRoot;
  class Inventory;
  class FlightDate;
  class SegmentDate;
  class SegmentCabin;
  class EventQueue;
  struct TravelSolutionStruct;
}

namespace AIRINV {

  // //////////////// Type definitions ////////////////
  typedef std::map<const stdair::Date_T,
                   stdair::SegmentCabin*> DepartureDateSegmentCabinMap_T;
  typedef std::map<const std::string,
                   DepartureDateSegmentCabinMap_T> SimilarSegmentCabinSetMap_T;

  /** Command wrapping the travel request process. */
  class InventoryManager {
    friend class AIRINV_Master_Service;
    friend class AIRINV_Service;

  private:
    /** Initialise the snapshot events for the inventories. */
    static void initSnapshotEvents (SEVMGR::SEVMGR_ServicePtr_T,
				    const stdair::Date_T&, 
                                    const stdair::Date_T&);
    
    /** Initialise the RM events for the inventories. */
    static void initRMEvents (const stdair::Inventory&, stdair::RMEventList_T&,
                              const stdair::Date_T&, const stdair::Date_T&);

    /** Add the RM events to the event queue. */
    static void addRMEventsToEventQueue (SEVMGR::SEVMGR_ServicePtr_T,
                                         stdair::RMEventList_T&);
    
    /** Compute the availability for the given travel solution. */
    static void calculateAvailability (const stdair::BomRoot&,
                                       stdair::TravelSolutionStruct&);
    
    /** Compute the availability for each fare option using the AU's. */
    static void calculateAvailabilityByAU (stdair::TravelSolutionStruct&);

    /** Compute the availability for each fare option using Revenue Availability Exchange. */
    static void calculateAvailabilityByRAE (stdair::TravelSolutionStruct&);
    
    /**
     * Compute the availability for each fare option using Interline Bid Price.
     * BP_(total) = sum (BP_i), where BP_i is the bid price of partner i.
     */
    static void calculateAvailabilityByIBP (stdair::TravelSolutionStruct&);

    /**
     * Compute the availability for each fare option using the protective Interline Bid Price.
     * BP_(total) = max (BP_i/PF_i), where PF_i is the yield proration factor for partner i.
     * For now, it assumes that yield is equally divided between partners: PF_i = 1/{number of partners}.
     * This guaranties individual minimum revenue (it protects from partner's bid price fluctuation).
     */
    static void calculateAvailabilityByProtectiveIBP (stdair::TravelSolutionStruct&);

    /** Register a booking (segment sell). */
    static bool sell (stdair::Inventory&, const std::string& iSegmentDateKey,
                      const stdair::ClassCode_T&, const stdair::PartySize_T&);

    /** Register a booking (segment sell). */
    static bool sell (const stdair::BookingClassID_T&,
                      const stdair::PartySize_T&);

    /** Register a cancellation. */
    static bool cancel (stdair::Inventory&, const std::string& iSegmentDateKey,
                        const stdair::ClassCode_T&, const stdair::PartySize_T&);

    /** Register a cancellation. */
    static bool cancel (const stdair::BookingClassID_T&,
                        const stdair::PartySize_T&);

    /** Take inventory snapshots. */
    static void takeSnapshots (const stdair::Inventory&,
                               const stdair::DateTime_T&);

    /** Update booking controls after optimisation. */
    static void updateBookingControls (stdair::FlightDate&);

    /** Recalculate the availabilities after optimisation. */
    static void recalculateAvailability (stdair::FlightDate&);

  public:
    /** Create the direct accesses within the inventories suck as links between
        leg-date and segment-date, ect. */
    static void createDirectAccesses (const stdair::BomRoot&);
    static void createDirectAccesses (const stdair::BomRoot&,
                                      stdair::Inventory&);
    static void createDirectAccesses (const stdair::BomRoot&,
                                      stdair::Inventory&, stdair::FlightDate&);
    static void createDirectAccesses (stdair::SegmentDate&);

    /** Create the direct accesses within the inventories suck as the link
        between a marketing segment date and its operating one. */
    static void createPartnerAccesses (const stdair::BomRoot&,
                                       stdair::Inventory&);
    static void createPartnerAccesses (stdair::FlightDate&);
    static void createPartnerAccesses (const stdair::BomRoot&,
                                       stdair::Inventory&, stdair::FlightDate&);


    /** Build the similar segment-cabin sets and the corresponding 
        snapshot tables and other data. */
    static void buildSimilarSegmentCabinSets (const stdair::BomRoot&);
    static void buildSimilarSegmentCabinSets (stdair::Inventory&);
    static void buildSegmentSnapshotTable (stdair::Inventory&,
                                           const stdair::TableID_T&,
                                           const DepartureDateSegmentCabinMap_T&);


    /** Bid price vectors initialisation */
    static void setDefaultBidPriceVector (stdair::BomRoot&);
    static void setDefaultBidPriceVector (stdair::Inventory&);

    /** Yield-based nesting structure initialisation */
    static void initialiseYieldBasedNestingStructures (const stdair::BomRoot&);

    /** Lists of usable policies initialisation. */
    static void initialiseListsOfUsablePolicies (const stdair::BomRoot&);
    
  private:
    /** Constructors. */
    InventoryManager() {}
    InventoryManager(const InventoryManager&) {}
    /** Destructor. */
    ~InventoryManager() {}
  };

}
#endif // __AIRINV_CMD_INVENTORYMANAGER_HPP
