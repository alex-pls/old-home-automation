/* (C) 2003 XDA Developers  itsme@xs4all.nl
 *
 * $Header$
 */
#ifndef __VECTORUTILS_H_


#include <windows.h>
#include <vector>
#include <string>
#include <limits>
#include <stdarg.h>

#ifndef __STRINGUTILS_H_
namespace std {
typedef std::basic_string<WCHAR> Wstring;
}
#endif
// used for easy instantiation of arrays
template<class T> std::vector<T> MakeVector(int n, ...)
{
    std::vector<T> v;

    va_list ap;
    va_start(ap, n);
    while (n--)
        v.push_back(va_arg(ap, T));
    va_end(ap);

    return v;
}

typedef std::vector<BYTE> ByteVector;

typedef std::vector<TCHAR> TCharVector;
typedef std::vector<LPCTSTR> LPCTSTRVector;

typedef std::vector<WORD> WordVector;
typedef std::vector<DWORD> DwordVector;

typedef std::vector<int> IntVector;

#define vectorptr(v)  ((v).empty()?NULL:&(v)[0])

// NOTE: the checked stl cannot take iteratorptr(v.end())
#define iteratorptr(v)  (&(*(v)))

void BV_AppendByte(ByteVector& v, BYTE b);
void BV_AppendWord(ByteVector& v, WORD b);
void BV_AppendDword(ByteVector& v, DWORD b);
void BV_AppendNetWord(ByteVector& v, WORD b);
void BV_AppendNetDword(ByteVector& v, DWORD b);
void BV_AppendVector(ByteVector& v1, const ByteVector& v2);
void BV_AppendString(ByteVector& v, const std::string& s);
void BV_AppendWString(ByteVector& v, const std::Wstring& s);
void BV_AppendRange(ByteVector& v, const ByteVector::const_iterator& begin, const ByteVector::const_iterator& end);

ByteVector BV_FromBuffer(BYTE* buf, int len);
ByteVector BV_FromDword(DWORD value);
ByteVector BV_FromString(const std::string& str);
ByteVector BV_FromWString(const std::Wstring& wstr);

BYTE BV_GetByte(ByteVector::const_iterator &i);
WORD BV_GetWord(ByteVector::const_iterator &i);
WORD BV_GetNetWord(ByteVector::const_iterator &i);
DWORD BV_GetDword(ByteVector::const_iterator &i);
DWORD BV_GetNetDword(ByteVector::const_iterator &i);
std::string* BV_GetString(ByteVector::const_iterator &i, int len);
std::Wstring* BV_GetWString(ByteVector::const_iterator &i, int len);

BYTE BV_GetByte(const ByteVector& bv);
WORD BV_GetWord(const ByteVector& bv);
WORD BV_GetNetWord(const ByteVector& bv);
DWORD BV_GetDword(const ByteVector& bv);
DWORD BV_GetNetDword(const ByteVector& bv);
std::string* BV_GetString(const ByteVector& bv, int len);
std::Wstring* BV_GetWString(const ByteVector& bv, int len);

// bufpack/bufunpack are vaguely based on perl's pack/unpack
void vbufpack(ByteVector& buf, const char *fmt, va_list ap);
void bufpack(ByteVector& buf, const char *fmt, ...);
DwordVector bufunpack(const ByteVector& buf, const char *fmt);

#define __VECTORUTILS_H_
#endif
