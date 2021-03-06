#ifndef __AIRINV_SVC_AIRINV_MASTER_SERVICE_HPP
#define __AIRINV_SVC_AIRINV_MASTER_SERVICE_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// StdAir
#include <stdair/stdair_basic_types.hpp>
#include <stdair/stdair_file.hpp>
#include <stdair/stdair_service_types.hpp>
#include <stdair/stdair_inventory_types.hpp>
#include <stdair/stdair_maths_types.hpp>
#include <stdair/bom/BomIDTypes.hpp>
// AirRAC
#include <airrac/AIRRAC_Types.hpp>
// SEvMgr
#include <sevmgr/SEVMGR_Types.hpp>
// AirInv
#include <airinv/AIRINV_Types.hpp>


/// Forward declarations
namespace stdair {
  class BomRoot;
  class AirlineFeatureSet;
  class Inventory;
  class JSONString;
  class STDAIR_Service;
  struct BasLogParams;
  struct BasDBParams;
  struct SnapshotStruct;
  struct RMEventStruct;
  struct TravelSolutionStruct;
}

namespace AIRINV {

  /// Forward declarations
  class AIRINV_Master_ServiceContext;

  
  /**
   * @brief Interface for the AIRINV Services.
   */
  class AIRINV_Master_Service {
  public:   
    // ////////// Constructors and destructors //////////
    /**
     * Constructor.
     *
     * The initSlaveAirinvService() method is called; see the
     * corresponding documentation for more details.
     *
     * A reference on an output stream is given, so that log outputs
     * can be directed onto that stream.
     *
     * Moreover, database connection parameters are given, so that a
     * session can be created on the corresponding database.
     *
     * @param const stdair::BasLogParams& Parameters for the output log stream.
     * @param const stdair::BasDBParams& Parameters for the database access.
     */
    AIRINV_Master_Service (const stdair::BasLogParams&,
                           const stdair::BasDBParams&);

    /**
     * Constructor.
     *
     * The initSlaveAirinvService() method is called; see the
     * corresponding documentation for more details.
     *
     * A reference on an output stream is given, so that log outputs
     * can be directed onto that stream.
     *
     * @param const stdair::BasLogParams& Parameters for the output log stream.
     */
    AIRINV_Master_Service (const stdair::BasLogParams&);  

    /**
     * Constructor.
     *
     * The initSlaveAirinvService() method is called; see the
     * corresponding documentation for more details.
     *
     * Moreover, as no reference on any output stream is given, it is
     * assumed that the StdAir log service has already been initialised
     * with the proper log output stream by some other methods in the
     * calling chain (for instance, when the AIRINV_Master_Service is
     * itself being initialised by another library service such as
     * SIMCRS_Service).
     *
     * @param stdair::STDAIR_ServicePtr_T Reference on the STDAIR service.
     */
    AIRINV_Master_Service (stdair::STDAIR_ServicePtr_T);

    /**
     * Constructor.
     *
     * The initSlaveAirinvService() method is called; see the
     * corresponding documentation for more details.
     *
     * Moreover, as no reference on any output stream is given, it is
     * assumed that the StdAir log service has already been initialised
     * with the proper log output stream by some other methods in the
     * calling chain (for instance, when the AIRINV_Master_Service is
     * itself being initialised by another library service such as
     * SIMCRS_Service).
     *
     * @param stdair::STDAIR_ServicePtr_T Reference on the STDAIR service.
     * @param stdair::SEVMGR_ServicePtr_T Reference on the SEVMGR service.
     */
    AIRINV_Master_Service (stdair::STDAIR_ServicePtr_T,
			   SEVMGR::SEVMGR_ServicePtr_T);

    /**
     * Parse the inventory dump and load it into memory.
     *
     * The CSV file, describing the airline inventory for the
     * simulator, is parsed and instantiated in memory accordingly.
     *
     * @param const InventoryFilePath& Filename of the input inventory file.
     */
    void parseAndLoad (const InventoryFilePath&);

    /**
     * Parse the schedule and O&D input files, and load them into memory.
     *
     * The CSV files, describing the airline schedule and the O&Ds for
     * the simulator, are parsed and instantiated in memory accordingly.
     *
     * @param const stdair::ScheduleFilePath& Filename of the input schedule file.
     * @param const stdair::ODFilePath& Filename of the input O&D file.
     * @param const stdair::FRAT5FilePath& Filename of the input FRAT5 file.
     * @param const stdair::FFDisutilityFilePath& Filename of the input FF disutility file.
     * @param const AIRRAC::YieldFilePath& Filename of the input yield file.
     */
    void parseAndLoad (const stdair::ScheduleFilePath&,
                       const stdair::ODFilePath&,
                       const stdair::FRAT5FilePath&,
                       const stdair::FFDisutilityFilePath&,
                       const AIRRAC::YieldFilePath&);

    /**
     * Destructor.
     */
    ~AIRINV_Master_Service();

    /** Initialise the snapshot and RM events for the inventories.
        @param const stdiar::Date_T& Parameters for the start date.
        @param const stdiar::Date_T& Parameters for the end date.
     */
    void initSnapshotAndRMEvents (const stdair::Date_T&, const stdair::Date_T&);
    

  public:
    // /////////// Business Methods /////////////
    /**
     * Build a sample BOM tree, and attach it to the BomRoot instance.
     *
     * The BOM tree is based on two actual inventories (one for BA,
     * another for AF). Each inventory contains one flight. One of
     * those flights has two legs (and therefore three segments).
     */
    void buildSampleBom();
      
    /**
     * Clone the persistent BOM object.
     */
    void clonePersistentBom (); 

    /**
     * Build all the complementary links in the given bom root object.
     * \note Do nothing for now.
     */
    void buildComplementaryLinks (stdair::BomRoot&);

    /**
     * Compute the availability for the given travel solution.
     */
    void calculateAvailability (stdair::TravelSolutionStruct&);

    /**
     * Register a booking.
     *
     * @param const std::string& Key for the segment on which the sale is made.
     * @param const stdair::ClassCode_T& Class code where the sale is made.
     * @param const stdair::PartySize_T& Party size.
     * @return bool Whether or not the sale was successfull
     */
    bool sell (const std::string& iSegmentDateKey, const stdair::ClassCode_T&,
               const stdair::PartySize_T&);

    /**
     * Register a booking.
     *
     * @param const stdair::BookingClassID_T&
     * @param const stdair::PartySize_T& Party size.
     * @return bool Whether or not the sale was successfull
     */
    bool sell (const stdair::BookingClassID_T&, const stdair::PartySize_T&);
    
    /**
     * Register a cancellation.
     *
     * @param const std::string& Key for the segment on which the cancellation
     *                   is made.
     * @param const stdair::ClassCode_T& Class code where the sale is made.
     * @param const stdair::PartySize_T& Party size.
     * @return bool Whether or not the cancellation was successfull
     */
    bool cancel (const std::string& iSegmentDateKey, const stdair::ClassCode_T&,
                 const stdair::PartySize_T&);
    
    /**
     * Register a cancellation.
     *
     * @param stdair::BookingClassID_T& 
     * @param const stdair::PartySize_T& Party size.
     * @return bool Whether or not the cancellation was successfull
     */
    bool cancel (const stdair::BookingClassID_T&, const stdair::PartySize_T&);

    /**
     * Take inventory snapshots.
     */
    void takeSnapshots (const stdair::SnapshotStruct&);
    
    /**
     * Optimise (revenue management) an flight-date/network-date
     */
    void optimise (const stdair::RMEventStruct&);    


  public:

    // //////////////// Export support methods /////////////////   
    /**
     * Dispatch the JSon command string to the right JSon Service, according
     * to the JSon command type.
     *
     * @param const stdair::JSONString& Input string which contained the JSon
     *        command string.
     * @return std::string Output string in which the asking objects are
     *         logged/dumped in a JSon format.
     */
    std::string jsonHandler (const stdair::JSONString&) const;

    /**
     * Recursively dump, in the returned string and in JSON format,
     * the flight-date list corresponding to the parameters given as 
     * input.
     *   
     * @param const AirlineCode& Airline for which the flight-dates should be
     *        displayed. If set to "all" (default), all the inventories will
     *        be displayed. 
     * @param const FlightNumber_T& Flight number for which all the departure
     *        dates should be displayed. If set to 0 (the default),
     *        all the flight numbers will be displayed.
     */
    std::string jsonExportFlightDateList (const stdair::AirlineCode_T& iAirlineCode = "all",
                                          const stdair::FlightNumber_T& iFlightNumber = 0) const;

    /**
     * Recursively dump, in the returned string and in JSON format,
     * the flight-date corresponding to the parameters given as input.
     *
     * @param const stdair::AirlineCode_T& Airline code of the flight to dump.
     * @param const stdair::FlightNumber_T& Flight number of the
     *        flight to dump.
     * @param const stdair::Date_T& Departure date of the flight to dump.
     * @return std::string Output string in which the BOM tree is JSON-ified.
     */
    std::string jsonExportFlightDateObjects (const stdair::AirlineCode_T&,
					     const stdair::FlightNumber_T&,
					     const stdair::Date_T& iDepartureDate) const;


  public:
    // //////////////// Display support methods /////////////////
    /**
     * Display the list of flight-dates (contained within the BOM tree)
     * corresponding to the parameters given as input.
     *
     * @param const AirlineCode& Airline for which the flight-dates should be
     *        displayed. If set to "all" (the default), all the inventories
     *        will be displayed.
     * @param const FlightNumber_T& Flight number for which all the departure
     *        dates should be displayed. If set to 0 (the default),
     *        all the flight numbers will be displayed.
     * @return std::string Output string in which the BOM tree is
     *         logged/dumped.
     */
    std::string list (const stdair::AirlineCode_T& iAirlineCode = "all",
                      const stdair::FlightNumber_T& iFlightNumber = 0) const;

    /**
     * Check whether the given flight-date is a valid one.
     *
     * @param const stdair::AirlineCode_T& Airline code of the flight to check.
     * @param const stdair::FlightNumber_T& Flight number of the
     *        flight to check.
     * @param const stdair::Date_T& Departure date of the flight to check.
     * @return bool Whether or not the given flight date is valid.
     */
    bool check (const stdair::AirlineCode_T&, const stdair::FlightNumber_T&,
                const stdair::Date_T& iDepartureDate) const;

    /**
     * Recursively display (dump in the returned string) the objects
     * of the BOM tree.
     *
     * @return std::string Output string in which the BOM tree is
     *        logged/dumped.
     */
    std::string csvDisplay() const;

    /**
     * Recursively display (dump in the returned string) the flight-date
     * corresponding to the parameters given as input.
     *
     * @param const stdair::AirlineCode_T& Airline code of the flight to display.
     * @param const stdair::FlightNumber_T& Flight number of the
     *        flight to display.
     * @param const stdair::Date_T& Departure date of the flight to display.
     * @return std::string Output string in which the BOM tree is
     *        logged/dumped.
     */
    std::string csvDisplay (const stdair::AirlineCode_T&,
                            const stdair::FlightNumber_T&,
                            const stdair::Date_T& iDepartureDate) const;


  private:
    // /////// Construction and Destruction helper methods ///////
    /**
     * Default constructor. It should not be used.
     */
    AIRINV_Master_Service();

    /**
     * Default copy constructor. It should not be used.
     */
    AIRINV_Master_Service (const AIRINV_Master_Service&);

    /**
     * Initialise the STDAIR service (including the log service).
     *
     * A reference on the root of the BOM tree, namely the BomRoot object,
     * is stored within the service context for later use.
     *
     * @param const stdair::BasLogParams& Parameters for the output log stream.
     * @param const stdair::BasDBParams& Parameters for the database access.
     */
    stdair::STDAIR_ServicePtr_T initStdAirService (const stdair::BasLogParams&,
                                                   const stdair::BasDBParams&);

    /**
     * Initialise the STDAIR service (including the log service).
     *
     * A reference on the root of the BOM tree, namely the BomRoot object,
     * is stored within the service context for later use.
     *
     * @param const stdair::BasLogParams& Parameters for the output log stream.
     */
    stdair::STDAIR_ServicePtr_T initStdAirService (const stdair::BasLogParams&);
    
    /**
     * Attach the STDAIR service (holding the log and database services) to
     * the AIRINV_Master_Service.
     *
     * @param stdair::STDAIR_ServicePtr_T Reference on the STDAIR service.
     * @param const bool State whether or not AirInv owns the STDAIR service
     *        resources.
     */
    void addStdAirService (stdair::STDAIR_ServicePtr_T,
                           const bool iOwnStdairService);

    /**
     * Attach the SEVMGR service to the AIRINV_Master_Service.
     *
     * @param SEVMGR::SEVMGR_ServicePtr_T Reference on the SEVMGR service.
     */
    void addSEVMGRService (SEVMGR::SEVMGR_ServicePtr_T ioSEVMGR_ServicePtr,
			   const bool iOwnSEVMGRService);
    
    
    /**
     * Initialise the (AIRINV) service context (i.e., the
     * AIRINV_Master_ServiceContext object).
     */
    void initServiceContext();

    /**
     * Initialise the slave AIRINV_Service.
     *
     * No input file is given. A sample BOM tree may be built by
     * calling the buildSampleBom() method.
     */
    void initSlaveAirinvService();

    /**
     * Finalise.
     */
    void finalise();
    

  private:
    // ///////// Service Context /////////
    /**
     * Airinv context.
     */
    AIRINV_Master_ServiceContext* _airinvMasterServiceContext;
  };
}
#endif // __AIRINV_SVC_AIRINV_MASTER_SERVICE_HPP
