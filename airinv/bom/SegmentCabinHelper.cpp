// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
#include <limits>
// StdAir
#include <stdair/basic/BasConst_Inventory.hpp>
#include <stdair/basic/float_utils.hpp>
#include <stdair/bom/BomManager.hpp>
#include <stdair/bom/FlightDate.hpp>
#include <stdair/bom/LegCabin.hpp>
#include <stdair/bom/SegmentCabin.hpp>
#include <stdair/bom/FareFamily.hpp>
#include <stdair/bom/BookingClass.hpp>
#include <stdair/bom/SimpleNestingStructure.hpp>
#include <stdair/bom/NestingNode.hpp>
#include <stdair/bom/Policy.hpp>
#include <stdair/factory/FacBomManager.hpp>
// AirInv
#include <airinv/bom/SegmentCabinHelper.hpp>
#include <airinv/bom/FlightDateHelper.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  void SegmentCabinHelper::initialiseAU (stdair::SegmentCabin& iSegmentCabin) {

    // Initialise the capacity and availability pool.
    const stdair::LegCabinList_T& lLCList =
      stdair::BomManager::getList<stdair::LegCabin> (iSegmentCabin);

    stdair::CabinCapacity_T lCapacity =
      std::numeric_limits<stdair::CabinCapacity_T>::max();
    for (stdair::LegCabinList_T::const_iterator itLC = lLCList.begin();
         itLC != lLCList.end(); ++itLC) {

      const stdair::LegCabin* lLC_ptr = *itLC;
      assert (lLC_ptr != NULL);

      const stdair::CabinCapacity_T& lCabinCap = lLC_ptr->getOfferedCapacity();
      if (lCapacity > lCabinCap) {
        lCapacity = lCabinCap;
      }
    }
    iSegmentCabin.setCapacity (lCapacity);
    iSegmentCabin.setAvailabilityPool (lCapacity);
    
    // Browse the list of booking classes and set the AU of each booking
    // class to the availability pool of the cabin.
    const stdair::BookingClassList_T& lBCList =
      stdair::BomManager::getList<stdair::BookingClass> (iSegmentCabin);
    for (stdair::BookingClassList_T::const_iterator itBC = lBCList.begin();
         itBC != lBCList.end(); ++itBC) {
      stdair::BookingClass* lBC_ptr = *itBC;
      assert (lBC_ptr != NULL);
      lBC_ptr->setAuthorizationLevel (lCapacity);
    }
  }
  
  // ////////////////////////////////////////////////////////////////////
  void SegmentCabinHelper::
  updateFromReservation (const stdair::FlightDate& iFlightDate,
                         stdair::SegmentCabin& ioSegmentCabin,
                         const stdair::PartySize_T& iNbOfBookings){
    // Update the commited space of the segment-cabin.
    ioSegmentCabin.updateFromReservation (iNbOfBookings);

    // Update the availability of the flight-date.
    FlightDateHelper::updateAvailability (iFlightDate, ioSegmentCabin,
                                          iNbOfBookings);
  }

  // ////////////////////////////////////////////////////////////////////
  void SegmentCabinHelper::
  buildPseudoBidPriceVector (stdair::SegmentCabin& ioSegmentCabin) {
    // Retrieve the segment-cabin capacity.
    const stdair::Availability_T& lAvlPool =
      ioSegmentCabin.getAvailabilityPool();
    unsigned int lAvlPoolInt;
    if (lAvlPool < 0) {
      lAvlPoolInt = 0;
    } else {
      assert (lAvlPool >= 0);
      lAvlPoolInt = static_cast<unsigned int> (lAvlPool);
    }
    stdair::BidPriceVector_T lPseudoBidPriceVector (lAvlPoolInt, 0.0);

    // Browse the leg-cabin list.
    const stdair::LegCabinList_T& lLCList =
      stdair::BomManager::getList<stdair::LegCabin> (ioSegmentCabin);
    for (stdair::LegCabinList_T::const_iterator itLC = lLCList.begin();
         itLC != lLCList.end(); ++itLC) {
      const stdair::LegCabin* lLC_ptr = *itLC;
      assert (lLC_ptr != NULL);

      const stdair::BidPriceVector_T& lBPV = lLC_ptr->getBidPriceVector();
      stdair::BidPriceVector_T::const_reverse_iterator itBP = lBPV.rbegin();
      for (stdair::BidPriceVector_T::reverse_iterator itPBP =
             lPseudoBidPriceVector.rbegin();
           itPBP != lPseudoBidPriceVector.rend(); ++itPBP, ++itBP) {
        assert (itBP != lBPV.rend());
        stdair::BidPrice_T& lCurrentPBP = *itPBP;
        const stdair::BidPrice_T& lCurrentBP = *itBP;
        lCurrentPBP += lCurrentBP;
      }
    }

    ioSegmentCabin.setBidPriceVector (lPseudoBidPriceVector);
    
    // // DEBUG
    // std::ostringstream ostr;
    // ostr << "Pseudo BPV: ";
    // for (stdair::BidPriceVector_T::const_iterator itBP =
    //       lPseudoBidPriceVector.begin(); itBP != lPseudoBidPriceVector.end();
    //      ++itBP) {
    //   const stdair::BidPrice_T& lCurrentBP = *itBP;      
    //   ostr << lCurrentBP << " ";
    // }
    // //    STDAIR_LOG_DEBUG (ostr.str());
    // std::cout << ostr.str() << std::endl;
  }

  // ////////////////////////////////////////////////////////////////////
  void SegmentCabinHelper::
  updateBookingControlsUsingPseudoBidPriceVector (const stdair::SegmentCabin& iSegmentCabin) {
    // Retrieve the pseudo bid price vector.
    const stdair::BidPriceVector_T& lPseudoBPV =
      iSegmentCabin.getBidPriceVector();
    const stdair::Availability_T& lAvlPool= iSegmentCabin.getAvailabilityPool();

    // Update the cumulative booking limit for all booking classes.
    // Browse the nesting structure
    const stdair::SimpleNestingStructure& lYieldBasedNestingStructure = 
      stdair::BomManager::getObject<stdair::SimpleNestingStructure>(iSegmentCabin, stdair::YIELD_BASED_NESTING_STRUCTURE_CODE);
    const stdair::NestingNodeList_T& lNestingNodeList = 
      stdair::BomManager::getList<stdair::NestingNode>(lYieldBasedNestingStructure);
    for (stdair::NestingNodeList_T::const_iterator itNS =
           lNestingNodeList.begin();
         itNS != lNestingNodeList.end(); ++itNS) {
      stdair::NestingNode* lNestingNode_ptr = *itNS;
      assert (lNestingNode_ptr != NULL);
      const stdair::Yield_T lNodeYield = 
        lNestingNode_ptr->getYield();
      if (lNodeYield < 0) {
        continue;
      }
      const stdair::BookingClassList_T& lBCList =
        stdair::BomManager::getList<stdair::BookingClass> (*lNestingNode_ptr);
      stdair::BookingClassList_T::const_iterator itBC = lBCList.begin();
      assert(itBC != lBCList.end());
      // Browse the booking class list of the current node   
      const stdair::Yield_T& lYield = lNestingNode_ptr->getYield();
      stdair::BookingLimit_T lCumuBL = lAvlPool;
      for (stdair::BidPriceVector_T::const_reverse_iterator itBP =
           lPseudoBPV.rbegin(); itBP != lPseudoBPV.rend(); ++itBP) {
        const stdair::BidPrice_T& lBP = *itBP;
        if (lYield < lBP) {
          lCumuBL = itBP - lPseudoBPV.rbegin();
          break;
        }
      }
      for (; itBC != lBCList.end(); ++itBC) {
        stdair::BookingClass* lBC_ptr = *itBC;
        assert (lBC_ptr != NULL);
        lBC_ptr->setCumulatedBookingLimit (lCumuBL);
      }
    }
    // Update the authorization levels from the booking limits
    updateAUs (iSegmentCabin);
  }

  // ////////////////////////////////////////////////////////////////////
  void SegmentCabinHelper::updateAUs(const stdair::SegmentCabin& iSegmentCabin){
    // Browse the nesting structure and compute the AU from the
    // cumulative booking counter and the cumulative booking limit.
    stdair::NbOfBookings_T lCumulativeBookingCounter = 0.0;
    // Browse the nesting structure
    const stdair::SimpleNestingStructure& lYieldBasedNestingStructure = 
      stdair::BomManager::getObject<stdair::SimpleNestingStructure>(iSegmentCabin, stdair::YIELD_BASED_NESTING_STRUCTURE_CODE);
    const stdair::NestingNodeList_T& lNestingNodeList = 
      stdair::BomManager::getList<stdair::NestingNode>(lYieldBasedNestingStructure);
    for (stdair::NestingNodeList_T::const_reverse_iterator itNS =
           lNestingNodeList.rbegin();
         itNS != lNestingNodeList.rend(); ++itNS) {
      stdair::NestingNode* lNestingNode_ptr = *itNS;
      assert (lNestingNode_ptr != NULL);
      const stdair::Yield_T lNodeYield = 
        lNestingNode_ptr->getYield();
      if (lNodeYield < 0) {
        continue;
      } 
      const stdair::BookingClassList_T& lBCList =
        stdair::BomManager::getList<stdair::BookingClass> (*lNestingNode_ptr);
      stdair::BookingClassList_T::const_iterator itBC = lBCList.begin();
      assert(itBC != lBCList.end());
      const stdair::BookingLimit_T& lCumuBookingLimit =
        (*itBC)->getCumulatedBookingLimit();
      // Browse the booking class list of the current node to update the 
      // cumulative booking counter
      for (; itBC != lBCList.end(); ++itBC) {
        stdair::BookingClass* lBC_ptr = *itBC;
        assert (lBC_ptr != NULL);
        assert(lCumuBookingLimit == lBC_ptr->getCumulatedBookingLimit());
        const stdair::NbOfBookings_T& lNbOfBookings = lBC_ptr->getNbOfBookings();
        lCumulativeBookingCounter += lNbOfBookings;
      }
      stdair::AuthorizationLevel_T lAU =
          lCumulativeBookingCounter + lCumuBookingLimit;
      // Browse the booking class list of the current node to set
      // the authorization level of all booking classes of the node
      for (itBC = lBCList.begin(); itBC != lBCList.end(); ++itBC) {
        stdair::BookingClass* lBC_ptr = *itBC;
        assert (lBC_ptr != NULL);
        lBC_ptr->setAuthorizationLevel (lAU);
        // DEBUG
        // STDAIR_LOG_DEBUG ("Updating the AU for class: "
        //                   << lBC_ptr->describeKey()
        //                   << ", with BL: " << lCumuBookingLimit
        //                   << ", CumuBkg: " << lCumulativeBookingCounter
        //                   << ", AU: " << lAU);
      }
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void SegmentCabinHelper::
  updateAvailabilities (const stdair::SegmentCabin& iSegmentCabin) {
    // Browse the nesting structure and compute the avl from the
    // cumulative booking counter and the AU.
    stdair::NbOfBookings_T lCumulativeBookingCounter = 0.0;
    const stdair::SimpleNestingStructure& lYieldBasedNestingStructure = 
      stdair::BomManager::getObject<stdair::SimpleNestingStructure>(iSegmentCabin, stdair::YIELD_BASED_NESTING_STRUCTURE_CODE);
    const stdair::NestingNodeList_T& lNestingNodeList = 
      stdair::BomManager::getList<stdair::NestingNode>(lYieldBasedNestingStructure);
    for (stdair::NestingNodeList_T::const_reverse_iterator itNS =
           lNestingNodeList.rbegin();
         itNS != lNestingNodeList.rend(); ++itNS) {
      stdair::NestingNode* lNestingNode_ptr = *itNS;
      assert (lNestingNode_ptr != NULL);
      const stdair::Yield_T& lNodeYield = lNestingNode_ptr->getYield();
      if (lNodeYield < 0) {
        continue;
      }
      const stdair::BookingClassList_T& lBCList =
        stdair::BomManager::getList<stdair::BookingClass> (*lNestingNode_ptr);
      stdair::BookingClassList_T::const_iterator itBC = lBCList.begin();
      assert(itBC != lBCList.end());
      stdair::BookingClass* lFirstBC_ptr = *itBC;
      assert (lFirstBC_ptr != NULL);
      const stdair::AuthorizationLevel_T& lNodeAU =
        lFirstBC_ptr->getAuthorizationLevel();
      // Browse the booking class list of the current node to update the 
      // cumulative booking counter
      for (; itBC != lBCList.end(); ++itBC) {
        stdair::BookingClass* lBC_ptr = *itBC;
        assert (lBC_ptr != NULL);
        assert(lNodeAU == lBC_ptr->getAuthorizationLevel());
        const stdair::NbOfBookings_T& lNbOfBookings = lBC_ptr->getNbOfBookings();
        lCumulativeBookingCounter += lNbOfBookings;
      }
      const stdair::Availability_T lNodeAvl = lNodeAU - lCumulativeBookingCounter;
      // Browse the booking class list of the current node to set
      // the availability of all booking classes of the node
      for (itBC = lBCList.begin(); itBC != lBCList.end(); ++itBC) {
        stdair::BookingClass* lBC_ptr = *itBC;
        assert (lBC_ptr != NULL);    
        lBC_ptr->setSegmentAvailability (lNodeAvl);
      }
    }

    // Cascading
    stdair::NestingNodeList_T::const_iterator itCurrentNode = 
      lNestingNodeList.begin();
    assert (itCurrentNode != lNestingNodeList.end());
    stdair::NestingNodeList_T::const_iterator itNextNode = itCurrentNode; 
    ++itNextNode;
    for (; itNextNode != lNestingNodeList.end(); ++itCurrentNode, ++itNextNode) {
      assert(itCurrentNode != lNestingNodeList.end());
      stdair::NestingNode* lCurrentNode_ptr = *itCurrentNode;
      assert (lCurrentNode_ptr != NULL);
      const stdair::Yield_T& lCurrentNodeYield = lCurrentNode_ptr->getYield();
      if (lCurrentNodeYield < 0) {
        break;
      }
      const stdair::BookingClassList_T& lCurrentBCList = 
        stdair::BomManager::getList<stdair::BookingClass> (*lCurrentNode_ptr);
      stdair::BookingClassList_T::const_iterator itCurrentBC = 
        lCurrentBCList.begin();
      stdair::BookingClass* lCurrentBC_ptr = *(itCurrentBC);
      assert (lCurrentBC_ptr != NULL);
      assert(itNextNode != lNestingNodeList.end());
      stdair::NestingNode* lNextNode_ptr = *itNextNode;
      assert (lNextNode_ptr != NULL);
      const stdair::Yield_T& lNextNodeYield = lNextNode_ptr->getYield();
      if (lNextNodeYield < 0) {
        break;
      }
      const stdair::BookingClassList_T& lNextBCList = 
        stdair::BomManager::getList<stdair::BookingClass> (*lNextNode_ptr);
      stdair::BookingClassList_T::const_iterator itNextBC = 
        lNextBCList.begin();
      stdair::BookingClass* lNextBC_ptr = *(itNextBC);
      assert (lNextBC_ptr != NULL);
      const stdair::Availability_T& lCurrentAvl = 
        lCurrentBC_ptr->getSegmentAvailability();
      const stdair::Availability_T& lNextAvl = 
        lNextBC_ptr->getSegmentAvailability();
      if (lCurrentAvl < lNextAvl) {
        for (; itNextBC != lNextBCList.end(); ++itNextBC) {
          lNextBC_ptr = *itNextBC;
          assert (lNextBC_ptr != NULL);
          lNextBC_ptr->setSegmentAvailability (lCurrentAvl);
        }
      }
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void SegmentCabinHelper::
  initYieldBasedNestingStructure (stdair::SegmentCabin& ioSegmentCabin) {
    // Create the nesting structure.
    stdair::NestingStructureKey lKey (stdair::YIELD_BASED_NESTING_STRUCTURE_CODE);
    stdair::SimpleNestingStructure& lNestingStructure =
      stdair::FacBom<stdair::SimpleNestingStructure>::instance().create(lKey);
    stdair::FacBomManager::addToListAndMap (ioSegmentCabin, lNestingStructure);
    stdair::FacBomManager::linkWithParent (ioSegmentCabin, lNestingStructure);
    
    // Build a multimap of booking classes with their yields as keys.
    std::multimap<const stdair::Yield_T, stdair::BookingClass*> lClassMap;
    const stdair::BookingClassList_T& lBCList =
      stdair::BomManager::getList<stdair::BookingClass> (ioSegmentCabin);
    for (stdair::BookingClassList_T::const_iterator itBC = lBCList.begin();
         itBC != lBCList.end(); ++itBC) {
      stdair::BookingClass* lBC_ptr = *itBC;
      assert (lBC_ptr != NULL);
      const stdair::Yield_T& lYield = lBC_ptr->getYield();
      lClassMap.insert(std::multimap<const stdair::Yield_T, stdair::BookingClass*>::value_type(lYield, lBC_ptr));
    }

    stdair::Yield_T lLastYield = -1.0;
    stdair::NestingNode* lCurrentNode_ptr = NULL;
    for (std::multimap<const stdair::Yield_T, stdair::BookingClass*>::reverse_iterator itBC = lClassMap.rbegin();
         itBC != lClassMap.rend(); ++itBC) {
      const stdair::Yield_T& lCurrentYield = itBC->first;
      stdair::BookingClass* lBC_ptr = itBC->second;
      
      // Compare the current yield and the last one.
      // TODO: use float utils
      //if (lCurrentYield.AlmostEquals (lLastYield) == false) {
      if (lCurrentYield != lLastYield) {
        // Create a nesting node
        stdair::NestingNodeCode_T lNodeCode (lBC_ptr->describeKey());
        stdair::NestingNodeKey lNodeKey (lNodeCode);
        stdair::NestingNode& lNestingNode =
          stdair::FacBom<stdair::NestingNode>::instance().create (lNodeKey);
        stdair::FacBomManager::addToList (lNestingStructure, lNestingNode);
        stdair::FacBomManager::linkWithParent (lNestingStructure, lNestingNode);
        lCurrentNode_ptr = &lNestingNode;
        lCurrentNode_ptr->setYield(lCurrentYield);
        // Add the booking class to the node.
        stdair::FacBomManager::addToList (lNestingNode, *lBC_ptr);
        lLastYield = lCurrentYield;
      } else {        
        // Add the booking class to the current node.
        stdair::FacBomManager::addToList (*lCurrentNode_ptr, *lBC_ptr);      
      }
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void SegmentCabinHelper::
  initListOfUsablePolicies (stdair::SegmentCabin& ioSegmentCabin) {
    const stdair::FareFamilyList_T& lFareFamilyList =
      stdair::BomManager::getList<stdair::FareFamily> (ioSegmentCabin);
    stdair::FareFamilyList_T::const_iterator itFF = lFareFamilyList.begin();
    
    unsigned int lPolicyCounter = 0;
    std::ostringstream oStr;
    oStr << lPolicyCounter;
    stdair::PolicyKey lKey (oStr.str());
    stdair::Policy& lPolicy =
      stdair::FacBom<stdair::Policy>::instance().create(lKey);
    stdair::FacBomManager::addToList (ioSegmentCabin, lPolicy);
    stdair::FacBomManager::linkWithParent (ioSegmentCabin, lPolicy);
    createPolicies (ioSegmentCabin, lFareFamilyList, itFF, lPolicy,
                    lPolicyCounter,
                    std::numeric_limits<stdair::Yield_T>::max());
  }

  // ////////////////////////////////////////////////////////////////////
  void SegmentCabinHelper::
  createPolicies (stdair::SegmentCabin& ioSegmentCabin,
                  const stdair::FareFamilyList_T& iFareFamilyList,
                  const stdair::FareFamilyList_T::const_iterator& itFF,
                  stdair::Policy& ioCurrentPolicy,
                  unsigned int& ioPolicyCounter,
                  const stdair::Yield_T& iPreviousYield) {
    if (itFF != iFareFamilyList.end()) {
      // We add a booking class of the next Fare Family if it is cheapest than 
      // the previous booking class in the policy. 
      // Assumption: the fare family list is sorted according to their fares:
      // Fare_1 > Fare_2 > ... > Fare_n
      const stdair::FareFamily* lFF_ptr = *itFF;
      //Retrieve the booking class list of the current fare family
      const stdair::BookingClassList_T& lBookingClassList =
        stdair::BomManager::getList<stdair::BookingClass> (*lFF_ptr);
      stdair::BookingClassList_T::const_iterator itBC =
        lBookingClassList.begin();
      stdair::FareFamilyList_T::const_iterator lItFF = itFF;
      lItFF++;
      
      // Browse the booking class list
      for (; itBC != lBookingClassList.end(); ++itBC) {
        stdair::BookingClass* lBC_ptr = *itBC;
        assert(lBC_ptr != NULL);
        const stdair::Yield_T& lCurrentYield = lBC_ptr->getYield();
        if (lCurrentYield >= iPreviousYield) {
          continue;
        }
        assert(lCurrentYield < iPreviousYield);
        // Add the current booking class to the list, update the current policy
        // and call the same method for the next fare family
        ++ioPolicyCounter;
        std::ostringstream oStr;
        oStr << ioPolicyCounter;
        stdair::PolicyKey lKey (oStr.str());
        stdair::Policy& lNewPolicy =
          stdair::FacBom<stdair::Policy>::instance().create(lKey);
        stdair::FacBomManager::addToList (ioSegmentCabin, lNewPolicy);
        stdair::FacBomManager::linkWithParent (ioSegmentCabin, lNewPolicy);

        // Copy the list of booking classes of the current policy to the new one
        bool hasAListOfBC = 
          stdair::BomManager::hasList<stdair::BookingClass> (ioCurrentPolicy);
        if (hasAListOfBC == true) { 
          const stdair::BookingClassList_T& lToBeCopiedBCList =
            stdair::BomManager::getList<stdair::BookingClass> (ioCurrentPolicy);
          for (stdair::BookingClassList_T::const_iterator itBCToBeCopied =
                 lToBeCopiedBCList.begin();
               itBCToBeCopied != lToBeCopiedBCList.end(); ++itBCToBeCopied) {
            stdair::BookingClass* lBCToBeCopied_ptr = *itBCToBeCopied;
            assert (lBCToBeCopied_ptr != NULL);
            stdair::FacBomManager::addToList (lNewPolicy, *lBCToBeCopied_ptr);
          }
        }          
        stdair::FacBomManager::addToList(lNewPolicy, *lBC_ptr);
        
        createPolicies (ioSegmentCabin, iFareFamilyList, lItFF, lNewPolicy,
                        ioPolicyCounter, lCurrentYield);
      }
    }

  }
  
}
