#ifndef __AIRINV_SVC_AIRINVSERVICECONTEXT_HPP
#define __AIRINV_SVC_AIRINVSERVICECONTEXT_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// STDAIR
#include <stdair/STDAIR_Types.hpp>
#include <stdair/bom/Inventory.hpp>
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
    const stdair::AirlineCode_T& getAirlineCode () const {
      return _airlineCode;
    }

    /** Get the airline inventory. */
    stdair::Inventory& getInventory () {
      return _inventory;
    }
    
    // ///////// Setters //////////
    /** Set the airline code. */
    void setAirlineCode (const stdair::AirlineCode_T& iAirlineCode) {
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
    AIRINV_ServiceContext (const stdair::AirlineCode_T& iAirlineCode,
                           stdair::Inventory& ioInventory);
    AIRINV_ServiceContext (const AIRINV_ServiceContext&);

    /** Destructor. */
    ~AIRINV_ServiceContext();

  private:
    // //////////// Attributes //////////////////
    /** Airline code. */
    stdair::AirlineCode_T _airlineCode;

    /** Airline inventory object. */
    stdair::Inventory& _inventory;
  };

}
#endif // __AIRINV_SVC_AIRINVSERVICECONTEXT_HPP
