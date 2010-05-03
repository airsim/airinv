#ifndef __AIRINV_BOM_BOOKINGCLASS_HPP
#define __AIRINV_BOM_BOOKINGCLASS_HPP

// //////////////////////////////////////////////////////////////////////
// Import section
// //////////////////////////////////////////////////////////////////////
// STDAIR
#include <stdair/bom/BookingClass.hpp>

// Forward declarations
namespace stdair {
  class FacBomContent;
}

namespace AIRINV {
  /** Class representing the actual business functions for
      an airline booking class. */
  class BookingClass : public stdair::BookingClass {
    friend class stdair::FacBomContent;

  public:
    /** Definition allowing to retrieve the associated BOM structure type. */
    typedef stdair::BookingClassStructure_T Structure_T;

  private:
    /** Constructors are private so as to force the usage of the Factory
        layer. */
    /** Constructors. */
    BookingClass (const Key_T&, Structure_T&);
    /** Destructor. */
    ~BookingClass();
    /** Default constructors. */
    BookingClass ();
    BookingClass (const BookingClass&);

  };

}
#endif // __AIRINV_BOM_BOOKINGCLASS_HPP
