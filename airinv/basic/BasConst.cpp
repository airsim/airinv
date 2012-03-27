// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
#include <airinv/basic/BasConst_General.hpp>
#include <airinv/basic/BasConst_Curves.hpp>
#include <airinv/basic/BasConst_AIRINV_Service.hpp>

namespace AIRINV {

  /** Default airline name for the AIRINV_Service. */
  const std::string DEFAULT_AIRLINE_CODE = "BA";

  /** Default pick-up FRAT5 curve for Q-equivalent booking conversion. */
  const FRAT5Curve_T DEFAULT_PICKUP_FRAT5_CURVE =
    DefaultMap::createPickupFRAT5Curve();
  FRAT5Curve_T DefaultMap::createPickupFRAT5Curve() {
    FRAT5Curve_T oCurve;
    oCurve[365] = 1.1; oCurve[63] = 1.2; 
    oCurve[49] = 1.3;  oCurve[35] = 1.4;
    oCurve[23] = 1.7;  oCurve[16] = 2.0; 
    oCurve[10] = 2.3;  oCurve[5]  = 2.44;
    oCurve[1]  = 2.5;  oCurve[0]  = 2.5;
    
    return oCurve;
  };

}
