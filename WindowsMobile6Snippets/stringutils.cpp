/* (C) 2003 XDA Developers  itsme@xs4all.nl
 *
 * $Header: /var/db/cvs/xda-devtools/itsutils/common/stringutils.cpp,v 1.12 2005/06/12 22:52:04 itsme Exp $
 */

#include <windows.h>

// todo: add optional length to , length==-1 : use strlen
// ToString(const chartype* p, size_t length /*=-1*/) { ... conversion code ... }
// to 
#include "stringutils.h"
#ifdef _GCC
extern "C" {
int strcasecmp(const char *, const char *);
size_t strlen(const char*);
}
#endif
//#include "debug.h"

// NOTE:  in the ms version of std::string  'clear' is not implemented,  use 'erase' instead.
#include <string>
#include <algorithm>

#ifdef __GNUC__
#include <iconv.h>
#include <errno.h>
#endif
#ifdef _GCC

// MSVC: the buffer is assumed to contain a 0 at buf[size]
// NOTE: with gcc, you'd have to specify str.size()+1
// msvc does not add the terminating 0 when result is size.
int _snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int n= vsnprintf(buf, size+1, fmt, ap);
    va_end(ap);
    return n;
}
int _snwprintf(wchar_t *wbuf, size_t size, const wchar_t *wfmt, ...)
{
    va_list ap;
    va_start(ap, wfmt);
    int n= vswprintf(wbuf, size+1, wfmt, ap);
    va_end(ap);
    return n;
}
int _vsnprintf(char *buf, size_t size, const char *fmt, va_list ap)
{
    return vsnprintf(buf, size+1, fmt, ap);
}
int _vsnwprintf(wchar_t *wbuf, size_t size, const wchar_t *wfmt, va_list ap) 
{
    return vswprintf(wbuf, size+1, wfmt, ap);
}

#endif

//-------------------------------------------------------------
//  conversion routines

// this may crash on some unicode strings, for example:
// 201a 003a 0048 003b 0038 0039 003e 0038 003d 0034 0037 4d
// 3b00 3f00 3400 3b00 4800 4d00 4b00 3400 4900 3b00 4d00 00
// 0037 0034 004d 0048 004d 003b 0049 003b 003b 003b 0037 4d
// 3b00 4a00 1e00 0020 0000 0000 0000 0000 0000 0000 0000 00

#ifdef __GNUC__
size_t Wcslen(const WCHAR*w)
{
    size_t size=0;
    while (*w++)
        size++;
    return size;
}
#endif
#ifdef WIN32
#define Wcslen wcslen
#endif

//  from WCHAR*
std::string ToString(const WCHAR* wbuf)
{
    std::string str; 
#ifdef WIN32
    // returns length in bytes including NUL
    str.resize(WideCharToMultiByte( CP_UTF8, 0, wbuf, -1, 0, 0, NULL, NULL)); 
    str.resize(WideCharToMultiByte( CP_UTF8, 0, wbuf, -1, (LPSTR)stringptr(str), str.size(), NULL, NULL)-1);
#else
    iconv_t ic= iconv_open("UTF-8", "UTF-16LE");
    if (ic==(iconv_t)(-1))
        return std::string();
    size_t outbytes= Wcslen(wbuf);
    size_t inbytes= outbytes*sizeof(WCHAR);
    str.resize(outbytes);
    char *outbuf= stringptr(str);
    while (inbytes) {
        // note: on osx10.5 /usr/include/iconv.h param 2 is not const !!
        size_t cv= iconv(ic, (char**)&wbuf, &inbytes, &outbuf, &outbytes); 
        if (cv==(size_t)(-1) && errno==E2BIG) {
            size_t outofs= outbuf-stringptr(str);
            str.resize(str.size()+4+inbytes*2);
            outbuf= stringptr(str)+outofs;
            outbytes= str.size()-outofs;
        }
        else if (cv==(size_t)(-1)) {
            return std::string();
        }
    }
    iconv_close(ic);
    str.resize(str.size()-outbytes);
#endif

    return str;
}
std::Wstring ToWString(const WCHAR* wbuf)
{
    return std::Wstring(wbuf);
}

std::tstring ToTString(const WCHAR* wbuf)
{
#ifdef _UNICODE
    return std::tstring(wbuf);
#else
    std::tstring tstr; 
#ifdef WIN32
    tstr.resize(WideCharToMultiByte( CP_UTF8, 0, wbuf, -1, 0, 0, NULL, NULL)); 
    tstr.resize(WideCharToMultiByte( CP_UTF8, 0, wbuf, -1, (LPSTR)stringptr(tstr), tstr.size(), NULL, NULL)-1);
#else
    iconv_t ic= iconv_open("UTF-8", "UTF-16LE");
    if (ic==(iconv_t)(-1))
        return std::string();
    size_t outbytes= Wcslen(wbuf);
    size_t inbytes= outbytes*sizeof(WCHAR);
    tstr.resize(outbytes);
    char *outbuf= stringptr(tstr);
    size_t outofs= 0; 
    while (inbytes) {
        size_t cv= iconv(ic, (char**)&wbuf, &inbytes, &outbuf, &outbytes); 
        if (cv==(size_t)(-1) && errno==E2BIG) {
            outofs= outbuf-stringptr(tstr);
            tstr.resize(tstr.size()+4+inbytes*2);
            outbuf= stringptr(tstr)+outofs;
            outbytes= tstr.size()-outofs;
        }
        else if (cv==(size_t)(-1)) {
            return std::string();
        }
    }
    iconv_close(ic);
    tstr.resize(tstr.size()-outbytes);
#endif

    return tstr;
#endif
}
/*
//  from TCHAR*
std::string ToString(const TCHAR* tbuf)
{
#ifdef _UNICODE
    std::string str; 
    str.resize(_tcslen(tbuf)); 
    _snprintf(stringptr(str), str.size(), "%ls", tbuf);

    return str;
#else
    return std::Wstring(tbuf);
#endif
}
std::Wstring ToWString(const TCHAR* tbuf)
{
#ifdef _UNICODE
    return std::Wstring(tbuf);
#else
    std::string str; 
    str.resize(_tcslen(tbuf)); 
    _snprintf(stringptr(str), str.size(), "%hs", tbuf);

    return str;
#endif
}
std::tstring ToTString(const TCHAR* tbuf)
{
    return std::tstring(tbuf);
}
// from tstring
std::string ToString(const std::tstring& tstr)
{
    return ToString((const TCHAR*)tstr.c_str());
}
std::Wstring ToWString(const std::tstring& tstr)
{
    return ToWString((const TCHAR*)tstr.c_str());
}
*/

// from char*
std::string ToString(const char* buf)
{
    return std::string(buf);
}

std::Wstring ToWString(const char* buf)
{
    std::Wstring wstr; 
#ifdef WIN32
    // returns nr of WCHARs incl NUL,  0 for error
    wstr.resize(MultiByteToWideChar( CP_UTF8, 0, buf, -1, NULL, 0 )); 
    size_t len= MultiByteToWideChar( CP_UTF8, 0, buf, -1, stringptr(wstr), wstr.size() );
    if (len>0)
        wstr.resize(len-1);
    else
        wstr.resize(0);
#else
    iconv_t ic= iconv_open("UTF-16LE", "UTF-8");
    if (ic==(iconv_t)(-1))
        return std::Wstring();
    size_t inbytes= strlen(buf);
    size_t outbytes= inbytes*sizeof(WCHAR);
    wstr.resize(inbytes);
    WCHAR *outbuf= stringptr(wstr);
    while (inbytes) {
        size_t cv= iconv(ic, (char**)&buf, &inbytes, (char**)&outbuf, &outbytes); 
        if (cv==(size_t)(-1) && errno==E2BIG) {
            size_t outofs= outbuf-stringptr(wstr);
            wstr.resize(wstr.size()+4+inbytes);
            outbuf= stringptr(wstr)+outofs;
            outbytes= (wstr.size()-outofs)*sizeof(WCHAR);
        }
        else if (cv==(size_t)(-1)) {
            return std::Wstring();
        }
    }
    iconv_close(ic);
    wstr.resize(wstr.size()-outbytes/sizeof(WCHAR));
#endif

    return wstr;
}
std::tstring ToTString(const char* buf)
{
#ifdef _UNICODE
    std::tstring tstr; 
#ifdef WIN32
    // returns nr of WCHARs incl NUL,  0 for error
    tstr.resize(MultiByteToWideChar( CP_UTF8, 0, buf, -1, NULL, 0 )); 
    size_t len= MultiByteToWideChar( CP_UTF8, 0, buf, -1, stringptr(tstr), tstr.size() );
    if (len>0)
        tstr.resize(len-1);
    else
        tstr.resize(0);
#else
    iconv_t ic= iconv_open("UTF-16LE", "UTF-8");
    if (ic==(iconv_t)(-1))
        return std::tstring();
    size_t inbytes= strlen(buf);
    size_t outbytes= inbytes*sizeof(WCHAR);
    tstr.resize(inbytes);
    WCHAR *outbuf= stringptr(tstr);
    while (inbytes) {
        size_t cv= iconv(ic, (char**)&buf, &inbytes, (char**)&outbuf, &outbytes); 
        if (cv==(size_t)(-1) && errno==E2BIG) {
            size_t outofs= outbuf-stringptr(tstr);
            tstr.resize(tstr.size()+4+inbytes);
            outbuf= stringptr(tstr)+outofs;
            outbytes= (str.size()-outofs)*sizeof(WCHAR);
        }
        else if (cv==(size_t)(-1)) {
            return std::tstring();
        }
    }
    iconv_close(ic);
    tstr.resize(tstr.size()-outbytes/sizeof(WCHAR));
#endif

    return tstr;
#else
    return std::tstring(buf);
#endif
}

// from string
std::string ToString(const std::string& str)
{
    return str;
}
std::Wstring ToWString(const std::string& str)
{
    return ToWString((const char*)str.c_str());
}
std::tstring ToTString(const std::string& str)
{
    return ToTString((const char*)str.c_str());
}

// from Wstring
std::string ToString(const std::Wstring& wstr)
{
    return ToString((const WCHAR*)wstr.c_str());
}
std::Wstring ToWString(const std::Wstring& wstr)
{
    return wstr;
}
std::tstring ToTString(const std::Wstring& wstr)
{
    return ToTString((const WCHAR*)wstr.c_str());
}




// removes cr, lf, whitespace from end of string
void chomp(char *str)
{
    char *p= str+strlen(str)-1;

    while (p>=str && isspace(*p))
    {
        *p--= 0;
    }
}
void chomp(std::string& str)
{
    while (!str.empty() && isspace(str.at(str.size()-1)))
    {
        str.resize(str.size()-1);
    }
}

// splits a list of blank separated optionally quoted parameters
// in a list of strings
bool SplitString(const std::string& str, StringList& strlist, bool bWithEscape/*= true*/, const std::string& separator/*= " \t"*/)
{
    std::string::const_iterator pos= str.begin();
    bool bQuoted= false;
    bool bEscaped= false;
    std::string current;

    while (pos != str.end())
    {
        if (bEscaped)
        {
            current += *pos++;
            bEscaped= false;
        }
        else if (bQuoted)
        {
            switch(*pos)
            {
            case '"':
                bQuoted= false;
                strlist.push_back(std::string(current));
                //debug("added %hs\n", current.c_str());
                current.erase();
                ++pos;
                break;
            case '\\':
                if (bWithEscape)
                {
                    bEscaped= true;
                    ++pos;  // skip escaped char
                }
                // else fall through
            default:
                current += *pos++;
            }
        }
        else    // not escaped, and not quoted
        {
            if (separator.find(*pos)!=separator.npos)
            {
                ++pos;
                if (!current.empty())
                {
                    strlist.push_back(std::string(current));
                    //debug("added %hs\n", current.c_str());
                    current.erase();
                }
            }
            else switch(*pos)
            {
            case '"':
                bQuoted=true;
                ++pos;
                break;
            case '\\':
                if (bWithEscape) {
                    bEscaped= true;
                    ++pos;  // skip escaped char
                    break;
                }
                // else fall through
            default:
                current += *pos++;
            }
        }
    }
    if (!current.empty())
    {
        strlist.push_back(std::string(current));
        //debug("added %hs\n", current.c_str());
        current.erase();
    }
    if (bQuoted || bEscaped)
    {
        //debug("ERROR: Unterminated commandline\n");
        return false;
    }

    return true;
}
bool SplitString(const std::Wstring& str, WStringList& strlist, bool bWithEscape/*= true*/, const std::Wstring& separator/*= " \t"*/)
{
    std::Wstring::const_iterator pos= str.begin();
    bool bQuoted= false;
    bool bEscaped= false;
    std::Wstring current;

    while (pos != str.end())
    {
        if (bEscaped)
        {
            current += *pos++;
            bEscaped= false;
        }
        else if (bQuoted)
        {
            switch(*pos)
            {
            case '"':
                bQuoted= false;
                strlist.push_back(std::Wstring(current));
                //debug("added %hs\n", current.c_str());
                current.erase();
                ++pos;
                break;
            case '\\':
                if (bWithEscape)
                {
                    bEscaped= true;
                    ++pos;  // skip escaped char
                }
                // else fall through
            default:
                current += *pos++;
            }
        }
        else    // not escaped, and not quoted
        {
            if (separator.find(*pos)!=separator.npos)
            {
                ++pos;
                if (!current.empty())
                {
                    strlist.push_back(std::Wstring(current));
                    //debug("added %hs\n", current.c_str());
                    current.erase();
                }
            }
            else switch(*pos)
            {
            case '"':
                bQuoted=true;
                ++pos;
                break;
            case '\\':
                if (bWithEscape) {
                    bEscaped= true;
                    ++pos;  // skip escaped char
                    break;
                }
                // else fall through
            default:
                current += *pos++;
            }
        }
    }
    if (!current.empty())
    {
        strlist.push_back(std::Wstring(current));
        //debug("added %hs\n", current.c_str());
        current.erase();
    }
    if (bQuoted || bEscaped)
    {
        //debug("ERROR: Unterminated commandline\n");
        return false;
    }

    return true;
}
#if 0
// joins strings from a list to form a single string
std::string JoinStringList(const StringList& strlist, const std::string& sep)
{
    std::string result;
    //debug("join(%d, '%hs')\n", strlist.size(), sep.c_str());
    for (StringList::const_iterator i=strlist.begin() ; i!=strlist.end() ; ++i)
    {
        if (!result.empty())
            result += sep;
        result += *i;
        //debug("  added %hs\n", (*i).c_str());
    }
    return result;
}
#endif

// sprintf like string formatting
std::string stringformat(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    std::string str= stringvformat(fmt, ap);
    va_end(ap);
    return str;
}

std::string stringvformat(const char *fmt, va_list ap)
{
#ifdef _WIN32_WCE
// unfortunately in CE there is now way of determining the resulting
// length of a formatted string.

    int desired_length= 1024;
#elif defined(WIN32)
    // or use _scprintf to calculate result length
    // on ANSI-C compliant platforms snprintf will always return the desired length
    int desired_length= _vsnprintf(NULL, 0, fmt, ap);
#else
    int desired_length= 1024;
#endif

    std::string str; str.resize(desired_length);

    int printedlength= _vsnprintf(stringptr(str), str.size(), fmt, ap);

    // '-1' means the buffer was too small.
    if (printedlength!=-1)
        str.resize(printedlength);
    return str;
}

std::string tolower(const std::string& str)
{
    std::string lstr; 
    lstr.reserve(str.size());
    for (std::string::const_iterator i=str.begin() ; i!=str.end() ; ++i)
        lstr += (char)tolower(*i);
    return lstr;
}
#if 0
int stringicompare(const std::string& a, const std::string& b)
{
    // todo: make this platform independent
#ifdef WIN32
    return _stricmp(a.c_str(), b.c_str());
#else
    return strcasecmp(a.c_str(), b.c_str());
#endif
}
#endif

static int nyble2hexchar(int n)
{
    return n<0?'?':n<10?('0'+n) : n<16 ? (n-10+'a') : '?';
}
static void byte2hexchars(BYTE b, char *p)
{
    p[0]= nyble2hexchar((b>>4)&0xf);
    p[1]= nyble2hexchar(b&0xf);
}
void word2hexchars(WORD w, char *p)
{
    byte2hexchars((w>>8)&0xff, p);  p+=2;
    byte2hexchars(w&0xff, p);  p+=2;
}
void dword2hexchars(DWORD d, char *p)
{
    word2hexchars((d>>16)&0xffff, p);  p+=4;
    word2hexchars(d&0xffff, p);  p+=4;
}
//----------------------------------------------------------------------------
// utility functions for 'hexdump'
void hexdumpbytes(std::string &str, const BYTE *buf, int nLength)
{
    str.resize(str.size()+nLength*3);
    char *p= &str[str.size()-nLength*3];
    while(nLength--)
    {
        *p++ = ' ';
        byte2hexchars(*buf++, p); p+=2;
    }
}
std::string hash_as_string(const ByteVector& hash)
{
    std::string str;
    str.resize(hash.size()*2);
    char *p= &str[str.size()-hash.size()*2];
    for (size_t i=0 ; i<hash.size() ; ++i)
    {
        byte2hexchars(hash[i], p); p+=2;
    }
    return str;
}
void hexdumpwords(std::string &str, const WORD *buf, int nLength)
{
    str.resize(nLength*5);
    char *p= &str[str.size()-nLength*5];
    while(nLength--)
    {
        *p++ = ' ';
        word2hexchars(*buf++, p); p+=4;
    }
}
void hexdumpdwords(std::string &str, const DWORD *buf, int nLength)
{
    str.resize(nLength*9);
    char *p= &str[str.size()-nLength*9];
    while(nLength--)
    {
        *p++ = ' ';
        dword2hexchars(*buf++, p); p+=8;
    }
}
void dumpascii(std::string &str, const BYTE *buf, int nLength)
{
    while(nLength--)
    {
        BYTE c= *buf++;
        str += (c>=' ' && c<='~')?c:'.';
    }
}
std::string asciidump(const BYTE *buf, size_t bytelen)
{
    std::string str;
    str.reserve(bytelen);
    while(bytelen--)
    {
        BYTE c= *buf++;
        str += (c>=' ' && c<='~')?c:'.';
    }

    return str;
}
void writespaces(std::string &str, int n)
{
    while(n--)
    {
        str += ' ';
    }
}

//----------------------------------------------------------------------------
// various ways of generating a hexdump of binary data.

// dumps bytes, shorts, longs from a bytevector.
// in one long line, without offsets printed
std::string hexdump(const ByteVector& buf, int nDumpUnitSize /*=1*/)
{
    return hexdump(vectorptr(buf), buf.size()/nDumpUnitSize, nDumpUnitSize);
}

// dumps bytes, shorts or longs from a BYTE ptr + length, in one long line.
std::string hexdump(const BYTE *buf, int nLength, int nDumpUnitSize /*=1*/)
{
    if (nLength<0)
        return "hexdump-ERROR";
    if (nLength>0 && buf==NULL)
        return "(null)";
    if (nLength==0)
        return "";

    int nCharsInResult= nLength*(nDumpUnitSize==1?3:
                                 nDumpUnitSize==2?5:
                                 nDumpUnitSize==4?9:
                                 9);
    std::string line;

    line.reserve(nCharsInResult);

    switch(nDumpUnitSize)
    {
        case 1: hexdumpbytes(line, buf, nLength); break;
        case 2: hexdumpwords(line, (const WORD*)buf, nLength); break;
        case 4: hexdumpdwords(line, (const DWORD*)buf, nLength); break;
    }
    return line;
}

// dumps data with a limited nr of items per line, followed by ascii data, prefixed with offsets.
std::string hexdump(int64_t llOffset, const BYTE *buf, int nLength, int nDumpUnitSize /*=1*/, int nMaxUnitsPerLine /*=16*/)
{
    int nCharsInLine= 20+nMaxUnitsPerLine*(nDumpUnitSize==1?4:nDumpUnitSize==2?6:nDumpUnitSize==4?10:10);
    int nCharsInResult= nCharsInLine*(nLength/nDumpUnitSize/nMaxUnitsPerLine+1);

    std::string all; all.reserve(nCharsInResult);

    while(nLength>0)
    {
        std::string line;
        // is rounding correct here?
        int nUnitsInLine= nLength/nDumpUnitSize;
        if (nMaxUnitsPerLine<nUnitsInLine)
            nUnitsInLine= nMaxUnitsPerLine;
        
        line.reserve(nCharsInLine);

        if (llOffset>>32)
            line +=stringformat("%x", static_cast<DWORD>(llOffset>>32));
        line += stringformat("%08x", static_cast<DWORD>(llOffset));

        switch(nDumpUnitSize)
        {
            case 1: hexdumpbytes(line, buf, nUnitsInLine); break;
            case 2: hexdumpwords(line, (const WORD*)buf, nUnitsInLine); break;
            case 4: hexdumpdwords(line, (const DWORD*)buf, nUnitsInLine); break;
        }

        if (nUnitsInLine<nMaxUnitsPerLine)
            writespaces(line, (nMaxUnitsPerLine-nUnitsInLine)*(2*nDumpUnitSize+1));

        line += "  ";

        dumpascii(line, buf, nUnitsInLine*nDumpUnitSize);
        if (nUnitsInLine<nMaxUnitsPerLine)
            writespaces(line, nMaxUnitsPerLine-nUnitsInLine);

        all += line;
        all += "\n";

        nLength -= nUnitsInLine*nDumpUnitSize;
        llOffset += nUnitsInLine*nDumpUnitSize;
        buf += nUnitsInLine*nDumpUnitSize;
    }

    return all;
}

// todo: also recognize unicode strings
std::string ascdump(const ByteVector& buf, const std::string& escaped, bool bBreakOnEol/*= false*/)
{
    std::string result;
    bool bQuoted= false;
    bool bLastWasEolChar= false;

    for (size_t i=0 ; i<buf.size() ; i++)
    {
        bool bNeedsEscape= escaped.find((char)buf[i])!=escaped.npos 
            || buf[i]=='\"' 
            || buf[i]=='\\';

        bool bThisIsEolChar= (buf[i]==0x0a || buf[i]==0x0d || buf[i]==0);

        if (bLastWasEolChar && !bThisIsEolChar && bBreakOnEol) {
            if (bQuoted)
                result += "\"";
            bQuoted= false;
            result += "\n";
        }

        if (isprint(buf[i]) || bNeedsEscape) {
            if (!bQuoted) {
                if (!result.empty() && *result.rbegin()!='\n')
                    result += ",";
                result += "\"";
                bQuoted= true;
            }
            if (bNeedsEscape) {
                std::string escapecode;
                switch(buf[i]) {
                    case '\n': escapecode= "\\n"; break;
                    case '\r': escapecode= "\\r"; break;
                    case '\t': escapecode= "\\t"; break;
                    case '\"': escapecode= "\\\""; break;
                    case '\\': escapecode= "\\\\"; break;
                    default:
                       escapecode= stringformat("\\x%02x", buf[i]);
                }
                result += escapecode;
            }
            else {
                result += (char) buf[i];
            }
        }
        else {
            if (bQuoted) {
                result += "\"";
                bQuoted= false;
            }
            if (!result.empty())
                result += ",";
            result += stringformat("%02x", buf[i]);
        }
        bLastWasEolChar= bThisIsEolChar;
    }

    if (bQuoted) {
        result += "\"";
        bQuoted= false;
    }

    return result;
}

std::string GuidToString(const GUID *guid)
{
    return stringformat("{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
            guid->Data1, guid->Data2, guid->Data3, guid->Data4[0], guid->Data4[1],
            guid->Data4[2] , guid->Data4[3] , guid->Data4[4],
            guid->Data4[5] , guid->Data4[6] , guid->Data4[7]);
}
