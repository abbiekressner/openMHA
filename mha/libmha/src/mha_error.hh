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


#ifndef MHA_ERROR_HH
#define MHA_ERROR_HH

#ifdef __cplusplus

#include <exception>

#ifdef MHA_DEBUG
#define Getmsg(e) ((e).get_longmsg())
#else
#define Getmsg(e) ((e).get_msg())
#endif

class MHA_Error : public std::exception {
public:
    MHA_Error(const char* file,int line,const char* fmt,...);
    MHA_Error(const MHA_Error&);
    MHA_Error& operator=(const MHA_Error&);
    ~MHA_Error() throw ();
    /** Return the error message without source position*/
    const char* get_msg() const {return msg;};
    /** Return the error message with source position*/
    const char* get_longmsg() const {return longmsg;};
    /** overwrite std::execption::what()*/
    const char* what() const throw () {return Getmsg(*this);} ;
private:
    char* msg;
    char* longmsg;
};

/**
   \ingroup mhaerror
   \brief Throw a MHA error with a text message.
   \param x Text message.
 */
#define MHA_ErrorMsg(x) MHA_Error(__FILE__,__LINE__,"%s",x)

/**
   \ingroup mhaerror
   \brief Assertion macro, which throws an MHA_Error.
   \param x Boolean expression which should be true.
 */
#define MHA_assert(x) if(!(x)) throw MHA_Error(__FILE__,__LINE__,"\"%s\" is false.",#x)

/**
   \ingroup mhaerror
   \brief Equality assertion macro, which throws an MHA_Error with the values.
   \param a Numeric expression which can be converted to double (for printing).
   \param b Numeric expression which should be equal to a
 */
#define MHA_assert_equal(a,b) if( a != b ) throw MHA_Error(__FILE__,__LINE__,"\"%s == %s\" is false (%s = %g, %s = %g).",#a,#b,#a,(double)(a),#b,(double)(b))

#endif /* __cplusplus */

/**
   \ingroup mhaerror
   \brief Print an info message (stderr on Linux, OutputDebugString in Windows).
 */
void mha_debug(const char *fmt,...);

#endif

/*
 * Local Variables:
 * compile-command: "make -C .."
 * coding: utf-8-unix
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */
