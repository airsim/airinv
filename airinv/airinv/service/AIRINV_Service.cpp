// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// StdAir
#include <stdair/basic/BasChronometer.hpp>
#include <stdair/bom/BomManager.hpp> // for display()
#include <stdair/service/Logger.hpp>
// Airinv
#include <airinv/basic/BasConst_AIRINV_Service.hpp>
#include <airinv/command/InventoryManager.hpp>
#include <airinv/factory/FacAirinvServiceContext.hpp>
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
  AIRINV_Service::AIRINV_Service (const stdair::AirlineCode_T& iAirlineCode,
                                  stdair::Inventory& ioIventory)
    : _airinvServiceContext (NULL) {

    // Initialise the context
    init (iAirlineCode, ioIventory);
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::AIRINV_Service (const stdair::BasLogParams& iLogParams,
                                  const stdair::AirlineCode_T& iAirlineCode,
                                  stdair::Inventory& ioIventory)
    : _airinvServiceContext (NULL) {
    
    // Set the log file
    logInit (iLogParams);

    // Initialise the (remaining of the) context
    init (iAirlineCode, ioIventory);
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::~AIRINV_Service () {
    // Delete/Clean all the objects from memory
    finalise();
  }

  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Service::logInit (const stdair::BasLogParams& iLogParams) {
    stdair::Logger::init (iLogParams);
  }

  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Service::init (const stdair::AirlineCode_T& iAirlineCode,
                             stdair::Inventory& ioIventory) {
    // Initialise the context
    AIRINV_ServiceContext& lAIRINV_ServiceContext = 
      FacAirinvServiceContext::instance().create (iAirlineCode, ioIventory);
    _airinvServiceContext = &lAIRINV_ServiceContext;
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
    AIRINV_ServiceContext& lAIRINV_ServiceContext= *_airinvServiceContext;

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
