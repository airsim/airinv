// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// StdAir
#include <stdair/bom/BomManager.hpp>
#include <stdair/bom/FlightDate.hpp>
#include <stdair/bom/LegCabin.hpp>
#include <stdair/bom/SegmentCabin.hpp>
#include <stdair/bom/FareFamily.hpp>
#include <stdair/bom/BookingClass.hpp>
#include <stdair/bom/SimpleNestingStruct.hpp>
#include <stdair/bom/NestingTypes.hpp>
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

    // Update the commited space of the member leg-cabins.
    const stdair::LegCabinList_T& lLegCabinList =
      stdair::BomManager::getList<stdair::LegCabin> (ioSegmentCabin);
    for (stdair::LegCabinList_T::const_iterator itLegCabin =
           lLegCabinList.begin();
         itLegCabin != lLegCabinList.end(); ++itLegCabin) {
      stdair::LegCabin* lLegCabin_ptr = *itLegCabin;
      assert (lLegCabin_ptr != NULL);
      lLegCabin_ptr->updateFromReservation (iNbOfBookings);
    }

    // Update the availability pool of all the segment-cabin which belong to the
    // same flight-date.
    const stdair::CabinCode_T& lCabinCode = ioSegmentCabin.getCabinCode();
    FlightDateHelper::updateAvailablityPool (iFlightDate, lCabinCode);
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
    const stdair::SimpleNestingStruct& lNestingStruct = 
      iSegmentCabin.getNestingStruct();
    for (stdair::NestingNodeMap_T::const_reverse_iterator itNS = lNestingStruct.rbegin();
         itNS != lNestingStruct.rend(); ++itNS) {
      const stdair::BookingClassList_T& lBCList = (*itNS).second;
      stdair::BookingClassList_T::const_iterator itBC = lBCList.begin();
      assert(itBC != lBCList.end());
      // Browse the booking class list of the current node   
      const stdair::Yield_T& lYield = (*itNS).first;
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
    const stdair::SimpleNestingStruct& lNestingStruct = 
      iSegmentCabin.getNestingStruct();
    for (stdair::NestingNodeMap_T::const_iterator itNS = lNestingStruct.begin();
         itNS != lNestingStruct.end(); ++itNS) {
      const stdair::BookingClassList_T& lBCList = (*itNS).second;
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
    const stdair::SimpleNestingStruct& lNestingStruct = iSegmentCabin.getNestingStruct();
    for (stdair::NestingNodeMap_T::const_iterator itNS = lNestingStruct.begin();
         itNS != lNestingStruct.end(); ++itNS) {
      const stdair::BookingClassList_T& lBCList = (*itNS).second;
      stdair::BookingClassList_T::const_iterator itBC = lBCList.begin();
      assert(itBC != lBCList.end());
      const stdair::AuthorizationLevel_T& lNodeAU = (*itBC)->getAuthorizationLevel();
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
    stdair::NestingNodeMap_T::const_reverse_iterator itCurrentNode = lNestingStruct.rbegin();
    assert (itCurrentNode != lNestingStruct.rend());
    stdair::NestingNodeMap_T::const_reverse_iterator itNextNode = itCurrentNode; 
    ++itNextNode;
    for (; itNextNode != lNestingStruct.rend(); ++itCurrentNode, ++itNextNode) {
      stdair::BookingClassList_T lCurrentBCList = (*itCurrentNode).second;
      stdair::BookingClass* lCurrentBC_ptr = *(lCurrentBCList.begin());
      assert (lCurrentBC_ptr != NULL);
      stdair::BookingClassList_T lNextBCList = (*itNextNode).second;
      stdair::BookingClass* lNextBC_ptr = *(lNextBCList.begin());
      assert (lNextBC_ptr != NULL);
      const stdair::Availability_T& lCurrentAvl = lCurrentBC_ptr->getSegmentAvailability();
      const stdair::Availability_T& lNextAvl = lNextBC_ptr->getSegmentAvailability();
      if (lCurrentAvl < lNextAvl) {
        for (stdair::BookingClassList_T::const_iterator itNextBC = lNextBCList.begin();
             itNextBC != lNextBCList.end(); ++itNextBC) {
          lNextBC_ptr = *itNextBC;
          assert (lNextBC_ptr != NULL);
          lNextBC_ptr->setSegmentAvailability (lCurrentAvl);
        }
      }
    }
  }
}
