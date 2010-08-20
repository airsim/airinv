// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// STDAIR
#include <stdair/basic/BasConst_Inventory.hpp>
#include <stdair/bom/BomManager.hpp>
#include <stdair/bom/FlightDate.hpp>
#include <stdair/bom/SegmentDate.hpp>
#include <stdair/bom/SegmentCabin.hpp>
#include <stdair/bom/LegCabin.hpp>
// AIRINV
#include <airinv/bom/FlightDateHelper.hpp>

namespace AIRINV {
  // ////////////////////////////////////////////////////////////////////
  void FlightDateHelper::
  updateAvailablityPool (const stdair::FlightDate& iFlightDate,
                         const stdair::CabinCode_T& iCabinCode){
    const stdair::SegmentDateList_T& lSegmentDateList =
      stdair::BomManager::getList<stdair::SegmentDate> (iFlightDate);
    for (stdair::SegmentDateList_T::const_iterator itSegmentDate =
           lSegmentDateList.begin(); itSegmentDate != lSegmentDateList.end();
         ++itSegmentDate) {
      const stdair::SegmentDate* lSegmentDate_ptr = *itSegmentDate;
      assert (lSegmentDate_ptr != NULL);
      stdair::SegmentCabin& lSegmentCabin =
        stdair::BomManager::getChild<stdair::SegmentCabin> (*lSegmentDate_ptr,
                                                            iCabinCode);

      // Update the availability pool of the segment-cabin to the minimal
      // availability pool of the member leg-cabins.
      const stdair::LegCabinList_T& lLegCabinList =
        stdair::BomManager::getList<stdair::LegCabin> (lSegmentCabin);
      stdair::Availability_T lAvailabilityPool = stdair::MAXIMAL_AVAILABILITY;
      for (stdair::LegCabinList_T::const_iterator itLegCabin =
             lLegCabinList.begin();
           itLegCabin != lLegCabinList.end(); ++itLegCabin) {
        const stdair::LegCabin* lLegCabin_ptr = *itLegCabin;
        assert (lLegCabin_ptr != NULL);
        const stdair::Availability_T& lLegCabinAvailabilityPool =
          lLegCabin_ptr->getAvailabilityPool();
        if (lAvailabilityPool > lLegCabinAvailabilityPool) {
          lAvailabilityPool = lLegCabinAvailabilityPool;
        }
      }
      lSegmentCabin.setAvailabilityPool (lAvailabilityPool);
    }
  }

}
