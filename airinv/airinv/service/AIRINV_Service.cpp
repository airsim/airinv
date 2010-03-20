// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// Boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/make_shared.hpp>
// StdAir
#include <stdair/basic/BasChronometer.hpp>
#include <stdair/basic/BasFileMgr.hpp>
#include <stdair/bom/BomManager.hpp> // for display()
#include <stdair/bom/BomRoot.hpp>
#include <stdair/bom/AirlineFeature.hpp>
#include <stdair/bom/AirlineFeatureSet.hpp>
#include <stdair/factory/FacBomContent.hpp>
#include <stdair/service/Logger.hpp>
#include <stdair/STDAIR_Service.hpp>
// Airinv
#include <airinv/basic/BasConst_AIRINV_Service.hpp>
#include <airinv/factory/FacAirinvServiceContext.hpp>
#include <airinv/command/InventoryParser.hpp>
#include <airinv/command/InventoryManager.hpp>
#include <airinv/service/AIRINV_ServiceContext.hpp>
#include <airinv/AIRINV_Service.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::AIRINV_Service () : _airinvServiceContext (NULL) {
    assert (false);
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::AIRINV_Service (const AIRINV_Service& iService)
  : _airinvServiceContext (NULL) {
    assert (false);
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::
  AIRINV_Service (stdair::STDAIR_ServicePtr_T ioSTDAIR_Service_ptr,
                  const stdair::AirlineCode_T& iAirlineCode,
                  const stdair::Filename_T& iInventoryInputFilename)
    : _airinvServiceContext (NULL) {

    // Retrieve the Inventory object, at the root of the BOM tree, on
    // which all of the other BOM objects of the airline inventory will be
    // attached
    assert (ioSTDAIR_Service_ptr != NULL);
    stdair::Inventory& lInventory =
      ioSTDAIR_Service_ptr->getInventory (iAirlineCode);

    // Initialise the service context
    initServiceContext (iAirlineCode, lInventory);

    // Retrieve the AirInv service context
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;
    
    // Store the STDAIR service object within the (AIRINV) service context
    lAIRINV_ServiceContext.setSTDAIR_Service (ioSTDAIR_Service_ptr);
    
    // Initialise the context
    init (iInventoryInputFilename);
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::
  AIRINV_Service (const stdair::BasLogParams& iLogParams,
                  const stdair::BasDBParams& iDBParams,
                  const stdair::AirlineCode_T& iAirlineCode,
                  const stdair::Filename_T& iInventoryInputFilename)
    : _airinvServiceContext (NULL) {
    
    // Initialise the STDAIR service handler
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      initStdAirService (iLogParams, iDBParams);
    
    // Create an Inventory object, at the root of the BOM tree, and on
    // which all of the other BOM objects of the airline inventory will be
    // attached
    assert (lSTDAIR_Service_ptr != NULL);
    stdair::Inventory& lInventory =
      lSTDAIR_Service_ptr->createInventory (iAirlineCode);

    // Initialise the service context
    initServiceContext (iAirlineCode, lInventory);

    // Add the StdAir service context to the AIRINV service context
    addStdAirService (lSTDAIR_Service_ptr);
    
    // Initialise the (remaining of the) context
    init (iInventoryInputFilename);
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::
  AIRINV_Service (const stdair::BasLogParams& iLogParams,
                  const stdair::AirlineCode_T& iAirlineCode,
                  const stdair::Filename_T& iInventoryInputFilename)
    : _airinvServiceContext (NULL) {
    
    // Initialise the STDAIR service handler
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      initStdAirService (iLogParams);
    
    // Create an Inventory object, at the root of the BOM tree, and on
    // which all of the other BOM objects of the airline inventory will be
    // attached
    assert (lSTDAIR_Service_ptr != NULL);
    stdair::Inventory& lInventory =
      lSTDAIR_Service_ptr->createInventory (iAirlineCode);

    // Initialise the service context
    initServiceContext (iAirlineCode, lInventory);
    
    // Add the StdAir service context to the AIRINV service context
    addStdAirService (lSTDAIR_Service_ptr);
    
    // Initialise the (remaining of the) context
    init (iInventoryInputFilename);
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::~AIRINV_Service () {
    // Delete/Clean all the objects from memory
    finalise();
  }

  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Service::
  initServiceContext (const stdair::AirlineCode_T& iAirlineCode,
                      stdair::Inventory& ioIventory) {
    // Initialise the context
    AIRINV_ServiceContext& lAIRINV_ServiceContext = 
      FacAirinvServiceContext::instance().create (iAirlineCode, ioIventory);
    _airinvServiceContext = &lAIRINV_ServiceContext;
  }

  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Service::
  addStdAirService (stdair::STDAIR_ServicePtr_T ioSTDAIR_Service_ptr) {

    // Retrieve the Airinv service context
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // Store the STDAIR service object within the (AIRINV) service context
    lAIRINV_ServiceContext.setSTDAIR_Service (ioSTDAIR_Service_ptr);
  }
  
  // //////////////////////////////////////////////////////////////////////
  stdair::STDAIR_ServicePtr_T AIRINV_Service::
  initStdAirService (const stdair::BasLogParams& iLogParams,
                     const stdair::BasDBParams& iDBParams) {

    // Initialise the STDAIR service handler
    // Note that the track on the object memory is kept thanks to the Boost
    // Smart Pointers component.
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr = 
      boost::make_shared<stdair::STDAIR_Service> (iLogParams, iDBParams);
    
    return lSTDAIR_Service_ptr;
  }
  
  // //////////////////////////////////////////////////////////////////////
  stdair::STDAIR_ServicePtr_T AIRINV_Service::
  initStdAirService (const stdair::BasLogParams& iLogParams) {

    // Initialise the STDAIR service handler
    // Note that the track on the object memory is kept thanks to the Boost
    // Smart Pointers component.
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr = 
      boost::make_shared<stdair::STDAIR_Service> (iLogParams);

    return lSTDAIR_Service_ptr;
  }
  
  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Service::
  init (const stdair::Filename_T& iInventoryInputFilename) {
    // Check that the file path given as input corresponds to an actual file
    const bool doesExistAndIsReadable =
      stdair::BasFileMgr::doesExistAndIsReadable (iInventoryInputFilename);
    if (doesExistAndIsReadable == false) {
      STDAIR_LOG_ERROR ("The inventory input file, '" << iInventoryInputFilename
                        << "', can not be retrieved on the file-system");
      throw stdair::FileNotFoundException();
    }

    // Retrieve the AirInv service context
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // Retrieve the StdAir service context
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_ServicePtr();
    assert (lSTDAIR_Service != NULL);
    
    // Get the root of the BOM tree, on which all of the other BOM objects
    // will be attached
    stdair::BomRoot& lBomRoot = lSTDAIR_Service->getBomRoot();

    // Initialise the airline inventory
    InventoryParser::generateInventory (iInventoryInputFilename, lBomRoot);

    // DEBUG
    STDAIR_LOG_DEBUG ("Generated BomRoot:");
    std::ostringstream oStream;
    stdair::BomManager::display (oStream, lBomRoot);
    STDAIR_LOG_DEBUG (oStream.str());
  }
  
  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Service::finalise () {
    assert (_airinvServiceContext != NULL);
  }

  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Service::sell (const stdair::TravelSolutionStruct& iTravelSolution,
                             const stdair::PartySize_T& iPartySize) {
    
    if (_airinvServiceContext == NULL) {
      throw NonInitialisedServiceException();
    }
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    try {
      
      // Retrieve the airline inventory
      stdair::Inventory& lInventory =
        lAIRINV_ServiceContext.getInventory();
      
      // Delegate the booking to the dedicated command
      stdair::BasChronometer lSellChronometer;
      lSellChronometer.start();
      InventoryManager::sell (lInventory, iTravelSolution, iPartySize);
      const double lSellMeasure = lSellChronometer.elapsed();
      
      // DEBUG
      STDAIR_LOG_DEBUG ("Booking sell: " << lSellMeasure << " - "
                        << lAIRINV_ServiceContext.display());

    } catch (const std::exception& error) {
      STDAIR_LOG_ERROR ("Exception: "  << error.what());
      throw BookingException();
    }
  }
  
}
