// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <cmath>
// StdAir
#include <stdair/basic/BasConst_Inventory.hpp>
#include <stdair/bom/BomRetriever.hpp>
#include <stdair/bom/BomManager.hpp>
#include <stdair/bom/SegmentDate.hpp>
#include <stdair/bom/SegmentCabin.hpp>
#include <stdair/bom/FareFamily.hpp>
#include <stdair/bom/BookingClass.hpp>
#include <stdair/bom/SegmentSnapshotTable.hpp>
#include <stdair/service/Logger.hpp>
// AirInv
#include <airinv/basic/BasConst_Curves.hpp>
#include <airinv/bom/SegmentSnapshotTableHelper.hpp>
#include <airinv/bom/FlightDateHelper.hpp>
#include <airinv/bom/SegmentCabinHelper.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  void SegmentSnapshotTableHelper::
  takeSnapshots (stdair::SegmentSnapshotTable& ioSegmentSnapshotTable,
                 const stdair::DateTime_T& iSnapshotTime) {
    // Retrieve the segment-cabin index and take the snapshots for
    // each segment-cabin.
    const stdair::SegmentCabinIndexMap_T& lSegmentCabinIndexMap =
      ioSegmentSnapshotTable.getSegmentCabinIndexMap();
    for (stdair::SegmentCabinIndexMap_T::const_iterator itSCIdx =
           lSegmentCabinIndexMap.begin();
         itSCIdx != lSegmentCabinIndexMap.end(); ++itSCIdx) {
      const stdair::SegmentCabin* lSC_ptr = itSCIdx->first;
      assert (lSC_ptr != NULL);
      const stdair::SegmentDataID_T& lSCIdx = itSCIdx->second;

      const stdair::Date_T& lSnapshotDate = iSnapshotTime.date();
      
      // Compare the date of the snapshot time and the departure date of
      // the segment-cabin in order to verify the necescity of taking snapshots.
      const stdair::SegmentDate& lSegmentDate =
        stdair::BomManager::getParent<stdair::SegmentDate> (*lSC_ptr);
      const stdair::Date_T& lDepartureDate = lSegmentDate.getBoardingDate();
      const stdair::DateOffset_T lDateOffset = lDepartureDate - lSnapshotDate;
      const stdair::DTD_T lDTD = lDateOffset.days() + 1;

      if (lDTD >= 0 && lDTD <= stdair::DEFAULT_MAX_DTD) {
        SegmentCabinHelper::updateAvailabilities (*lSC_ptr);
        takeSnapshots (ioSegmentSnapshotTable, lDTD, *lSC_ptr, lSCIdx);
        registerProductAndPriceOrientedBookings (ioSegmentSnapshotTable,
                                                 lDTD, *lSC_ptr, lSCIdx);
      }
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void SegmentSnapshotTableHelper::
  takeSnapshots (stdair::SegmentSnapshotTable& ioSegmentSnapshotTable,
                 const stdair::DTD_T& iDTD,
                 const stdair::SegmentCabin& iSegmentCabin,
                 const stdair::SegmentDataID_T iSegmentCabinIdx) {

    // Extract the views for the corresponding DTD and segment-cabin.
    stdair::SegmentCabinDTDSnapshotView_T lBookingView = ioSegmentSnapshotTable.
      getSegmentCabinDTDBookingSnapshotView (iSegmentCabinIdx,
                                             iSegmentCabinIdx, iDTD);
    stdair::SegmentCabinDTDSnapshotView_T lCancellationView = ioSegmentSnapshotTable.
      getSegmentCabinDTDCancellationSnapshotView (iSegmentCabinIdx,
                                                  iSegmentCabinIdx, iDTD);
    stdair::SegmentCabinDTDSnapshotView_T lAvailabilityView = ioSegmentSnapshotTable.
      getSegmentCabinDTDAvailabilitySnapshotView (iSegmentCabinIdx,
                                                  iSegmentCabinIdx, iDTD);
    
    // Retrieve the block index of the segment-cabin.
    std::ostringstream lSCMapKey;
    lSCMapKey << stdair::DEFAULT_SEGMENT_CABIN_VALUE_TYPE
              << iSegmentCabin.describeKey();    
    const stdair::ClassIndex_T& lCabinIdx =
      ioSegmentSnapshotTable.getClassIndex (lSCMapKey.str());
    lAvailabilityView[lCabinIdx] = iSegmentCabin.getAvailabilityPool();
    lBookingView[lCabinIdx] = iSegmentCabin.getCommittedSpace();
    

    // Browse the booking class list
    const stdair::BookingClassList_T& lBCList =
      stdair::BomManager::getList<stdair::BookingClass> (iSegmentCabin);
    for (stdair::BookingClassList_T::const_iterator itBC = lBCList.begin();
         itBC != lBCList.end(); ++itBC) {
      const stdair::BookingClass* lBookingClass_ptr = *itBC;
      assert (lBookingClass_ptr != NULL);

      // Retrieve the block index of the booking class.
      const stdair::ClassIndex_T& lIdx =
        ioSegmentSnapshotTable.getClassIndex (lBookingClass_ptr->describeKey());

      // DEBUG
      // STDAIR_LOG_DEBUG ("Taking snapshot for "
      //                   << iSegmentCabin.describeKey() << ", "
      //                   << lBookingClass_ptr->describeKey()
      //                   << ", DTD: " << iDTD << ", nb of bookings: "
      //                   << lBookingClass_ptr->getNbOfBookings());

      // Write the snapshot.
      lBookingView[lIdx]=lBookingClass_ptr->getNbOfBookings();
      lCancellationView[lIdx] =
        lBookingClass_ptr->getNbOfCancellations();
      lAvailabilityView[lIdx] =
        lBookingClass_ptr->getSegmentAvailability();
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void SegmentSnapshotTableHelper::registerProductAndPriceOrientedBookings
  (stdair::SegmentSnapshotTable& ioSegmentSnapshotTable, const stdair::DTD_T& iDTD,
   const stdair::SegmentCabin& iSegmentCabin,
   const stdair::SegmentDataID_T iSegmentCabinIdx) {

    // Extract the views for the corresponding DTD and segment-cabin.
    stdair::SegmentCabinDTDRangeSnapshotView_T lRangeBookingView =
      ioSegmentSnapshotTable.getSegmentCabinDTDRangeBookingSnapshotView (iSegmentCabinIdx, iSegmentCabinIdx, iDTD, iDTD + 1);
    stdair::SegmentCabinDTDRangeSnapshotView_T lRangeCancellationView =
      ioSegmentSnapshotTable.getSegmentCabinDTDRangeCancellationSnapshotView (iSegmentCabinIdx, iSegmentCabinIdx, iDTD, iDTD + 1);
    stdair::SegmentCabinDTDSnapshotView_T lProductOrientedGrossBookingView =
      ioSegmentSnapshotTable.getSegmentCabinDTDProductOrientedGrossBookingSnapshotView (iSegmentCabinIdx, iSegmentCabinIdx, iDTD);
    stdair::SegmentCabinDTDSnapshotView_T lPriceOrientedGrossBookingView =
      ioSegmentSnapshotTable.getSegmentCabinDTDPriceOrientedGrossBookingSnapshotView (iSegmentCabinIdx, iSegmentCabinIdx, iDTD);
    stdair::SegmentCabinDTDSnapshotView_T lProductOrientedNetBookingView =
      ioSegmentSnapshotTable.getSegmentCabinDTDProductOrientedNetBookingSnapshotView (iSegmentCabinIdx, iSegmentCabinIdx, iDTD);
    stdair::SegmentCabinDTDSnapshotView_T lPriceOrientedNetBookingView =
      ioSegmentSnapshotTable.getSegmentCabinDTDPriceOrientedNetBookingSnapshotView (iSegmentCabinIdx, iSegmentCabinIdx, iDTD);

    // Retrieve the lowest yield and the lowest class and treat the number of gross
    // bookings of this class the price oriented bookings.
    const stdair::BookingClassList_T& lBCList =
      stdair::BomManager::getList<stdair::BookingClass> (iSegmentCabin);
    stdair::BookingClassList_T::const_iterator iterBC = lBCList.begin();
    assert (iterBC != lBCList.end());
    stdair::BookingClass* lLowestClass_ptr = *iterBC; 
    assert (lLowestClass_ptr  != NULL);
    stdair::Yield_T lLowestYield = lLowestClass_ptr->getYield();
    for (; iterBC != lBCList.end(); iterBC++) {
      const stdair::BookingClass* lBookingClass_ptr = *iterBC;
      assert (lBookingClass_ptr != NULL);
      const stdair::Yield_T& lYield = lBookingClass_ptr->getYield();
      if (lYield < lLowestYield) {
        lLowestYield = lYield;
        lLowestClass_ptr = *iterBC;
      }
    }
    assert (lLowestClass_ptr != NULL);

    // Retrieve the block index of the booking class.
    const stdair::ClassIndex_T& lClassIdx =
      ioSegmentSnapshotTable.getClassIndex (lLowestClass_ptr->describeKey());

    // Compute the number of gross bookings for this class.
    const stdair::NbOfBookings_T lNbOfNetBkgs = 
      lRangeBookingView[lClassIdx][0] - lRangeBookingView[lClassIdx][1];
    const stdair::NbOfCancellations_T lNbOfCx =
      lRangeCancellationView[lClassIdx][0]-lRangeCancellationView[lClassIdx][1];
    const stdair::NbOfBookings_T lNbOfGrossBkgs = lNbOfNetBkgs + lNbOfCx;

    // Write these numbern of bookings to the price-oriented value.
    lPriceOrientedGrossBookingView[lClassIdx] = lNbOfGrossBkgs;
    lPriceOrientedNetBookingView[lClassIdx] = lNbOfNetBkgs;
    
    // Boolean for "no lower class available" verification.
    bool noLowerClassAvl = true;
    if (lLowestClass_ptr->getSegmentAvailability() >= 1.0) {
      noLowerClassAvl = false;
    }
    
    // Browse the booking class list
    stdair::BookingClassList_T::const_reverse_iterator itBC = lBCList.rbegin();
    for (; itBC != lBCList.rend(); itBC++) {
      const stdair::BookingClass* lBookingClass_ptr = *itBC;
      assert (lBookingClass_ptr != NULL);

      // Retrieve the yield of the this class.
      const stdair::Yield_T& lYield = lBookingClass_ptr->getYield();
      assert (lYield >= lLowestYield);
      if (lYield > lLowestYield) {
      
        // Retrieve the block index of the booking class.
        const stdair::ClassIndex_T& lIdx =
          ioSegmentSnapshotTable.getClassIndex (lBookingClass_ptr->describeKey());

        // Compute the number of gross bookings for this class.
        const stdair::NbOfBookings_T lNetBkgs = 
          lRangeBookingView[lIdx][0] - lRangeBookingView[lIdx][1];
        const stdair::NbOfCancellations_T lCx =
          lRangeCancellationView[lIdx][0] - lRangeCancellationView[lIdx][1];
        const stdair::NbOfBookings_T lGrossBkgs = lNetBkgs + lCx;
      
        // If there is a lower class available, these gross bookings
        // will be considered product-oriented. Otherwise, they will be
        // considered price-oriented
        if (noLowerClassAvl == false) {
          lProductOrientedGrossBookingView[lIdx] = lGrossBkgs;
          lProductOrientedNetBookingView[lIdx] = lNetBkgs;
        } else {
          lPriceOrientedGrossBookingView[lIdx] = lGrossBkgs;
          lPriceOrientedNetBookingView[lIdx] = lNetBkgs;
        
          if (lBookingClass_ptr->getSegmentAvailability() >= 1.0) {
            noLowerClassAvl = false;
          }
        }
      }
    }
  }
  
}
