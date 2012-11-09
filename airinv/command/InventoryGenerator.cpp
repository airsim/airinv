// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// Boost
#include <boost/date_time/date_iterator.hpp>
// StdAir
#include <stdair/stdair_types.hpp>
#include <stdair/basic/BasConst_Inventory.hpp>
#include <stdair/basic/BasConst_SellUpCurves.hpp>
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
#include <stdair/bom/SimpleNestingStructure.hpp>
#include <stdair/bom/NestingNode.hpp>
#include <stdair/bom/Policy.hpp>
#include <stdair/bom/Bucket.hpp>  
#include <stdair/bom/BomKeyManager.hpp>
#include <stdair/factory/FacBomManager.hpp>
#include <stdair/service/Logger.hpp>
// AirInv
#include <airinv/bom/FlightPeriodStruct.hpp>
#include <airinv/command/InventoryGenerator.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  void InventoryGenerator::
  createFlightDate (stdair::BomRoot& ioBomRoot,
                    const FlightPeriodStruct& iFlightPeriod) {
    const stdair::AirlineCode_T& lAirlineCode = iFlightPeriod._airlineCode;
 
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
    
    // Generate all the dates corresponding to the period
    // and create the corresponding flight-dates.
    const stdair::DatePeriod_T lDateRange = iFlightPeriod._dateRange;

    for (boost::gregorian::day_iterator itDate = lDateRange.begin();
         itDate != lDateRange.end(); ++itDate) {
      const stdair::Date_T& currentDate = *itDate;

      // Retrieve, for the current day, the Day-Of-the-Week (thanks to Boost)
      const unsigned short currentDoW = currentDate.day_of_week().as_number();
        
      // The FlightPeriod structure stores which Days (-Of-the-Week) are
      // active within the week. For each day (Mon., Tue., etc.), a boolean
      // states whether the Flight is active for that day.
      const stdair::DoWStruct& lDoWList = iFlightPeriod._dow;
      const bool isDoWActive = lDoWList.getStandardDayOfWeek (currentDoW);

      if (isDoWActive == true) {
        createFlightDate (ioBomRoot, *lInventory_ptr, currentDate,
                          iFlightPeriod);
      }
    }
  }
  
  // ////////////////////////////////////////////////////////////////////
  void InventoryGenerator::
  createFlightDate (stdair::BomRoot& ioBomRoot,
                    stdair::Inventory& ioInventory,
                    const stdair::Date_T& iFlightDate,
                    const FlightPeriodStruct& iFlightPeriod) {
    // Create the FlightDateKey
    const stdair::FlightNumber_T& lFlightNumber = iFlightPeriod._flightNumber;
    stdair::FlightDateKey lFlightDateKey (lFlightNumber, iFlightDate);

    // DEBUG
    // STDAIR_LOG_DEBUG ("Creating flight-date: " << lFlightDateKey.toString());
    
    // Check that the flight-date object is not already existing. If a
    // FlightDate object with the same key has already been created,
    // it means that the schedule input file is invalid (two flight-periods
    // are overlapping).
    stdair::FlightDate* lFlightDate_ptr = stdair::BomManager::
      getObjectPtr<stdair::FlightDate> (ioInventory, lFlightDateKey.toString());
    if (lFlightDate_ptr != NULL) {
      std::ostringstream oMessage;
      oMessage << ioInventory.describeKey() << ", "
               << lFlightDate_ptr->describeKey();
      throw FlightDateDuplicationException (oMessage.str());
    }

    // Instantiate a fligh-date object with the given key (flight number and
    // flight date)
    lFlightDate_ptr =
      &stdair::FacBom<stdair::FlightDate>::instance().create (lFlightDateKey);
    stdair::FacBomManager::addToListAndMap (ioInventory, *lFlightDate_ptr);
    stdair::FacBomManager::linkWithParent (ioInventory, *lFlightDate_ptr);
      
    // Iterate on the leg-dates
    stdair::Duration_T currentOffTime (0, 0, 0);
    stdair::AirportCode_T previousOffPoint;
    const LegStructList_T& lLegList = iFlightPeriod._legList;
    for (LegStructList_T::const_iterator itLeg = lLegList.begin();
         itLeg != lLegList.end(); ++itLeg) {
      const LegStruct& lLeg = *itLeg;

      // Create the leg-branch of the flight-date BOM
      stdair::LegDate& lLegDate =
        createLegDate (*lFlightDate_ptr, iFlightDate, lLeg);

      // TODO: Check that the boarding date/time of the next leg is greated
      // than the off date/time of the current leg. Throw an exception
      // otherwise.

      // TODO: specify, in the schedule input file specifications, that the
      // legs should be given in their natural order.
      // Then, replace the assertion by a thrown exception.
      //
      // Check that the legs are given in their natural order. If the schedule
      // input does not respect that assumption, the following assertion will
      // fail.
      if (itLeg != lLegList.begin()) {
        const stdair::AirportCode_T& currentBoardingPoint =
          lLegDate.getBoardingPoint();
        assert (currentBoardingPoint == previousOffPoint);
      }
        
      // Set the local variable for the next iteration
      previousOffPoint = lLegDate.getOffPoint();
    }

    // Iterate on the segment structures
    const SegmentStructList_T& lSegmentList = iFlightPeriod._segmentList;
    for (SegmentStructList_T::const_iterator itSegment = lSegmentList.begin();
         itSegment != lSegmentList.end(); ++itSegment) {
      const SegmentStruct& lSegment = *itSegment;

      createSegmentDate (ioBomRoot, *lFlightDate_ptr, lSegment);
    }

    createRoutingLegKey (*lFlightDate_ptr);
  }  

  // ////////////////////////////////////////////////////////////////////
  void InventoryGenerator::
  createRoutingLegKey (stdair::FlightDate& ioFlightDate) {

    // Browse the list of segment-dates and create direct accesses
    // within each segment-date.
    const stdair::SegmentDateList_T& lSegmentDateList = 
      stdair::BomManager::getList<stdair::SegmentDate> (ioFlightDate);
    for (stdair::SegmentDateList_T::const_iterator itSegmentDate = 
           lSegmentDateList.begin();
         itSegmentDate != lSegmentDateList.end(); ++itSegmentDate) {

      stdair::SegmentDate* lCurrentSegmentDate_ptr = *itSegmentDate;
      assert (lCurrentSegmentDate_ptr != NULL);

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

  // ////////////////////////////////////////////////////////////////////
  stdair::LegDate& InventoryGenerator::
  createLegDate (stdair::FlightDate& ioFlightDate,
                 const stdair::Date_T& iReferenceDate,
                 const LegStruct& iLeg) {
    // Create the leg-date corresponding to the boarding point.
    stdair::LegDateKey lKey (iLeg._boardingPoint);
    stdair::LegDate& lLegDate =
      stdair::FacBom<stdair::LegDate>::instance().create (lKey);
    stdair::FacBomManager::addToListAndMap (ioFlightDate, lLegDate);
    stdair::FacBomManager::linkWithParent (ioFlightDate, lLegDate);

    // Set the leg-date attributes
    iLeg.fill (iReferenceDate, lLegDate);
    
    // Iterate on the cabins
    const LegCabinStructList_T& lCabinList = iLeg._cabinList;
    for (LegCabinStructList_T::const_iterator itCabin = lCabinList.begin();
         itCabin != lCabinList.end(); ++itCabin) {
      const LegCabinStruct& lCabin = *itCabin;

      // Create the leg-cabin-branch of the leg-date 
      createLegCabin (lLegDate, lCabin);
    }

    return lLegDate;
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryGenerator::
  createLegCabin (stdair::LegDate& ioLegDate,
                  const LegCabinStruct& iCabin) {
    // Instantiate an leg-cabin object with the corresponding cabin code
    const stdair::LegCabinKey lKey (iCabin._cabinCode);
    stdair::LegCabin& lLegCabin =
      stdair::FacBom<stdair::LegCabin>::instance().create (lKey);
    stdair::FacBomManager::addToListAndMap (ioLegDate, lLegCabin);
    stdair::FacBomManager::linkWithParent (ioLegDate, lLegCabin);

    // Set the Leg-Cabin attributes
    iCabin.fill (lLegCabin);

    // Iterate on the bucket
    const BucketStructList_T& lBucketList = iCabin._bucketList;
    for (BucketStructList_T::const_iterator itBucket = lBucketList.begin();
         itBucket != lBucketList.end(); ++itBucket) {
      const BucketStruct& lBucket = *itBucket;

      // Create the bucket of the leg-cabin
      createBucket (lLegCabin, lBucket);
    }
  }
    
  // ////////////////////////////////////////////////////////////////////
  void InventoryGenerator::createBucket (stdair::LegCabin& ioLegCabin,
                                         const BucketStruct& iBucket) {
    // Instantiate a bucket object with the corresponding seat index
    const stdair::BucketKey lKey (iBucket._seatIndex);
    stdair::Bucket& lBucket =
      stdair::FacBom<stdair::Bucket>::instance().create (lKey);
    stdair::FacBomManager::addToListAndMap (ioLegCabin, lBucket);
    stdair::FacBomManager::linkWithParent (ioLegCabin, lBucket);

    // Set the Bucket attributes
    iBucket.fill (lBucket);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryGenerator::
  createSegmentDate (stdair::BomRoot& ioBomRoot,
                     stdair::FlightDate& ioFlightDate,
                     const SegmentStruct& iSegment) {
    // Set the segment-date primary key
    const stdair::AirportCode_T& lBoardingPoint = iSegment._boardingPoint;
    const stdair::AirportCode_T& lOffPoint = iSegment._offPoint;
    stdair::SegmentDateKey lSegmentDateKey (lBoardingPoint, lOffPoint);
    // Instantiate an segment-date object with the key.
    stdair::SegmentDate& lSegmentDate =
      stdair::FacBom<stdair::SegmentDate>::instance().create (lSegmentDateKey);
    stdair::FacBomManager::addToListAndMap (ioFlightDate, lSegmentDate);
    stdair::FacBomManager::linkWithParent (ioFlightDate, lSegmentDate);
    
    // Set the segment-date attributes
    iSegment.fill (lSegmentDate);
      
    // Iterate on the Cabins
    const SegmentCabinStructList_T& lCabinList = iSegment._cabinList;
    for (SegmentCabinStructList_T::const_iterator itCabin =
           lCabinList.begin(); itCabin != lCabinList.end(); ++itCabin) {
      const SegmentCabinStruct& lCabin = *itCabin;

      // Create the segment-cabin-branch of the segment-date BOM
      createSegmentCabin (ioBomRoot, lSegmentDate, lCabin);
    }
  }
    
  // ////////////////////////////////////////////////////////////////////
  void InventoryGenerator::
  createSegmentCabin (stdair::BomRoot& ioBomRoot,
                      stdair::SegmentDate& ioSegmentDate,
                      const SegmentCabinStruct& iCabin) {

    // Instantiate an segment-cabin object with the corresponding cabin code
    stdair::SegmentCabinKey lKey (iCabin._cabinCode);
    stdair::SegmentCabin& lSegmentCabin =
      stdair::FacBom<stdair::SegmentCabin>::instance().create (lKey);

    // Link the segment-cabin to its parent, the segment-date
    stdair::FacBomManager::addToListAndMap (ioSegmentDate, lSegmentCabin);
    stdair::FacBomManager::linkWithParent (ioSegmentDate, lSegmentCabin);
    
    // Set the segment-cabin attributes
    iCabin.fill (lSegmentCabin);

    // Create the list of fare families        
    for (FareFamilyStructList_T::const_iterator itFareFamily =
           iCabin._fareFamilies.begin();
         itFareFamily != iCabin._fareFamilies.end(); itFareFamily++) {
      const FareFamilyStruct& lFareFamilyStruct = *itFareFamily;

      // Create the fare families and the booking classes.
      createFareFamily (ioBomRoot, lSegmentCabin, lFareFamilyStruct);
    } 

    const unsigned int lNbOfFareFamilies = iCabin._fareFamilies.size();
    if (lNbOfFareFamilies > 1) {
      lSegmentCabin.activateFareFamily();
    } 

    // Create the display nesting structure.
    createDisplayNestingStructure (lSegmentCabin);
  }
    
  // ////////////////////////////////////////////////////////////////////
  void InventoryGenerator::
  createFareFamily (stdair::BomRoot& ioBomRoot,
                    stdair::SegmentCabin& ioSegmentCabin,
                    const FareFamilyStruct& iFF) {
    // Instantiate an segment-cabin object with the corresponding cabin code
    stdair::FareFamilyKey lKey (iFF._familyCode);
    stdair::FareFamily& lFareFamily =
      stdair::FacBom<stdair::FareFamily>::instance().create (lKey);

    // Link the fare family to its parent, the segment-cabin
    stdair::FacBomManager::addToListAndMap (ioSegmentCabin, lFareFamily);
    stdair::FacBomManager::linkWithParent (ioSegmentCabin, lFareFamily);
    
    // Set the fare family attributes
    iFF.fill (lFareFamily);
    const stdair::FRAT5Curve_T& lFRAT5Curve =
      ioBomRoot.getFRAT5Curve (iFF._frat5CurveKey);
    lFareFamily.setFrat5Curve (lFRAT5Curve);
    const stdair::FFDisutilityCurve_T& lDisutilityCurve =
      ioBomRoot.getFFDisutilityCurve (iFF._ffDisutilityCurveKey);
    lFareFamily.setDisutilityCurve (lDisutilityCurve);

    // Iterate on the classes
    const stdair::ClassList_String_T& lClassList = iFF._classes;
    for (stdair::ClassList_String_T::const_iterator itClass =
           lClassList.begin(); itClass != lClassList.end(); ++itClass) {
      // Transform the single-character class code into a STL string
      std::ostringstream ostr;
      ostr << *itClass;
      const stdair::ClassCode_T lClassCode (ostr.str());
      
      // Create the booking class branch of the segment-cabin BOM
      createClass (lFareFamily, lClassCode);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryGenerator::createClass (stdair::FareFamily& ioFareFamily,
                                        const stdair::ClassCode_T& iClassCode) {

    // Instantiate a booking class object with the given class code
    const stdair::BookingClassKey lClassKey (iClassCode);
    stdair::BookingClass& lClass =
      stdair::FacBom<stdair::BookingClass>::instance().create (lClassKey);

    // Link the booking-class to the fare family
    stdair::FacBomManager::addToListAndMap (ioFareFamily, lClass);
    stdair::FacBomManager::linkWithParent (ioFareFamily, lClass);

    // Link the booking-class to the segment-cabin
    stdair::SegmentCabin& lSegmentCabin =
      stdair::BomManager::getParent<stdair::SegmentCabin> (ioFareFamily);
    stdair::FacBomManager::addToListAndMap (lSegmentCabin, lClass);

    // Link the booking-class to the segment-date
    stdair::SegmentDate& lSegmentDate =
      stdair::BomManager::getParent<stdair::SegmentDate> (lSegmentCabin);
    stdair::FacBomManager::addToListAndMap (lSegmentDate, lClass);
  }
    
  // ////////////////////////////////////////////////////////////////////
  void InventoryGenerator::
  createDisplayNestingStructure (stdair::SegmentCabin& ioSegmentCabin) {
    // Create the nesting structure.
    stdair::NestingStructureKey lKey (stdair::DISPLAY_NESTING_STRUCTURE_CODE);
    stdair::SimpleNestingStructure& lNestingStructure =
      stdair::FacBom<stdair::SimpleNestingStructure>::instance().create(lKey);
    stdair::FacBomManager::addToListAndMap (ioSegmentCabin, lNestingStructure);
    stdair::FacBomManager::linkWithParent (ioSegmentCabin, lNestingStructure);
    
    // Browse the list of booking classes and create the nesting structure
    // based on that list. Each nesting node consists of a booking class.
    const stdair::BookingClassList_T& lBCList =
      stdair::BomManager::getList<stdair::BookingClass>(ioSegmentCabin);
    for (stdair::BookingClassList_T::const_iterator itBC = lBCList.begin();
         itBC != lBCList.end(); ++itBC) {
      stdair::BookingClass* lBC_ptr = *itBC;
      assert (lBC_ptr != NULL);

      // Create a nesting node
      stdair::NestingNodeCode_T lNodeCode (lBC_ptr->describeKey());
      stdair::NestingNodeKey lNodeKey (lNodeCode);
      stdair::NestingNode& lNestingNode =
        stdair::FacBom<stdair::NestingNode>::instance().create (lNodeKey);
      stdair::FacBomManager::addToList (lNestingStructure, lNestingNode);
      stdair::FacBomManager::linkWithParent (lNestingStructure, lNestingNode);

      // Add the booking class to the node.
      stdair::FacBomManager::addToList (lNestingNode, *lBC_ptr);
    }
  }
}
