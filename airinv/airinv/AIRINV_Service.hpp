#ifndef __AIRINV_SVC_AIRINV_SERVICE_HPP
#define __AIRINV_SVC_AIRINV_SERVICE_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// StdAir
#include <stdair/STDAIR_Types.hpp>
#include <stdair/basic/BasLogParams.hpp>

namespace AIRINV {

  // Forward declaration
  class AIRINV_ServiceContext;

  
  /** Interface for the AIRINV Services. */
  class AIRINV_Service {
  public:
    // /////////// Business Methods /////////////
    /** Register a booking (segment sell). */
    void sell (const stdair::PartySize_T&);

    
    // ////////// Constructors and destructors //////////
    /** Constructor.
        <br>The init() method is called; see the corresponding documentation
        for more details.
        <br>Moreover, a reference on an output stream is given, so
        that log outputs can be directed onto that stream.       
        @param const stdair::BasLogParams& Parameters for the output log stream.
        @param AirlineCode_T& Code of the owner airline. */
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
        @param AirlineCode_T& Code of the owner airline. */
    AIRINV_Service (const stdair::AirlineCode_T&);

    /** Destructor. */
    ~AIRINV_Service();

    
  private:
    // /////// Construction and Destruction helper methods ///////
    /** Default constructor. */
    AIRINV_Service ();
    /** Default copy constructor. */
    AIRINV_Service (const AIRINV_Service&);

    /** Initialise the log. */
    void logInit (const stdair::BasLogParams&);

    /** Initialise.
        @param const stdair::AirlineCode_T& Airline code of the inventory
               owner. */
    void init (const stdair::AirlineCode_T&);

    /** Finalise. */
    void finalise ();

    
  private:
    // ///////// Service Context /////////
    /** Airinv context. */
    AIRINV_ServiceContext* _airinvServiceContext;
  };
}
#endif // __AIRINV_SVC_AIRINV_SERVICE_HPP
