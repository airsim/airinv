// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STL
#include <cassert>
#include <sstream>
// AIRINV
#include <airinv/bom/FareFamilyStruct.hpp>

namespace AIRINV {

  // //////////////////////////////////////////////////////////////////////
  FareFamilyStruct_T::
  FareFamilyStruct_T (const stdair::FamilyCode_T& iFamilyCode,
                      const stdair::ClassList_String_T& iClasses)
    : _familyCode (iFamilyCode),
      _classes (iClasses) {
  }

  // //////////////////////////////////////////////////////////////////////
  const std::string FareFamilyStruct_T::describe() const {
    std::ostringstream ostr;
    ostr << "        " << _familyCode << " " << _classes << ", ";
    return ostr.str();
  }

}