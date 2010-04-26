#ifndef __AIRINV_SVR_REQUEST_HPP
#define __AIRINV_SVR_REQUEST_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
#include <vector>
// AirInv

namespace AIRINV {

  /** A request received from a client. */
  struct Request {
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
  };

}
#endif // __AIRINV_SVR_REQUEST_HPP
