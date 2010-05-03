// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// BOOST
#include <boost/date_time/date_iterator.hpp>
// StdAir
#include <stdair/STDAIR_Types.hpp>
#include <stdair/basic/BasConst_BookingClass.hpp>
#include <stdair/basic/BasConst_Yield.hpp>
#include <stdair/basic/BasConst_Inventory.hpp>
#include <stdair/bom/BomSource.hpp>
#include <stdair/factory/FacBomContent.hpp>
#include <stdair/command/CmdBomManager.hpp>
#include <stdair/service/Logger.hpp>
// AIRINV
#include <airinv/bom/FlightDateStruct.hpp>
#include <airinv/command/InventoryBuilder.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildFlightDate (const stdair::Inventory& iInventory,
                   const FlightDateStruct_T& iFlightDateStruct) {
    // Create the FlightDateKey
    const stdair::FlightDateKey_T lFlightDateKey(iFlightDateStruct._flightNumber,
                                                 iFlightDateStruct._flightDate);

    // Check that the flight-date object is not already existing. If a
    // flight-date object with the same key has already been created,
    // then just update it, ifnot, create a flight-date and update it.
    stdair::FlightDate* lFlightDate_ptr = 
      iInventory.getFlightDate (lFlightDateKey);
    if (lFlightDate_ptr == NULL) {
      // Instantiate a flighy-date object for the given key (flight number and
    // flight date)
    lFlightDate_ptr = 
      &stdair::CmdBomManager::createFlightDate (iInventory, lFlightDateKey);
    }
    assert (lFlightDate_ptr != NULL);

    // Update the BOM flight-date with the attributes of the flight-date struct.
    
    // Browse the list of leg-date struct and segment-date struct and
    // create the corresponding BOM.
    for (LegStructList_T::const_iterator itLegDate =
           iFlightDateStruct._legList.begin();
         itLegDate != iFlightDateStruct._legList.end(); ++itLegDate) {
      const LegStruct_T& lCurrentLegDateStruct = *itLegDate;
      buildLegDate (*lFlightDate_ptr, lCurrentLegDateStruct);
    }

    for (SegmentStructList_T::const_iterator itSegmentDate =
           iFlightDateStruct._segmentList.begin();
         itSegmentDate != iFlightDateStruct._segmentList.end();
         ++itSegmentDate) {
      const SegmentStruct_T& lCurrentSegmentDateStruct = *itSegmentDate;
      buildSegmentDate (*lFlightDate_ptr, lCurrentSegmentDateStruct);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildLegDate (const stdair::FlightDate& iFlightDate,
                const LegStruct_T& iLegDateStruct) {
    // Check that the leg-date object is not already existing. If a
    // leg-date object with the same key has already been created,
    // then just update it, ifnot, create a leg-date and update it.
    stdair::LegDate* lLegDate_ptr = 
      iFlightDate.getLegDate (iLegDateStruct._boardingPoint);
    if (lLegDate_ptr == NULL) {
      // Instantiate a leg-date object for the given key (boarding point);
      lLegDate_ptr = 
        &stdair::CmdBomManager::createLegDate (iFlightDate,
                                               iLegDateStruct._boardingPoint);
    }
    assert (lLegDate_ptr != NULL);

    // Update the BOM leg-date with the attributes of the leg-date struct.
    iLegDateStruct.fill (*lLegDate_ptr);

    // Browse the list of leg-cabin struct and create the corresponding BOM.
    for (LegCabinStructList_T::const_iterator itLegCabin =
           iLegDateStruct._cabinList.begin();
         itLegCabin != iLegDateStruct._cabinList.end(); ++itLegCabin) {
      const LegCabinStruct_T& lCurrentLegCabinStruct = *itLegCabin;
      buildLegCabin (*lLegDate_ptr, lCurrentLegCabinStruct);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildLegCabin (const stdair::LegDate& iLegDate,
                 const LegCabinStruct_T& iLegCabinStruct) {
    // Check that the leg-cabin object is not already existing. If a
    // leg-cabin object with the same key has already been created,
    // then just update it, ifnot, create a leg-cabin and update it.
    stdair::LegCabin* lLegCabin_ptr = 
      iLegDate.getLegCabin (iLegCabinStruct._cabinCode);
    if (lLegCabin_ptr == NULL) {
      // Instantiate a leg-cabin object for the given key (cabin code);
      lLegCabin_ptr = 
        &stdair::CmdBomManager::createLegCabin (iLegDate,
                                                iLegCabinStruct._cabinCode);
    }
    assert (lLegCabin_ptr != NULL);

    // TODO: Update the BOM leg-cabin with the attributes of the
    // leg-cabin struct.
    iLegCabinStruct.fill (*lLegCabin_ptr);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildSegmentDate (const stdair::FlightDate& iFlightDate,
                const SegmentStruct_T& iSegmentDateStruct) {
    // Check that the segment-date object is not already existing. If a
    // segment-date object with the same key has already been created,
    // then just update it, ifnot, create a segment-date and update it.
    const stdair::SegmentDateKey_T 
      lSegmentDateKey (iSegmentDateStruct._boardingPoint,
                       iSegmentDateStruct._offPoint);
    stdair::SegmentDate* lSegmentDate_ptr = 
      iFlightDate.getSegmentDate (lSegmentDateKey);
    if (lSegmentDate_ptr == NULL) {
      // Instantiate a segment-date object for the given key (boarding
      // and off points);
      lSegmentDate_ptr = 
        &stdair::CmdBomManager::createSegmentDate (iFlightDate,
                                                   lSegmentDateKey);
    }
    assert (lSegmentDate_ptr != NULL);

    // Update the BOM segment-date with the attributes of the
    // segment-date struct.
    iSegmentDateStruct.fill (*lSegmentDate_ptr);

    // Browse the list of segment-cabin struct and create the corresponding BOM.
    for (SegmentCabinStructList_T::const_iterator itSegmentCabin =
           iSegmentDateStruct._cabinList.begin();
         itSegmentCabin != iSegmentDateStruct._cabinList.end(); 
         ++itSegmentCabin) {
      const SegmentCabinStruct_T& lCurrentSegmentCabinStruct = *itSegmentCabin;
      buildSegmentCabin (*lSegmentDate_ptr, lCurrentSegmentCabinStruct);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildSegmentCabin (const stdair::SegmentDate& iSegmentDate,
                 const SegmentCabinStruct_T& iSegmentCabinStruct) {
    // Check that the segment-cabin object is not already existing. If a
    // segment-cabin object with the same key has already been created,
    // then just update it, ifnot, create a segment-cabin and update it.
    stdair::SegmentCabin* lSegmentCabin_ptr = 
      iSegmentDate.getSegmentCabin (iSegmentCabinStruct._cabinCode);
    if (lSegmentCabin_ptr == NULL) {
      // Instantiate a segment-cabin object for the given key (cabin code);
      lSegmentCabin_ptr = &stdair::
        CmdBomManager::createSegmentCabin (iSegmentDate,
                                           iSegmentCabinStruct._cabinCode);
    }
    assert (lSegmentCabin_ptr != NULL);

    // TODO: Update the BOM segment-cabin with the attributes of the
    // segment-cabin struct.
    iSegmentCabinStruct.fill (*lSegmentCabin_ptr);

    // Browse the list of booking-class struct and create the corresponding BOM.
    for (BookingClassStructList_T::const_iterator itBookingClass =
           iSegmentCabinStruct._classList.begin();
         itBookingClass != iSegmentCabinStruct._classList.end(); 
         ++itBookingClass) {
      const BookingClassStruct_T& lCurrentBookingClassStruct = *itBookingClass;
      buildBookingClass (*lSegmentCabin_ptr, lCurrentBookingClassStruct);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildBookingClass (const stdair::SegmentCabin& iSegmentCabin,
                     const BookingClassStruct_T& iBookingClassStruct){
    // Check that the booking class object is not already existing. If a
    // booking-class object with the same key has already been created,
    // then just upcabin it, ifnot, create a booking-class and upcabin it.
    stdair::BookingClass* lBookingClass_ptr = 
      iSegmentCabin.getBookingClass (iBookingClassStruct._classCode);
    if (lBookingClass_ptr == NULL) {
      // Instantiate a booking class object for the given key (class code);
      lBookingClass_ptr = &stdair::
        CmdBomManager::createBookingClass (iSegmentCabin,
                                           iBookingClassStruct._classCode);
    }
    assert (lBookingClass_ptr != NULL);

    // TODO: Upcabin the BOM booking-class with the attributes of the
    // booking-class struct.
    iBookingClassStruct.fill (*lBookingClass_ptr);
  }
  
}
