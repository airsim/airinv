// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// STDAIR
#include <stdair/bom/BomList.hpp>
#include <stdair/bom/Inventory.hpp>
#include <stdair/bom/TravelSolutionStruct.hpp>
#include <stdair/bom/SegmentDateTypes.hpp>
#include <stdair/bom/BookingClass.hpp>
#include <stdair/service/Logger.hpp>
// AIRINV
#include <airinv/bom/Inventory.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  Inventory::Inventory (const BomKey_T& iKey,
                        BomStructure_T& ioInventoryStructure)
    : stdair::Inventory (iKey, ioInventoryStructure) {
  }
  
  // ////////////////////////////////////////////////////////////////////
  Inventory::~Inventory () {
  }

  // ////////////////////////////////////////////////////////////////////
  void Inventory::sell (stdair::Inventory& ioInventory,
                        const stdair::TravelSolutionStruct& iTravelSolution,
                        const stdair::PartySize_T& iPartySize) {
    const stdair::AirlineCode_T& lAirlineCode = ioInventory.getAirlineCode();

    // Browse the booking class list and make the reservation for the
    // booking classes which belong to the given airline.
    const stdair::BookingClassSTLList_T& lBookingClassList =
      iTravelSolution.getBookingClassList();

    for (stdair::BookingClassSTLList_T::const_iterator itBookingClass =
           lBookingClassList.begin();
         itBookingClass != lBookingClassList.end(); ++itBookingClass) {
      stdair::BookingClass* lBookingClass_ptr = *itBookingClass;
      assert (lBookingClass_ptr != NULL);

      const stdair::AirlineCode_T& lCurrentAirlineCode =
        lBookingClass_ptr->getAirlineCode ();

      if (lAirlineCode == lCurrentAirlineCode) {
        STDAIR_LOG_DEBUG ("Make a reservation of " << iPartySize
                          << " seats for the following class: "
                          << lBookingClass_ptr->describeKey());
      }
    }
  }
}
