// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <iostream>
// STDAIR
#include <stdair/basic/BasConst_Period_BOM.hpp>
#include <stdair/bom/LegDate.hpp>
// AIRINV
#include <airinv/bom/LegStruct.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  LegStruct_T::LegStruct_T ()
    : _boardingDateOffSet (stdair::DEFAULT_DATE_OFFSET),
      _offDateOffSet (stdair::DEFAULT_DATE_OFFSET) {
  }
    
  // //////////////////////////////////////////////////////////////////////
  const std::string LegStruct_T::describe() const {
    std::ostringstream ostr;
    ostr << "    " << _boardingPoint << " / "
         << boost::posix_time::to_simple_string(_boardingTime);
    if (_boardingDateOffSet.days() != 0) {
      ostr << " [" << _boardingDateOffSet.days() << "]";
    }
    ostr << " -- " << _offPoint << " / "
         << boost::posix_time::to_simple_string(_offTime);
    if (_offDateOffSet.days() != 0) {
      ostr << " [" << _offDateOffSet.days() << "]";
    }
    ostr << " --> "
         << boost::posix_time::to_simple_string(_elapsed)
         << std::endl;
    for (LegCabinStructList_T::const_iterator itCabin = _cabinList.begin();
         itCabin != _cabinList.end(); itCabin++) {
      const LegCabinStruct_T& lCabin = *itCabin;
      ostr << lCabin.describe();
    }
    ostr << std::endl;
    
    return ostr.str();
  }

  // //////////////////////////////////////////////////////////////////////
  void LegStruct_T::fill (const stdair::Date_T& iRefDate,
                          stdair::LegDate& ioLegDate) const {
    // Set the Off Point
    ioLegDate.setOffPoint (_offPoint);

    // Set the Boarding Date
    const boost::gregorian::date_duration lBoardingDateOffSet (_boardingDateOffSet);
    ioLegDate.setBoardingDate (iRefDate+ lBoardingDateOffSet);

    // Set the Boarding Time
    ioLegDate.setBoardingTime (_boardingTime);
      
    // Set the Off Date
    const boost::gregorian::date_duration lOffDateOffSet (_offDateOffSet);
    ioLegDate.setOffDate (iRefDate + lOffDateOffSet);

    // Set the Off Time
    ioLegDate.setOffTime (_offTime);

    // Set the Elapsed Time
    ioLegDate.setElapsedTime (_elapsed);
  }

}
