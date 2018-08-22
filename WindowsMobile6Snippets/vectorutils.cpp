/* (C) 2003 XDA Developers  itsme@xs4all.nl
 *
 * $Header$
 *
 * utility functions for manipulating ByteVectors
 *
 * the 'BV_' functions are used to add/get values to/from bytevectors
 * in a alignment independent way
 *
 * the bufpack/unpack functions are used to easily construct packets, to be sent to
 * the machine, or over the user data channel.
 *
 */
#include "vectorutils.h"

void BV_AppendByte(ByteVector& v, BYTE value)
{
    v.push_back(value);
}

void BV_AppendWord(ByteVector& v, WORD value)
{
    BV_AppendByte(v, (BYTE)(value));
    BV_AppendByte(v, (BYTE)(value>>8));
}

void BV_AppendDword(ByteVector& v, DWORD value)
{
    BV_AppendWord(v, (WORD)(value));
    BV_AppendWord(v, (WORD)(value>>16));
}

void BV_AppendNetWord(ByteVector& v, WORD value)
{
    BV_AppendByte(v, (BYTE)(value>>8));
    BV_AppendByte(v, (BYTE)(value));
}

void BV_AppendNetDword(ByteVector& v, DWORD value)
{
    BV_AppendNetWord(v, (WORD)(value>>16));
    BV_AppendNetWord(v, (WORD)(value));
}

void BV_AppendVector(ByteVector& v1, const ByteVector& v2)
{
    v1.insert(v1.end(), v2.begin(), v2.end());
}
void BV_AppendString(ByteVector& v, const std::string& s)
{
    for (std::string::const_iterator i= s.begin() ; i!=s.end() ; ++i)
        BV_AppendByte(v, (BYTE)(*i));
}
void BV_AppendWString(ByteVector& v, const std::Wstring& s)
{
    for (std::Wstring::const_iterator i= s.begin() ; i!=s.end() ; ++i)
        BV_AppendWord(v, (WORD)(*i));
}

void BV_AppendRange(ByteVector& v, const ByteVector::const_iterator& begin, const ByteVector::const_iterator& end)
{
    v.insert(v.end(), begin, end);
}
ByteVector BV_FromBuffer(BYTE* buf, int len)
{
    return ByteVector(buf, buf+len);
}
ByteVector BV_FromDword(DWORD value)
{
    return ByteVector((BYTE*)&value, 4+(BYTE*)&value);
}
ByteVector BV_FromString(const std::string& str)
{
	if (str.empty())
		return ByteVector();
    return ByteVector((BYTE*)iteratorptr(str.begin()), (BYTE*)(iteratorptr(str.end()-1)+1));
}
ByteVector BV_FromWString(const std::Wstring& wstr)
{
	if (wstr.empty())
		return ByteVector();
    return ByteVector((BYTE*)iteratorptr(wstr.begin()), (BYTE*)(iteratorptr(wstr.end()-1)+1));
}


BYTE BV_GetByte(const ByteVector& bv)
{
    ByteVector::const_iterator i= bv.begin();
    return BV_GetByte(i);
}
BYTE BV_GetByte(ByteVector::const_iterator &i)
{
    return *i++;
}

WORD BV_GetNetWord(const ByteVector& bv)
{
    ByteVector::const_iterator i= bv.begin();
    return BV_GetNetWord(i);
}
WORD BV_GetNetWord(ByteVector::const_iterator &i)
{
    WORD w= BV_GetByte(i)<<8;
    w= w | BV_GetByte(i);

    return w;
}

WORD BV_GetWord(const ByteVector& bv)
{
    ByteVector::const_iterator i= bv.begin();
    return BV_GetWord(i);
}
WORD BV_GetWord(ByteVector::const_iterator &i)
{
    WORD w= BV_GetByte(i);
    w= w | ( BV_GetByte(i)<<8 );

    return w;
}

DWORD BV_GetNetDword(const ByteVector& bv)
{
    ByteVector::const_iterator i= bv.begin();
    return BV_GetNetDword(i);
}
DWORD BV_GetNetDword(ByteVector::const_iterator &i)
{
    DWORD w= BV_GetNetWord(i)<<16;
    w= w | BV_GetNetWord(i);

    return w;
}

DWORD BV_GetDword(const ByteVector& bv)
{
    ByteVector::const_iterator i= bv.begin();
    return BV_GetDword(i);
}
DWORD BV_GetDword(ByteVector::const_iterator &i)
{
    DWORD w= BV_GetWord(i);
    w= w | ( BV_GetWord(i)<<16 );

    return w;
}

std::string* BV_GetString(const ByteVector& bv, int len)
{
    ByteVector::const_iterator i= bv.begin();
    return BV_GetString(i, len);
}
std::string* BV_GetString(ByteVector::const_iterator &i, int len)
{
    std::string *s= new std::string();
    while (len--)
        *s += (char) *i++;

    return s;
}

std::Wstring* BV_GetWString(const ByteVector& bv, int len)
{
    ByteVector::const_iterator i= bv.begin();
    return BV_GetWString(i, len);
}
std::Wstring* BV_GetWString(ByteVector::const_iterator &i, int len)
{
    std::Wstring *s= new std::Wstring();
    while (len--)
        *s += (WCHAR) *i++;

    return s;
}


ByteVector* BV_GetByteVector(ByteVector::const_iterator &i, ByteVector::const_iterator end)
{
    return new ByteVector(i, end);
}

// vaguely based on perl's pack/unpack
//
// 'v'  packs a 16 bit word
// 'V'  packs a 32 bit word
// 'C'  packs a 8 bit character
// 'B'  packs a ByteVector*
// 'S'  packs a string* as a length prefixed character string.
void vbufpack(ByteVector& buf, const char *fmt, va_list ap)
{
    while (*fmt) {
        switch(*fmt++)
        {
        case 'v': BV_AppendWord(buf, va_arg(ap,WORD)); break;
        case 'V': BV_AppendDword(buf, va_arg(ap,DWORD)); break;
        case 'C': BV_AppendByte(buf, va_arg(ap,BYTE)); break;
        case 'B': BV_AppendVector(buf, *va_arg(ap,ByteVector*)); break;
        case 'S': 
              {
                  std::string* strptr= va_arg(ap,std::string*);
                  BV_AppendByte(buf, (BYTE)strptr->size());
                  BV_AppendString(buf, *strptr);
              }
              break;
        default:
//                  debug("ERROR: unknown pack format character\n");
			;
        }
    }
}
void bufpack(ByteVector& buf, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vbufpack(buf, fmt, ap);
    va_end(ap);
}

// 'B' can only be last in the format string
DwordVector bufunpack(const ByteVector& buf, const char *fmt)
{
    DwordVector items;
    ByteVector::const_iterator i= buf.begin();
    while (*fmt) {
        switch(*fmt++)
        {
        case 'v': items.push_back(BV_GetWord(i)); break;
        case 'V': items.push_back(BV_GetDword(i)); break;
        case 'C': items.push_back(BV_GetByte(i)); break;
        case 'S': items.push_back((DWORD)BV_GetString(i, BV_GetByte(i))); break;
        case 'B': 
            {
                items.push_back((DWORD)BV_GetByteVector(i, buf.end()));
                if (fmt[1])
                {
   //                 debug("ERROR: bufunpack: B must be last in format\n");
                    return items;
                }
            }
            break;
        default:
  //                debug("ERROR: unknown pack format character\n");
			;
        }
    }
    return items;
}

