#ifndef __AIRINV_SVC_AIRINVSERVICECONTEXT_HPP
#define __AIRINV_SVC_AIRINVSERVICECONTEXT_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// Airinv
#include <airinv/AIRINV_Types.hpp>
#include <airinv/service/ServiceAbstract.hpp>

namespace AIRINV {

  /** Class holding the context of the Airinv services. */
  class AIRINV_ServiceContext : public ServiceAbstract {
    friend class FacAirinvServiceContext;
  public:
    // ///////// Getters //////////
    /** Get the airline code. */
    const AirlineCode_T& getAirlineCode () const {
      return _airlineCode;
    }

    
    // ///////// Setters //////////
    /** Set the airline code. */
    void setAirlineCode (const AirlineCode_T& iAirlineCode) {
      _airlineCode = iAirlineCode;
    }


    // ///////// Display Methods //////////
    /** Display the short AIRINV_ServiceContext content. */
    const std::string shortDisplay() const;
    
    /** Display the full AIRINV_ServiceContext content. */
    const std::string display() const;

    
  private:
    // /////// Construction / initialisation ////////
    /** Constructors. */
    AIRINV_ServiceContext ();
    AIRINV_ServiceContext (const AirlineCode_T& iAirlineCode);
    AIRINV_ServiceContext (const AIRINV_ServiceContext&);

    /** Destructor. */
    ~AIRINV_ServiceContext();

  private:
    // //////////// Attributes //////////////////
    /** Airline code. */
    AirlineCode_T _airlineCode;
  };

}
#endif // __AIRINV_SVC_AIRINVSERVICECONTEXT_HPP
