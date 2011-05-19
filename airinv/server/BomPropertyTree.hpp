#ifndef __AIRINV_SVR_BOMPROPERTYTREE_HPP
#define __AIRINV_SVR_BOMPROPERTYTREE_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <string>
#include <set>

namespace stdair {

  /** Structure representing a list of airports. */
  struct BomPropertyTree {
    std::string _airlineCode;
    unsigned int _flightNumber;
    std::set<std::string> _airportCodeList;
    void load (const std::string& iBomTree);
    std::string  save();
  };

}
#endif // __AIRINV_SVR_BOMPROPERTYTREE_HPP
