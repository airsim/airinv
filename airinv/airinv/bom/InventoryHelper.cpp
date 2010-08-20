// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// STDAIR
#include <stdair/bom/BomManager.hpp>
#include <stdair/bom/Inventory.hpp>
#include <stdair/bom/FlightDate.hpp>
#include <stdair/bom/SegmentDate.hpp>
#include <stdair/bom/SegmentCabin.hpp>
#include <stdair/bom/BookingClass.hpp>
#include <stdair/service/Logger.hpp>
// AIRINV
#include <airinv/bom/InventoryHelper.hpp>
#include <airinv/bom/SegmentCabinHelper.hpp>

namespace AIRINV {
  // ////////////////////////////////////////////////////////////////////
  bool InventoryHelper::sell (stdair::Inventory& ioInventory, 
                              const std::string& iSegmentDateKey,
                              const stdair::ClassCode_T& iClassCode,
                              const stdair::PartySize_T& iPartySize) {
    const int lSize = iSegmentDateKey.size();
    // Retrieve the corresponding flight-date.
    std::string lFlightDateKey;
    lFlightDateKey.append (iSegmentDateKey, 3, lSize - 9);
    const stdair::FlightDate& lFlightDate =
      stdair::BomManager::getChild<stdair::FlightDate> (ioInventory,
                                                        lFlightDateKey);

    // Retrieve the corresponding segment-date.
    std::string lSegmentDateKey;
    lSegmentDateKey.append (iSegmentDateKey, lSize - 7, 7);
    const stdair::SegmentDate& lSegmentDate =
      stdair::BomManager::getChild<stdair::SegmentDate> (lFlightDateKey,
                                                         lSegmentDateKey);

    // Browse the segment-cabins and make the sale with the
    // corresponding booking class.
    stdair::SegmentCabinList_T& lSegmentCabinList =
      stdair::BomManager::getList<stdair::SegmentCabin> (lSegmentDate);
    for (stdair::SegmentCabinList_T::iterator itCabin =lSegmentCabinList.begin();
         itCabin != lSegmentCabinList.end(); ++itCabin) {
      stdair::SegmentCabin* lSegmentCabin_ptr = *itCabin;
      assert (lSegmentCabin_ptr != NULL);
      stdair::BookingClass* lBookingClass_ptr =
        stdair::BomManager::getChildPtr<stdair::BookingClass>(*lSegmentCabin_ptr,
                                                              iClassCode);
      if (lBookingClass_ptr) {
        // Register the sale in the class.
        lBookingClass_ptr->sell (iPartySize);
        // Update the commited space of the segment-cabins and the leg-cabins.
        SegmentCabinHelper::updateFromReservation (lFlightDate,
                                                   *lSegmentCabin_ptr,
                                                   iPartySize);
        return true;
      }
    }
    return false;
  }
}
