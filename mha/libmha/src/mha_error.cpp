// This file is part of the HörTech Open Master Hearing Aid (openMHA)
// Copyright © 2003 2004 2005 2006 2008 2009 2013 2016 HörTech gGmbH
//
// openMHA is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, version 3 of the License.
//
// openMHA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License, version 3 for more details.
//
// You should have received a copy of the GNU Affero General Public License, 
// version 3 along with openMHA.  If not, see <http://www.gnu.org/licenses/>.

/**
   \internal
   \file   mha_error.cpp
   \brief  Implementation of MHA error handling
   
   This file forms a seperate library.
   
*/

#include "mha_error.hh"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "mha_errno.h"
#include "mha_defs.h"

#ifdef _MSC_VER
        #define vsnprintf _vsnprintf
        #define snprintf _snprintf
#endif

/**
   \ingroup mhatoolbox
   \defgroup mhaerror Error handling in the MHA

   Errors are reported to the user via the MHA_Error exception.

*/


/**
   \ingroup mhaerror
   \class MHA_Error
   \brief Error reporting exception class
   
   This class is used for error handling in the MHA. It is used by the
   MHA kernel and by the MHA toolbox library. Please note that
   exceptions should not be used accross ANSI-C interfaces. It is
   necessary to catch exceptions within the library.

   The MHA_Error class holds source file name, line number and an
   error message.
 */

/**\internal
 * Compute number of digits in an unsigned integer.
 */
static unsigned digits(unsigned n)
{
    // The following is the iteration version of the recursion
    // return (n<10) ? 1 : (1+digits(n/10));
    unsigned digits;
    for (digits = 1; n>9; ++digits, n/=10)
        {}
    return digits;
}

/** 
    Create an instance of a MHA_Error.

    \param s_file       source file name (\c __FILE__)
    \param l     source line (\c __LINE__)
    \param fmt  format string for error message (as in printf)
*/
MHA_Error::MHA_Error(const char* s_file,int l,const char* fmt,...)
{
    int len;
    va_list ap;
    va_start(ap,fmt);
    // Determine how much space is needed by msg
#if !defined(_WIN32) || defined(__BORLANDC__)
    // In Linux' glibc (>=2.1) and Borland's libc, vsnprintf returns the number
    // of characters needed for converting the complete format string if the 
    // current buffer's size is 0. This is conforming to ISO C99.
    char * dummy_str = 0;
    len = vsnprintf(dummy_str,0,fmt,ap);
    va_end(ap);
    va_start(ap,fmt);    
#else // which means defined(_WIN32) && !defined(__BORLANDC__)
    // in Microsoft's libc (i.e. MSVCRT.DLL), _vsnprintf returns -1 if there
    // is not enough room for the conversion. This affects VC6, VC7, MinGW,
    // and likely more.
    // As a workaround, write the conversion to File "NUL" once. "NUL" is
    // Microsoft's /dev/null. This works even if "." is on a read-only medium.

    FILE * f = fopen("NUL","wb"); // may fail if there are too many open files.
    if (f != 0) {
        len = vfprintf(f, fmt, ap);
        fclose(f);
    }
    else {
        // Too many open files. We don't care about accuracy in this case:
        len = 65535;
    }
#endif
    std::string module(s_file);
    if( module.find_last_of("/\\") < module.size() )
        module.erase(0,module.find_last_of("/\\")+1);
    if( module.find_first_of(".") < module.size() )
        module.erase(module.find_first_of("."),
                     module.size()-module.find_first_of("."));
    module.insert(0,"(");
    module += ") ";
    len += module.size();
    msg = new char[len+1];
    vsnprintf(msg,len+1,fmt,ap);
    msg[len] = '\0';
    va_end(ap);
    module+=msg;
    snprintf(msg,len+1,"%s",module.c_str());
    msg[len] = '\0';
    len = strlen(s_file) + strlen(msg) + digits(l) + 3;
    // 3 corresponds to the number of extra characters in the following format
    // string (i.e. 2 colons and 1 space).
    longmsg = new char[len+1];
    snprintf(longmsg,len+1,"%s:%d: %s",s_file,l,msg);
    longmsg[len] = '\0';
#ifdef MHA_DEBUG
    mha_debug("%s\n",longmsg);
#endif
}

MHA_Error::~MHA_Error() throw ()
{
    delete [] msg;
    delete [] longmsg;
}

MHA_Error& MHA_Error::operator=(const MHA_Error& p)
{
    if( msg )
        delete [] msg;
    if( longmsg )
        delete [] longmsg;
    int len;
    len = strlen(p.msg);
    msg = new char[len+1];
    memset(msg,0,len+1);
    strncpy(msg,p.msg,len);
    len = strlen(p.longmsg);
    longmsg = new char[len+1];
    memset(longmsg,0,len+1);
    strncpy(longmsg,p.longmsg,len);
    return *this;
}

MHA_Error::MHA_Error(const MHA_Error& p)
{
    int len;
    len = strlen(p.msg);
    msg = new char[len+1];
    memset(msg,0,len+1);
    strncpy(msg,p.msg,len);
    len = strlen(p.longmsg);
    longmsg = new char[len+1];
    memset(longmsg,0,len+1);
    strncpy(longmsg,p.longmsg,len);
}

void mha_debug(const char *fmt,...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr,fmt,ap);
    va_end(ap);
}

#ifdef MHA_DEBUG_MEMORY
#include <fstream>
std::ofstream debug_memory_file("debug_memory.log");
#endif


// Local Variables:
// compile-command: "make -C .."
// c-basic-offset: 4
// indent-tabs-mode: nil
// coding: utf-8-unix
// End:
