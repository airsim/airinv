// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
// Boost
#include <boost/make_shared.hpp>
// StdAir
#include <stdair/stdair_json.hpp>
#include <stdair/basic/BasChronometer.hpp>
#include <stdair/basic/JSonCommand.hpp>
#include <stdair/basic/PartnershipTechnique.hpp>
#include <stdair/basic/UnconstrainingMethod.hpp>
#include <stdair/basic/OptimisationMethod.hpp>
#include <stdair/bom/BomKeyManager.hpp> 
#include <stdair/bom/BomManager.hpp> 
#include <stdair/bom/BomKeyManager.hpp> 
#include <stdair/bom/BomRoot.hpp>
#include <stdair/bom/Inventory.hpp>
#include <stdair/bom/FlightDate.hpp>
#include <stdair/bom/SegmentCabin.hpp>
#include <stdair/bom/AirlineFeature.hpp>
#include <stdair/bom/RMEventStruct.hpp>
#include <stdair/bom/BomJSONImport.hpp>
#include <stdair/bom/BomJSONExport.hpp>
#include <stdair/factory/FacBomManager.hpp>
#include <stdair/service/Logger.hpp>
#include <stdair/STDAIR_Service.hpp>
// RMOL
#include <rmol/RMOL_Service.hpp>
// AirRAC
#include <airrac/AIRRAC_Service.hpp>
// SEvMgr
#include <sevmgr/SEVMGR_Service.hpp>
// AirInv
#include <airinv/basic/BasConst_AIRINV_Service.hpp>
#include <airinv/factory/FacAirinvServiceContext.hpp>
#include <airinv/command/ScheduleParser.hpp>
#include <airinv/command/FRAT5Parser.hpp>
#include <airinv/command/FFDisutilityParser.hpp>
#include <airinv/command/InventoryParser.hpp>
#include <airinv/command/InventoryManager.hpp>
#include <airinv/service/AIRINV_ServiceContext.hpp>
#include <airinv/AIRINV_Service.hpp>

namespace AIRINV {

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Service::AIRINV_Service () : _airinvServiceContext (NULL) {
    assert (false);
  }

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Service::AIRINV_Service (const AIRINV_Service& iService)
  : _airinvServiceContext (NULL) {
    assert (false);
  }

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Service::AIRINV_Service (const stdair::BasLogParams& iLogParams)
    : _airinvServiceContext (NULL) {
    
    // Initialise the STDAIR service handler
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      initStdAirService (iLogParams);
    
    // Initialise the service context
    initServiceContext();

    // Add the StdAir service context to the AIRINV service context
    // \note AIRINV owns the STDAIR service resources here.
    const bool ownStdairService = true;
    addStdAirService (lSTDAIR_Service_ptr, ownStdairService);

    // Initalise the RMOL service.
    initRMOLService();

    // Initalise the AIRRAC service.
    initAIRRACService();  

    // Initalise the SEvMgr service.
    initSEVMGRService();
    
    // Initialise the (remaining of the) context
    initAirinvService();
  }

  // ////////////////////////////////////////////////////////////////////
  AIRINV_Service::AIRINV_Service (const stdair::BasLogParams& iLogParams,
                                  const stdair::BasDBParams& iDBParams)
    : _airinvServiceContext (NULL) {
    
    // Initialise the STDAIR service handler
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      initStdAirService (iLogParams, iDBParams);
    
    // Initialise the service context
    initServiceContext();

    // Add the StdAir service context to the AIRINV service context
    // \note AIRINV owns the STDAIR service resources here.
    const bool ownStdairService = true;
    addStdAirService (lSTDAIR_Service_ptr, ownStdairService);

    // Initalise the RMOL service.
    initRMOLService();

    // Initalise the AIRRAC service.
    initAIRRACService(); 

    // Initalise the SEVMGR service.
    initSEVMGRService();
    
    // Initialise the (remaining of the) context
    initAirinvService();
  } 

  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::
  AIRINV_Service (stdair::STDAIR_ServicePtr_T ioSTDAIR_Service_ptr)

    : _airinvServiceContext (NULL) {

    // Initialise the service context
    initServiceContext();
    
    // Store the STDAIR service object within the (AIRINV) service context
    // \note AirInv does not own the STDAIR service resources here.
    const bool doesNotOwnStdairService = false;
    addStdAirService (ioSTDAIR_Service_ptr, doesNotOwnStdairService);
    
    // Initalise the RMOL service.
    initRMOLService();
    
    // Initalise the AIRRAC service.
    initAIRRACService();  

    // Initalise the SEVMGR service.
    initSEVMGRService(); 

    // Initialise the (remaining of the) context
    initAirinvService();
    
  }
  
  // //////////////////////////////////////////////////////////////////////
  AIRINV_Service::
  AIRINV_Service (stdair::STDAIR_ServicePtr_T ioSTDAIR_Service_ptr,
		  SEVMGR::SEVMGR_ServicePtr_T ioSEVMGR_Service_ptr)

    : _airinvServiceContext (NULL) {

    // Initialise the service context
    initServiceContext();
    
    // Store the STDAIR service object within the (AIRINV) service context
    // \note AirInv does not own the STDAIR service resources here.
    const bool doesNotOwnStdairService = false;
    addStdAirService (ioSTDAIR_Service_ptr, doesNotOwnStdairService);

    //Add the SEvMgr service to the TRADEMGEN service context. 
    const bool doesNotOwnSEVMGRService = false;
    addSEVMGRService (ioSEVMGR_Service_ptr, doesNotOwnSEVMGRService);
    
    // Initalise the RMOL service.
    initRMOLService();
    
    // Initalise the AIRRAC service.
    initAIRRACService();  

    // Initialise the (remaining of the) context
    initAirinvService();
    
  }
  
  // ////////////////////////////////////////////////////////////////////
  AIRINV_Service::~AIRINV_Service() {
    // Delete/Clean all the objects from memory
    finalise();
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::finalise() {
    assert (_airinvServiceContext != NULL);
    // Reset the (Boost.)Smart pointer pointing on the STDAIR_Service object.
    _airinvServiceContext->reset();
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::initServiceContext() {
    // Initialise the context
    AIRINV_ServiceContext& lAIRINV_ServiceContext = 
      FacAirinvServiceContext::instance().create();
    _airinvServiceContext = &lAIRINV_ServiceContext;
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::
  addStdAirService (stdair::STDAIR_ServicePtr_T ioSTDAIR_Service_ptr,
                    const bool iOwnStdairService) {

    // Retrieve the Airinv service context
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // Store the STDAIR service object within the (AIRINV) service context
    lAIRINV_ServiceContext.setSTDAIR_Service (ioSTDAIR_Service_ptr,
                                              iOwnStdairService);
  }

 // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::
  addSEVMGRService (SEVMGR::SEVMGR_ServicePtr_T ioSEVMGR_Service_ptr,
		    const bool iOwnSEVMGRService) { 

    // Retrieve the Airinv service context
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // Store the STDAIR service object within the (TRADEMGEN) service context
    lAIRINV_ServiceContext.setSEVMGR_Service (ioSEVMGR_Service_ptr, 
					      iOwnSEVMGRService);
  }
  
  // ////////////////////////////////////////////////////////////////////
  stdair::STDAIR_ServicePtr_T AIRINV_Service::
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
  stdair::STDAIR_ServicePtr_T AIRINV_Service::
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
  void AIRINV_Service::initRMOLService() {

    // Retrieve the AirInv service context
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // Retrieve the StdAir service context
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      lAIRINV_ServiceContext.getSTDAIR_ServicePtr();

    /**
     * Initialise the RMOL service handler.
     *
     * \note The (Boost.)Smart Pointer keeps track of the references
     *       on the Service object, and deletes that object when it is
     *       no longer referenced (e.g., at the end of the process).
     */
    RMOL::RMOL_ServicePtr_T lRMOL_Service_ptr = 
      boost::make_shared<RMOL::RMOL_Service> (lSTDAIR_Service_ptr);
    
    // Store the RMOL service object within the (AIRINV) service context
    lAIRINV_ServiceContext.setRMOL_Service (lRMOL_Service_ptr);
  }
  
  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::initAIRRACService() {

    // Retrieve the AirInv service context
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // Retrieve the StdAir service context
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      lAIRINV_ServiceContext.getSTDAIR_ServicePtr();

    /**
     * Initialise the AIRRAC service handler.
     *
     * \note The (Boost.)Smart Pointer keeps track of the references
     *       on the Service object, and deletes that object when it is
     *       no longer referenced (e.g., at the end of the process).
     */
    AIRRAC::AIRRAC_ServicePtr_T lAIRRAC_Service_ptr = 
      boost::make_shared<AIRRAC::AIRRAC_Service> (lSTDAIR_Service_ptr);
    
    // Store the AIRRAC service object within the (AIRINV) service context
    lAIRINV_ServiceContext.setAIRRAC_Service (lAIRRAC_Service_ptr);
  } 

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::initSEVMGRService() {

    // Retrieve the AIRINV service context   
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = 
      *_airinvServiceContext;
 
    // Retrieve the StdAir service context
    stdair::STDAIR_ServicePtr_T lSTDAIR_Service_ptr =
      lAIRINV_ServiceContext.getSTDAIR_ServicePtr();

    /**
     * Initialise the SEvMgr service handler.
     *
     * \note The (Boost.)Smart Pointer keeps track of the references
     *       on the Service object, and deletes that object when it is
     *       no longer referenced (e.g., at the end of the process).
     */
    SEVMGR::SEVMGR_ServicePtr_T lSEVMGR_Service_ptr = 
      boost::make_shared<SEVMGR::SEVMGR_Service> (lSTDAIR_Service_ptr);
    
    // Store the SEvMgr service object within the (TraDemGen) service context
    const bool doesOwnSEVMGRService = true;
    lAIRINV_ServiceContext.setSEVMGR_Service (lSEVMGR_Service_ptr,
					      doesOwnSEVMGRService);
  }
  
  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::initAirinvService() {
    // Do nothing at this stage. A sample BOM tree may be built by
    // calling the buildSampleBom() method
  }
  
  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::
  parseAndLoad (const AIRINV::InventoryFilePath& iInventoryInputFilename) {

    // Retrieve the AirInv service context
    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException("The AirInv service has not "
                                                   "been initialised");
    }
    assert (_airinvServiceContext != NULL);

    // Retrieve the AirInv service context and whether it owns the Stdair
    // service
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;
    const bool doesOwnStdairService =
      lAIRINV_ServiceContext.getOwnStdairServiceFlag();

    // Retrieve the StdAir service object from the (AirInv) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();

    // Retrieve the persistent BOM root object.
    stdair::BomRoot& lPersistentBomRoot = 
      lSTDAIR_Service.getPersistentBomRoot();
    
    /**
     * 1. Initialise the airline inventories
     */
    InventoryParser::buildInventory (iInventoryInputFilename, 
				     lPersistentBomRoot);

    /**
     * 2. Try to export the airline features from the configuration holder.
     */    
    lSTDAIR_Service.updateAirlineFeatures();  
  
    /**
     * 3. Build the complementary objects/links for the current component (here,
     *    AirInv)
     */ 
    buildComplementaryLinks (lPersistentBomRoot);
    
    /**
     * 4. Have AirInv clone the whole persistent BOM tree, only when the StdAir
     *    service is owned by the current component (AirInv here).
     */
    if (doesOwnStdairService == true) {
 
      //
      clonePersistentBom ();
    }
  }
  
  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::
  parseAndLoad (const stdair::ScheduleFilePath& iScheduleInputFilename,
                const stdair::ODFilePath& iODInputFilename,
                const stdair::FRAT5FilePath& iFRAT5InputFilename,
                const stdair::FFDisutilityFilePath& iFFDisutilityInputFilename,
                const AIRRAC::YieldFilePath& iYieldFilename) {   

    // Retrieve the AirInv service context
    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException("The AirInv service has not "
                                                   "been initialised");
    }
    assert (_airinvServiceContext != NULL);

    // Retrieve the AirInv service context and whether it owns the Stdair
    // service
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;
    const bool doesOwnStdairService =
      lAIRINV_ServiceContext.getOwnStdairServiceFlag();

    // Retrieve the StdAir service object from the (AirInv) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();

    // Retrieve the perssitent BOM root object.
    stdair::BomRoot& lPersistentBomRoot = 
      lSTDAIR_Service.getPersistentBomRoot();

    /**
     * 1. Initialise the airline inventories
     */
    FRAT5Parser::parse (iFRAT5InputFilename, lPersistentBomRoot);
    FFDisutilityParser::parse (iFFDisutilityInputFilename, lPersistentBomRoot);
    ScheduleParser::generateInventories (iScheduleInputFilename, 
					 lPersistentBomRoot);  

    /**
     * 2. Try to export the airline features from the configuration holder.
     */   
    lSTDAIR_Service.updateAirlineFeatures();
   
    /**
     * 3. Build the complementary objects/links for the current component (here,
     *    AirInv)
     */ 
    buildComplementaryLinks (lPersistentBomRoot);
 
    /**
     * 4. Delegate the complementary building of objects and links by the
     *    appropriate levels/components
     * 
     */
    /**
     * Parse the yield structures.
     */
    AIRRAC::AIRRAC_Service& lAIRRAC_Service =
      lAIRINV_ServiceContext.getAIRRAC_Service();
    lAIRRAC_Service.parseAndLoad (iYieldFilename);
    /**
     * Update yield values for booking classes and O&D.
     *
     * \note Need to be done after the buildComplementaryLinks call
     *       because it needs information filled in by this call
     *       (such as the boarding date and time at the segment date level)
     */ 
    lAIRRAC_Service.updateYields(lPersistentBomRoot); 

    /**
     * 5. Have AirInv clone the whole persistent BOM tree, only when the StdAir
     *    service is owned by the current component (AirInv here).
     */
    if (doesOwnStdairService == true) {
 
      //
      clonePersistentBom ();
    }
  }
  
  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::buildSampleBom() {

    // Retrieve the AirInv service context
    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException("The AirInv service has not "
                                                   "been initialised");
    }
    assert (_airinvServiceContext != NULL);

    // Retrieve the AirInv service context and whether it owns the Stdair
    // service
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;
    const bool doesOwnStdairService =
      lAIRINV_ServiceContext.getOwnStdairServiceFlag();

    // Retrieve the StdAir service object from the (AirInv) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service(); 

    // Retrieve the perssitent BOM root object.  
    stdair::BomRoot& lPersistentBomRoot = 
      lSTDAIR_Service.getPersistentBomRoot();

    /**
     * 1. Have StdAir build the whole BOM tree, only when the StdAir service is
     *    owned by the current component (AirSched here)
     */
    if (doesOwnStdairService == true) {
      //
      lSTDAIR_Service.buildSampleBom();
    }

    /**
     * 2. Delegate the complementary building of objects and links by the
     *    appropriate levels/components
     * 
     * \note: Currently, no more things to do by AirInv at that stage,
     *        as there is no child
     */
    /**
     * Let the revenue accounting (i.e., the AirRAC component) build the yields.
     */
    AIRRAC::AIRRAC_Service& lAIRRAC_Service =
      lAIRINV_ServiceContext.getAIRRAC_Service();
    lAIRRAC_Service.buildSampleBom();

    /**
     * Let the revenue management (i.e., the RMOL component) complement the BOM.
     *
     * \note As of now, RMOL does not add anything to the sample BOM.
     */
    RMOL::RMOL_Service& lRMOL_Service = 
      lAIRINV_ServiceContext.getRMOL_Service();
    lRMOL_Service.buildSampleBom();
    
    /**
     * 3. Build the complementary objects/links for the current component (here,
     *    AirInv.
     */
    buildComplementaryLinks (lPersistentBomRoot); 

    /**
     * 4. Try to export the airline features from the configuration holder.
     */   
    lSTDAIR_Service.updateAirlineFeatures();

    /**
     * 5. Have AirInv clone the whole persistent BOM tree, only when the StdAir
     *    service is owned by the current component (AirInv here).
     */
    if (doesOwnStdairService == true) {
 
      //
      clonePersistentBom ();
    }
  } 

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::clonePersistentBom () { 

    // Retrieve the AirInv service context
    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException("The AirInv service has not "
                                                   "been initialised");
    }
    assert (_airinvServiceContext != NULL);

    // Retrieve the AirInv service context and whether it owns the Stdair
    // service
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;
    const bool doesOwnStdairService =
      lAIRINV_ServiceContext.getOwnStdairServiceFlag();

    // Retrieve the StdAir service object from the (AirInv) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();
 
    /**
     * 1. Have AirInv clone the whole persistent BOM tree, only when the StdAir
     *    service is owned by the current component (AirInv here).
     */
    if (doesOwnStdairService == true) {
 
      //
      lSTDAIR_Service.clonePersistentBom ();
    }

    /**
     * 2. Build the complementary links on the clone BOM root object
     */ 
    stdair::BomRoot& lBomRoot = 
      lSTDAIR_Service.getBomRoot();
    buildComplementaryLinks (lBomRoot); 
  }     

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::buildComplementaryLinks (stdair::BomRoot& ioBomRoot) {

    /**
     * 1. Create the routings for all the inventories.
     *
     * \note Those links must be directly be provided by the
     *       stdair::CmdBomManager::buildSampleBom() method, as that latter
     *       may also be called by non-Inventory-aware components
     *       (e.g., AirRAC, AvlCal, SimFQT, TraDemGen, TravelCCM),
     *       which therefore would not be able to build those links.
     *     
     */
    InventoryManager::createDirectAccesses (ioBomRoot);

    /**
     * 2. Build the similar flight-date sets and the corresponding
     * guillotine blocks.
     */
    InventoryManager::buildSimilarSegmentCabinSets (ioBomRoot);

    /**
     * 3. Initialise the bid price vectors.
     */
    InventoryManager::setDefaultBidPriceVector (ioBomRoot);

    /**
     * Initialise the nesting structures   
     */
    InventoryManager::initialiseYieldBasedNestingStructures (ioBomRoot);

    /**
     * Initialise the lists of all usable policies
     */
    InventoryManager::initialiseListsOfUsablePolicies (ioBomRoot);
  }

  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Service::
  jsonHandler (const stdair::JSONString& iJSONString) const {

    //
    // Extract from the JSON-ified string the command
    //
    stdair::JSonCommand::EN_JSonCommand lEN_JSonCommand;
    const bool hasCommandBeenRetrieved =
      stdair::BomJSONImport::jsonImportCommand (iJSONString,
                                                lEN_JSonCommand);
    
    if (hasCommandBeenRetrieved == false) {
      // Return an error JSON-ified string
      std::ostringstream oErrorStream;
      oErrorStream << "{\"error\": \"Wrong JSON-ified string: "
                   << "the command is not understood.\"}";
      return oErrorStream.str();
    }
    assert (hasCommandBeenRetrieved == true);

    //
    // Extract from the JSON-ified string an airline code
    //
    stdair::AirlineCode_T lAirlineCode;
    const bool hasKeyBeenRetrieved =
      stdair::BomJSONImport::jsonImportInventoryKey (iJSONString,
                                                     lAirlineCode);

    if (hasKeyBeenRetrieved == false) {
      // Return an error JSON-ified string
      std::ostringstream oErrorStream;
      oErrorStream << "{\"error\": \"Wrong JSON-ified string: "
                   << "the inventory key is not understood.\"}";
      return oErrorStream.str();
    }
    assert (hasKeyBeenRetrieved == true);

    //
    // Extract from the JSON-ified string a flight number
    //
    stdair::FlightNumber_T lFlightNumber;
    const bool hasFlightNumBeenRetrieved =
      stdair::BomJSONImport::jsonImportFlightNumber (iJSONString,
                                                     lFlightNumber);

    if (hasFlightNumBeenRetrieved == false) {
      // Return an error JSON-ified string
      std::ostringstream oErrorStream;
      oErrorStream << "{\"error\": \"Wrong JSON-ified string: "
                   << "the flight number is not understood.\"}";
      return oErrorStream.str();
    }
    assert (hasFlightNumBeenRetrieved == true);
        
    switch (lEN_JSonCommand) {
    case stdair::JSonCommand::FLIGHT_DATE:{
    
      // Extract from the JSON-ified string a flight date
      stdair::Date_T lDate;
      const bool hasDateBeenRetrieved =
        stdair::BomJSONImport::jsonImportFlightDate (iJSONString,
                                                     lDate);

      if (hasDateBeenRetrieved == false) {
        // Return an error JSON-ified string
        std::ostringstream oErrorStream;
        oErrorStream << "{\"error\": \"Wrong JSON-ified string: "
                     << "the flight date is not understood.\"}";
        return oErrorStream.str();
      }
             
      // DEBUG
      STDAIR_LOG_DEBUG ("=> airline code = '" << lAirlineCode
                        << "', flight number = " << lFlightNumber
                        << "', departure date = '" << lDate << "'");
          
      // DEBUG: Display the flight-date details dump
      const std::string& lFlightDateDetailsCSVDump =
        csvDisplay (lAirlineCode, lFlightNumber, lDate);
      STDAIR_LOG_DEBUG (std::endl << lFlightDateDetailsCSVDump);
      
      // Dump the full details of the flight-date into the JSON-ified flight-date
      const std::string& lFlightDateDetailsJSONDump =
        jsonExportFlightDateObjects (lAirlineCode, 
                                     lFlightNumber, 
                                     lDate);
      
      // DEBUG
      STDAIR_LOG_DEBUG ("Send: '" << lFlightDateDetailsJSONDump << "'");

      return lFlightDateDetailsJSONDump;
      break;
    }
    case stdair::JSonCommand::LIST:{

      // DEBUG
      STDAIR_LOG_DEBUG ("=> airline code = '" << lAirlineCode
                        << "', flight number = " << lFlightNumber << "'");

      // DEBUG: Display the flight-date list dump
      const std::string& lFlightDateListCSVDump =
        list (lAirlineCode, lFlightNumber);
      STDAIR_LOG_DEBUG (std::endl << lFlightDateListCSVDump);

      // Dump the full list of the flight-date into the JSON-ified flight-date
      const std::string& lFlightDateListJSONDump =
        jsonExportFlightDateList (lAirlineCode, lFlightNumber);

      // DEBUG
      STDAIR_LOG_DEBUG ("Send: '" << lFlightDateListCSVDump << "'");
      
      return lFlightDateListJSONDump;
      break;
    }
    default: {
      // Return an Error string
      std::ostringstream lErrorCmdMessage;
      const std::string& lCommandStr =
        stdair::JSonCommand::getLabel(lEN_JSonCommand);
      lErrorCmdMessage << "{\"error\": \"The command '" << lCommandStr
                       << "' is not handled by the AirInv service.\"}";
      return lErrorCmdMessage.str();
      break;
    }
    }
    // Return an error JSON-ified string
    assert (false);
    std::string lJSONDump ("{\"error\": \"Wrong JSON-ified string\"}");
    return lJSONDump;
  }

  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Service::
  jsonExportFlightDateList (const stdair::AirlineCode_T& iAirlineCode,
			    const stdair::FlightNumber_T& iFlightNumber) const {

    // Retrieve the AIRINV service context
    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);

    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;
  
    // Retrieve the STDAIR service object from the (AIRINV) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();

    // Delegate the JSON export to the dedicated service
    return lSTDAIR_Service.jsonExportFlightDateList (iAirlineCode, 
						     iFlightNumber);
  } 

  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Service::
  jsonExportFlightDateObjects (const stdair::AirlineCode_T& iAirlineCode,
			       const stdair::FlightNumber_T& iFlightNumber,
			       const stdair::Date_T& iDepartureDate) const {

    // Retrieve the AIRINV service context
    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);

    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;
  
    // Retrieve the STDAIR service object from the (AIRINV) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();

    // Delegate the JSON export to the dedicated service
    return lSTDAIR_Service.jsonExportFlightDateObjects (iAirlineCode, 
							iFlightNumber,
							iDepartureDate);
  }
  
  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Service::
  list (const stdair::AirlineCode_T& iAirlineCode,
        const stdair::FlightNumber_T& iFlightNumber) const {
    std::ostringstream oFlightListStr;

    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // \todo Check that the current AIRINV_Service is actually operating for
    //       the given airline

    // Retrieve the STDAIR service object from the (AirInv) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();

    // Delegate the BOM display to the dedicated service
    return lSTDAIR_Service.list (iAirlineCode, iFlightNumber);
  }
  
  // ////////////////////////////////////////////////////////////////////
  bool AIRINV_Service::
  check (const stdair::AirlineCode_T& iAirlineCode,
         const stdair::FlightNumber_T& iFlightNumber,
         const stdair::Date_T& iDepartureDate) const {
    std::ostringstream oFlightListStr;

    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // \todo Check that the current AIRINV_Service is actually operating for
    //       the given airline

    // Retrieve the STDAIR service object from the (AirInv) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();

    // Delegate the BOM display to the dedicated service
    return lSTDAIR_Service.check (iAirlineCode, iFlightNumber, iDepartureDate);
  }
  
  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Service::csvDisplay() const {

    // Retrieve the AIRINV service context
    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);

    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;
  
    // Retrieve the STDAIR service object from the (AirInv) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();  
    const stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    // Delegate the BOM display to the dedicated service
    return lSTDAIR_Service.csvDisplay(lBomRoot);
  }
  
  // ////////////////////////////////////////////////////////////////////
  std::string AIRINV_Service::
  csvDisplay (const stdair::AirlineCode_T& iAirlineCode,
              const stdair::FlightNumber_T& iFlightNumber,
              const stdair::Date_T& iDepartureDate) const {

    // Retrieve the AIRINV service context
    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);

    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;
  
    // Retrieve the STDAIR service object from the (AirInv) service context
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();

    // Delegate the BOM display to the dedicated service
    return lSTDAIR_Service.csvDisplay (iAirlineCode, iFlightNumber,
                                       iDepartureDate);
  }
  
  // ////////////////////////////////////////////////////////////////////
  stdair::RMEventList_T AIRINV_Service::
  initRMEvents (const stdair::Date_T& iStartDate,
                const stdair::Date_T& iEndDate) {

    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // \todo Retrieve the corresponding inventory
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    stdair::RMEventList_T oRMEventList;
    const stdair::InventoryList_T& lInventoryList =
      stdair::BomManager::getList<stdair::Inventory> (lBomRoot);
    for (stdair::InventoryList_T::const_iterator itInv = lInventoryList.begin();
         itInv != lInventoryList.end(); ++itInv) {
      const stdair::Inventory* lInv_ptr = *itInv;
      assert (lInv_ptr != NULL);
      
      InventoryManager::initRMEvents (*lInv_ptr, oRMEventList,
                                      iStartDate, iEndDate); 
    }

    return oRMEventList;
  }
  
  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::
  calculateAvailability (stdair::TravelSolutionStruct& ioTravelSolution) {
    
    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // Retrieve the corresponding inventory.
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    // Delegate the booking to the dedicated command
    stdair::BasChronometer lAvlChronometer;
    lAvlChronometer.start();
    InventoryManager::calculateAvailability (lBomRoot, ioTravelSolution);
    // const double lAvlMeasure = lAvlChronometer.elapsed();

    // DEBUG
    // STDAIR_LOG_DEBUG ("Availability retrieval: " << lAvlMeasure << " - "
    //                   << lAIRINV_ServiceContext.display());
  }
  
  // ////////////////////////////////////////////////////////////////////
  bool AIRINV_Service::sell (const std::string& iSegmentDateKey,
                             const stdair::ClassCode_T& iClassCode,
                             const stdair::PartySize_T& iPartySize) {
    bool isSellSuccessful = false;

    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // \todo Check that the current AIRINV_Service is actually operating for
    //       the given airline (inventory key)
    // Retrieve the corresponding inventory key
    const stdair::InventoryKey& lInventoryKey =
      stdair::BomKeyManager::extractInventoryKey (iSegmentDateKey);

    // Retrieve the root of the BOM tree
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    // Retrieve the corresponding inventory
    stdair::Inventory& lInventory = stdair::BomManager::
      getObject<stdair::Inventory> (lBomRoot, lInventoryKey.toString());

    // Delegate the booking to the dedicated command
    stdair::BasChronometer lSellChronometer; lSellChronometer.start();
    isSellSuccessful = InventoryManager::sell (lInventory, iSegmentDateKey,
                                               iClassCode, iPartySize);
    // const double lSellMeasure = lSellChronometer.elapsed();

    // DEBUG
    // STDAIR_LOG_DEBUG ("Booking sell: " << lSellMeasure << " - "
    //                  << lAIRINV_ServiceContext.display());

    return isSellSuccessful;
  }
  
  // ////////////////////////////////////////////////////////////////////
  bool AIRINV_Service::sell (const stdair::BookingClassID_T& iClassID,
                             const stdair::PartySize_T& iPartySize) {
    bool isSellSuccessful = false;

    // Delegate the booking to the dedicated command
    stdair::BasChronometer lSellChronometer; lSellChronometer.start();
    isSellSuccessful = InventoryManager::sell (iClassID, iPartySize);
    // const double lSellMeasure = lSellChronometer.elapsed();

    // DEBUG
    // STDAIR_LOG_DEBUG ("Booking sell: " << lSellMeasure << " - "
    //                  << lAIRINV_ServiceContext.display());

    return isSellSuccessful;
  }
  
  // ////////////////////////////////////////////////////////////////////
  bool AIRINV_Service::cancel (const std::string& iSegmentDateKey,
                             const stdair::ClassCode_T& iClassCode,
                             const stdair::PartySize_T& iPartySize) {
    bool isCancellationSuccessful = false;

    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // \todo Check that the current AIRINV_Service is actually operating for
    //       the given airline (inventory key)
    // Retrieve the corresponding inventory key
    const stdair::InventoryKey& lInventoryKey =
      stdair::BomKeyManager::extractInventoryKey (iSegmentDateKey);

    // Retrieve the root of the BOM tree
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    // Retrieve the corresponding inventory
    stdair::Inventory& lInventory = stdair::BomManager::
      getObject<stdair::Inventory> (lBomRoot, lInventoryKey.toString());

    // Delegate the booking to the dedicated command
    stdair::BasChronometer lCancellationChronometer;
    lCancellationChronometer.start();
    isCancellationSuccessful = InventoryManager::cancel (lInventory,
                                                         iSegmentDateKey,
                                                         iClassCode,iPartySize);
    // const double lCancellationMeasure = lCancellationChronometer.elapsed();

    // DEBUG
    // STDAIR_LOG_DEBUG ("Booking cancellation: "
    //                  << lCancellationMeasure << " - "
    //                  << lAIRINV_ServiceContext.display());

    return isCancellationSuccessful;
  }
  
  // ////////////////////////////////////////////////////////////////////
  bool AIRINV_Service::cancel (const stdair::BookingClassID_T& iClassID,
                             const stdair::PartySize_T& iPartySize) {
    bool isCancelSuccessful = false;

    // Delegate the booking to the dedicated command
    stdair::BasChronometer lCancelChronometer; lCancelChronometer.start();
    isCancelSuccessful = InventoryManager::cancel (iClassID, iPartySize);
    // const double lCancelMeasure = lCancelChronometer.elapsed();

    // DEBUG
    // STDAIR_LOG_DEBUG ("Booking cancel: " << lCancelMeasure << " - "
    //                  << lAIRINV_ServiceContext.display());

    return isCancelSuccessful;
  }
  
  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::takeSnapshots (const stdair::AirlineCode_T& iAirlineCode,
                                      const stdair::DateTime_T& iSnapshotTime) {

    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext; 

    // TODO: Retrieve the corresponding inventory.
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();

    const stdair::InventoryList_T lInventoryList =
      stdair::BomManager::getList<stdair::Inventory> (lBomRoot);
    for (stdair::InventoryList_T::const_iterator itInv = lInventoryList.begin();
         itInv != lInventoryList.end(); ++itInv) {
      const stdair::Inventory* lInv_ptr = *itInv;
      assert (lInv_ptr != NULL);
      
      InventoryManager::takeSnapshots (*lInv_ptr, iSnapshotTime); 
    }
  }

  // ////////////////////////////////////////////////////////////////////
  void AIRINV_Service::optimise (const stdair::AirlineCode_T& iAirlineCode,
                                 const stdair::KeyDescription_T& iFDDescription,
                                 const stdair::DateTime_T& iRMEventTime) {
    
    if (_airinvServiceContext == NULL) {
      throw stdair::NonInitialisedServiceException ("The AirInv service "
                                                    "has not been initialised");
    }
    assert (_airinvServiceContext != NULL);
    AIRINV_ServiceContext& lAIRINV_ServiceContext = *_airinvServiceContext;

    // Retrieve the corresponding inventory & flight-date
    stdair::STDAIR_Service& lSTDAIR_Service =
      lAIRINV_ServiceContext.getSTDAIR_Service();
    stdair::BomRoot& lBomRoot = lSTDAIR_Service.getBomRoot();
    stdair::Inventory* lInventory_ptr =
      stdair::BomManager::getObjectPtr<stdair::Inventory> (lBomRoot, iAirlineCode);
    if (lInventory_ptr == NULL) {
      std::ostringstream oErrorMessage;
      oErrorMessage << "The Inventory '" << iAirlineCode
                    << "', can not be retrieved.";
      STDAIR_LOG_ERROR (oErrorMessage.str());
      throw InventoryNotFoundException (oErrorMessage.str());
    }
    assert (lInventory_ptr != NULL);
    stdair::FlightDate* lFlightDate_ptr =
      stdair::BomManager::getObjectPtr<stdair::FlightDate> (*lInventory_ptr,
                                                            iFDDescription);
    if (lFlightDate_ptr == NULL) {
      std::ostringstream oErrorMessage;
      oErrorMessage << "The flight date '" << iFDDescription
                    << "', can not be retrieved in the '"
                    << iAirlineCode << "' inventory.";
      STDAIR_LOG_ERROR (oErrorMessage.str());
      throw FlightDateNotFoundException (oErrorMessage.str());
    }
    assert (lFlightDate_ptr != NULL);

    const stdair::UnconstrainingMethod& lUnconstrainingMethod =
      lInventory_ptr->getUnconstrainingMethod();   
    const stdair::ForecastingMethod& lForecastingMethod =
      lInventory_ptr->getForecastingMethod(); 
    const stdair::PreOptimisationMethod& lPreOptimisationMethod =
      lInventory_ptr->getPreOptimisationMethod();
    const stdair::OptimisationMethod& lOptimisationMethod =
      lInventory_ptr->getOptimisationMethod();
    const stdair::PartnershipTechnique& lPartnershipTechnique =
      lInventory_ptr->getPartnershipTechnique();

    // Retrieve the RMOL service.
    RMOL::RMOL_Service& lRMOL_Service = lAIRINV_ServiceContext.getRMOL_Service();

    // Optimise the flight-date.
    bool isOptimised = lRMOL_Service.optimise (*lFlightDate_ptr, iRMEventTime,
                                               lUnconstrainingMethod,
                                               lForecastingMethod,
                                               lPreOptimisationMethod,
                                               lOptimisationMethod,
                                               lPartnershipTechnique);

    const stdair::OptimisationMethod::EN_OptimisationMethod& lENOptimisationMethod = lOptimisationMethod.getMethod();
    const bool isEMSRb = 
      (lENOptimisationMethod == stdair::OptimisationMethod::LEG_BASED_EMSR_B);
    // Update the inventory with the new controls.
    // updateBookingControls uses bid price vector to set 
    // the authorization level. But EMSRb sets directly the 
    // authorization level and does not compute the bid price vector.
    // So if EMSRb is used, do not call updateBookingControls.
    if (isOptimised == true && isEMSRb == false) {
      InventoryManager::updateBookingControls (*lFlightDate_ptr);
      InventoryManager::recalculateAvailability (*lFlightDate_ptr);
    } 
  }
}
