/* (C) 2003 XDA Developers  itsme@xs4all.nl
 *
 * $Header$
 */

#ifndef __STRINGUTILS_H_

// make sure everything is compiled with '_MT' defined.
// otherwise this will not link with code that has any of the mfc
// headers included.
//    without _MT this code compiles to non multithreaded templates,
//    with it compiles to templates with threading support.

#include <tchar.h>
#include "stdint.h"

#include <string>
#include <vector>
#include "vectorutils.h"

typedef std::vector<std::string> StringList;

// with uppercase W, wchar_t is 32 bit on most posix systems
// we want 16bit chars
#ifndef __VECTORUTILS_H_
namespace std {
typedef std::basic_string<WCHAR> Wstring;
}
#endif
typedef std::vector<std::Wstring> WStringList;

namespace std {
#ifdef _UNICODE
    typedef Wstring tstring;
#else
    typedef string tstring;
#endif
}
typedef std::vector<std::tstring> TStringList;

// preprocessor helper macro
#define STR$(s) XSTR$(s)
#define XSTR$(s) #s

//-------------------------------------------------------
//  string conversion functions
//
std::string ToString(const WCHAR* tbuf);
std::Wstring ToWString(const WCHAR* tbuf);
std::tstring ToTString(const WCHAR* tbuf);

std::string ToString(const char* buf);
std::Wstring ToWString(const char* buf);
std::tstring ToTString(const char* buf);

std::string  ToString(const std::string& str);
std::Wstring ToWString(const std::string& str);
std::tstring ToTString(const std::string& str);

std::string  ToString(const std::Wstring& wstr);
std::Wstring ToWString(const std::Wstring& str);
std::tstring ToTString(const std::Wstring& str);

/*  -- unfortunately TCHAR is not a type, just a define.
std::string ToString(const TCHAR* tbuf);
std::Wstring ToWString(const TCHAR* tbuf);
std::tstring ToTString(const TCHAR* tbuf);

std::string  ToString(const std::tstring& str);
std::Wstring ToWString(const std::tstring& str);
//std::tstring ToTString(const std::tstring& str);
*/

//-------------------------------------------------------
//  string manipulation functions
//
void chomp(std::string& str);
void chomp(char *str);
bool SplitString(const std::string& str, StringList& strlist, bool bWithEscape= true, const std::string& separator=" \t");
bool SplitString(const std::Wstring& str, WStringList& strlist, bool bWithEscape= true, const std::Wstring& separator=(const WCHAR*)L" \t");
template<typename STRING, typename SEPSTRING>
STRING JoinStringList(const std::vector<STRING>& strlist, const SEPSTRING& sep)
{
    if (strlist.empty())
        return STRING();

    STRING sep2= sep;
    size_t size=sep2.size()*(strlist.size()-1);
    for (typename std::vector<STRING>::const_iterator i=strlist.begin() ; i!=strlist.end() ; ++i)
        size += (*i).size();
    STRING result;
    result.reserve(size);
    //debug("join(%d, '%hs')\n", strlist.size(), sep.c_str());
    for (typename std::vector<STRING>::const_iterator i=strlist.begin() ; i!=strlist.end() ; ++i)
    {
        if (!result.empty())
            result += sep2;
        result += *i;
        //debug("  added %hs\n", (*i).c_str());
    }
    return result;
}
std::string stringformat(const char *fmt, ...);
std::string stringvformat(const char *fmt, va_list ap);

//int stringicompare(const std::string& a, const std::string& b);
template<typename T>
int charicompare(T a,T b)
{
	a=(T)tolower(a);
	b=(T)tolower(b);
	if (a<b) return -1;
	if (a>b) return 1;
	return 0;
}

template<class T>
int stringicompare(const T& a, const T& b)
{
    typename T::const_iterator pa= a.begin();
    typename T::const_iterator pa_end= a.end();
    typename T::const_iterator pb= b.begin();
    typename T::const_iterator pb_end= b.end();
    while (pa!=pa_end && pb!=pb_end && charicompare(*pa, *pb)==0)
    {
        pa++;
        pb++;
    }

	if (pa==pa_end && pb==pb_end)
		return 0;
	if (pa==pa_end)
		return -1;
	if (pb==pb_end)
		return 1;
	return charicompare(*pa, *pb);
}

std::string tolower(const std::string& str);

#define stringptr(v)  ((v).empty()?NULL:&(v)[0])

std::string hexdump(const ByteVector& buf, int nDumpUnitSize=1);
std::string hexdump(const BYTE *buf, int nLength, int nDumpUnitSize=1);
std::string hexdump(int64_t llOffset, const BYTE *buf, int nLength, int nDumpUnitSize=1, int nMaxUnitsPerLine=16);

std::string ascdump(const ByteVector& buf, const std::string& escaped= "", bool bBreakOnEol= false);
std::string asciidump(const BYTE *buf, size_t bytelen);

std::string hash_as_string(const ByteVector& hash);
std::string GuidToString(const GUID *guid);

template<typename T>
size_t stringlength(const T *str)
{
    size_t len=0;
    while (*str++)
        len++;
    return len;
}

template<typename T>
int stringcompare(const T *a, const T *b)
{
    while (*a && *b && *a == *b) 
    {
        a++;
        b++;
    }
    if (*a<*b)
        return -1;
    if (*a>*b)
        return 1;
    return 0;
}

#define __STRINGUTILS_H_
#endif
