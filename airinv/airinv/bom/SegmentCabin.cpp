// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// STDAIR
#include <stdair/bom/BomSource.hpp>
// AIRINV
#include <airinv/bom/SegmentCabin.hpp>
#include <airinv/bom/FlightDate.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  SegmentCabin::SegmentCabin (const Key_T& iKey,
                              Structure_T& ioSegmentCabinStructure)
    : stdair::SegmentCabin (iKey, ioSegmentCabinStructure) {
  }
  
  // ////////////////////////////////////////////////////////////////////
  SegmentCabin::~SegmentCabin () {
  }

  // ////////////////////////////////////////////////////////////////////
  void SegmentCabin::
  updateFromReservation (stdair::SegmentCabin& ioSegmentCabin,
                        const stdair::PartySize_T& iNbOfBookings){
    // Update the commited space of the segment-cabin.
    ioSegmentCabin.updateFromReservation (iNbOfBookings);

    // Update the commited space of the member leg-cabins.
    const stdair::LegCabinList_T& lLegCabinList =
      ioSegmentCabin.getLegCabinList();
    for (stdair::LegCabinList_T::iterator itLegCabin = lLegCabinList.begin();
         itLegCabin != lLegCabinList.end(); ++itLegCabin) {
      stdair::LegCabin& lLegCabin = *itLegCabin;
      lLegCabin.updateFromReservation (iNbOfBookings);
    }

    // Update the availability pool of all the segment-cabin which belong to the
    // same flight-date.
    // const stdair::FlightDate& lFlightDate =
    //   ioSegmentCabin.getParent().getParent();
    // const stdair::CabinCode_T& lCabinCode = ioSegmentCabin.getCabinCode();
    // FlightDate::updateAvailablityPool (lFlightDate, lCabinCode);
  }

 }
