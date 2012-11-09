// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// Boost
#include <boost/date_time/date_iterator.hpp>
// StdAir
#include <stdair/basic/BasConst_BookingClass.hpp>
#include <stdair/basic/BasConst_Yield.hpp>
#include <stdair/basic/BasConst_Inventory.hpp>
#include <stdair/bom/BomManager.hpp>
#include <stdair/bom/BomRoot.hpp>
#include <stdair/bom/Inventory.hpp>
#include <stdair/bom/AirlineFeature.hpp>
#include <stdair/bom/FlightDate.hpp>
#include <stdair/bom/SegmentDate.hpp>
#include <stdair/bom/SegmentCabin.hpp>
#include <stdair/bom/FareFamily.hpp>
#include <stdair/bom/BookingClass.hpp>
#include <stdair/bom/LegDate.hpp>
#include <stdair/bom/LegCabin.hpp>
#include <stdair/bom/Bucket.hpp>
#include <stdair/bom/BomKeyManager.hpp>
#include <stdair/bom/ParsedKey.hpp>
#include <stdair/bom/BomRetriever.hpp>
#include <stdair/command/CmdCloneBomManager.hpp>
#include <stdair/factory/FacBom.hpp>
#include <stdair/factory/FacBomManager.hpp>
#include <stdair/service/Logger.hpp>
// AirInv
#include <airinv/AIRINV_Types.hpp>
#include <airinv/bom/FlightDateStruct.hpp>
#include <airinv/command/InventoryBuilder.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildInventory (stdair::BomRoot& ioBomRoot,
                  const FlightDateStruct& iFlightDateStruct) {
    const stdair::AirlineCode_T& lAirlineCode = iFlightDateStruct._airlineCode;
 
    // Instantiate an inventory object (if not exist)
    // for the given key (airline code)
    stdair::Inventory* lInventory_ptr = stdair::BomManager::
      getObjectPtr<stdair::Inventory> (ioBomRoot, lAirlineCode);
    if (lInventory_ptr == NULL) {
      stdair::InventoryKey lKey (lAirlineCode);
      lInventory_ptr =
        &stdair::FacBom<stdair::Inventory>::instance().create (lKey);
      stdair::FacBomManager::addToListAndMap (ioBomRoot, *lInventory_ptr);
      stdair::FacBomManager::linkWithParent (ioBomRoot, *lInventory_ptr);

      // Add the airline feature object to the inventory
      const stdair::AirlineFeatureKey lAirlineFeatureKey (lAirlineCode);
      stdair::AirlineFeature& lAirlineFeature =
        stdair::FacBom<stdair::AirlineFeature>::instance().create (lAirlineFeatureKey);
      stdair::FacBomManager::setAirlineFeature (*lInventory_ptr,
                                                lAirlineFeature);
      stdair::FacBomManager::linkWithParent (*lInventory_ptr, lAirlineFeature);
      // Link the airline feature object with the top of the BOM tree
      stdair::FacBomManager::addToListAndMap (ioBomRoot, lAirlineFeature);
    }
    assert (lInventory_ptr != NULL);

    // Build the flight-date within the inventory.
    buildFlightDate (*lInventory_ptr, iFlightDateStruct);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildFlightDate (stdair::Inventory& ioInventory,
                   const FlightDateStruct& iFlightDateStruct) {
    // Create the FlightDateKey
    const stdair::FlightDateKey lFlightDateKey (iFlightDateStruct._flightNumber,
                                                iFlightDateStruct._flightDate);

    // Check that the flight-date object is not already existing. If a
    // flight-date object with the same key has already been created,
    // then just update it, ifnot, create a flight-date and update it.
    stdair::FlightDate* lFlightDate_ptr = stdair::BomManager::
      getObjectPtr<stdair::FlightDate> (ioInventory, lFlightDateKey.toString());
    if (lFlightDate_ptr == NULL) {
      // Instantiate a flighy-date object for the given key (flight number and
      // flight date)
      lFlightDate_ptr =
        &stdair::FacBom<stdair::FlightDate>::instance().create (lFlightDateKey);
      stdair::FacBomManager::addToListAndMap (ioInventory, *lFlightDate_ptr);
      stdair::FacBomManager::linkWithParent (ioInventory, *lFlightDate_ptr);
    }
    assert (lFlightDate_ptr != NULL);

    // Update the BOM flight-date with the attributes of the flight-date struct.
    
    // Browse the list of leg-date struct and segment-date struct and
    // create the corresponding BOM.
    for (LegStructList_T::const_iterator itLegDate =
           iFlightDateStruct._legList.begin();
         itLegDate != iFlightDateStruct._legList.end(); ++itLegDate) {
      const LegStruct& lCurrentLegDateStruct = *itLegDate;
      buildLegDate (*lFlightDate_ptr, lCurrentLegDateStruct);
    }

    for (SegmentStructList_T::const_iterator itSegmentDate =
           iFlightDateStruct._segmentList.begin();
         itSegmentDate != iFlightDateStruct._segmentList.end();
         ++itSegmentDate) {
      const SegmentStruct& lCurrentSegmentDateStruct = *itSegmentDate;
      buildSegmentDate (*lFlightDate_ptr, lCurrentSegmentDateStruct);
    } 

    buildRoutingLegKey (*lFlightDate_ptr);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildLegDate (stdair::FlightDate& ioFlightDate,
                const LegStruct& iLegDateStruct) {
    // Check that the leg-date object is not already existing. If a
    // leg-date object with the same key has already been created,
    // then just update it, ifnot, create a leg-date and update it.
    stdair::LegDate* lLegDate_ptr = stdair::BomManager::
      getObjectPtr<stdair::LegDate>(ioFlightDate, iLegDateStruct._boardingPoint);

    if (lLegDate_ptr == NULL) {
      // Instantiate a leg-date object for the given key (boarding point);
      stdair::LegDateKey lKey (iLegDateStruct._boardingPoint);
      lLegDate_ptr = &stdair::FacBom<stdair::LegDate>::instance().create (lKey);
      stdair::FacBomManager::addToListAndMap (ioFlightDate, *lLegDate_ptr);
      stdair::FacBomManager::linkWithParent (ioFlightDate, *lLegDate_ptr);
    }
    assert (lLegDate_ptr != NULL);

    // Update the BOM leg-date with the attributes of the leg-date struct.
    iLegDateStruct.fill (*lLegDate_ptr);

    // Browse the list of leg-cabin structs and create the corresponding BOM.
    for (LegCabinStructList_T::const_iterator itLegCabin =
           iLegDateStruct._cabinList.begin();
         itLegCabin != iLegDateStruct._cabinList.end(); ++itLegCabin) {
      const LegCabinStruct& lCurrentLegCabinStruct = *itLegCabin;
      buildLegCabin (*lLegDate_ptr, lCurrentLegCabinStruct);
    }
  }  

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildRoutingLegKey (stdair::FlightDate& ioFlightDate) {

    // Browse the list of segment-dates and create direct accesses
    // within each segment-date.
    const stdair::SegmentDateList_T& lSegmentDateList = 
      stdair::BomManager::getList<stdair::SegmentDate> (ioFlightDate);
    for (stdair::SegmentDateList_T::const_iterator itSegmentDate = 
           lSegmentDateList.begin();
         itSegmentDate != lSegmentDateList.end(); ++itSegmentDate) {

      stdair::SegmentDate* lCurrentSegmentDate_ptr = *itSegmentDate;
      assert (lCurrentSegmentDate_ptr != NULL);

      /*
       * If the segment is just marketed by this carrier,
       * retrieve the operating segment and call the createDirectAcces
       * method on its parent (flight date).
       */
      const stdair::SegmentDate* lOperatingSegmentDate_ptr =
        lCurrentSegmentDate_ptr->getOperatingSegmentDate ();
      if (lOperatingSegmentDate_ptr == NULL) {

        const stdair::AirportCode_T& lBoardingPoint =
          lCurrentSegmentDate_ptr->getBoardingPoint();
        
        stdair::AirportCode_T currentBoardingPoint = lBoardingPoint;
        const stdair::AirportCode_T& lOffPoint =
          lCurrentSegmentDate_ptr->getOffPoint();
        
        // Add a sanity check so as to ensure that the loop stops. If
        // there are more than MAXIMAL_NUMBER_OF_LEGS legs, there is
        // an issue somewhere in the code (not in the parser, as the
        // segments are derived from the legs thanks to the
        // FlightPeriodStruct::buildSegments() method).
        unsigned short i = 1;
        while (currentBoardingPoint != lOffPoint
               && i <= stdair::MAXIMAL_NUMBER_OF_LEGS_IN_FLIGHT) {
          // Retrieve the (unique) LegDate getting that Boarding Point
          stdair::LegDate& lLegDate = stdair::BomManager::
            getObject<stdair::LegDate> (ioFlightDate, currentBoardingPoint);
          
          // Link the SegmentDate and LegDate together
	  const std::string& lRoutingKeyStr = lLegDate.describeRoutingKey();
	  lCurrentSegmentDate_ptr->addLegKey(lRoutingKeyStr);
          
          // Prepare the next iteration
          currentBoardingPoint = lLegDate.getOffPoint();
          ++i;
        }
        assert (i <= stdair::MAXIMAL_NUMBER_OF_LEGS_IN_FLIGHT);
      }
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildLegCabin (stdair::LegDate& ioLegDate,
                 const LegCabinStruct& iLegCabinStruct) {
    // Check that the leg-cabin object is not already existing. If a
    // leg-cabin object with the same key has already been created,
    // then just update it, ifnot, create a leg-cabin and update it.
    stdair::LegCabin* lLegCabin_ptr = stdair::BomManager::
      getObjectPtr<stdair::LegCabin> (ioLegDate, iLegCabinStruct._cabinCode);
    if (lLegCabin_ptr == NULL) {
      // Instantiate a leg-cabin object for the given key (cabin code);
      stdair::LegCabinKey lKey (iLegCabinStruct._cabinCode);
      lLegCabin_ptr = &stdair::FacBom<stdair::LegCabin>::instance().create(lKey);
      stdair::FacBomManager::addToListAndMap (ioLegDate, *lLegCabin_ptr);
      stdair::FacBomManager::linkWithParent (ioLegDate, *lLegCabin_ptr);
    }
    assert (lLegCabin_ptr != NULL);

    // TODO: Update the BOM leg-cabin with the attributes of the
    // leg-cabin struct.
    iLegCabinStruct.fill (*lLegCabin_ptr);

    // Browse the list of bucket structs and create the corresponding BOM.
    for (BucketStructList_T::const_iterator itBucket =
           iLegCabinStruct._bucketList.begin();
         itBucket != iLegCabinStruct._bucketList.end(); ++itBucket) {
      const BucketStruct& lCurrentBucketStruct = *itBucket;
      buildBucket (*lLegCabin_ptr, lCurrentBucketStruct);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::buildBucket (stdair::LegCabin& ioLegCabin,
                                      const BucketStruct& iBucketStruct) {
    // Create the BucketKey
    const stdair::BucketKey lBucketKey (iBucketStruct._seatIndex);

    // Check that the bucket object is not already existing. If a
    // bucket object with the same key has already been created,
    // then just update it, ifnot, create a bucket and update it.
    stdair::Bucket* lBucket_ptr = stdair::BomManager::
      getObjectPtr<stdair::Bucket> (ioLegCabin, lBucketKey.toString());
    if (lBucket_ptr == NULL) {
      // Instantiate a bucket object for the given key (seat index);
      stdair::BucketKey lKey (iBucketStruct._seatIndex);
      lBucket_ptr = &stdair::FacBom<stdair::Bucket>::instance().create (lKey);
      stdair::FacBomManager::addToListAndMap (ioLegCabin, *lBucket_ptr);
      stdair::FacBomManager::linkWithParent (ioLegCabin, *lBucket_ptr);
    }
    assert (lBucket_ptr != NULL);

    //
    iBucketStruct.fill (*lBucket_ptr);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildSegmentDate (stdair::FlightDate& ioFlightDate,
                    const SegmentStruct& iSegmentDateStruct) {
    // Check that the segment-date object is not already existing. If a
    // segment-date object with the same key has already been created,
    // then just update it, ifnot, create a segment-date and update it.
    const stdair::SegmentDateKey
      lSegmentDateKey (iSegmentDateStruct._boardingPoint,
                       iSegmentDateStruct._offPoint);
    stdair::SegmentDate* lSegmentDate_ptr = stdair::BomManager::
      getObjectPtr<stdair::SegmentDate>(ioFlightDate,lSegmentDateKey.toString());
    if (lSegmentDate_ptr == NULL) {
      // Instantiate a segment-date object for the given key (boarding
      // and off points);
      lSegmentDate_ptr =  &stdair::FacBom<stdair::SegmentDate>::
        instance().create (lSegmentDateKey);
      stdair::FacBomManager::addToListAndMap (ioFlightDate, *lSegmentDate_ptr);
      stdair::FacBomManager::linkWithParent (ioFlightDate, *lSegmentDate_ptr);
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
      const SegmentCabinStruct& lCurrentSegmentCabinStruct = *itSegmentCabin;
      buildSegmentCabin (*lSegmentDate_ptr, lCurrentSegmentCabinStruct);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildSegmentCabin (stdair::SegmentDate& ioSegmentDate,
                     const SegmentCabinStruct& iSegmentCabinStruct) {
    // Check that the segment-cabin object is not already existing. If a
    // segment-cabin object with the same key has already been created,
    // then just update it, ifnot, create a segment-cabin and update it.
    stdair::SegmentCabin* lSegmentCabin_ptr = stdair::BomManager::
      getObjectPtr<stdair::SegmentCabin> (ioSegmentDate,
                                          iSegmentCabinStruct._cabinCode);
    if (lSegmentCabin_ptr == NULL) {
      // Instantiate a segment-cabin object for the given key (cabin code);
      stdair::SegmentCabinKey lKey (iSegmentCabinStruct._cabinCode);
      lSegmentCabin_ptr = 
        &stdair::FacBom<stdair::SegmentCabin>::instance().create (lKey);

      // Link the segment-cabin to the segment-date
      stdair::FacBomManager::addToListAndMap (ioSegmentDate, *lSegmentCabin_ptr);
      stdair::FacBomManager::linkWithParent (ioSegmentDate, *lSegmentCabin_ptr);
    }
    assert (lSegmentCabin_ptr != NULL);

    // TODO: Update the BOM segment-cabin with the attributes of the
    // segment-cabin struct.
    iSegmentCabinStruct.fill (*lSegmentCabin_ptr);

    // Browse the list of fare family struct and create the corresponding BOM.
    for (FareFamilyStructList_T::const_iterator itFareFamily =
           iSegmentCabinStruct._fareFamilies.begin();
         itFareFamily != iSegmentCabinStruct._fareFamilies.end(); 
         ++itFareFamily) {
      const FareFamilyStruct& lCurrentFareFamilyStruct = *itFareFamily;
      buildFareFamily (*lSegmentCabin_ptr, lCurrentFareFamilyStruct);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildFareFamily (stdair::SegmentCabin& ioSegmentCabin,
                   const FareFamilyStruct& iFareFamilyStruct) {

    // Check that the fare family object is not already existing. If a
    // fare family object with the same key has already been created,
    // then just update it. If not, create a fare family and update it.
    stdair::FareFamily* lFareFamily_ptr = stdair::BomManager::
      getObjectPtr<stdair::FareFamily> (ioSegmentCabin,
                                        iFareFamilyStruct._familyCode);
    if (lFareFamily_ptr == NULL) {
      // Instantiate a fare family object for the given key (fare family code);
      const stdair::FareFamilyKey lFFKey (iFareFamilyStruct._familyCode);
      lFareFamily_ptr = 
        &stdair::FacBom<stdair::FareFamily>::instance().create (lFFKey);

      // Link the fare family to the segment-cabin
      stdair::FacBomManager::addToListAndMap (ioSegmentCabin, *lFareFamily_ptr);
      stdair::FacBomManager::linkWithParent (ioSegmentCabin, *lFareFamily_ptr);
    }
    assert (lFareFamily_ptr != NULL);

    // TODO: Upcabin the BOM fare family with the attributes of the
    // fare family struct.
    iFareFamilyStruct.fill (*lFareFamily_ptr);

    // Browse the list of booking-class struct and create the corresponding BOM.
    for (BookingClassStructList_T::const_iterator itBookingClass =
           iFareFamilyStruct._classList.begin();
         itBookingClass != iFareFamilyStruct._classList.end(); 
         ++itBookingClass) {
      const BookingClassStruct& lCurrentBookingClassStruct = *itBookingClass;
      buildBookingClass (*lFareFamily_ptr, lCurrentBookingClassStruct);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildBookingClass (stdair::FareFamily& ioFareFamily,
                     const BookingClassStruct& iBookingClassStruct) {

    // Check that the booking class object is not already existing. If a
    // booking-class object with the same key has already been created,
    // then just update it. If not, create a booking-class and update it.
    stdair::BookingClass* lBookingClass_ptr = stdair::BomManager::
      getObjectPtr<stdair::BookingClass> (ioFareFamily,
                                          iBookingClassStruct._classCode);
    if (lBookingClass_ptr == NULL) {
      // Instantiate a booking class object for the given key (class code);
      const stdair::BookingClassKey lClassKey (iBookingClassStruct._classCode);
      lBookingClass_ptr = 
        &stdair::FacBom<stdair::BookingClass>::instance().create (lClassKey);

      // Link the booking-class to the fare family
      stdair::FacBomManager::addToListAndMap (ioFareFamily, *lBookingClass_ptr);
      stdair::FacBomManager::linkWithParent (ioFareFamily, *lBookingClass_ptr);

      // Link the booking-class to the segment-cabin
      stdair::SegmentCabin& lSegmentCabin =
        stdair::BomManager::getParent<stdair::SegmentCabin> (ioFareFamily);
      stdair::FacBomManager::addToListAndMap (lSegmentCabin, *lBookingClass_ptr);

      // Link the booking-class to the segment-date
      stdair::SegmentDate& lSegmentDate =
        stdair::BomManager::getParent<stdair::SegmentDate> (lSegmentCabin);
      stdair::FacBomManager::addToListAndMap (lSegmentDate, *lBookingClass_ptr);
    }
    assert (lBookingClass_ptr != NULL);

    // TODO: Upcabin the BOM booking-class with the attributes of the
    // booking-class struct.
    iBookingClassStruct.fill (*lBookingClass_ptr);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::buildPartnerInventories (stdair::BomRoot& ioBomRoot) {

    // Browse the list of inventories to build partner inventories
    // within each inventory.
    const stdair::InventoryList_T& lInvList =
      stdair::BomManager::getList<stdair::Inventory> (ioBomRoot);
    for (stdair::InventoryList_T::const_iterator itInv = lInvList.begin();
         itInv != lInvList.end(); ++itInv) {
      stdair::Inventory* lCurrentInv_ptr = *itInv;
      assert (lCurrentInv_ptr != NULL);

      buildPartnerInventories (ioBomRoot, *lCurrentInv_ptr);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::buildPartnerInventories (stdair::BomRoot& ioBomRoot,
                                                  stdair::Inventory& ioInventory) {
    
    // Browse the list of flight-dates to build partner inventories.
    const stdair::FlightDateList_T& lFlightDateList = 
      stdair::BomManager::getList<stdair::FlightDate> (ioInventory);
    for (stdair::FlightDateList_T::const_iterator itFlightDate = 
           lFlightDateList.begin();
         itFlightDate != lFlightDateList.end(); ++itFlightDate) {
      stdair::FlightDate* lCurrentFlightDate_ptr = *itFlightDate;
      assert (lCurrentFlightDate_ptr != NULL);

      buildPartnerInventories (ioBomRoot, ioInventory, *lCurrentFlightDate_ptr);
    }
    
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::buildPartnerInventories (stdair::BomRoot& ioBomRoot,
                                                  stdair::Inventory& ioInventory,
                                                  stdair::FlightDate& iFlightDate) {
    
    // Browse the list of flight-dates to build partner inventories.
    const stdair::SegmentDateList_T& lSegmentDateList = 
      stdair::BomManager::getList<stdair::SegmentDate> (iFlightDate);
    for (stdair::SegmentDateList_T::const_iterator itSegmentDate = 
           lSegmentDateList.begin();
         itSegmentDate != lSegmentDateList.end(); ++itSegmentDate) {

      stdair::SegmentDate* lCurrentSegmentDate_ptr = *itSegmentDate;
      assert (lCurrentSegmentDate_ptr != NULL);	

      const stdair::RoutingLegKeyList_T& lRoutingLegKeyList = 
	lCurrentSegmentDate_ptr->getLegKeyList ();

      // Browse the list of routing leg keys.
      for (stdair::RoutingLegKeyList_T::const_iterator itRoutingLegKey = 
           lRoutingLegKeyList.begin();
         itRoutingLegKey != lRoutingLegKeyList.end(); ++itRoutingLegKey) {

        // Extract the operating airline code
        const stdair::ParsedKey& lParsedKey = 
          stdair::BomKeyManager::extractKeys (*itRoutingLegKey);
        const stdair::InventoryKey& lInventoryKey = 
          lParsedKey.getInventoryKey();
        const stdair::AirlineCode_T lOperatingAirlineCode = 
          lInventoryKey.getAirlineCode();

        // Extract the current airline code
        const stdair::AirlineCode_T lAirlineCode = 
          iFlightDate.getAirlineCode();

        // If the operating airline is not the current one
        if (lOperatingAirlineCode != lAirlineCode) {
          
          // Look for the inventory of the partner within the Bom root
          stdair::Inventory* lInventory_ptr =
            stdair::BomRetriever::
            retrieveInventoryFromKey (ioBomRoot, lOperatingAirlineCode);

          // Build the current segment full key
          std::ostringstream lRoutingSegment;
          lRoutingSegment << iFlightDate.getAirlineCode() << ";"
                          << iFlightDate.describeKey() << ";"
                          << lCurrentSegmentDate_ptr->getBoardingPoint() << ";"
                          << lCurrentSegmentDate_ptr->getOffPoint();

          // If such inventory does not exist, throw an exception
          if (lInventory_ptr == NULL) {
            std::ostringstream oMessage;
            oMessage << "The input file does not contain information about "
                     << "the '" << *itRoutingLegKey << "' leg date needed by "
                     << "the '" << lRoutingSegment.str() << "' segment.";
            STDAIR_LOG_ERROR (oMessage.str());
            throw MissingPartnerFlightDateWithinScheduleFile (oMessage.str());
          }
          assert (lInventory_ptr != NULL);

          // Build the partner inventory within the current inventory
          buildInventory (ioBomRoot, ioInventory, *itRoutingLegKey);

          // Build the current inventory as a partner of the partner inventory
          buildInventory (ioBomRoot, *lInventory_ptr, lRoutingSegment.str());
        }
      }
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildInventory (stdair::BomRoot& ioBomRoot,
                  stdair::Inventory& ioInventory,
                  const std::string& iFullKeyStr) {
    
    // Check that the inventory object is not already existing. If an
    // inventory object with the same key has already been created,
    // then just update it, ifnot, create an inventory and update it.
    // for the given key (iFullKeyStr)
    stdair::Inventory* lInventory_ptr =
      stdair::BomRetriever::retrieveInventoryFromLongKey (ioInventory,
                                                          iFullKeyStr);
    if (lInventory_ptr == NULL) {
      // Instantiate a flighy-date object for the given key (airline code
      // within the iFullKeyStr)
      stdair::Inventory* lOperatingInventory_ptr =
        stdair::BomRetriever::retrieveInventoryFromLongKey (ioBomRoot,
                                                            iFullKeyStr);
      assert (lOperatingInventory_ptr != NULL);
      lInventory_ptr =
        &stdair::FacBom<stdair::Inventory>::instance().create (*lOperatingInventory_ptr);
      stdair::FacBomManager::addToListAndMap (ioInventory, *lInventory_ptr);
      stdair::FacBomManager::linkWithParent (ioInventory, *lInventory_ptr);
    }
    assert (lInventory_ptr != NULL);

    // Build the flight-date within the inventory.
    buildFlightDate (ioBomRoot, *lInventory_ptr, iFullKeyStr);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  buildFlightDate (stdair::BomRoot& ioBomRoot,
                   stdair::Inventory& ioInventory,
                   const std::string& iFullKeyStr) {

    // Check that the flight-date object is not already existing. If a
    // flight-date object with the same key has already been created,
    // then just update it, ifnot, create a flight-date and update it.
    stdair::FlightDate* lFlightDate_ptr  =
      stdair::BomRetriever::retrieveFlightDateFromLongKey (ioInventory,
                                                           iFullKeyStr);
    if (lFlightDate_ptr == NULL) {
      // Instantiate a flighy-date object for the given key (flight number and
      // flight date within the iFullKeyStr)
      stdair::FlightDate* lOperatingFlightDate_ptr =
        stdair::BomRetriever::retrieveFlightDateFromLongKey (ioBomRoot,
                                                             iFullKeyStr);
      assert (lOperatingFlightDate_ptr != NULL);
      stdair::FlightDate& lFlightDate =
        cloneFlightDate (*lOperatingFlightDate_ptr);
      stdair::FacBomManager::addToListAndMap (ioInventory, lFlightDate);
      stdair::FacBomManager::linkWithParent (ioInventory, lFlightDate);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::FlightDate& InventoryBuilder::
  cloneFlightDate (const stdair::FlightDate& iFlightDate) { 

    /**
     * FlightDate level (only)
     */ 
    stdair::FlightDate& lCloneFlightDate = 
      stdair::FacBom<stdair::FlightDate>::instance().create (iFlightDate);   
    
    // Check whether there are LegDate objects
    const bool hasLegDateList = stdair::BomManager::hasList<stdair::LegDate> (iFlightDate);
    if (hasLegDateList == true) {

      // Browse the leg-dates
      const stdair::LegDateList_T& lLegDateList =
	stdair::BomManager::getList<stdair::LegDate> (iFlightDate);
      for (stdair::LegDateList_T::const_iterator itLD = lLegDateList.begin();
	   itLD != lLegDateList.end(); ++itLD) {
	const stdair::LegDate* lLD_ptr = *itLD;
	assert (lLD_ptr != NULL); 

	// Clone the current leg-date
	stdair::LegDate& lCloneLegDate = cloneLegDate (*lLD_ptr);
	stdair::FacBomManager::addToListAndMap (lCloneFlightDate, lCloneLegDate);
	stdair::FacBomManager::linkWithParent (lCloneFlightDate, lCloneLegDate);
      }  
    }

    // Check whether there are SegmentDate objects
    const bool hasSegmentDateList = 
      stdair::BomManager::hasList<stdair::SegmentDate> (iFlightDate);
    if (hasSegmentDateList == true) {
    
      // Browse the segment-dates
      const stdair::SegmentDateList_T& lSegmentDateList =
	stdair::BomManager::getList<stdair::SegmentDate> (iFlightDate);
      for (stdair::SegmentDateList_T::const_iterator itSD = lSegmentDateList.begin();
	   itSD != lSegmentDateList.end(); ++itSD) {
	const stdair::SegmentDate* lSD_ptr = *itSD;
	assert (lSD_ptr != NULL);	

	// Clone the current segment-date
	stdair::SegmentDate& lCloneSegmentDate = cloneSegmentDate (*lSD_ptr);
	stdair::FacBomManager::addToListAndMap (lCloneFlightDate, lCloneSegmentDate);
	stdair::FacBomManager::linkWithParent (lCloneFlightDate, lCloneSegmentDate);

      }
    }
      
    return lCloneFlightDate; 
  } 

  // ////////////////////////////////////////////////////////////////////
  stdair::LegDate& InventoryBuilder::cloneLegDate (const stdair::LegDate& iLegDate) { 

    /**
     * Leg Date level (only)
     */ 
    stdair::LegDate& lCloneLegDate = 
      stdair::FacBom<stdair::LegDate>::instance().create (iLegDate);

    // Check whether there are LegCabin objects
    const bool hasLegCabinList = stdair::BomManager::hasList<stdair::LegCabin> (iLegDate);
    if (hasLegCabinList == true) {
      // Browse the leg-cabins
      const stdair::LegCabinList_T& lLegCabinList =
        stdair::BomManager::getList<stdair::LegCabin> (iLegDate);
      for (stdair::LegCabinList_T::const_iterator itLC = lLegCabinList.begin();
           itLC != lLegCabinList.end(); ++itLC) {
        const stdair::LegCabin* lLC_ptr = *itLC;
        assert (lLC_ptr != NULL);

        // Clone the current leg-cabin	
        stdair::LegCabin& lCloneLegCabin = cloneLegCabin (*lLC_ptr);
	stdair::FacBomManager::addToListAndMap (lCloneLegDate, lCloneLegCabin);
	stdair::FacBomManager::linkWithParent (lCloneLegDate, lCloneLegCabin);
      }
    }

    return lCloneLegDate;
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::LegCabin& InventoryBuilder::cloneLegCabin (const stdair::LegCabin& iLegCabin) { 

    /**
     * Leg Cabin level (only)
     */ 
    stdair::LegCabin& lCloneLegCabin = 
      stdair::FacBom<stdair::LegCabin>::instance().create (iLegCabin);

    // Check whether there are Bucket objects
    const bool hasBucketList = stdair::BomManager::hasList<stdair::Bucket> (iLegCabin);
    if (hasBucketList == true) {
      // Browse the buckets
      const stdair::BucketList_T& lBucketList =
        stdair::BomManager::getList<stdair::Bucket> (iLegCabin);
      for (stdair::BucketList_T::const_iterator itBucket = lBucketList.begin();
           itBucket != lBucketList.end(); ++itBucket) {
        const stdair::Bucket* lBucket_ptr = *itBucket;
        assert (lBucket_ptr != NULL);

        // Clone the current bucket
        stdair::Bucket& lCloneBucket = cloneBucket (*lBucket_ptr);
	stdair::FacBomManager::addToListAndMap (lCloneLegCabin, lCloneBucket);
	stdair::FacBomManager::linkWithParent (lCloneLegCabin, lCloneBucket);
      }  
    }

    return lCloneLegCabin;
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::Bucket& InventoryBuilder::cloneBucket (const stdair::Bucket& iBucket) { 

    /**
     * Leg Cabin level (only)
     */ 
    stdair::Bucket& lCloneBucket = 
      stdair::FacBom<stdair::Bucket>::instance().create (iBucket);  

    return lCloneBucket;
  } 

  // ////////////////////////////////////////////////////////////////////
  stdair::SegmentDate& InventoryBuilder::
  cloneSegmentDate (const stdair::SegmentDate& iSegmentDate) { 

    /**
     * Segment Date level (only)
     */ 
    stdair::SegmentDate& lCloneSegmentDate = 
      stdair::FacBom<stdair::SegmentDate>::instance().create (iSegmentDate);

    // Check whether there are SegmentCabin objects
    const bool hasSegmentCabinList = 
      stdair::BomManager::hasList<stdair::SegmentCabin> (iSegmentDate);
    if (hasSegmentCabinList == true) {
      // Browse the segment-cabins
      const stdair::SegmentCabinList_T& lSegmentCabinList =
        stdair::BomManager::getList<stdair::SegmentCabin> (iSegmentDate);
      for (stdair::SegmentCabinList_T::const_iterator itSC = lSegmentCabinList.begin();
           itSC != lSegmentCabinList.end(); ++itSC) {
        const stdair::SegmentCabin* lSC_ptr = *itSC;
        assert (lSC_ptr != NULL);

        // Clone the current segment-cabin	
        stdair::SegmentCabin& lCloneSegmentCabin = cloneSegmentCabin (*lSC_ptr);
	stdair::FacBomManager::addToListAndMap (lCloneSegmentDate, lCloneSegmentCabin);
	stdair::FacBomManager::linkWithParent (lCloneSegmentDate, lCloneSegmentCabin);
   
	linkBookingClassesWithSegment (lCloneSegmentDate,
				       lCloneSegmentCabin);

      }
    }
    return lCloneSegmentDate;
  } 

  // ////////////////////////////////////////////////////////////////////
  void InventoryBuilder::
  linkBookingClassesWithSegment (stdair::SegmentDate& iCloneSegmentDate,
				 stdair::SegmentCabin& iCloneSegmentCabin) { 	

    // Browse the fare families to link the booking-classes to the 
    // segment-cabin and to the segment-date 
    const bool hasFareFamilyList = 
      stdair::BomManager::hasList<stdair::FareFamily> (iCloneSegmentCabin);
    if (hasFareFamilyList == true) {
      const stdair::FareFamilyList_T& lCloneFFList =
	stdair::BomManager::getList<stdair::FareFamily> (iCloneSegmentCabin);
      for (stdair::FareFamilyList_T::const_iterator itCloneFF = lCloneFFList.begin();
	   itCloneFF != lCloneFFList.end(); ++itCloneFF) {
	const stdair::FareFamily* lCloneFF_ptr = *itCloneFF;
	assert (lCloneFF_ptr != NULL);

	// Browse the list of booking classes 
	const bool hasBookingClasslist = 
	  stdair::BomManager::hasList<stdair::BookingClass> (*lCloneFF_ptr);
	if (hasBookingClasslist == true) {
	  const stdair::BookingClassList_T& lCloneBCList =
	    stdair::BomManager::getList<stdair::BookingClass> (*lCloneFF_ptr);
	  for (stdair::BookingClassList_T::const_iterator itCloneBC =
		 lCloneBCList.begin();
	       itCloneBC != lCloneBCList.end(); ++itCloneBC) {
	    const stdair::BookingClass* lCloneBC_ptr = *itCloneBC;
	    assert (lCloneBC_ptr != NULL);
		
	    // Link the booking-class to the segment-cabin
	    stdair::FacBomManager::addToListAndMap (iCloneSegmentCabin, 
						    *lCloneBC_ptr);

	    // Link the booking-class to the segment-date
	    stdair::FacBomManager::addToListAndMap (iCloneSegmentDate, 
						    *lCloneBC_ptr);
	  }
	}
      }
    }
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::SegmentCabin& InventoryBuilder::
  cloneSegmentCabin (const stdair::SegmentCabin& iSegmentCabin) { 

    /**
     * Segment Cabin level (only)
     */ 
    stdair::SegmentCabin& lCloneSegmentCabin = 
      stdair::FacBom<stdair::SegmentCabin>::instance().create (iSegmentCabin);

    // Check whether there are fare family objects 
    const bool hasFareFamilyList = 
      stdair::BomManager::hasList<stdair::FareFamily> (iSegmentCabin);
    if (hasFareFamilyList == true) {
      // Browse the fare families
      const stdair::FareFamilyList_T& lFareFamilyList =
       stdair::BomManager::getList<stdair::FareFamily> (iSegmentCabin);
      for (stdair::FareFamilyList_T::const_iterator itFF = lFareFamilyList.begin();
           itFF != lFareFamilyList.end(); ++itFF) {
        const stdair::FareFamily* lFF_ptr = *itFF;
        assert (lFF_ptr != NULL);

        // Clone the current fare-family	
        stdair::FareFamily& lCloneFareFamily = cloneFareFamily (*lFF_ptr);
	stdair::FacBomManager::addToListAndMap (lCloneSegmentCabin, lCloneFareFamily);
	stdair::FacBomManager::linkWithParent (lCloneSegmentCabin, lCloneFareFamily);
      }
    }
    
    return lCloneSegmentCabin;
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::FareFamily& InventoryBuilder::
  cloneFareFamily (const stdair::FareFamily& iFareFamily) {
    /**
     * Fare Family level (only)
     */ 
    stdair::FareFamily& lCloneFareFamily = 
      stdair::FacBom<stdair::FareFamily>::instance().create (iFareFamily);

    // Check whether there are booking classes objects
    const bool hasBookingClassList = 
      stdair::BomManager::hasList<stdair::BookingClass> (iFareFamily);
    if (hasBookingClassList == true) {
      // Browse the list of booking classes
      const stdair::BookingClassList_T& lBookingClassList =
        stdair::BomManager::getList<stdair::BookingClass> (iFareFamily);
      for (stdair::BookingClassList_T::const_iterator itBookingClass =
             lBookingClassList.begin();
           itBookingClass != lBookingClassList.end(); ++itBookingClass) {
        const stdair::BookingClass* lBC_ptr = *itBookingClass;
        assert (lBC_ptr != NULL);

        // Clone the current booking class
        stdair::BookingClass& lCloneBookingClass = cloneBookingClass (*lBC_ptr);
        stdair::FacBomManager::addToListAndMap (lCloneFareFamily, lCloneBookingClass);
        stdair::FacBomManager::linkWithParent (lCloneFareFamily, lCloneBookingClass);
      }
    }

    return lCloneFareFamily;
  }

  // ////////////////////////////////////////////////////////////////////
  stdair::BookingClass& InventoryBuilder::
  cloneBookingClass (const stdair::BookingClass& iBookingClass) {
    
    /**
     * Booking Class level (only)
     */ 
    stdair::BookingClass& lCloneBookingClass = 
       stdair::FacBom<stdair::BookingClass>::instance().create (iBookingClass);

    return lCloneBookingClass;
  }
}

