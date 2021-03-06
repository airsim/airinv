// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <cmath>
// Boost
#include <boost/make_shared.hpp>
// StdAir
#include <stdair/stdair_json.hpp>
#include <stdair/basic/BasChronometer.hpp>
#include <stdair/basic/EventType.hpp>
#include <stdair/bom/BomKeyManager.hpp> 
#include <stdair/bom/SnapshotStruct.hpp>
#include <stdair/bom/RMEventStruct.hpp>
#include <stdair/service/Logger.hpp>
#include <stdair/STDAIR_Service.hpp>
// SEvMgr
#include <sevmgr/SEVMGR_Service.hpp>
// AirInv
#include <airinv/basic/BasConst_AIRINV_Service.hpp>
#include <airinv/factory/FacAirinvMasterServiceContext.hpp>
#include <airinv/command/InventoryParser.hpp>
#include <airinv/command/InventoryManager.hpp>
#include <airinv/service/AIRINV_Master_ServiceContext.hpp>
#include <airinv/AIRINV_Service.hpp>
#include <airinv/AIRINV_Master_Service.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Master_Service::AIRINV_Master_Service() 
    : _airinvMasterServiceContext (NULL) {
    assert (false);
  }

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Master_Service::
  AIRINV_Master_Service (const AIRINV_Master_Service& iService)
    : _airinvMasterServiceContext (NULL) {
    assert (false);
  }

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Master_Service::
  AIRINV_Master_Service (const stdair::BasLogParams& iLogParams,
                         const stdair::BasDBParams& iDBParams)
    : _airinvMasterServiceContext (NULL) {
    
    // Initialise the STDAIR service handler
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      initStdAirService (iLogParams, iDBParams);
    
    // Initialise the service context
    initServiceContext();

    // Add the StdAir service context to the AIRINV service context
    // \note RMOL owns the STDAIR service resources here.
    const bool ownStdairService = true;
    addStdAirService (lSTDAIR_Service_ptr, ownStdairService);
    
    // Initialise the (remaining of the) context
    initSlaveAirinvService();
  }

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Master_Service::
  AIRINV_Master_Service (const stdair::BasLogParams& iLogParams)
    : _airinvMasterServiceContext (NULL) {
    
    // Initialise the STDAIR service handler
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      initStdAirService (iLogParams);

    // Initialise the service context
    initServiceContext();
    
    // Add the StdAir service context to the AIRINV service context
    // \note RMOL owns the STDAIR service resources here.
    const bool ownStdairService = true;
    addStdAirService (lSTDAIR_Service_ptr, ownStdairService);

    // Initialise the (remaining of the) context
    initSlaveAirinvService();
  } 

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Master_Service::
  AIRINV_Master_Service (stdair::STDAIR_ServicePtr_T ioSTDAIR_Service_ptr)
    : _airinvMasterServiceContext (NULL) {

    // Initialise the service context
    initServiceContext();

    // Store the STDAIR service object within the (AIRINV) service context
    // \note AirInv does not own the STDAIR service resources here.
    const bool doesNotOwnStdairService = false;
    addStdAirService (ioSTDAIR_Service_ptr, doesNotOwnStdairService); 
    
    // Initialise the (remaining of the) context
    initSlaveAirinvService();
  }

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Master_Service::
  AIRINV_Master_Service (stdair::STDAIR_ServicePtr_T ioSTDAIR_Service_ptr,
			 SEVMGR::SEVMGR_ServicePtr_T ioSEVMGR_Service_ptr)
    : _airinvMasterServiceContext (NULL) {

    // Initialise the service context
    initServiceContext();

    // Store the STDAIR service object within the (AIRINV) service context
    // \note AirInv does not own the STDAIR service resources here.
    const bool doesNotOwnStdairService = false;
    addStdAirService (ioSTDAIR_Service_ptr, doesNotOwnStdairService); 

    //Add the SEvMgr service to the TRADEMGEN service context. 
    const bool doesNotOwnSEVMGRService = false;
    addSEVMGRService (ioSEVMGR_Service_ptr, doesNotOwnSEVMGRService);
    
    // Initialise the (remaining of the) context
    initSlaveAirinvService();
  }

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Master_Service::~AIRINV_Master_Service() {
    // Delete/Clean all the objects from memory
    finalise();
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::finalise() {
    assert (_airinvMasterServiceContext != NULL);
    // Reset the (Boost.)Smart pointer pointing on the STDAIR_Service object.
    _airinvMasterServiceContext->reset();
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::initServiceContext() {
    // Initialise the context
    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext = 
      FacAirinvMasterServiceContext::instance().create();
    _airinvMasterServiceContext = &lAIRINV_Master_ServiceContext;
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::
  addStdAirService (stdair::STDAIR_ServicePtr_T ioSTDAIR_Service_ptr,
                    const bool iOwnStdairService) {

    // Retrieve the AirInv Master service context
    assert (_airinvMasterServiceContext != NULL);
    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;

    // Store the STDAIR service object within the (AIRINV) service context
    lAIRINV_Master_ServiceContext.setSTDAIR_Service (ioSTDAIR_Service_ptr,
                                                     iOwnStdairService);
  } 

// ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::
  addSEVMGRService (SEVMGR::SEVMGR_ServicePtr_T ioSEVMGR_Service_ptr,
		    const bool iOwnSEVMGRService)  {  

    // Retrieve the AirInv Master service context
    assert (_airinvMasterServiceContext != NULL);
    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;

    // Store the STDAIR service object within the (TRADEMGEN) service context
    lAIRINV_Master_ServiceContext.setSEVMGR_Service (ioSEVMGR_Service_ptr,
						     iOwnSEVMGRService);
  }
  
  // ////////////////////////////////////////////////////////////////////
  stdair::STDAIR_ServicePtr_T AIRINV_Master_Service::
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
  stdair::STDAIR_ServicePtr_T AIRINV_Master_Service::
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
  void AIRINV_Master_Service::initSlaveAirinvService() {

    // Retrieve the AirInv Master service context
    assert (_airinvMasterServiceContext != NULL);
    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;

    // Retrieve the StdAir service
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      lAIRINV_Master_ServiceContext.getSTDAIR_ServicePtr();
    assert (lSTDAIR_Service_ptr != NULL); 

    /**
     * Initialise the AirInv service handler.
     *
     * \note The (Boost.)Smart Pointer keeps track of the references
     *       on the Service object, and deletes that object when it is
     *       no longer referenced (e.g., at the end of the process).
     *
     * \note Each Airinv slave service has its own StdAir Service.
     */
    AIRINV_ServicePtr_T lAIRINV_Service_ptr;
    const bool ownSEVMGRService = 
      lAIRINV_Master_ServiceContext.getOwnSEVMGRServiceFlag();
    if (ownSEVMGRService == false) { 
      // Retrieve the SEVMGR service
      SEVMGR::SEVMGR_ServicePtr_T lSEVMGR_Service_ptr =
	lAIRINV_Master_ServiceContext.getSEVMGR_ServicePtr();
      assert (lSEVMGR_Service_ptr != NULL);
      lAIRINV_Service_ptr = boost::make_shared<AIRINV_Service> (lSTDAIR_Service_ptr, 
								lSEVMGR_Service_ptr);
    } else {
      lAIRINV_Service_ptr = boost::make_shared<AIRINV_Service> (lSTDAIR_Service_ptr);
    }
    assert (lAIRINV_Service_ptr != NULL);

    // Store the AIRINV service object within the AIRINV Master service context.
    lAIRINV_Master_ServiceContext.setAIRINV_Service (lAIRINV_Service_ptr);
  }
  
  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::
  parseAndLoad (const InventoryFilePath& iInventoryInputFilename) {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);
  
    // Retrieve the AirInv service context and whether it owns the Stdair
    // service
    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;   
    const bool doesOwnStdairService =
      lAIRINV_Master_ServiceContext.getOwnStdairServiceFlag();
  
    // Retrieve the slave AIRINV service object from the (AIRINV)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the file parsing and BOM building to the dedicated service
    lAIRINV_Service.parseAndLoad (iInventoryInputFilename);  

    /**
     * Have AIRINV_Master clone the whole persistent BOM tree, only when the StdAir
     * service is owned by the current component (AIRINV_Master here).
     */ 
    if (doesOwnStdairService == true) {

      //
      clonePersistentBom ();
    }  
  }
  
  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::buildSampleBom() {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    // Retrieve the AirInv service context and whether it owns the Stdair
    // service
    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
    const bool doesOwnStdairService =
      lAIRINV_Master_ServiceContext.getOwnStdairServiceFlag();

    // Retrieve the StdAir service object from the (AirInv) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_Master_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lPersistentBomRoot = 
      lSTDAIR_Service.getPersistentBomRoot();

    /**
     * 1. Have StdAir build the whole BOM tree, only when the StdAir service is
     *    owned by the current component (AIRINV_Master here)
     */
    if (doesOwnStdairService == true) {
      //
      lSTDAIR_Service.buildSampleBom();
    }

    /**
     * 2. Delegate the complementary building of objects and links by the
     *    appropriate levels/components
     */
    /**
     * Retrieve the slave AirInv service object from the (AirInv)
     * service context.
     */
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();
    lAIRINV_Service.buildSampleBom();

    /**
     * 3. Build the complementary objects/links for the current component (here,
     *    AIRINV_Master)
     */ 
    buildComplementaryLinks (lPersistentBomRoot);

    /**
     * 4. Have AIRINV_Master clone the whole persistent BOM tree, only when the StdAir
     *    service is owned by the current component (AIRINV_Master here).
     */ 
    if (doesOwnStdairService == true) {

      //
      clonePersistentBom ();
    }  
  }  

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::
  parseAndLoad (const stdair::ScheduleFilePath& iScheduleInputFilename,
                const stdair::ODFilePath& iODInputFilename,
                const stdair::FRAT5FilePath& iFRAT5InputFilename,
                const stdair::FFDisutilityFilePath& iFFDisutilityInputFilename,
                const AIRRAC::YieldFilePath& iYieldFilename) {
    
    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);
 
    // Retrieve the AirInv service context and whether it owns the Stdair
    // service
    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;   
    const bool doesOwnStdairService =
      lAIRINV_Master_ServiceContext.getOwnStdairServiceFlag();  

    // Retrieve the slave AirInv service object from the (AirInv)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service(); 

    // Retrieve the StdAir service object from the (AirInv) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_Master_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lPersistentBomRoot = 
      lSTDAIR_Service.getPersistentBomRoot();

    /**
     * 1. Delegate the file parsing and BOM building to the dedicated service
     */
    lAIRINV_Service.parseAndLoad (iScheduleInputFilename, iODInputFilename,
                                  iFRAT5InputFilename,
                                  iFFDisutilityInputFilename, iYieldFilename);  

    /**
     * 2. Delegate the complementary building of objects and links by the
     *    appropriate levels/components
     * \note Currently, no more things to do.
     */

    /**
     * 3. Build the complementary objects/links for the current component (here,
     *    AIRINV_Master)
     */ 
    buildComplementaryLinks (lPersistentBomRoot);

    /**
     * Have AIRINV_Master clone the whole persistent BOM tree, only when the StdAir
     * service is owned by the current component (AIRINV_Master here).
     */
    if (doesOwnStdairService == true) {

      //
      clonePersistentBom ();
    }
  } 

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::clonePersistentBom () {   

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);
  
    // Retrieve the AirInv service context and whether it owns the Stdair
    // service
    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;   
    const bool doesOwnStdairService =
      lAIRINV_Master_ServiceContext.getOwnStdairServiceFlag();  
  
    // Retrieve the slave AIRINV service object from the (AIRINV)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();   

    // Retrieve the StdAir service object from the (AIRINV) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_Master_ServiceContext.getSTDAIR_Service();

    /**
     * 1. Have AirInv clone the whole persistent BOM tree, only when the StdAir
     *    service is owned by the current component (AirInv here).
     */ 
    if (doesOwnStdairService == true) {
 
      //
      lSTDAIR_Service.clonePersistentBom ();
    }
    
    /**
     * 2. Delegate the complementary building of objects and links by the
     *    appropriate component: the slave AIRINV.
     */
    lAIRINV_Service.clonePersistentBom ();
     
    /**
     * 3. Build the complementary objects/links for the current component (here,
     *    TVLSIM)
     */ 
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();   
    buildComplementaryLinks (lBomRoot);
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::
  buildComplementaryLinks (stdair::BomRoot& ioBomRoot) {
    // Currently, no more things to do by AIRINV_Master at that stage.
  }

  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Master_Service::
  jsonHandler (const stdair::JSONString& lJSONString) const {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the slave AirInv (slave) service object from
    // the (AirInv master) service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the BOM dump to the dedicated service
    return lAIRINV_Service.jsonHandler (lJSONString);

  } 

  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Master_Service::
  jsonExportFlightDateList (const stdair::AirlineCode_T& iAirlineCode,
			    const stdair::FlightNumber_T& iFlightNumber) const {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext  == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;   

    // Retrieve the slave AirInv (slave) service object from
    // the (AirInv master) service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the JSON export to the dedicated service
    return lAIRINV_Service.jsonExportFlightDateList (iAirlineCode, 
						     iFlightNumber);
  }

  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Master_Service::
  jsonExportFlightDateObjects (const stdair::AirlineCode_T& iAirlineCode,
			       const stdair::FlightNumber_T& iFlightNumber,
			       const stdair::Date_T& iDepartureDate) const {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the slave AirInv (slave) service object from
    // the (AirInv master) service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the BOM dump to the dedicated service
    return lAIRINV_Service.jsonExportFlightDateObjects (iAirlineCode, 
							iFlightNumber,
							iDepartureDate);
  }

  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Master_Service::
  list (const stdair::AirlineCode_T& iAirlineCode,
        const stdair::FlightNumber_T& iFlightNumber) const {
    std::ostringstream oFlightListStr;

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the slave AirInv (slave) service object from
    // the (AirInv master) service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the BOM display to the dedicated service
    return lAIRINV_Service.list (iAirlineCode, iFlightNumber);
  }
  
  // ////////////////////////////////////////////////////////////////////
  bool AIRINV_Master_Service::
  check (const stdair::AirlineCode_T& iAirlineCode,
         const stdair::FlightNumber_T& iFlightNumber,
         const stdair::Date_T& iDepartureDate) const {
    std::ostringstream oFlightListStr;

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the slave AirInv (slave) service object from
    // the (AirInv master) service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the BOM display to the dedicated service
    return lAIRINV_Service.check (iAirlineCode, iFlightNumber, iDepartureDate);
  }
  
  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Master_Service::csvDisplay() const {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the slave AIRINV service object from
    // the (AIRINV) service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the BOM display to the dedicated service
    return lAIRINV_Service.csvDisplay();
  }

  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Master_Service::
  csvDisplay (const stdair::AirlineCode_T& iAirlineCode,
              const stdair::FlightNumber_T& iFlightNumber,
              const stdair::Date_T& iDepartureDate) const {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the slave AIRINV service object from
    // the (AIRINV) service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the BOM display to the dedicated service
    return lAIRINV_Service.csvDisplay (iAirlineCode, iFlightNumber,
                                       iDepartureDate);
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::
  initSnapshotAndRMEvents (const stdair::Date_T& iStartDate,
                           const stdair::Date_T& iEndDate) {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext; 

    // Retrieve the pointer on the SEvMgr service context
    SEVMGR::SEVMGR_ServicePtr_T lSEVMGR_Service_ptr =
      lAIRINV_Master_ServiceContext.getSEVMGR_ServicePtr();
    assert (lSEVMGR_Service_ptr != NULL);

    // Initialise the snapshot events
    InventoryManager::initSnapshotEvents (lSEVMGR_Service_ptr, iStartDate, iEndDate);

    // \todo Browse the list of inventories and itinialise the RM events of
    //       each inventory.
  
    // Retrieve the slave AIRINV service object from the (AIRINV)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();
    lSEVMGR_Service_ptr->addStatus (stdair::EventType::RM, 0);
    stdair::RMEventList_T lRMEventList =
      lAIRINV_Service.initRMEvents (iStartDate, iEndDate);
    assert (lRMEventList.empty() == false);
    InventoryManager::addRMEventsToEventQueue (lSEVMGR_Service_ptr, lRMEventList);
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::
  calculateAvailability (stdair::TravelSolutionStruct& ioTravelSolution) {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the slave AIRINV service object from the (AIRINV)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the availability retrieval to the dedicated service
    stdair::BasChronometer lAvlChronometer;
    lAvlChronometer.start();

    lAIRINV_Service.calculateAvailability (ioTravelSolution);

    // DEBUG
    // const double lAvlMeasure = lAvlChronometer.elapsed();
    // STDAIR_LOG_DEBUG ("Availability retrieval: " << lAvlMeasure << " - "
    //                   << lAIRINV_Master_ServiceContext.display());
  }

  // ////////////////////////////////////////////////////////////////////
  bool AIRINV_Master_Service::sell (const std::string& iSegmentDateKey,
                                    const stdair::ClassCode_T& iClassCode,
                                    const stdair::PartySize_T& iPartySize) {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the corresponding inventory key
    // const stdair::InventoryKey& lInventoryKey =
    // stdair::BomKeyManager::extractInventoryKey (iSegmentDateKey);

    // Retrieve the slave AirInv service object from the (AirInv Master)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the booking to the dedicated command
    stdair::BasChronometer lSellChronometer;
    lSellChronometer.start();

    // Delegate the BOM building to the dedicated service
    const bool hasBeenSaleSuccessful =
      lAIRINV_Service.sell (iSegmentDateKey, iClassCode, iPartySize);

    // const double lSellMeasure = lSellChronometer.elapsed();

    // DEBUG
    // STDAIR_LOG_DEBUG ("Booking sell: " << lSellMeasure << " - "
    //                   << lAIRINV_Master_ServiceContext.display());

    //
    return hasBeenSaleSuccessful;
  }

  // ////////////////////////////////////////////////////////////////////
  bool AIRINV_Master_Service::sell (const stdair::BookingClassID_T& iClassID,
                                    const stdair::PartySize_T& iPartySize) {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the corresponding inventory key
    // const stdair::InventoryKey& lInventoryKey =
    // stdair::BomKeyManager::extractInventoryKey (iSegmentDateKey);

    // Retrieve the slave AirInv service object from the (AirInv Master)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the booking to the dedicated command
    stdair::BasChronometer lSellChronometer;
    lSellChronometer.start();

    // Delegate the BOM building to the dedicated service
    const bool hasBeenSaleSuccessful =
      lAIRINV_Service.sell (iClassID, iPartySize);

    // const double lSellMeasure = lSellChronometer.elapsed();

    // DEBUG
    // STDAIR_LOG_DEBUG ("Booking sell: " << lSellMeasure << " - "
    //                   << lAIRINV_Master_ServiceContext.display());

    //
    return hasBeenSaleSuccessful;
  }

  // ////////////////////////////////////////////////////////////////////
  bool AIRINV_Master_Service::cancel (const std::string& iSegmentDateKey,
                                      const stdair::ClassCode_T& iClassCode,
                                      const stdair::PartySize_T& iPartySize) {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the corresponding inventory key
    // const stdair::InventoryKey& lInventoryKey =
    // stdair::BomKeyManager::extractInventoryKey (iSegmentDateKey);

    // Retrieve the slave AirInv service object from the (AirInv Master)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the booking to the dedicated command
    stdair::BasChronometer lCancelChronometer;
    lCancelChronometer.start();

    // Delegate the BOM building to the dedicated service
    const bool hasBeenSaleSuccessful =
      lAIRINV_Service.cancel (iSegmentDateKey, iClassCode, iPartySize);

    // const double lCancelMeasure = lCancelChronometer.elapsed();

    // DEBUG
    // STDAIR_LOG_DEBUG ("Booking cancel: " << lCancelMeasure << " - "
    //                   << lAIRINV_Master_ServiceContext.display());

    //
    return hasBeenSaleSuccessful;
  }

  // ////////////////////////////////////////////////////////////////////
  bool AIRINV_Master_Service::cancel (const stdair::BookingClassID_T& iClassID,
                                    const stdair::PartySize_T& iPartySize) {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the corresponding inventory key
    // const stdair::InventoryKey& lInventoryKey =
    // stdair::BomKeyManager::extractInventoryKey (iSegmentDateKey);

    // Retrieve the slave AirInv service object from the (AirInv Master)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Delegate the booking to the dedicated command
    stdair::BasChronometer lCancelChronometer;
    lCancelChronometer.start();

    // Delegate the BOM building to the dedicated service
    const bool hasBeenSaleSuccessful =
      lAIRINV_Service.cancel (iClassID, iPartySize);

    // const double lCancelMeasure = lCancelChronometer.elapsed();

    // DEBUG
    // STDAIR_LOG_DEBUG ("Booking cancel: " << lCancelMeasure << " - "
    //                   << lAIRINV_Master_ServiceContext.display());

    //
    return hasBeenSaleSuccessful;
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::
  takeSnapshots (const stdair::SnapshotStruct& iSnapshot) {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the slave AIRINV service object from the (AIRINV)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Retrieve  the snapshot time and the airline code.
    const stdair::DateTime_T& lSnapshotTime = iSnapshot.getSnapshotTime();
    const stdair::AirlineCode_T& lAirlineCode = iSnapshot.getAirlineCode();

    lAIRINV_Service.takeSnapshots (lAirlineCode, lSnapshotTime);
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Master_Service::
  optimise (const stdair::RMEventStruct& iRMEvent) {

    // Retrieve the AirInv Master service context
    if (_airinvMasterServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInvMaster service "
                                                    "has not been initialised");
    }
    assert (_airinvMasterServiceContext != NULL);

    AIRINV_Master_ServiceContext& lAIRINV_Master_ServiceContext =
      *_airinvMasterServiceContext;
  
    // Retrieve the slave AIRINV service object from the (AIRINV)
    // service context
    AIRINV_Service& lAIRINV_Service =
      lAIRINV_Master_ServiceContext.getAIRINV_Service();

    // Retrieve  the snapshot time and the airline code.
    const stdair::DateTime_T& lRMEventTime = iRMEvent.getRMEventTime();
    const stdair::AirlineCode_T& lAirlineCode = iRMEvent.getAirlineCode();
    const stdair::KeyDescription_T& lFDDescription =
      iRMEvent.getFlightDateDescription();

    lAIRINV_Service.optimise (lAirlineCode, lFDDescription, lRMEventTime);
  }
}
