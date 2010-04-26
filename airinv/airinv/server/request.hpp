#ifndef __AIRINV_SVR_REQUEST_HPP
#define __AIRINV_SVR_REQUEST_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
#include <vector>
// AirInv
#include "header.hpp"

namespace AIRINV {

  /** A request received from a client. */
  struct request {
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::vector<header> headers;
  };

}
#endif // __AIRINV_SVR_REQUEST_HPP
