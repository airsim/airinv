// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// Boost Property Tree
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
// Boost ForEach
#include <boost/foreach.hpp>
// AirInvServer
#include <airinv/server/BomPropertyTree.hpp>

namespace bpt = boost::property_tree;

namespace stdair {

  // Loads BomPropertyTree structure from the specified JSON file
  void BomPropertyTree::load (const std::string& iBomTree) {
    // Create an empty property tree object
    bpt::ptree pt;

    // Load the JSON file into the property tree. If reading fails
    // (cannot open file, parse error), an exception is thrown.
    std::istringstream iStr (iBomTree);
    read_json (iStr, pt);

    // Get the filename and store it in the m_file variable.
    // Note that we construct the path to the value by separating
    // the individual keys with dots. If dots appear in the keys,
    // a path type with a different separator can be used.
    // If the debug.filename key is not found, an exception is thrown.
    _airlineCode = pt.get<std::string> ("debug.airline_code");

    // Get the debug level and store it in the m_level variable.
    // This is another version of the get method: if the value is
    // not found, the default value (specified by the second
    // parameter) is returned instead. The type of the value
    // extracted is determined by the type of the second parameter,
    // so we can simply write get(...) instead of get<int>(...).
    _flightNumber = pt.get ("debug.flight_number", 100);

    // Iterate over the debug.modules section and store all found
    // modules in the m_modules set. The get_child() function
    // returns a reference to the child at the specified path; if
    // there is no such child, it throws. Property tree iterators
    // are models of BidirectionalIterator.
    BOOST_FOREACH (bpt::ptree::value_type &v, pt.get_child ("debug.airport_codes")) {
      _airportCodeList.insert (v.second.data());
    }
  }

  // Saves the BomPropertyTree structure to the specified JSON file
  std::string BomPropertyTree::save() {
    std::ostringstream oStr;

    // Create an empty property tree object
    bpt::ptree pt;

    // Put log filename in property tree
    pt.put ("debug.airline_code", _airlineCode);

    // Put debug level in property tree
    pt.put ("debug.flight_number", _flightNumber);

    // Iterate over the modules in the set and put them in the
    // property tree. Note that the put function places the new
    // key at the end of the list of keys. This is fine most of
    // the time. If you want to place an item at some other place
    // (i.e. at the front or somewhere in the middle), this can
    // be achieved using a combination of the insert and put_own
    // functions.
    BOOST_FOREACH (const std::string& name, _airportCodeList) {
      pt.add ("debug.airport_codes.code", name);
    }

    // Write the property tree to the JSON stream.
    write_json (oStr, pt);

    return oStr.str();
  }

}
