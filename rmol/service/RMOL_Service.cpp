// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// Boost
#include <boost/make_shared.hpp>
// StdAir
#include <stdair/stdair_inventory_types.hpp>
#include <stdair/basic/BasChronometer.hpp>
#include <stdair/bom/BomManager.hpp>
#include <stdair/bom/BomRoot.hpp>
#include <stdair/bom/Inventory.hpp>
#include <stdair/bom/FlightDate.hpp>
#include <stdair/bom/LegCabin.hpp>
#include <stdair/command/CmdBomManager.hpp>
#include <stdair/service/Logger.hpp>
#include <stdair/STDAIR_Service.hpp>
// RMOL
#include <rmol/basic/BasConst_RMOL_Service.hpp>
#include <rmol/factory/FacRmolServiceContext.hpp>
#include <rmol/command/InventoryParser.hpp>
#include <rmol/command/Optimiser.hpp>
#include <rmol/command/Unconstrainer.hpp>
#include <rmol/command/Forecaster.hpp>
#include <rmol/service/RMOL_ServiceContext.hpp>
#include <rmol/RMOL_Service.hpp>

namespace RMOL {

  // ////////////////////////////////////////////////////////////////////
  RMOL_Service::RMOL_Service() : _rmolServiceContext (NULL) {
    assert (false);
  }

  // ////////////////////////////////////////////////////////////////////
  RMOL_Service::RMOL_Service (const RMOL_Service& iService) :
    _rmolServiceContext (NULL) {
    assert (false);
  }

  // ////////////////////////////////////////////////////////////////////
  RMOL_Service::RMOL_Service (const stdair::BasLogParams& iLogParams,
                              const stdair::BasDBParams& iDBParams,
                              const stdair::CabinCapacity_T& iCabinCapacity,
                              const stdair::Filename_T& iInputFileName) :
    _rmolServiceContext (NULL) {

    // Initialise the STDAIR service handler
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      initStdAirService (iLogParams, iDBParams);
    
    // Initialise the service context
    initServiceContext();

    // Add the StdAir service context to the RMOL service context
    // \note RMOL owns the STDAIR service resources here.
    const bool ownStdairService = true;
    addStdAirService (lSTDAIR_Service_ptr, ownStdairService);

    // Initialise the (remaining of the) context
    initRmolService (iCabinCapacity, iInputFileName);
  }

  // ////////////////////////////////////////////////////////////////////
  RMOL_Service::RMOL_Service (const stdair::BasLogParams& iLogParams,
                              const stdair::CabinCapacity_T& iCabinCapacity,
                              const stdair::Filename_T& iInputFileName) :
    _rmolServiceContext (NULL) {

    // Initialise the STDAIR service handler
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      initStdAirService (iLogParams);
    
    // Initialise the service context
    initServiceContext();

    // Add the StdAir service context to the RMOL service context
    // \note RMOL owns the STDAIR service resources here.
    const bool ownStdairService = true;
    addStdAirService (lSTDAIR_Service_ptr, ownStdairService);

    // Initialise the (remaining of the) context
    initRmolService (iCabinCapacity, iInputFileName);
  }

  // ////////////////////////////////////////////////////////////////////
  RMOL_Service::RMOL_Service (const stdair::BasLogParams& iLogParams,
                              const stdair::BasDBParams& iDBParams,
                              const stdair::CabinCapacity_T& iCabinCapacity) :
    _rmolServiceContext (NULL) {

    // Initialise the STDAIR service handler
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      initStdAirService (iLogParams, iDBParams);
    
    // Initialise the service context
    initServiceContext();

    // Add the StdAir service context to the RMOL service context
    // \note RMOL owns the STDAIR service resources here.
    const bool ownStdairService = true;
    addStdAirService (lSTDAIR_Service_ptr, ownStdairService);

    // Initialise the (remaining of the) context
    initRmolService (iCabinCapacity);
  }

  // ////////////////////////////////////////////////////////////////////
  RMOL_Service::RMOL_Service (const stdair::BasLogParams& iLogParams,
                              const stdair::CabinCapacity_T& iCabinCapacity) :
    _rmolServiceContext (NULL) {

    // Initialise the STDAIR service handler
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      initStdAirService (iLogParams);
    
    // Initialise the service context
    initServiceContext();

    // Add the StdAir service context to the RMOL service context
    // \note RMOL owns the STDAIR service resources here.
    const bool ownStdairService = true;
    addStdAirService (lSTDAIR_Service_ptr, ownStdairService);

    // Initialise the (remaining of the) context
    initRmolService (iCabinCapacity);
  }

  // ////////////////////////////////////////////////////////////////////
  RMOL_Service::RMOL_Service (stdair::STDAIR_ServicePtr_T ioSTDAIRServicePtr,
                              const stdair::CabinCapacity_T& iCabinCapacity,
                              const stdair::Filename_T& iInputFileName)
    : _rmolServiceContext (NULL) {
    
    // Initialise the context
    initServiceContext();
    
    // Add the StdAir service context to the RMOL service context.
    // \note RMOL does not own the STDAIR service resources here.
    const bool doesNotOwnStdairService = false;
    addStdAirService (ioSTDAIRServicePtr, doesNotOwnStdairService);

    // Initialise the (remaining of the) context
    initRmolService (iCabinCapacity, iInputFileName);
  }

  // ////////////////////////////////////////////////////////////////////
  RMOL_Service::RMOL_Service (stdair::STDAIR_ServicePtr_T ioSTDAIRServicePtr)
    : _rmolServiceContext (NULL) {
    
    // Initialise the context
    initServiceContext();
    
    // Add the StdAir service context to the RMOL service context.
    // \note RMOL does not own the STDAIR service resources here.
    const bool doesNotOwnStdairService = false;
    addStdAirService (ioSTDAIRServicePtr, doesNotOwnStdairService);
  }

  // ////////////////////////////////////////////////////////////////////
  RMOL_Service::~RMOL_Service() {
    // Delete/Clean all the objects from memory
    finalise();
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::finalise() {
    assert (_rmolServiceContext != NULL);
    // Reset the (Boost.)Smart pointer pointing on the STDAIR_Service object.
    _rmolServiceContext->reset();
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::initServiceContext() {
    // Initialise the service context
    RMOL_ServiceContext& lRMOL_ServiceContext = 
      FacRmolServiceContext::instance().create();
    _rmolServiceContext = &lRMOL_ServiceContext;
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::
  addStdAirService (stdair::STDAIR_ServicePtr_T ioSTDAIR_Service_ptr,
                    const bool iOwnStdairService) {

    // Retrieve the RMOL service context
    assert (_rmolServiceContext != NULL);
    RMOL_ServiceContext& lRMOL_ServiceContext = *_rmolServiceContext;

    // Store the STDAIR service object within the (AIRINV) service context
    lRMOL_ServiceContext.setSTDAIR_Service (ioSTDAIR_Service_ptr,
                                            iOwnStdairService);
  }

  // //////////////////////////////////////////////////////////////////////
  stdair::STDAIR_ServicePtr_T RMOL_Service::
  initStdAirService (const stdair::BasLogParams& iLogParams,
                     const stdair::BasDBParams& iDBParams) {

    /**
     * Initialise the STDAIR service handler.
     *
     * \note The (Boost.)Smart Pointer keeps track of the references
     *       on the Service object, and deletes that object when it is
     *       no longer referenced (e.g., at the end of the process).
     */
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr = 
      boost::make_shared<stdair::STDAIR_Service> (iLogParams, iDBParams);
    
    return lSTDAIR_Service_ptr;
  }
  
  // ////////////////////////////////////////////////////////////////////
  stdair::STDAIR_ServicePtr_T RMOL_Service::
  initStdAirService (const stdair::BasLogParams& iLogParams) {
    
    /**
     * Initialise the STDAIR service handler.
     *
     * \note The (Boost.)Smart Pointer keeps track of the references
     *       on the Service object, and deletes that object when it is
     *       no longer referenced (e.g., at the end of the process).
     */
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr = 
      boost::make_shared<stdair::STDAIR_Service> (iLogParams);
    
    return lSTDAIR_Service_ptr;
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::initRmolService() {
    // Do nothing at this stage. A sample BOM tree may be built by
    // calling the buildSampleBom() method
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::
  initRmolService (const stdair::CabinCapacity_T& iCabinCapacity) {
    // Build a dummy inventory with a leg-cabin which has the given capacity.
    // The STDAIR-held BOM tree is altered correspondingly.
    const bool isForDemo = true;
    buildSampleBom (isForDemo, iCabinCapacity);
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::
  initRmolService (const stdair::CabinCapacity_T& iCabinCapacity,
                   const stdair::Filename_T& iInputFileName) {

    // Build a dummy inventory with a leg-cabin which has the given capacity.
    // The STDAIR-held BOM tree is altered correspondingly.
    const bool isForDemo = true;
    buildSampleBom (isForDemo, iCabinCapacity);

    // Retrieve the BOM tree root
    assert (_rmolServiceContext != NULL);
    RMOL_ServiceContext& lRMOL_ServiceContext = *_rmolServiceContext;
    stdair::STDAIR_Service& lSTDAIR_Service =
      lRMOL_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    // Complete the BOM tree with the optimisation problem specification
    InventoryParser::parseInputFileAndBuildBom (iInputFileName, lBomRoot);
  }
  
  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::
  buildSampleBom (const bool isForDemo,
                  const stdair::CabinCapacity_T iCabinCapacity) {

    // Retrieve the BOM tree root
    assert (_rmolServiceContext != NULL);
    RMOL_ServiceContext& lRMOL_ServiceContext = *_rmolServiceContext;

    // Retrieve the StdAir service
    stdair::STDAIR_Service& lSTDAIR_Service =
      lRMOL_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    // Delegate the action to the dedicated command
    if (isForDemo == true) {
      stdair::CmdBomManager::buildSampleBomForRMOL (lBomRoot, iCabinCapacity);

    } else {
      stdair::CmdBomManager::buildSampleBom (lBomRoot);
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::optimalOptimisationByMCIntegration (const int K) {
    assert (_rmolServiceContext != NULL);
    RMOL_ServiceContext& lRMOL_ServiceContext = *_rmolServiceContext;

    // Retrieve the StdAir service
    stdair::STDAIR_Service& lSTDAIR_Service =
      lRMOL_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    //
    stdair::LegCabin& lLegCabin = InventoryParser::getSampleLegCabin (lBomRoot);

    stdair::BasChronometer lOptimisationChronometer;
    lOptimisationChronometer.start();

    Optimiser::optimalOptimisationByMCIntegration (K, lLegCabin);

    const double lOptimisationMeasure = lOptimisationChronometer.elapsed();
    
    // DEBUG
    STDAIR_LOG_DEBUG ("Optimisation by Monte-Carlo performed in "
                      << lOptimisationMeasure);
    STDAIR_LOG_DEBUG ("Result: " << lLegCabin.displayVirtualClassList());

    std::ostringstream logStream;
    stdair::BidPriceVector_T lBidPriceVector = lLegCabin.getBidPriceVector();
    logStream << "Bid-Price Vector (BPV): ";
    unsigned int size = lBidPriceVector.size();
    
    for (unsigned int i = 0; i < size - 1; ++i) {
      const double bidPrice = lBidPriceVector.at(i);
      logStream << std::fixed << std::setprecision (2) << bidPrice << ", ";
    }
    const double bidPrice = lBidPriceVector.at(size -1);
    logStream << std::fixed << std::setprecision (2) << bidPrice;
    STDAIR_LOG_DEBUG (logStream.str());
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::optimalOptimisationByDP() {
  }
  
  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::heuristicOptimisationByEmsr() {
    assert (_rmolServiceContext != NULL);
    RMOL_ServiceContext& lRMOL_ServiceContext = *_rmolServiceContext;

    // Retrieve the StdAir service
    stdair::STDAIR_Service& lSTDAIR_Service =
      lRMOL_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    //
    stdair::LegCabin& lLegCabin = InventoryParser::getSampleLegCabin (lBomRoot);

    stdair::BasChronometer lOptimisationChronometer;
    lOptimisationChronometer.start();
    
    Optimiser::heuristicOptimisationByEmsr (lLegCabin);
    
    const double lOptimisationMeasure = lOptimisationChronometer.elapsed();
    // DEBUG
    STDAIR_LOG_DEBUG ("Optimisation EMSR performed in "
                      << lOptimisationMeasure);
    STDAIR_LOG_DEBUG ("Result: " << lLegCabin.displayVirtualClassList());

    std::ostringstream logStream;
    stdair::BidPriceVector_T lBidPriceVector = lLegCabin.getBidPriceVector();
    logStream << "Bid-Price Vector (BPV): ";
    const unsigned int size = lBidPriceVector.size();
    
    for (unsigned int i = 0; i < size - 1; ++i) {
      const double bidPrice = lBidPriceVector.at(i);
      logStream << std::fixed << std::setprecision (2) << bidPrice << ", ";
    }
    const double bidPrice = lBidPriceVector.at(size -1);
    logStream << std::fixed << std::setprecision (2) << bidPrice;
    STDAIR_LOG_DEBUG (logStream.str());
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::heuristicOptimisationByEmsrA() {
    assert (_rmolServiceContext != NULL);
    RMOL_ServiceContext& lRMOL_ServiceContext = *_rmolServiceContext;

    // Retrieve the StdAir service
    stdair::STDAIR_Service& lSTDAIR_Service =
      lRMOL_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    //
    stdair::LegCabin& lLegCabin = InventoryParser::getSampleLegCabin (lBomRoot);

    Optimiser::heuristicOptimisationByEmsrA (lLegCabin);

    // DEBUG
    STDAIR_LOG_DEBUG ("Result: " << lLegCabin.displayVirtualClassList());
    
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::heuristicOptimisationByEmsrB() {
    assert (_rmolServiceContext != NULL);
    RMOL_ServiceContext& lRMOL_ServiceContext = *_rmolServiceContext;

    // Retrieve the StdAir service
    stdair::STDAIR_Service& lSTDAIR_Service =
      lRMOL_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    //
    stdair::LegCabin& lLegCabin = InventoryParser::getSampleLegCabin (lBomRoot);

    Optimiser::heuristicOptimisationByEmsrB (lLegCabin);

    // DEBUG
    STDAIR_LOG_DEBUG ("Result: " << lLegCabin.displayVirtualClassList());
  }

  // ////////////////////////////////////////////////////////////////////
  void RMOL_Service::optimise (const stdair::AirlineCode_T& iAirlineCode,
                               const stdair::KeyDescription_T& iFDDescription,
                               const stdair::DateTime_T& iRMEventTime) {
    if (_rmolServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The Rmol service "
                                                    "has not been initialised");
    }
    assert (_rmolServiceContext != NULL);
    RMOL_ServiceContext& lRMOL_ServiceContext = *_rmolServiceContext;

    // Retrieve the corresponding inventory & flight-date
    stdair::STDAIR_Service& lSTDAIR_Service =
      lRMOL_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();
    stdair::Inventory& lInventory =
      stdair::BomManager::getObject<stdair::Inventory> (lBomRoot, iAirlineCode);
    stdair::FlightDate& lFlightDate =
      stdair::BomManager::getObject<stdair::FlightDate> (lInventory,
                                                         iFDDescription);

    Optimiser::optimise (lFlightDate, iRMEventTime);
  }
}