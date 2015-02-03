/*****************************************************************************

        test_conf.h
        Copyright (c) 2005 Laurent de Soras

Depending on your CPU, define/undef symbols in this file.

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



#if ! defined (hiir_test_conf_HEADER_INCLUDED)
#define	hiir_test_conf_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace hiir
{



// CPU configuration (check and modify this, depending on your CPU)
#define	HIIR_3DNOW
#define	HIIR_SSE



// Removes code that isn't available for compilers/architectures
#if ! defined (_MSC_VER)

#undef	HIIR_3DNOW
#undef	HIIR_SSE

#endif



// Testing options
#undef	HIIR_SAVE_RESULTS
#define	HIIR_LONG_FUNC_TESTS
#define	HIIR_LONG_SPEED_TESTS



}	// namespace hiir



#endif	// hiir_test_conf_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
