// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// STDAIR
#include <stdair/bom/BomList.hpp>
#include <stdair/bom/BomSource.hpp>
#include <stdair/service/Logger.hpp>
// AIRINV
#include <airinv/bom/Inventory.hpp>
#include <airinv/bom/SegmentCabin.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  Inventory::Inventory (const Key_T& iKey,
                        Structure_T& ioInventoryStructure)
    : stdair::Inventory (iKey, ioInventoryStructure) {
  }
  
  // ////////////////////////////////////////////////////////////////////
  Inventory::~Inventory () {
  }

  // ////////////////////////////////////////////////////////////////////
  bool Inventory::sell (stdair::Inventory& ioInventory, 
                        const std::string& iSegmentDateKey,
                        const stdair::ClassCode_T& iClassCode,
                        const stdair::PartySize_T& iPartySize) {
    const int lSize = iSegmentDateKey.size();
    // Retrieve the corresponding flight-date.
    std::string lFlightDateKey;
    lFlightDateKey.append (iSegmentDateKey, 3, lSize - 9);
    const stdair::FlightDate* lFlightDate_ptr =
      ioInventory.getFlightDate (lFlightDateKey);
    assert (lFlightDate_ptr);

    // Retrieve the corresponding segment-date.
    std::string lSegmentDateKey;
    lSegmentDateKey.append (iSegmentDateKey, lSize - 7, 7);
    const stdair::SegmentDate* lSegmentDate_ptr =
      lFlightDate_ptr->getSegmentDate (lSegmentDateKey);
    assert (lSegmentDate_ptr);

    // Browse the segment-cabins and make the sale with the
    // corresponding booking class.
    const stdair::SegmentCabinList_T& lSegmentCabinList =
      lSegmentDate_ptr->getSegmentCabinList();
    for (stdair::SegmentCabinList_T::iterator itCabin= lSegmentCabinList.begin();
         itCabin != lSegmentCabinList.end(); ++itCabin) {
      stdair::SegmentCabin& lSegmentCabin = *itCabin;
      stdair::BookingClass* lBookingClass_ptr =
        lSegmentCabin.getBookingClass (iClassCode);
      if (lBookingClass_ptr) {
        // Register the sale in the class.
        lBookingClass_ptr->sell (iPartySize);

        // Update the commited space of the segment-cabins and the leg-cabins.
        SegmentCabin::updateFromReservation (lSegmentCabin, iPartySize);
        
        return true;
      }
    }
    return false;
  }

  // ////////////////////////////////////////////////////////////////////
  bool Inventory::sell (const std::string& iSegmentDateKey,
                        const stdair::ClassCode_T& iClassCode,
                        const stdair::PartySize_T& iPartySize) {
    // Forward the business the the corresponding static function.
    return sell (*this, iSegmentDateKey, iClassCode, iPartySize);
  }
}
