// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// Boost
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
// StdAir
#include <stdair/basic/BasChronometer.hpp>
// Airinv
#include <airinv/basic/BasConst_AIRINV_Service.hpp>
#include <airinv/command/InventoryManager.hpp>
#include <airinv/factory/FacAirinvServiceContext.hpp>
#include <airinv/service/AIRINV_ServiceContext.hpp>
#include <airinv/service/Logger.hpp>
#include <airinv/AIRINV_Service.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::
  AIRINV_Service (std::ostream& ioLogStream, 
                  const stdair::AirlineCode_T& iAirlineCode)
    : _airinvServiceContext (NULL) {
    init (ioLogStream, iAirlineCode);
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::AIRINV_Service ()
    : _airinvServiceContext (NULL) {
    assert (false);
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::AIRINV_Service (const AIRINV_Service& iService) {
    assert (false);
  }

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::~AIRINV_Service () {
    // Delete/Clean all the objects from memory
    finalise();
  }

  // //////////////////////////////////////////////////////////////////////
  void logInit (const LOG::EN_LogLevel iLogLevel,
                std::ostream& ioLogOutputFile) {
    Logger::instance().setLogParameters (iLogLevel, ioLogOutputFile);
  }

  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Service::init (std::ostream& ioLogStream,
                             const stdair::AirlineCode_T& iAirlineCode) {
    // Set the log file
    logInit (LOG::DEBUG, ioLogStream);

    // Initialise the context
    AIRINV_ServiceContext& lAIRINV_ServiceContext = 
      FacAirinvServiceContext::instance().create (iAirlineCode);
    _airinvServiceContext = &lAIRINV_ServiceContext;
  }
  
  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Service::finalise () {
    assert (_airinvServiceContext != NULL);
  }

  // //////////////////////////////////////////////////////////////////////
  void AIRINV_Service::sell (const stdair::PartySize_T& iPartySize) {
    
    if (_airinvServiceContext == NULL) {
      throw NonInitialisedServiceException();
    }
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext= *_airinvServiceContext;

    try {
      
      // Retrieve the airline code
      const AirlineCode_T& lAirlineCode =
        lAIRINV_ServiceContext.getAirlineCode();
      
      // Delegate the booking to the dedicated command
      stdair::BasChronometer lSellChronometer;
      lSellChronometer.start();
      InventoryManager::sell (lAirlineCode, iPartySize);
      const double lSellMeasure = lSellChronometer.elapsed();
      
      // DEBUG
      AIRINV_LOG_DEBUG ("Booking sell: " << lSellMeasure << " - "
                        << lAIRINV_ServiceContext.display());

    } catch (const std::exception& error) {
      AIRINV_LOG_ERROR ("Exception: "  << error.what());
      throw BookingException();
    }
  }
  
}
