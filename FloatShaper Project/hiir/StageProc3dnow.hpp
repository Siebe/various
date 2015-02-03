/*****************************************************************************

        StageProc3dnow.hpp
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



#if defined (hiir_StageProc3dnow_CURRENT_CODEHEADER)
	#error Recursive inclusion of StageProc3dnow code header.
#endif
#define	hiir_StageProc3dnow_CURRENT_CODEHEADER

#if ! defined (hiir_StageProc3dnow_CODEHEADER_INCLUDED)
#define	hiir_StageProc3dnow_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"StageData3dnow.h"

#if defined (_MSC_VER)
	#pragma inline_depth (255)
#endif



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int CUR>
void	StageProc3dnow <CUR>::process_sample_pos ()
{
	StageProc3dnow <CUR - 1>::process_sample_pos ();

	enum {	PREV_CELL	= (CUR - 1) * sizeof (StageData3dnow)	};
	enum {	CURR_CELL	=  CUR      * sizeof (StageData3dnow)	};

	__asm
	{
		movq				mm1, [edx + PREV_CELL + 1*8]
		movq				[edx + PREV_CELL + 1*8], mm0

		pfsub				mm0, [edx + CURR_CELL + 1*8]
		pfmul				mm0, [edx + CURR_CELL + 0*8]
		pfadd				mm0, mm1
	}
}

template <>
void	StageProc3dnow <0>::process_sample_pos ()
{
	// Nothing, stops the recursion
}



template <int CUR>
void	StageProc3dnow <CUR>::process_sample_neg ()
{
	StageProc3dnow <CUR - 1>::process_sample_neg ();

	enum {	PREV_CELL	= (CUR - 1) * sizeof (StageData3dnow)	};
	enum {	CURR_CELL	=  CUR      * sizeof (StageData3dnow)	};

	__asm
	{
		movq				mm1, [edx + PREV_CELL + 1*8]
		movq				[edx + PREV_CELL + 1*8], mm0

		pfadd				mm0, [edx + CURR_CELL + 1*8]
		pfmul				mm0, [edx + CURR_CELL + 0*8]
		pfsub				mm0, mm1
	}
}

template <>
void	StageProc3dnow <0>::process_sample_neg ()
{
	// Nothing, stops the recursion
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



#endif	// hiir_StageProc3dnow_CODEHEADER_INCLUDED

#undef hiir_StageProc3dnow_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
