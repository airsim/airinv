// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <exception>
#include <algorithm> // To use min
// Boost
#include <boost/make_shared.hpp>
// StdAir
#include <stdair/basic/BasConst_Inventory.hpp>
#include <stdair/basic/BasConst_BomDisplay.hpp>
#include <stdair/bom/BomManager.hpp>
#include <stdair/bom/BomKeyManager.hpp> 
#include <stdair/bom/BomRoot.hpp>
#include <stdair/bom/Inventory.hpp>
#include <stdair/bom/FlightDate.hpp>
#include <stdair/bom/SegmentDate.hpp>
#include <stdair/bom/SegmentCabin.hpp>
#include <stdair/bom/LegDate.hpp>
#include <stdair/bom/LegCabin.hpp>
#include <stdair/bom/FareFamily.hpp>
#include <stdair/bom/BookingClass.hpp>
#include <stdair/bom/SegmentSnapshotTable.hpp>
#include <stdair/bom/TravelSolutionStruct.hpp>
#include <stdair/bom/FareOptionStruct.hpp>
#include <stdair/bom/EventStruct.hpp>
#include <stdair/bom/SnapshotStruct.hpp>
#include <stdair/bom/RMEventStruct.hpp>
#include <stdair/bom/FareFamily.hpp> // Contains the definition of FareFamilyList_T
#include <stdair/bom/BookingClass.hpp> //
#include <stdair/bom/BomRetriever.hpp>
#include <stdair/factory/FacBomManager.hpp>
#include <stdair/factory/FacBom.hpp>
#include <stdair/service/Logger.hpp>// SEvMgr
#include <sevmgr/SEVMGR_Service.hpp>
// AirInv
#include <airinv/AIRINV_Types.hpp>
#include <airinv/bom/BomRootHelper.hpp>
#include <airinv/bom/InventoryHelper.hpp>
#include <airinv/bom/FlightDateHelper.hpp>
#include <airinv/bom/SegmentCabinHelper.hpp>
#include <airinv/command/InventoryManager.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  calculateAvailability (const stdair::BomRoot& iBomRoot,
                         stdair::TravelSolutionStruct& ioTravelSolution) {

    stdair::PartnershipTechnique::EN_PartnershipTechnique lENPartnershipTechnique =
      stdair::PartnershipTechnique::NONE;
    
    // Browse the list of segments and get the availability for the
    // children classes.
    const stdair::SegmentPath_T& lSegmentPath =
      ioTravelSolution.getSegmentPath();
    for (stdair::SegmentPath_T::const_iterator itSK = lSegmentPath.begin();
         itSK != lSegmentPath.end(); ++itSK) {
      const std::string& lSegmentKey = *itSK;
      const stdair::InventoryKey lInvKey =
        stdair::BomKeyManager::extractInventoryKey (lSegmentKey);
      stdair::Inventory& lInventory =
        stdair::BomManager::getObject<stdair::Inventory>(iBomRoot,
                                                         lInvKey.toString());

      lENPartnershipTechnique = lInventory.getPartnershipTechnique();
      
      switch (lENPartnershipTechnique) {

      case stdair::PartnershipTechnique::NONE:{
        InventoryHelper::calculateAvailability (lInventory, lSegmentKey,
                                                ioTravelSolution);
        break;
      }
      default:{
        InventoryHelper::getYieldAndBidPrice (lInventory, lSegmentKey,
                                              ioTravelSolution);
        break;
      }
      }
      
    }

    switch (lENPartnershipTechnique) {
    case stdair::PartnershipTechnique::NONE:{
      // Compute the availabitliy for each fare option using the AU's.
      calculateAvailabilityByAU (ioTravelSolution);
      break;
    }
    case stdair::PartnershipTechnique::RAE_DA:
    case stdair::PartnershipTechnique::RAE_YP:{ 
      // 1. Compute the availability for each fare option using RAE
      calculateAvailabilityByRAE (ioTravelSolution);
      break;
    }
    case stdair::PartnershipTechnique::IBP_DA:
    case stdair::PartnershipTechnique::IBP_YP:{
      // 2. Compute the availability for each fare option using protective IBP
      calculateAvailabilityByProtectiveIBP (ioTravelSolution);
      break;
    }
    case stdair::PartnershipTechnique::IBP_YP_U:
    case stdair::PartnershipTechnique::RMC:
    case stdair::PartnershipTechnique::A_RMC:{
      // 3. Compute the availability for each fare option using IBP
      calculateAvailabilityByIBP (ioTravelSolution);
      break;
    }
    default: {
      assert (false);
      break;
    }
    }

  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  calculateAvailabilityByAU (stdair::TravelSolutionStruct& ioTravelSolution) {

    // MODIF: segment path string for availability display
    std::ostringstream oStr;
    const stdair::SegmentPath_T& lSP = ioTravelSolution.getSegmentPath();
    for (stdair::SegmentPath_T::const_iterator itSP = lSP.begin();
	 itSP != lSP.end(); itSP++) {
      oStr << *itSP << ";";
    }

    // Browse the fare options
    stdair::FareOptionList_T& lFOList = ioTravelSolution.getFareOptionListRef();
    for (stdair::FareOptionList_T::iterator itFO = lFOList.begin();
         itFO != lFOList.end(); ++itFO) {

      stdair::FareOptionStruct& lFO = *itFO;
      
      // Check the availability
      const stdair::ClassList_StringList_T& lClassPath = lFO.getClassPath();
      
      const stdair::ClassAvailabilityMapHolder_T& lClassAvailabilityMapHolder =
        ioTravelSolution.getClassAvailabilityMapHolder();
      
      // Initialise the flag stating whether the availability is enough
      stdair::Availability_T lAvl =
        std::numeric_limits<stdair::Availability_T>::max();
      
      // Sanity check: the travel solution must contain two lists,
      // one for the booking class availabilities, the other for the
      // fare options.
      assert (lClassAvailabilityMapHolder.empty() == false
              && lClassPath.empty() == false);
      
      // List of booking class availability maps (one map per segment)
      stdair::ClassAvailabilityMapHolder_T::const_iterator itCAMH =
        lClassAvailabilityMapHolder.begin();
      
      // List of fare options
      stdair::ClassList_StringList_T::const_iterator itClassList =
        lClassPath.begin();
      
      // Browse both lists at the same time, i.e., one element per segment
      for (; itCAMH != lClassAvailabilityMapHolder.end()
             && itClassList != lClassPath.end(); ++itCAMH, ++itClassList) {
        
        // Retrieve the booking class list for the current segment
        const stdair::ClassList_String_T& lCurrentClassList = *itClassList;
        assert (lCurrentClassList.size() > 0);
        
        // TODO: instead of just extracting the first booking class,
        //       perform a choice on the full list of classes.
        // Extract one booking class key (class code)
        stdair::ClassCode_T lFirstClass;
        lFirstClass.append (lCurrentClassList, 0, 1);
        
        // Retrieve the booking class map for the current segment
        const stdair::ClassAvailabilityStruct& lClassAvlStruct = *itCAMH;
        const stdair::ClassAvailabilityMap_T& lClassAvlMap =
          lClassAvlStruct.getClassAvailabilityMap();
        
        // Retrieve the availability of the chosen booking class
        const stdair::ClassAvailabilityMap_T::const_iterator itClassAvl =
          lClassAvlMap.find (lFirstClass);
        
        if (itClassAvl == lClassAvlMap.end()) {
          // DEBUG
          STDAIR_LOG_DEBUG ("No availability has been set up for the class '"
                            << lFirstClass << "'. Travel solution: "
                            << ioTravelSolution.display());
        }
        assert (itClassAvl != lClassAvlMap.end());
        
        const stdair::Availability_T& lCurrentAvl = itClassAvl->second;
        if (lAvl > lCurrentAvl) {
          lAvl = lCurrentAvl;
        }
      }
      
      lFO.setAvailability (lAvl);

      //MODIF: availability display
      STDAIR_LOG_DEBUG ("Fare option " << lFO.describe() << ", "
                        << "Availability " << lFO.getAvailability() << ", "
                        << "Segment Path " << oStr.str());
    }
  }

  // \todo: the following code must be either re-written or removed.
  //        There is indeed a lot of code duplication.
  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  calculateAvailabilityByRAE (stdair::TravelSolutionStruct& ioTravelSolution) {
    
    std::ostringstream oStr;
    const stdair::SegmentPath_T& lSP = ioTravelSolution.getSegmentPath();
    for (stdair::SegmentPath_T::const_iterator itSP = lSP.begin();
	 itSP != lSP.end(); itSP++) {
      oStr << *itSP << ";";
    }

    //Retrieve bid price vector and yield maps
    const stdair::ClassYieldMapHolder_T& lClassYieldMapHolder =
      ioTravelSolution.getClassYieldMapHolder();
    const stdair::ClassBpvMapHolder_T& lClassBpvMapHolder =
      ioTravelSolution.getClassBpvMapHolder();

    //Retrieve the list of fare options and browse it
    stdair::FareOptionList_T& lFOList = ioTravelSolution.getFareOptionListRef();
    for (stdair::FareOptionList_T::iterator itFO = lFOList.begin();
	 itFO != lFOList.end(); ++itFO) {

      stdair::FareOptionStruct& lFO = *itFO;
  
      stdair::ClassYieldMapHolder_T::const_iterator itCYM =
        lClassYieldMapHolder.begin();
      stdair::ClassBpvMapHolder_T::const_iterator itCBPM =
        lClassBpvMapHolder.begin();

      const stdair::ClassList_StringList_T& lClassPath = lFO.getClassPath();

      
      // Sanity checks
      assert (lClassPath.size() == lClassYieldMapHolder.size());
      assert (lClassPath.size() == lClassBpvMapHolder.size());

      // Browse class path, class-yield maps, class-(bid price vector) maps.
      // Each iteration corresponds to one segment.

      std::ostringstream oCPStr;
      for (stdair::ClassList_StringList_T::const_iterator itCL =
             lClassPath.begin();
           itCL != lClassPath.end(); ++itCL, ++itCYM, ++itCBPM) {

	// Class path determination
	if (itCL == lClassPath.begin()) {
	  oCPStr << *itCL;
          
	} else {
	  oCPStr << "-" << *itCL;
	}

	const stdair::ClassList_String_T& lCL = *itCL;
	stdair::ClassCode_T lCC;
	lCC.append (lCL, 0, 1);

	const stdair::ClassYieldMap_T& lCYM = *itCYM;
	stdair::ClassYieldMap_T::const_iterator itCCCYM = lCYM.find (lCC);
	assert (itCCCYM != lCYM.end());

	const stdair::ClassBpvMap_T& lCBPM = *itCBPM;
	stdair::ClassBpvMap_T::const_iterator itCCCBPM = lCBPM.find (lCC);
	assert (itCCCBPM != lCBPM.end());

        const stdair::BidPriceVector_T* lBidPriceVector_ptr = itCCCBPM->second;
        assert (lBidPriceVector_ptr != NULL);
        	
	// Initialization of fare option availability
	if (itCL == lClassPath.begin()) {
	  lFO.setAvailability (lBidPriceVector_ptr->size());
	}

	// Availability update
	if (lFO.getAvailability() > 0) {
	  
	  //Segment availability calculation
	  stdair::BidPriceVector_T lReverseBPV (lBidPriceVector_ptr->size());
	  std::reverse_copy (lBidPriceVector_ptr->begin(),
                             lBidPriceVector_ptr->end(),
                             lReverseBPV.begin());

          const stdair::YieldValue_T& lYield = itCCCYM->second;
	  stdair::BidPriceVector_T::const_iterator lBidPrice =
            std::upper_bound (lReverseBPV.begin(), lReverseBPV.end(), lYield);

	  const stdair::Availability_T lAvl = lBidPrice - lReverseBPV.begin();

          // Availability update
	  lFO.setAvailability (std::min (lFO.getAvailability(), lAvl));
	}
      }
               
      // DEBUG
      STDAIR_LOG_DEBUG ("Fare option: " << lFO.describe() << ", "
                        << "Availability: " << lFO.getAvailability() << ", "
                        << "Segment Path: " << oStr.str() << ", ");
    }
  }
  
  // \todo: the following code must be either re-written or removed.
  //        There is indeed a lot of code duplication.
  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  calculateAvailabilityByIBP (stdair::TravelSolutionStruct& ioTravelSolution) {
    std::ostringstream oStr;

    // Yield valuation coefficient for multi-segment travel solutions   
    double alpha = 1.0;

    const stdair::SegmentPath_T& lSP = ioTravelSolution.getSegmentPath();
    for (stdair::SegmentPath_T::const_iterator itSP = lSP.begin();
	 itSP != lSP.end(); itSP++) {
      oStr << *itSP << ";";
    }

    //Retrieve bid price vector and yield maps
    const stdair::ClassYieldMapHolder_T& lClassYieldMapHolder =
      ioTravelSolution.getClassYieldMapHolder();
    const stdair::ClassBpvMapHolder_T& lClassBpvMapHolder =
      ioTravelSolution.getClassBpvMapHolder();

    // Retrieve the list of fare options and browse it
    stdair::FareOptionList_T& lFOList = ioTravelSolution.getFareOptionListRef();
    for (stdair::FareOptionList_T::iterator itFO = lFOList.begin();
	 itFO != lFOList.end(); ++itFO) {

      stdair::FareOptionStruct& lFO = *itFO;
  
      stdair::ClassYieldMapHolder_T::const_iterator itCYM =
        lClassYieldMapHolder.begin();
      stdair::ClassBpvMapHolder_T::const_iterator itCBPM =
        lClassBpvMapHolder.begin();

      const stdair::ClassList_StringList_T& lClassPath = lFO.getClassPath();

      // Sanity checks
      assert (lClassPath.size() == lClassYieldMapHolder.size());
      assert (lClassPath.size() == lClassBpvMapHolder.size());

      // Yield is taken to be equal to fare (connecting flights)

      // \todo: take yield instead
      stdair::YieldValue_T lTotalYield = lFO.getFare();
      // Bid price initialisation
      stdair::BidPrice_T lTotalBidPrice = 0;

      // Browse class path, class-yield maps, class-(bid price vector) maps.
      // Each iteration corresponds to one segment.

      std::ostringstream oCPStr;
      for (stdair::ClassList_StringList_T::const_iterator itCL =
             lClassPath.begin();
           itCL != lClassPath.end(); ++itCL, ++itCYM, ++itCBPM) {

	// Class path determination
	if (itCL == lClassPath.begin()) {
	  oCPStr << *itCL;

	} else {
	  oCPStr << "-" << *itCL;
	}

	const stdair::ClassList_String_T& lCL = *itCL;
	stdair::ClassCode_T lCC;
	lCC.append (lCL, 0, 1);

	const stdair::ClassYieldMap_T& lCYM = *itCYM;
	stdair::ClassYieldMap_T::const_iterator itCCCYM = lCYM.find (lCC);
	assert (itCCCYM != lCYM.end());

	const stdair::ClassBpvMap_T& lCBPM = *itCBPM;
	stdair::ClassBpvMap_T::const_iterator itCCCBPM = lCBPM.find (lCC);
	assert (itCCCBPM != lCBPM.end());

        const stdair::BidPriceVector_T* lBidPriceVector_ptr = itCCCBPM->second;
        assert (lBidPriceVector_ptr != NULL);
        	
	//Initialization of fare option availability
	if (itCL == lClassPath.begin()) {
	  lFO.setAvailability (lBidPriceVector_ptr->size());
	}

	// Availability update
	if (lFO.getAvailability() > 0) {
	  //Segment availability calculation
	  stdair::BidPriceVector_T lReverseBPV (lBidPriceVector_ptr->size());
	  std::reverse_copy (lBidPriceVector_ptr->begin(),
                             lBidPriceVector_ptr->end(), lReverseBPV.begin());

          const stdair::YieldValue_T& lYield = itCCCYM->second;
	  stdair::BidPriceVector_T::const_iterator lBidPrice =
            std::upper_bound (lReverseBPV.begin(), lReverseBPV.end(), lYield);

	  const stdair::Availability_T lAvl = lBidPrice - lReverseBPV.begin();

          // Availability update
	  lFO.setAvailability (std::min(lFO.getAvailability(), lAvl));
     	}

	// Total bid price calculation
	if (lBidPriceVector_ptr->size() > 0) {
	  lTotalBidPrice += lBidPriceVector_ptr->back();          

	} else {
	  lTotalBidPrice = std::numeric_limits<stdair::BidPrice_T>::max();
	}

	// Total yield calculation (has been replaced by total fare).
	//lTotalYield += lYield;
      }
      // Multi-segment bid price control

      if (lClassPath.size() > 1) {
      	if (lFO.getAvailability() > 0) {
      	  const stdair::Availability_T lAvl =
            alpha * lTotalYield >= lTotalBidPrice;
      	  lFO.setAvailability (lAvl * lFO.getAvailability());

      	} else {
      	  const stdair::Availability_T lAvl =
            alpha * lTotalYield >= lTotalBidPrice;
      	  lFO.setAvailability (lAvl);
      	}
      
        // DEBUG
      	STDAIR_LOG_DEBUG ("Class: " << oCPStr.str()
      			  << ", " << "Yield: " << alpha*lTotalYield << ", "
      			  << "Bid price: " << lTotalBidPrice << ", "
      			  << "Remaining capacity: " << "Undefined" << " "
      			  << "Segment date: " << oStr.str());
      }
         
      // DEBUG
      STDAIR_LOG_DEBUG ("Fare option " << lFO.describe() << ", "
                        << "Availability " << lFO.getAvailability() << ", "
                        << "Segment Path " << oStr.str() << ", ");
    }
  }

  // \todo: the following code must be either re-written or removed.
  //        There is indeed a lot of code duplication.
  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  calculateAvailabilityByProtectiveIBP (stdair::TravelSolutionStruct& ioTravelSolution) {
    std::ostringstream oStr;

    // Yield valuation coefficient for multi-segment travel solutions   
    double alpha = 1.0;

    const stdair::SegmentPath_T& lSP = ioTravelSolution.getSegmentPath();
    for (stdair::SegmentPath_T::const_iterator itSP = lSP.begin();
	 itSP != lSP.end(); itSP++) {
      oStr << *itSP << ";";
    }

    //Retrieve bid price vector and yield maps
    const stdair::ClassYieldMapHolder_T& lClassYieldMapHolder =
      ioTravelSolution.getClassYieldMapHolder();
    const stdair::ClassBpvMapHolder_T& lClassBpvMapHolder =
      ioTravelSolution.getClassBpvMapHolder();

    //Retrieve the list of fare options and browse it
    stdair::FareOptionList_T& lFOList = ioTravelSolution.getFareOptionListRef();
    for (stdair::FareOptionList_T::iterator itFO = lFOList.begin();
	 itFO != lFOList.end(); ++itFO) {

      stdair::FareOptionStruct& lFO = *itFO;
  
      stdair::ClassYieldMapHolder_T::const_iterator itCYM =
        lClassYieldMapHolder.begin();
      stdair::ClassBpvMapHolder_T::const_iterator itCBPM =
        lClassBpvMapHolder.begin();

      const stdair::ClassList_StringList_T& lClassPath = lFO.getClassPath();

      // Sanity checks
      assert (lClassPath.size() == lClassYieldMapHolder.size());
      assert (lClassPath.size() == lClassBpvMapHolder.size());

      // Yield is taken to be equal to fare (connecting flights)
      // TODO : take yield instead
      stdair::YieldValue_T lTotalYield = lFO.getFare();
      // Bid price initialisation
      stdair::BidPrice_T lTotalBidPrice = 0;
      // Maximal bid price initialisation
      stdair::BidPrice_T lMaxBidPrice = 0;

      //Browse class path, class-yield maps, class-(bid price vector) maps.
      //Each iteration corresponds to one segment.

      std::ostringstream oCPStr;
      for (stdair::ClassList_StringList_T::const_iterator itCL =
             lClassPath.begin();
           itCL != lClassPath.end(); ++itCL, ++itCYM, ++itCBPM) {

	// Class path determination
	if (itCL == lClassPath.begin()) {
	  oCPStr << *itCL;

	} else {
	  oCPStr << "-" << *itCL;
	}

	const stdair::ClassList_String_T& lCL = *itCL;
	stdair::ClassCode_T lCC;
	lCC.append (lCL, 0, 1);

	const stdair::ClassYieldMap_T& lCYM = *itCYM;
	stdair::ClassYieldMap_T::const_iterator itCCCYM = lCYM.find (lCC);
	assert (itCCCYM != lCYM.end());

        const stdair::YieldValue_T& lYield = itCCCYM->second;
	const stdair::ClassBpvMap_T& lCBPM = *itCBPM;
	stdair::ClassBpvMap_T::const_iterator itCCCBPM = lCBPM.find (lCC);
	assert (itCCCBPM != lCBPM.end());

        const stdair::BidPriceVector_T* lBidPriceVector_ptr = itCCCBPM->second;
        assert (lBidPriceVector_ptr != NULL);
        	
	// Initialization of fare option availability
	if (itCL == lClassPath.begin()) {
	  lFO.setAvailability (lBidPriceVector_ptr->size());
	}

	// Availability update
	if (lFO.getAvailability() > 0) {
	  
	  //Segment availability calculation
	  stdair::BidPriceVector_T lReverseBPV (lBidPriceVector_ptr->size());
	  std::reverse_copy (lBidPriceVector_ptr->begin(),
                             lBidPriceVector_ptr->end(), lReverseBPV.begin());

          stdair::BidPriceVector_T::const_iterator lBidPrice =
            std::upper_bound (lReverseBPV.begin(), lReverseBPV.end(), lYield);

	  const stdair::Availability_T lAvl = lBidPrice - lReverseBPV.begin();
          
          // Availability update
	  lFO.setAvailability (std::min(lFO.getAvailability(), lAvl));
     
	}

	// Total bid price calculation
	if (lBidPriceVector_ptr->size() > 0) {
	  lTotalBidPrice += lBidPriceVector_ptr->back();

          if (lMaxBidPrice < lBidPriceVector_ptr->back()) {
            lMaxBidPrice = lBidPriceVector_ptr->back();
          }
          
	} else {
	  lTotalBidPrice = std::numeric_limits<stdair::BidPrice_T>::max();
	}

	// Total yield calculation (has been replaced by total fare).
	//lTotalYield += lYield;
      }
      // Multi-segment bid price control

      // Protective IBP (maximin): guarantees the minimal yield for each airline
      // Proration factors are all equal to 1/{number of partners}.

      lTotalBidPrice = std::max (lMaxBidPrice * lClassPath.size(),
                                 lTotalBidPrice);

      if (lClassPath.size() > 1) {
      	if (lFO.getAvailability() > 0) {
      	  const stdair::Availability_T lAvl =
            alpha * lTotalYield >= lTotalBidPrice;
      	  lFO.setAvailability (lAvl * lFO.getAvailability());

      	} else {
      	  const stdair::Availability_T lAvl =
            alpha * lTotalYield >= lTotalBidPrice;
      	  lFO.setAvailability (lAvl);
      	}
      
        // DEBUG
      	STDAIR_LOG_DEBUG ("Class: " << oCPStr.str()
      			  << ", " << "Yield: " << alpha*lTotalYield << ", "
      			  << "Bid price: " << lTotalBidPrice << ", "
      			  << "Remaining capacity: " << "Undefined" << " "
      			  << "Segment date: " << oStr.str());
      }

      // DEBUG         
      STDAIR_LOG_DEBUG ("Fare option " << lFO.describe() << ", "
                        << "Availability " << lFO.getAvailability() << ", "
                        << "Segment Path " << oStr.str() << ", ");
    }
  }
  
  //MODIF
  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::setDefaultBidPriceVector (stdair::BomRoot& ioBomRoot) {

    const stdair::InventoryList_T& lInvList =
      stdair::BomManager::getList<stdair::Inventory> (ioBomRoot);
    for (stdair::InventoryList_T::const_iterator itInv = lInvList.begin();
         itInv != lInvList.end(); ++itInv) {
      stdair::Inventory* lCurrentInv_ptr = *itInv;
      assert (lCurrentInv_ptr != NULL);

      // Set the default bid price for own cabins.
      setDefaultBidPriceVector (*lCurrentInv_ptr);

      // Check if the inventory contains images of partner inventories.
      // If so, set the default bid price for their cabins.
      if (stdair::BomManager::hasList<stdair::Inventory> (*lCurrentInv_ptr)) {
        const stdair::InventoryList_T& lPartnerInvList =
          stdair::BomManager::getList<stdair::Inventory> (*lCurrentInv_ptr);

        for (stdair::InventoryList_T::const_iterator itPartnerInv =
               lPartnerInvList.begin();
             itPartnerInv != lPartnerInvList.end(); ++itPartnerInv) {
          stdair::Inventory* lCurrentPartnerInv_ptr = *itPartnerInv;
          assert (lCurrentPartnerInv_ptr != NULL);

          setDefaultBidPriceVector (*lCurrentPartnerInv_ptr);
        }
      }            
    }     
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  setDefaultBidPriceVector (stdair::Inventory& ioInventory) {

    const stdair::FlightDateList_T& lFlightDateList =
      stdair::BomManager::getList<stdair::FlightDate> (ioInventory);
    for (stdair::FlightDateList_T::const_iterator itFlightDate =
           lFlightDateList.begin();
         itFlightDate != lFlightDateList.end(); ++itFlightDate) {
      stdair::FlightDate* lCurrentFlightDate_ptr = *itFlightDate;
      assert (lCurrentFlightDate_ptr != NULL);

      // Check if the flight date holds a list of leg dates.
      // If so retrieve it and initialise the bid price vectors of their cabins.
      if (stdair::BomManager::hasList<stdair::LegDate> (*lCurrentFlightDate_ptr)) {
        const stdair::LegDateList_T& lLegDateList =
          stdair::BomManager::getList<stdair::LegDate> (*lCurrentFlightDate_ptr);
        for (stdair::LegDateList_T::const_iterator itLegDate =
               lLegDateList.begin();
             itLegDate != lLegDateList.end(); ++itLegDate) {
          stdair::LegDate* lCurrentLegDate_ptr = *itLegDate;
          assert (lCurrentLegDate_ptr != NULL);
          
          const stdair::LegCabinList_T& lLegCabinList =
            stdair::BomManager::getList<stdair::LegCabin> (*lCurrentLegDate_ptr);
          for (stdair::LegCabinList_T::const_iterator itLegCabin =
                 lLegCabinList.begin();
               itLegCabin != lLegCabinList.end(); ++itLegCabin) {
            stdair::LegCabin* lCurrentLegCabin_ptr = *itLegCabin;
            assert (lCurrentLegCabin_ptr != NULL);

            const stdair::CabinCapacity_T& lCabinCapacity =
              lCurrentLegCabin_ptr->getPhysicalCapacity();
            lCurrentLegCabin_ptr->emptyBidPriceVector();

            stdair::BidPriceVector_T& lBPV =
              lCurrentLegCabin_ptr->getBidPriceVector();

            //for (stdair::CabinCapacity_T k = 0;k!=lCabinCapacity;k++) {lBPV.push_back(400 + 300/sqrt(k+1));}
            for (stdair::CabinCapacity_T k = 0; k != lCabinCapacity; k++) {
              lBPV.push_back (400);
            }

            lCurrentLegCabin_ptr->setPreviousBidPrice (lBPV.back());
            lCurrentLegCabin_ptr->setCurrentBidPrice (lBPV.back());          
          }
        }
      }
    }
  }   
  
  // ////////////////////////////////////////////////////////////////////
  bool InventoryManager::sell (stdair::Inventory& ioInventory,
                               const std::string& iSegmentDateKey,
                               const stdair::ClassCode_T& iClassCode,
                               const stdair::PartySize_T& iPartySize) {

    // Make the sale within the inventory.
    return InventoryHelper::sell (ioInventory, iSegmentDateKey,
                                  iClassCode, iPartySize);
  }
  
  // ////////////////////////////////////////////////////////////////////
  bool InventoryManager::sell (const stdair::BookingClassID_T& iClassID,
                               const stdair::PartySize_T& iPartySize) {

    // Make the sale within the inventory.
    return InventoryHelper::sell (iClassID, iPartySize);
  }

  // ////////////////////////////////////////////////////////////////////
  bool InventoryManager::cancel (stdair::Inventory& ioInventory,
                                 const std::string& iSegmentDateKey,
                                 const stdair::ClassCode_T& iClassCode,
                                 const stdair::PartySize_T& iPartySize) {
    
    // Make the cancellation within the inventory.
    return InventoryHelper::cancel (ioInventory, iSegmentDateKey,
                                    iClassCode, iPartySize);
  }
  
  // ////////////////////////////////////////////////////////////////////
  bool InventoryManager::cancel (const stdair::BookingClassID_T& iClassID,
                                 const stdair::PartySize_T& iPartySize) {

    // Make the cancellation within the inventory.
    return InventoryHelper::cancel (iClassID, iPartySize);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  updateBookingControls (stdair::FlightDate& ioFlightDate) {

    // Forward the call to FlightDateHelper.
    FlightDateHelper::updateBookingControls (ioFlightDate);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  recalculateAvailability (stdair::FlightDate& ioFlightDate) {

    // Forward the call to FlightDateHelper.
    FlightDateHelper::recalculateAvailability (ioFlightDate);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::takeSnapshots(const stdair::Inventory& iInventory,
                                       const stdair::DateTime_T& iSnapshotTime){

    // Make the snapshots within the inventory
    InventoryHelper::takeSnapshots (iInventory, iSnapshotTime);
  } 
    
  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  createDirectAccesses (const stdair::BomRoot& iBomRoot) {

    // Browse the list of inventories and create direct accesses
    // within each inventory.
    const stdair::InventoryList_T& lInvList =
      stdair::BomManager::getList<stdair::Inventory> (iBomRoot);
    for (stdair::InventoryList_T::const_iterator itInv = lInvList.begin();
         itInv != lInvList.end(); ++itInv) {
      stdair::Inventory* lCurrentInv_ptr = *itInv;
      assert (lCurrentInv_ptr != NULL);

      createDirectAccesses (iBomRoot, *lCurrentInv_ptr);
    }

    // Browse the list of inventories and create partner accesses
    // within each inventory.
    for (stdair::InventoryList_T::const_iterator itInv = lInvList.begin();
         itInv != lInvList.end(); ++itInv) {
      stdair::Inventory* lCurrentInv_ptr = *itInv;
      assert (lCurrentInv_ptr != NULL);
      
      createPartnerAccesses (iBomRoot, *lCurrentInv_ptr);
    }

    // Fill some attributes of segment-date with the routing legs.
    BomRootHelper::fillFromRouting (iBomRoot);	
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  createDirectAccesses (const stdair::BomRoot& iBomRoot,
                        stdair::Inventory& ioInventory) {

    // Browse the list of flight-dates and create direct accesses
    // within each flight-date.
    const stdair::FlightDateList_T& lFlightDateList = 
      stdair::BomManager::getList<stdair::FlightDate> (ioInventory);
    for (stdair::FlightDateList_T::const_iterator itFlightDate = 
           lFlightDateList.begin();
         itFlightDate != lFlightDateList.end(); ++itFlightDate) {
      stdair::FlightDate* lCurrentFlightDate_ptr = *itFlightDate;
      assert (lCurrentFlightDate_ptr != NULL);

      createDirectAccesses (iBomRoot, ioInventory, *lCurrentFlightDate_ptr);
    }
    
    // Browse the list of inventories and create direct accesses
    // within each inventory.
    const bool hasInventoryList =
      stdair::BomManager::hasList<stdair::Inventory> (ioInventory);
    if (hasInventoryList == true) {
      const stdair::InventoryList_T& lInvList =
        stdair::BomManager::getList<stdair::Inventory> (ioInventory);
      for (stdair::InventoryList_T::const_iterator itInv = lInvList.begin();
           itInv != lInvList.end(); ++itInv) {
        stdair::Inventory* lCurrentInv_ptr = *itInv;
        assert (lCurrentInv_ptr != NULL);

        createDirectAccesses (iBomRoot, *lCurrentInv_ptr);
      }
    }
  } 

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  createDirectAccesses (const stdair::BomRoot& ioBomRoot,
                         stdair::Inventory& ioInventory,
                         stdair::FlightDate& ioFlightDate) {

    bool areSegmentAndRoutingLegLinked = false;

    // Browse the list of segment-dates and create direct accesses
    // within each segment-date.
    const stdair::SegmentDateList_T& lSegmentDateList = 
      stdair::BomManager::getList<stdair::SegmentDate> (ioFlightDate);
    for (stdair::SegmentDateList_T::const_iterator itSegmentDate = 
           lSegmentDateList.begin();
         itSegmentDate != lSegmentDateList.end(); ++itSegmentDate) {

      stdair::SegmentDate* lCurrentSegmentDate_ptr = *itSegmentDate;
      assert (lCurrentSegmentDate_ptr != NULL);	

      // Get the routing leg keys list
      const stdair::RoutingLegKeyList_T& lRoutingLegKeyList = 
	lCurrentSegmentDate_ptr->getLegKeyList ();

      // Browse the list of routing leg keys and try to create direct accesses
      // with each corresponding leg date.
      for (stdair::RoutingLegKeyList_T::const_iterator itRoutingLegKey = 
           lRoutingLegKeyList.begin();
         itRoutingLegKey != lRoutingLegKeyList.end(); ++itRoutingLegKey) {

	// Try to retrieve the routing LegDate within the flight date
	stdair::LegDate* lLegDate_ptr = stdair::BomRetriever::
	  retrieveOperatingLegDateFromLongKey (ioFlightDate, *itRoutingLegKey);
        
	if (lLegDate_ptr != NULL) {

          // Link the SegmentDate and LegDate together
          stdair::FacBomManager::addToListAndMap (*lCurrentSegmentDate_ptr,
                                                  *lLegDate_ptr);
          stdair::FacBomManager::addToListAndMap (*lLegDate_ptr,
                                                  *lCurrentSegmentDate_ptr);
          areSegmentAndRoutingLegLinked = true;
        }
      }
      if (areSegmentAndRoutingLegLinked == true) {
        createDirectAccesses (*lCurrentSegmentDate_ptr);
      }
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  createDirectAccesses (stdair::SegmentDate& ioSegmentDate) {

    // Browse the list of segment-cabins and create direct accesses
    // within each segment-cabin.
    const stdair::SegmentCabinList_T& lSegmentCabinList = 
      stdair::BomManager::getList<stdair::SegmentCabin> (ioSegmentDate);
    for (stdair::SegmentCabinList_T::const_iterator itSegmentCabin = 
           lSegmentCabinList.begin();
         itSegmentCabin != lSegmentCabinList.end(); ++itSegmentCabin) {

      //
      stdair::SegmentCabin* lCurrentSegmentCabin_ptr = *itSegmentCabin;
      assert (lCurrentSegmentCabin_ptr != NULL);

      //
      const stdair::CabinCode_T& lCabinCode =
          lCurrentSegmentCabin_ptr->getCabinCode();
      
      // Iterate on the routing legs
      const stdair::LegDateList_T& lLegDateList =
        stdair::BomManager::getList<stdair::LegDate> (ioSegmentDate);
      for (stdair::LegDateList_T::const_iterator itLegDate =
             lLegDateList.begin();
           itLegDate != lLegDateList.end(); ++itLegDate) {

        const stdair::LegDate* lCurrentLegDate_ptr = *itLegDate;        
        assert (lCurrentLegDate_ptr != NULL);

        // Retrieve the LegCabin getting the same class of service
        // (cabin code) as the SegmentCabin.
        stdair::LegCabin& lLegCabin = stdair::BomManager::
          getObject<stdair::LegCabin> (*lCurrentLegDate_ptr, lCabinCode);

        /**
         * Add the leg-cabin to the segment-cabin routing.
         *
         * As several leg-cabins may compose the segment-cabin routing,
         * and as the leg-cabin key is only made by a cabin code (which
         * is the same as for the segment-cabin), all the leg-cabins
         * composing the routing would have the same key.
         * Hence, the leg-cabins must be differentiated according to their
         * boarding point as well.
         */
        stdair::FacBomManager::addToListAndMap (*lCurrentSegmentCabin_ptr,
                                                lLegCabin,
                                                lLegCabin.getFullerKey());

        /**
         * Add the segment-cabin to the list which the leg-cabin crosses.
         *
         * As several segment-cabins may cross the leg-cabin,
         * and as the segment-cabin key is only made by a cabin code (which
         * is the same as for the leg-cabin), all the segment-cabins
         * crossing the leg-cabin would have the same key.
         * Hence, the segment-cabins must be differentiated according to their
         * boarding and off points as well.
         */
        stdair::FacBomManager::
          addToListAndMap (lLegCabin, *lCurrentSegmentCabin_ptr,
                           lCurrentSegmentCabin_ptr->getFullerKey());
      }      
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  createPartnerAccesses (const stdair::BomRoot& iBomRoot,
                        stdair::Inventory& ioInventory) {

    // Browse the list of flight-dates and create partner accesses
    // within each flight-date.
    const stdair::FlightDateList_T& lFlightDateList = 
      stdair::BomManager::getList<stdair::FlightDate> (ioInventory);
    for (stdair::FlightDateList_T::const_iterator itFlightDate = 
           lFlightDateList.begin();
         itFlightDate != lFlightDateList.end(); ++itFlightDate) {
      stdair::FlightDate* lCurrentFlightDate_ptr = *itFlightDate;
      assert (lCurrentFlightDate_ptr != NULL);

      createPartnerAccesses (iBomRoot, ioInventory, *lCurrentFlightDate_ptr);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  createPartnerAccesses (const stdair::BomRoot& ioBomRoot,
                         stdair::Inventory& ioInventory,
                         stdair::FlightDate& ioFlightDate) {

    // Browse the list of segment-dates and create partner accesses
    // within each segment-date.
    const stdair::SegmentDateList_T& lSegmentDateList = 
      stdair::BomManager::getList<stdair::SegmentDate> (ioFlightDate);
    for (stdair::SegmentDateList_T::const_iterator itSegmentDate = 
           lSegmentDateList.begin();
         itSegmentDate != lSegmentDateList.end(); ++itSegmentDate) {

      stdair::SegmentDate* lCurrentSegmentDate_ptr = *itSegmentDate;
      assert (lCurrentSegmentDate_ptr != NULL);	

      // Get the routing leg keys list
      const stdair::RoutingLegKeyList_T& lRoutingLegKeyList = 
	lCurrentSegmentDate_ptr->getLegKeyList ();

      // Browse the list of routing leg keys and try to create partner accesses
      // with each corresponding leg date.
      for (stdair::RoutingLegKeyList_T::const_iterator itRoutingLegKey = 
           lRoutingLegKeyList.begin();
         itRoutingLegKey != lRoutingLegKeyList.end(); ++itRoutingLegKey) {

	// Try to retrieve the LegDate getting that Boarding Point within the
        // flight date
	stdair::LegDate* lLegDate_ptr = stdair::BomRetriever::
	  retrieveOperatingLegDateFromLongKey (ioFlightDate, *itRoutingLegKey);

        // If there is no LegDate getting that Boarding Point within the flight
        // date, the segment is operating by a partner
	if (lLegDate_ptr == NULL) {

          // Retrieve the (unique) operating LegDate getting that Boarding Point
          // within the partner inventory
          std::ostringstream lRoutingSegmentKey;
          lRoutingSegmentKey << *itRoutingLegKey << ";"
                             << lCurrentSegmentDate_ptr->getOffPoint();
          
          stdair::SegmentDate* lPartnerOperatingSegmentDate_ptr =
            stdair::BomRetriever::
            retrievePartnerSegmentDateFromLongKey (ioInventory,
                                                   lRoutingSegmentKey.str());
          assert (lPartnerOperatingSegmentDate_ptr != NULL);

          // Link the current segment date with its operating one
          stdair::FacBomManager::linkWithOperating (*lCurrentSegmentDate_ptr,
                                                    *lPartnerOperatingSegmentDate_ptr);

          stdair::SegmentDate* lOperatingSegmentDate_ptr =
            stdair::BomRetriever::
            retrieveSegmentDateFromLongKey (ioBomRoot,
                                            lRoutingSegmentKey.str());
          assert(lOperatingSegmentDate_ptr != NULL);

          stdair::Inventory* lInventory_ptr =
            stdair::BomRetriever::
            retrieveInventoryFromLongKey (ioBomRoot, *itRoutingLegKey);
          assert (lInventory_ptr != NULL);

          std::ostringstream lRoutingSegment;
          lRoutingSegment << ioFlightDate.getAirlineCode() << ";"
                          << ioFlightDate.describeKey() << ";"
                          << lCurrentSegmentDate_ptr->getBoardingPoint() << ";"
                          << lCurrentSegmentDate_ptr->getOffPoint();

          stdair::SegmentDate* lPartnerMarketingSegmentDate_ptr =
            stdair::BomRetriever::
            retrievePartnerSegmentDateFromLongKey (*lInventory_ptr, lRoutingSegment.str());
          assert(lPartnerMarketingSegmentDate_ptr  != NULL);

          stdair::FacBomManager::addToList (*lOperatingSegmentDate_ptr, *lPartnerMarketingSegmentDate_ptr);

        }
      }
    }
  }

    
  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  buildSimilarSegmentCabinSets (const stdair::BomRoot& iBomRoot) {
    // Browse the list of inventories and create direct accesses
    // within each inventory.
    const stdair::InventoryList_T& lInvList =
      stdair::BomManager::getList<stdair::Inventory> (iBomRoot);
    for (stdair::InventoryList_T::const_iterator itInv = lInvList.begin();
         itInv != lInvList.end(); ++itInv) {
      stdair::Inventory* lCurrentInv_ptr = *itInv;
      assert (lCurrentInv_ptr != NULL);

      buildSimilarSegmentCabinSets (*lCurrentInv_ptr);
    }
  }
    
  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  buildSimilarSegmentCabinSets (stdair::Inventory& ioInventory) {
    // For instance, we consider two flight-dates are
    // similar if they have the same flight number and the same
    // day-of-the-week departure.

    // Browse the segment-cabin list and build the sets of segment-cabin
    // which have the same flight number and origin-destination
    SimilarSegmentCabinSetMap_T lSSCSM;

    // Browsing the flight-date list
    const stdair::FlightDateList_T& lFlightDateList =
      stdair::BomManager::getList<stdair::FlightDate> (ioInventory);
    for (stdair::FlightDateList_T::const_iterator itFD= lFlightDateList.begin();
         itFD != lFlightDateList.end(); ++itFD) {
      const stdair::FlightDate* lFD_ptr = *itFD;
      assert (lFD_ptr != NULL);
      const stdair::FlightNumber_T& lFlightNumber = lFD_ptr->getFlightNumber();

      // Browsing the segment-date list and retrieve the departure
      // date, the origine and the destination of the segment
      const stdair::SegmentDateList_T& lSegmentDateList =
        stdair::BomManager::getList<stdair::SegmentDate> (*lFD_ptr);
      for (stdair::SegmentDateList_T::const_iterator itSD =
             lSegmentDateList.begin(); itSD != lSegmentDateList.end(); ++itSD) {
        const stdair::SegmentDate* lSD_ptr = *itSD;
        assert (lSD_ptr != NULL);

        const stdair::Date_T& lDepartureDate = lSD_ptr->getBoardingDate();
        const stdair::AirportCode_T& lOrigin = lSD_ptr->getBoardingPoint();
        const stdair::AirportCode_T& lDestination = lSD_ptr->getOffPoint();

        // Browsing the segment-cabin list and retrieve the cabin code and
        // build the corresponding key map.
        const stdair::SegmentCabinList_T& lSegmentCabinList =
          stdair::BomManager::getList<stdair::SegmentCabin> (*lSD_ptr);
        for (stdair::SegmentCabinList_T::const_iterator itSC =
               lSegmentCabinList.begin();
             itSC != lSegmentCabinList.end(); ++itSC) {
          stdair::SegmentCabin* lSC_ptr = *itSC;
          assert (lSC_ptr != NULL);

          std::ostringstream oStr;
          oStr << lFlightNumber << lDepartureDate.day_of_week()
               << lOrigin << lDestination << lSC_ptr->getCabinCode();
          const std::string lMapKey = oStr.str();

          // Add the segment cabin to the similar segment cabin set map.
          SimilarSegmentCabinSetMap_T::iterator itSSCS = lSSCSM.find (lMapKey);
          if (itSSCS == lSSCSM.end()) {
            DepartureDateSegmentCabinMap_T lDDSCMap;
            lDDSCMap.insert (DepartureDateSegmentCabinMap_T::
                             value_type (lDepartureDate, lSC_ptr));
            lSSCSM.insert (SimilarSegmentCabinSetMap_T::
                           value_type (lMapKey, lDDSCMap));
          } else {
            DepartureDateSegmentCabinMap_T& lDDSCMap = itSSCS->second;
            lDDSCMap.insert (DepartureDateSegmentCabinMap_T::
                             value_type (lDepartureDate, lSC_ptr));
          }
        }
      }
    }

    // Initialise the segment data table.
    stdair::TableID_T lTableID = 1;
    for (SimilarSegmentCabinSetMap_T::const_iterator itSSCS = lSSCSM.begin();
         itSSCS != lSSCSM.end(); ++itSSCS, ++lTableID) {
      const DepartureDateSegmentCabinMap_T& lDDSCMap = itSSCS->second;

      buildSegmentSnapshotTable (ioInventory, lTableID, lDDSCMap);
    }    
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  buildSegmentSnapshotTable (stdair::Inventory& ioInventory,
			     const stdair::TableID_T& iTableID,
			     const DepartureDateSegmentCabinMap_T& iDDSCMap) {
    // Build an empty segment data table.
    const stdair::SegmentSnapshotTableKey lKey (iTableID);
    stdair::SegmentSnapshotTable& lSegmentSnapshotTable =
      stdair::FacBom<stdair::SegmentSnapshotTable>::instance().create (lKey);
    stdair::FacBomManager::addToListAndMap (ioInventory, lSegmentSnapshotTable);

    // Build the value type index map.
    DepartureDateSegmentCabinMap_T::const_iterator itDDSC = iDDSCMap.begin();
    assert (itDDSC != iDDSCMap.end());
    const stdair::SegmentCabin* lSegmentCabin_ptr = itDDSC->second;
    
    // Browse the booking class list and build the value type for the classes
    // as well as for the cabin (Q-equivalent).
    stdair::ClassIndexMap_T lClassIndexMap;
    stdair::ClassIndex_T lClassIndex = 0;
    std::ostringstream lSCMapKey;
    lSCMapKey << stdair::DEFAULT_SEGMENT_CABIN_VALUE_TYPE
              << lSegmentCabin_ptr->describeKey();
    lClassIndexMap.insert (stdair::ClassIndexMap_T::
                               value_type (lSCMapKey.str(), lClassIndex));
    ++lClassIndex;
    
    // Browse the booking class list
    const stdair::BookingClassList_T& lBCList =
      stdair::BomManager::getList<stdair::BookingClass>(*lSegmentCabin_ptr);
    for (stdair::BookingClassList_T::const_iterator itBC= lBCList.begin();
         itBC != lBCList.end(); ++itBC) {
      const stdair::BookingClass* lBookingClass_ptr = *itBC;
      assert (lBookingClass_ptr != NULL);
      lClassIndexMap.
        insert (stdair::ClassIndexMap_T::
                value_type(lBookingClass_ptr->describeKey(),lClassIndex));
      ++lClassIndex;
    }

    // Build the segment-cabin index map
    stdair::SegmentCabinIndexMap_T lSegmentCabinIndexMap;
    stdair::SegmentDataID_T lSegmentDataID = 0;
    for (; itDDSC != iDDSCMap.end(); ++itDDSC, ++lSegmentDataID) {
      stdair::SegmentCabin* lCurrentSC_ptr = itDDSC->second;
      assert (lCurrentSC_ptr != NULL);
      lSegmentCabinIndexMap.
        insert (stdair::SegmentCabinIndexMap_T::value_type (lCurrentSC_ptr,
                                                            lSegmentDataID));

      // Added the data table to the segment-cabin.
      lCurrentSC_ptr->setSegmentSnapshotTable (lSegmentSnapshotTable);
    }

    // Initialise the segment data table.
    lSegmentSnapshotTable.initSnapshotBlocks(lSegmentCabinIndexMap,
                                             lClassIndexMap);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::initSnapshotEvents (SEVMGR::SEVMGR_ServicePtr_T ioSEVMGR_ServicePtr,
					     const stdair::Date_T& iStartDate,
                                             const stdair::Date_T& iEndDate) {
    const stdair::Duration_T lTimeZero (0, 0, 0);
    const stdair::Duration_T lOneDayDuration (24, 0, 0);
    const stdair::DateTime_T lBeginSnapshotTime (iStartDate, lTimeZero);
    const stdair::DateTime_T lEndSnapshotTime (iEndDate, lTimeZero);

    // TODO: remove the defaut airline code.
    stdair::NbOfEvents_T lNbOfSnapshots = 0.0;
    for (stdair::DateTime_T lSnapshotTime = lBeginSnapshotTime;
         lSnapshotTime < lEndSnapshotTime; lSnapshotTime += lOneDayDuration) {
      // Create the snapshot event structure
      stdair::SnapshotPtr_T lSnapshotStruct =
        boost::make_shared<stdair::SnapshotStruct>(stdair::DEFAULT_AIRLINE_CODE,
                                                   lSnapshotTime);
      // Create the event structure
      stdair::EventStruct lEventStruct (stdair::EventType::SNAPSHOT,
                                        lSnapshotStruct);

    /**
       \note When adding an event in the event queue, the event can be
       altered. That happens when an event already exists, in the
       event queue, with exactly the same date-time stamp. In that
       case, the date-time stamp is altered for the newly added event,
       so that the unicity on the date-time stamp can be guaranteed.
    */
    ioSEVMGR_ServicePtr->addEvent (lEventStruct);
    ++lNbOfSnapshots;
    }

    // Update the status of snap shots within the event queue.
    ioSEVMGR_ServicePtr->addStatus (stdair::EventType::SNAPSHOT, lNbOfSnapshots);
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  initRMEvents (const stdair::Inventory& iInventory,
                stdair::RMEventList_T& ioRMEventList,
                const stdair::Date_T& iStartDate,
                const stdair::Date_T& iEndDate) {
    const stdair::Duration_T lTimeZero (0, 0, 0);
    const stdair::Duration_T lTime (0, 0, 10);
    const stdair::Duration_T lOneDayDuration (24, 0, 0);
    const stdair::DateTime_T lEarliestEventTime (iStartDate, lTimeZero);
    const stdair::DateTime_T lLatestEventTime (iEndDate, lTimeZero);

    const stdair::AirlineCode_T& lAirlineCode = iInventory.getAirlineCode();

    // Browse the list of flight-dates and initialise the RM events for
    // each flight-date.
    const stdair::FlightDateList_T& lFDList =
      stdair::BomManager::getList<stdair::FlightDate> (iInventory);
    for (stdair::FlightDateList_T::const_iterator itFD = lFDList.begin();
         itFD != lFDList.end(); ++itFD) {
      const stdair::FlightDate* lFD_ptr = *itFD;
      assert (lFD_ptr != NULL);

      // Retrive the departure date and initialise the RM events with
      // the data collection points of the inventory.
      const stdair::Date_T& lDepartureDate = lFD_ptr->getDepartureDate();
      const stdair::DateTime_T lDepatureDateTime (lDepartureDate, lTime);
      for (stdair::DCPList_T::const_iterator itDCP =
             stdair::DEFAULT_DCP_LIST.begin();
           itDCP != stdair::DEFAULT_DCP_LIST.end(); ++itDCP) {
        const stdair::DCP_T& lDCP = *itDCP;

        // Create the event time and check if it is in the validate interval
        const stdair::DateTime_T lEventTime =
          lDepatureDateTime - lOneDayDuration * lDCP;
        if (lEventTime >= lEarliestEventTime && lEventTime <= lLatestEventTime){
          const stdair::KeyDescription_T lKeyDes = lFD_ptr->describeKey();
          stdair::RMEventStruct lRMEvent (lAirlineCode, lKeyDes, lEventTime);
          ioRMEventList.push_back (lRMEvent);
        }
      }      
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  addRMEventsToEventQueue (SEVMGR::SEVMGR_ServicePtr_T ioSEVMGR_ServicePtr,
                           stdair::RMEventList_T& ioRMEventList) {
    // Browse the RM event list and add them to the queue.
    for (stdair::RMEventList_T::iterator itRMEvent = ioRMEventList.begin();
         itRMEvent != ioRMEventList.end(); ++itRMEvent) {
      stdair::RMEventStruct& lRMEvent = *itRMEvent;
      stdair::RMEventPtr_T lRMEventPtr =
        boost::make_shared<stdair::RMEventStruct> (lRMEvent);
      stdair::EventStruct lEventStruct (stdair::EventType::RM, lRMEventPtr);
      ioSEVMGR_ServicePtr->addEvent (lEventStruct);
    }

    // Update the status of RM events within the event queue. 
    const stdair::Count_T lRMEventListSize = ioRMEventList.size();
    ioSEVMGR_ServicePtr->addStatus (stdair::EventType::RM, lRMEventListSize);  

  }

  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  initialiseYieldBasedNestingStructures (const stdair::BomRoot& iBomRoot) {
    // DEBUG
    STDAIR_LOG_DEBUG ("Initialise the yield-based nesting structure for "
                      << "all segment-cabins");
    
    // Browse the list of inventories
    const stdair::InventoryList_T& lInvList =
      stdair::BomManager::getList<stdair::Inventory> (iBomRoot);
    
    // Browse the inventories
    for (stdair::InventoryList_T::const_iterator itInv = lInvList.begin();
         itInv != lInvList.end(); ++itInv) {
      stdair::Inventory* lCurrentInv_ptr = *itInv;
      assert (lCurrentInv_ptr != NULL);
      const stdair::FlightDateList_T& lFlightDateList =
        stdair::BomManager::getList<stdair::FlightDate> (*lCurrentInv_ptr);
      
      // Browse the flight dates
      for (stdair::FlightDateList_T::const_iterator itFD =
             lFlightDateList.begin(); itFD != lFlightDateList.end(); ++itFD) {
        const stdair::FlightDate* lFD_ptr = *itFD;
        assert (lFD_ptr != NULL);
        const stdair::SegmentDateList_T& lSegmentDateList =
          stdair::BomManager::getList<stdair::SegmentDate> (*lFD_ptr);
        
        // Browse the segment dates
        for (stdair::SegmentDateList_T::const_iterator itSD =
             lSegmentDateList.begin(); itSD != lSegmentDateList.end(); ++itSD) {
          const stdair::SegmentDate* lSD_ptr = *itSD;
          assert (lSD_ptr != NULL);
          const stdair::SegmentCabinList_T& lSegmentCabinList =
            stdair::BomManager::getList<stdair::SegmentCabin> (*lSD_ptr);
          
          // Browse the segment cabins
          for (stdair::SegmentCabinList_T::const_iterator itSC =
               lSegmentCabinList.begin(); itSC != lSegmentCabinList.end();
               ++itSC) {
            stdair::SegmentCabin* lSC_ptr = *itSC;
            assert (lSC_ptr != NULL);
            
            // Initialise the nesting structure of the segment cabin
            SegmentCabinHelper::initYieldBasedNestingStructure (*lSC_ptr);
          }
        }
      }
    }
  }
  
  // ////////////////////////////////////////////////////////////////////
  void InventoryManager::
  initialiseListsOfUsablePolicies (const stdair::BomRoot& iBomRoot) {
    // Browse the list of inventories
    const stdair::InventoryList_T& lInvList =
      stdair::BomManager::getList<stdair::Inventory> (iBomRoot);
    
    // Browse the inventories
    for (stdair::InventoryList_T::const_iterator itInv = lInvList.begin();
         itInv != lInvList.end(); ++itInv) {
      stdair::Inventory* lCurrentInv_ptr = *itInv;
      assert (lCurrentInv_ptr != NULL);

      // Create the policies if the optimisation method uses Marginal
      // Revenue Transformation
      stdair::PreOptimisationMethod::EN_PreOptimisationMethod lPreOptMethod =
        lCurrentInv_ptr->getPreOptimisationMethod();

      if (lPreOptMethod == stdair::PreOptimisationMethod::MRT) {
        const stdair::FlightDateList_T& lFlightDateList =
          stdair::BomManager::getList<stdair::FlightDate> (*lCurrentInv_ptr);
      
        // Browse the flight dates
        for (stdair::FlightDateList_T::const_iterator itFD =
               lFlightDateList.begin(); itFD != lFlightDateList.end(); ++itFD) {
          const stdair::FlightDate* lFD_ptr = *itFD;
          assert (lFD_ptr != NULL);
          const stdair::SegmentDateList_T& lSegmentDateList =
            stdair::BomManager::getList<stdair::SegmentDate> (*lFD_ptr);
        
          // Browse the segment dates
          for (stdair::SegmentDateList_T::const_iterator itSD =
                 lSegmentDateList.begin();
               itSD != lSegmentDateList.end(); ++itSD) {
            const stdair::SegmentDate* lSD_ptr = *itSD;
            assert (lSD_ptr != NULL);
            const stdair::SegmentCabinList_T& lSegmentCabinList =
              stdair::BomManager::getList<stdair::SegmentCabin> (*lSD_ptr);
          
            // Browse the segment cabins
            for (stdair::SegmentCabinList_T::const_iterator itSC =
                   lSegmentCabinList.begin(); itSC != lSegmentCabinList.end();
                 ++itSC) {
              stdair::SegmentCabin* lSC_ptr = *itSC;
              assert (lSC_ptr != NULL);

              if (lSC_ptr->getFareFamilyStatus() == true) {
                // Initialise the nesting structure of the segment cabin
                SegmentCabinHelper::initListOfUsablePolicies (*lSC_ptr);
              }
            }
          }
        }
      }
    }
  }

}
