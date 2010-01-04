#ifndef __AIRINV_SVC_AIRINV_SERVICE_HPP
#define __AIRINV_SVC_AIRINV_SERVICE_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <ostream>
#include <string>
// STDAIR
#include <stdair/STDAIR_Types.hpp>

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
        @param std::ostream& Output log stream (for instance, std::cout)
        @param AirlineCode_T& Code of the owner airline. */
    AIRINV_Service (std::ostream& ioLogStream, const stdair::AirlineCode_T&);

    /** Destructor. */
    ~AIRINV_Service();

    
  private:
    // /////// Construction and Destruction helper methods ///////
    /** Default constructor. */
    AIRINV_Service ();
    /** Default copy constructor. */
    AIRINV_Service (const AIRINV_Service&);

    /** Initialise. */
    void init (std::ostream& ioLogStream, const stdair::AirlineCode_T&);

    /** Finalise. */
    void finalise ();

    
  private:
    // ///////// Service Context /////////
    /** Airinv context. */
    AIRINV_ServiceContext* _airinvServiceContext;
  };
}
#endif // __AIRINV_SVC_AIRINV_SERVICE_HPP
