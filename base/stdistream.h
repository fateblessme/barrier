#include "stdpre.h"
#if defined(HAVE_ISTREAM)
#include <istream>
#else
#include <iostream>
#endif
#include "stdpost.h"

#if defined(_MSC_VER) && _MSC_VER <= 1200
// VC++6 istream has no overloads for __int* types, .NET does
inline
std::istream& operator>>(std::istream& s, SInt8& i)
{ return s >> (signed char&)i; }
inline
std::istream& operator>>(std::istream& s, SInt16& i)
{ return s >> (short&)i; }
inline
std::istream& operator>>(std::istream& s, SInt32& i)
{ return s >> (int&)i; }
inline
std::istream& operator>>(std::istream& s, UInt8& i)
{ return s >> (unsigned char&)i; }
inline
std::istream& operator>>(std::istream& s, UInt16& i)
{ return s >> (unsigned short&)i; }
inline
std::istream& operator>>(std::istream& s, UInt32& i)
{ return s >> (unsigned int&)i; }
#endif
