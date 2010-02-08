#ifndef __AIRINV_SVC_AIRINV_SERVICE_HPP
#define __AIRINV_SVC_AIRINV_SERVICE_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// StdAir
#include <stdair/STDAIR_Types.hpp>

// Forward declaration
namespace stdair {
  class AirlineFeatureSet;
  class STDAIR_Service;
  struct BasLogParams;
  struct BasDBParams;
  class Inventory;
  struct TravelSolutionStruct;
}

namespace AIRINV {

  // Forward declaration
  class AIRINV_ServiceContext;

  
  /** Interface for the AIRINV Services. */
  class AIRINV_Service {
  public:   
    // ////////// Constructors and destructors //////////
    /** Constructor.
        <br>The init() method is called; see the corresponding documentation
        for more details.
        <br>A reference on an output stream is given, so that log
        outputs can be directed onto that stream.
        <br>Moreover, database connection parameters are given, so that a
        session can be created on the corresponding database.
        @param const stdair::BasLogParams& Parameters for the output log stream.
        @param const stdair::BasDBParams& Parameters for the database access.
        @param const stdair::AirlineCode_T& Code of the owner airline. */
    AIRINV_Service (const stdair::BasLogParams&, const stdair::BasDBParams&,
                    const stdair::AirlineCode_T&);

    /** Constructor.
        <br>The init() method is called; see the corresponding documentation
        for more details.
        <br>Moreover, a reference on an output stream is given, so
        that log outputs can be directed onto that stream.       
        @param const stdair::BasLogParams& Parameters for the output log stream.
        @param const stdair::AirlineCode_T& Code of the owner airline. */
    AIRINV_Service (const stdair::BasLogParams&, const stdair::AirlineCode_T&);

    /** Constructor.
        <br>The init() method is called; see the corresponding documentation
        for more details.
        <br>Moreover, as no reference on any output stream is given,
        it is assumed that the StdAir log service has already been
        initialised with the proper log output stream by some other
        methods in the calling chain (for instance, when the AIRINV_Service
        is itself being initialised by another library service such as
        SIMCRS_Service).
        @param stdair::STDAIR_ServicePtr_T Reference on the STDAIR service.
        @param AirlineCode_T& Code of the owner airline. */
    AIRINV_Service (stdair::STDAIR_ServicePtr_T, const stdair::AirlineCode_T&);

    /** Destructor. */
    ~AIRINV_Service();

    
    // /////////// Business Methods /////////////
    /** Register a booking. */
    void sell (const stdair::TravelSolutionStruct&, const stdair::PartySize_T&);

    
  private:
    // /////// Construction and Destruction helper methods ///////
    /** Default constructor. It should not be used. */
    AIRINV_Service ();
    /** Default copy constructor. It should not be used. */
    AIRINV_Service (const AIRINV_Service&);

    /** Initialise the STDAIR service (including the log service).
        <br>A reference on the root of the BOM tree, namely the BomRoot object,
        is stored within the service context for later use.
        @param const stdair::BasLogParams& Parameters for the output log stream.
        @param const stdair::BasDBParams& Parameters for the database access. */
    stdair::STDAIR_ServicePtr_T initStdAirService (const stdair::BasLogParams&,
                                                   const stdair::BasDBParams&);

    /** Initialise the STDAIR service (including the log service).
        <br>A reference on the root of the BOM tree, namely the BomRoot object,
        is stored within the service context for later use.
        @param const stdair::BasLogParams& Parameters for the output log
               stream. */
    stdair::STDAIR_ServicePtr_T initStdAirService (const stdair::BasLogParams&);
    
    /** Attach the STDAIR service (holding the log and database services) to
        the AIRINV_Service.
        @param stdair::STDAIR_ServicePtr_T Reference on the STDAIR service. */
    void addStdAirService (stdair::STDAIR_ServicePtr_T ioSTDAIR_ServicePtr);
    
    /** Initialise the (AIRINV) service context (i.e., the
        AIRINV_ServiceContext object).
        @param const stdair::AirlineCode_T& Code of the owner airline.
        @param stdair::Inventory& Root of the Airline-dedicated BOM tree. */
    void initServiceContext (const stdair::AirlineCode_T&, stdair::Inventory&);

    /** Initialise. */
    void init ();

    /** Finalise. */
    void finalise ();

    
  private:
    // ///////// Service Context /////////
    /** Airinv context. */
    AIRINV_ServiceContext* _airinvServiceContext;
  };
}
#endif // __AIRINV_SVC_AIRINV_SERVICE_HPP
