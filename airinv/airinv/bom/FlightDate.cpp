// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// STDAIR
#include <stdair/basic/BasConst_Inventory.hpp>
#include <stdair/bom/BomSource.hpp>
// AIRINV
#include <airinv/bom/FlightDate.hpp>
#include <airinv/bom/SegmentDate.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  FlightDate::FlightDate (const Key_T& iKey,
                          Structure_T& ioFlightDateStructure)
    : stdair::FlightDate (iKey, ioFlightDateStructure) {
  }
  
  // ////////////////////////////////////////////////////////////////////
  FlightDate::~FlightDate () {
  }

  // ////////////////////////////////////////////////////////////////////
  void FlightDate::updateAvailablityPool (const stdair::FlightDate& iFlightDate,
                                          const stdair::CabinCode_T& iCabinCode){
    const stdair::SegmentDateList_T& lSegmentDateList =
      iFlightDate.getSegmentDateList();
    for (stdair::SegmentDateList_T::iterator itSegmentDate =
           lSegmentDateList.begin(); itSegmentDate != lSegmentDateList.end();
         ++itSegmentDate) {
      const stdair::SegmentDate& lSegmentDate = *itSegmentDate;
      stdair::SegmentCabin* lSegmentCabin_ptr =
        lSegmentDate.getSegmentCabin (iCabinCode);
      assert (lSegmentCabin_ptr);

      // Update the availability pool of the segment-cabin to the minimal
      // availability pool of the member leg-cabins.
      const stdair::LegCabinList_T& lLegCabinList =
        lSegmentCabin_ptr->getLegCabinList();
      stdair::Availability_T lAvailabilityPool = stdair::MAXIMAL_AVAILABILITY;
      for (stdair::LegCabinList_T::iterator itLegCabin = lLegCabinList.begin();
           itLegCabin != lLegCabinList.end(); ++itLegCabin) {
        const stdair::LegCabin& lLegCabin = *itLegCabin;
        const stdair::Availability_T& lLegCabinAvailabilityPool =
          lLegCabin.getAvailabilityPool();
        if (lAvailabilityPool > lLegCabinAvailabilityPool) {
          lAvailabilityPool = lLegCabinAvailabilityPool;
        }
      }
      lSegmentCabin_ptr->setAvailabilityPool (lAvailabilityPool);
    }
  }

}
