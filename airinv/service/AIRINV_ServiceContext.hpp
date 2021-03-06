#ifndef __AIRINV_SVC_AIRINVSERVICECONTEXT_HPP
#define __AIRINV_SVC_AIRINVSERVICECONTEXT_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
// Boost
#include <boost/shared_ptr.hpp>
// StdAir
#include <stdair/stdair_service_types.hpp>
#include <stdair/service/ServiceAbstract.hpp>
// RMOL
#include <rmol/RMOL_Types.hpp>
// AIRRAC
#include <airrac/AIRRAC_Types.hpp>
// SEvMgr
#include <sevmgr/SEVMGR_Types.hpp>
// AirInv
#include <airinv/AIRINV_Types.hpp>

namespace AIRINV {

  /**
   * @brief Class holding the context of the AirInv services.
   */
  class AIRINV_ServiceContext : public stdair::ServiceAbstract {
    /**
     * The AIRINV_Service class should be the sole class to get access to
     * ServiceContext content: general users do not want to bother
     * with a context interface.
     */
    friend class AIRINV_Service;
    friend class FacAirinvServiceContext;

  private:
    // ///////////////// Getters ///////////////////
    /**
     * Get the airline code (inventory key).
     */
    stdair::AirlineCode_T getAirlineCode() const {
      return _airlineCode;
    }

    /**
     * Get the pointer on the STDAIR service handler.
     */
    stdair::STDAIR_ServicePtr_T getSTDAIR_ServicePtr() const {
      return _stdairService;
    }

    /**
     * Get the STDAIR service handler.
     */
    stdair::STDAIR_Service& getSTDAIR_Service() const {
      assert (_stdairService != NULL);
      return *_stdairService;
    }

    /**
     * State whether or not AIRINV owns the STDAIR service resources.
     */
    const bool getOwnStdairServiceFlag() const {
      return _ownStdairService;
    }

    /**
     * Get the RMOL service handler.
     */
    RMOL::RMOL_Service& getRMOL_Service() const {
      assert (_rmolService != NULL);
      return *_rmolService;
    }

    /**
     * Get the AIRRAC service handler.
     */
    AIRRAC::AIRRAC_Service& getAIRRAC_Service() const {
      assert (_airracService != NULL);
      return *_airracService;
    }

    /**
     * Get the pointer on the SEvMgr service handler.
     */
    SEVMGR::SEVMGR_ServicePtr_T getSEVMGR_ServicePtr() const {
      return _sevmgrService;
    }

    /**
     * Get the SEvMgr service handler.
     */
    SEVMGR::SEVMGR_Service& getSEVMGR_Service() const {
      assert (_sevmgrService != NULL);
      return *_sevmgrService;
    } 

    /**
     * State whether or not AIRINV owns the SEVMGR service resources.
     */
    const bool getOwnSEVMGRServiceFlag() const {
      return _ownSEVMGRService;
    }

   
  private:
    // ///////////////// Setters ///////////////////
    /**
     * Set the airline code (inventory key).
     */
    void setAirlineCode (const stdair::AirlineCode_T& iAirlineCode) {
      _airlineCode = iAirlineCode;
    }

    /**
     * Set the pointer on the STDAIR service handler.
     */
    void setSTDAIR_Service (stdair::STDAIR_ServicePtr_T ioSTDAIR_ServicePtr,
                            const bool iOwnStdairService) {
      _stdairService = ioSTDAIR_ServicePtr;
      _ownStdairService = iOwnStdairService;
    }
    
    /**
     * Set the pointer on the RMOL service handler.
     */
    void setRMOL_Service (RMOL::RMOL_ServicePtr_T ioRMOL_ServicePtr) {
      _rmolService = ioRMOL_ServicePtr;
    }
    
    /**
     * Set the pointer on the AIRRAC service handler.
     */
    void setAIRRAC_Service (AIRRAC::AIRRAC_ServicePtr_T ioAIRRAC_ServicePtr) {
      _airracService = ioAIRRAC_ServicePtr;
    }  

    /**
     * Set the pointer on the SEVMGR service handler.
     */
    void setSEVMGR_Service (SEVMGR::SEVMGR_ServicePtr_T ioSEVMGR_ServicePtr,
			    const bool iOwnSEVMGRService) {
      _sevmgrService = ioSEVMGR_ServicePtr; 
      _ownSEVMGRService = iOwnSEVMGRService;
    }

  private:
    // //////////////////// Display Methods /////////////////////
    /**
     * Display the short AIRINV_ServiceContext content.
     */
    const std::string shortDisplay() const;
    
    /**
     * Display the full AIRINV_ServiceContext content.
     */
    const std::string display() const;
    
    /**
     * Display of the structure.
     */
    const std::string describe() const;

    
  private:
    /// //////////////// Constructors and destructors /////////////
    /**
     * Main constructor.
     */
    AIRINV_ServiceContext (const stdair::AirlineCode_T&);
    /**
     * Default constructor (not to be used).
     */
    AIRINV_ServiceContext();
    /**
     * Copy constructor (not to be used).
     */
    AIRINV_ServiceContext (const AIRINV_ServiceContext&);

    /**
     * Destructor.
     */
    ~AIRINV_ServiceContext();

    /**
     * Clear the context (cabin capacity, bucket holder).
     */
    void reset();


  private:
    // /////////////// Children ///////////////
    /**
     * Standard Airline (StdAir) Service Handler.
     */
    stdair::STDAIR_ServicePtr_T _stdairService;

    /**
     * State whether or not AIRINV owns the STDAIR service resources.
     */
    bool _ownStdairService;

    /**
     * Standard Airline (RMOL) Service Handler.
     */
    RMOL::RMOL_ServicePtr_T _rmolService;

    /**
     * Standard event queue (SEvMgr) Service Handler.
     */
    SEVMGR::SEVMGR_ServicePtr_T _sevmgrService; 

    /**
     * State whether or not AIRINV owns the SEVMGR service resources.
     */
    bool _ownSEVMGRService;

    /**
     * Standart Airline Yield service Handler.
     */
    AIRRAC::AIRRAC_ServicePtr_T _airracService;

  private:
    // //////////// Attributes ////////////
    /**
     * Airline code (inventory key) for which that instance of AirInv service
     * is operating.
     */
    stdair::AirlineCode_T _airlineCode;
  };

}
#endif // __AIRINV_SVC_AIRINVSERVICECONTEXT_HPP
