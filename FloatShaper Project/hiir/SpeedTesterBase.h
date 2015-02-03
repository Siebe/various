/*****************************************************************************

        SpeedTesterBase.h
        Copyright (c) 2005 Laurent de Soras

--- Legal stuff ---

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*Tab=3***********************************************************************/



#if ! defined (hiir_SpeedTesterBase_HEADER_INCLUDED)
#define	hiir_SpeedTesterBase_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"Array.h"



namespace hiir
{



class SpeedTesterBase
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {			MAX_BLOCK_LEN	= 256	};
	enum {			MAX_BUF_LEN		= MAX_BLOCK_LEN * 2	};	// For oversampled data
	enum {			MAX_BUFFERS		= 2	};

	typedef	Array <float, MAX_BUF_LEN>	Buffer;
	typedef	Array <Buffer, MAX_BUFFERS>	BufferArr;

						SpeedTesterBase ();
	virtual			~SpeedTesterBase () {}

	BufferArr		_src_arr;
	BufferArr		_dest_arr;
	long				_block_len;				// ]0 ; MAX_BLOCK_LEN]



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						SpeedTesterBase (const SpeedTesterBase &other);
	SpeedTesterBase &
						operator = (const SpeedTesterBase &other);
	bool				operator == (const SpeedTesterBase &other);
	bool				operator != (const SpeedTesterBase &other);

};	// class SpeedTesterBase



}	// namespace hiir



#endif	// hiir_SpeedTesterBase_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
