// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// StdAir
#include <stdair/bom/BomManager.hpp>
#include <stdair/bom/BomRoot.hpp>
#include <stdair/bom/AirportPair.hpp>
#include <stdair/bom/PosChannel.hpp>
#include <stdair/bom/DatePeriod.hpp>
#include <stdair/bom/TimePeriod.hpp>
#include <stdair/bom/YieldFeatures.hpp>
#include <stdair/bom/AirlineClassList.hpp>
#include <stdair/factory/FacBomManager.hpp>
#include <stdair/service/Logger.hpp>
// AirRAC
#include <airrac/bom/YieldRuleStruct.hpp>
#include <airrac/command/YieldRuleGenerator.hpp>

namespace AIRRAC {

  // //////////////////////////////////////////////////////////////////////
  void YieldRuleGenerator::
  createYieldRule (stdair::BomRoot& ioBomRoot,
                  YieldRuleStruct& iYieldRuleStruct) {

    // Set the airport-pair primary key.
    const stdair::AirportCode_T& lBoardPoint =
      iYieldRuleStruct._origin;
    const stdair::AirportCode_T& lOffPoint = 
      iYieldRuleStruct._destination;
    const stdair::AirportPairKey lAirportPairKey (lBoardPoint, lOffPoint);
  
    // If the AirportPair object corresponding to the yield rule set
    // having the same origin and destination airport does not exist, create
    // it and link it to the ioBomRoot object.
    stdair::AirportPair* lAirportPair_ptr = stdair::BomManager::
      getObjectPtr<stdair::AirportPair> (ioBomRoot, lAirportPairKey.toString());
    if (lAirportPair_ptr == NULL) {
      lAirportPair_ptr = &stdair::FacBom<stdair::AirportPair>::
        instance().create (lAirportPairKey);
      stdair::FacBomManager::addToListAndMap (ioBomRoot, *lAirportPair_ptr);
      stdair::FacBomManager::linkWithParent (ioBomRoot, *lAirportPair_ptr);
    }
    assert (lAirportPair_ptr != NULL);

    // Set the point-of-sale-channel primary key.
    const stdair::CityCode_T& lPoS = 
      iYieldRuleStruct._pos;
    const stdair::ChannelLabel_T& lChannel = 
      iYieldRuleStruct._channel;
    const stdair::PosChannelKey lYieldPosChannelKey (lPoS, lChannel);  

    // If the YieldPosChannel object corresponding to the yield rule set
    // having the same city code and the same channel does not exist,
    // create it and link it to the AirportPair object.     
    stdair::PosChannel* lYieldPosChannel_ptr = stdair::BomManager::
      getObjectPtr<stdair::PosChannel> (*lAirportPair_ptr, 
                                        lYieldPosChannelKey.toString());
    if (lYieldPosChannel_ptr == NULL) {
      lYieldPosChannel_ptr = &stdair::FacBom<stdair::PosChannel>::
        instance().create (lYieldPosChannelKey);
      stdair::FacBomManager::
        addToListAndMap (*lAirportPair_ptr, *lYieldPosChannel_ptr);
      stdair::FacBomManager::
        linkWithParent (*lAirportPair_ptr, *lYieldPosChannel_ptr);
    }
    assert (lYieldPosChannel_ptr != NULL);   

    // Set the yield date-period primary key.
    const stdair::Date_T& lDateRangeStart = 
      iYieldRuleStruct._dateRangeStart;
    const stdair::Date_T& lDateRangeEnd = 
      iYieldRuleStruct._dateRangeEnd;
    const stdair::DatePeriod_T lDatePeriod (lDateRangeStart, lDateRangeEnd); 
    const stdair::DatePeriodKey lYieldDatePeriodKey (lDatePeriod);

    // If the YieldDatePeriod object corresponding to the yield rule set
    // having the same date-period does not exist, create it and link it
    // to the YieldPosChannel object.     
    stdair::DatePeriod* lYieldDatePeriod_ptr = stdair::BomManager::
      getObjectPtr<stdair::DatePeriod> (*lYieldPosChannel_ptr, 
                                        lYieldDatePeriodKey.toString());
    if (lYieldDatePeriod_ptr == NULL) {
      lYieldDatePeriod_ptr = &stdair::FacBom<stdair::DatePeriod>::
        instance().create (lYieldDatePeriodKey);
      stdair::FacBomManager::
        addToListAndMap (*lYieldPosChannel_ptr, *lYieldDatePeriod_ptr);
      stdair::FacBomManager::
        linkWithParent (*lYieldPosChannel_ptr, *lYieldDatePeriod_ptr);
    }
    assert (lYieldDatePeriod_ptr != NULL);
   
    // Set the yield time-period primary key.
    const stdair::Time_T& lTimeRangeStart
      = iYieldRuleStruct._timeRangeStart;
    const stdair::Time_T& lTimeRangeEnd
      = iYieldRuleStruct._timeRangeEnd;
    const stdair::TimePeriodKey lYieldTimePeriodKey (lTimeRangeStart,
                                                    lTimeRangeEnd);

    // If the YieldTimePeriod object corresponding to the yield rule set
    // having the same time-period does not exist, create it and link it
    // to the YieldDatePeriod object.     
    stdair::TimePeriod* lYieldTimePeriod_ptr = stdair::BomManager::
      getObjectPtr<stdair::TimePeriod> (*lYieldDatePeriod_ptr, 
                                        lYieldTimePeriodKey.toString());
    if (lYieldTimePeriod_ptr == NULL) {
      lYieldTimePeriod_ptr = &stdair::FacBom<stdair::TimePeriod>::
        instance().create (lYieldTimePeriodKey);
      stdair::FacBomManager::
        addToListAndMap (*lYieldDatePeriod_ptr, *lYieldTimePeriod_ptr);
      stdair::FacBomManager::
        linkWithParent (*lYieldDatePeriod_ptr, *lYieldTimePeriod_ptr);
    }
    assert (lYieldTimePeriod_ptr != NULL);

    // Set the yield features primary key.
    stdair::CabinCode_T lCabinCode = iYieldRuleStruct._cabinCode;
    const stdair::YieldFeaturesKey lYieldFeaturesKey (lCabinCode);

    // If the YieldFeatures object corresponding to the yield rule set
    // having the same cabin-code and yield does not exist, create it
    // and link it to the YieldTimePeriod object.     
    stdair::YieldFeatures* lYieldFeatures_ptr = stdair::BomManager::
      getObjectPtr<stdair::YieldFeatures > (*lYieldTimePeriod_ptr, 
                                            lYieldFeaturesKey.toString());
    if (lYieldFeatures_ptr == NULL) {
      lYieldFeatures_ptr = &stdair::FacBom<stdair::YieldFeatures>::
        instance().create (lYieldFeaturesKey);
      stdair::FacBomManager::
        addToListAndMap (*lYieldTimePeriod_ptr, *lYieldFeatures_ptr);
      stdair::FacBomManager::
        linkWithParent (*lYieldTimePeriod_ptr, *lYieldFeatures_ptr);
    }
    assert (lYieldFeatures_ptr != NULL);

    // Generate AirlineClassList and link them to their YieldRule
    const unsigned int lAirlineListSize =
      iYieldRuleStruct.getAirlineListSize();
    const unsigned int lClassCodeListSize =
      iYieldRuleStruct.getClassCodeListSize();
    assert (lAirlineListSize == lClassCodeListSize);
    const stdair::AirlineClassListKey lAirlineClassListKey (iYieldRuleStruct._airlineCodeList, iYieldRuleStruct._classCodeList);

    // Create the AirlineClassList object and link it to the
    // YieldFeatures object.
    stdair::AirlineClassList* lAirlineClassList_ptr = stdair::BomManager::
      getObjectPtr<stdair::AirlineClassList> (*lYieldFeatures_ptr,
                                              lAirlineClassListKey.toString());
    if (lAirlineClassList_ptr != NULL) {
      STDAIR_LOG_ERROR ("Dublicated airline class list / yield: "
                        << lAirlineClassListKey.toString());
      assert (false);
    }

    lAirlineClassList_ptr = &stdair::
      FacBom<stdair::AirlineClassList>::instance().create(lAirlineClassListKey);
    stdair::FacBomManager::
      addToListAndMap (*lYieldFeatures_ptr, *lAirlineClassList_ptr); 
    stdair::FacBomManager::
      linkWithParent(*lYieldFeatures_ptr, *lAirlineClassList_ptr); 
    lAirlineClassList_ptr->setYield (iYieldRuleStruct._yield);
  }
        
}
