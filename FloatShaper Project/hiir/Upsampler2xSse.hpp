/*****************************************************************************

        Upsampler2xSse.hpp
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



#if defined (hiir_Upsampler2xSse_CURRENT_CODEHEADER)
	#error Recursive inclusion of Upsampler2xSse code header.
#endif
#define	hiir_Upsampler2xSse_CURRENT_CODEHEADER

#if ! defined (hiir_Upsampler2xSse_CODEHEADER_INCLUDED)
#define	hiir_Upsampler2xSse_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"StageProcSse.h"

#include	<xmmintrin.h>

#include	<cassert>



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: ctor
Throws: Nothing
==============================================================================
*/

template <int NC>
Upsampler2xSse <NC>::Upsampler2xSse ()
:	_filter ()
{
	for (int i = 0; i < NBR_STAGES + 1; ++i)
	{
		_filter [i]._coefs = _mm_setzero_ps ();
	}
	if ((NBR_COEFS & 1) != 0)
	{
		const int		pos = (NBR_COEFS ^ 1) & (STAGE_WIDTH - 1);
		_filter [NBR_STAGES]._coefs.m128_f32 [pos] = 1;
	}

	clear_buffers ();
}



/*
==============================================================================
Name: set_coefs
Description:
   Sets filter coefficients. Generate them with the PolyphaseIir2Designer
   class.
   Call this function before doing any processing.
Input parameters:
	- coef_arr: Array of coefficients. There should be as many coefficients as
      mentioned in the class template parameter.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	Upsampler2xSse <NC>::set_coefs (const double coef_arr [NBR_COEFS])
{
	assert (coef_arr != 0);

	for (int i = 0; i < NBR_COEFS; ++i)
	{
		const int		stage = (i / STAGE_WIDTH) + 1;
		const int		pos = (i ^ 1) & (STAGE_WIDTH - 1);
		_filter [stage]._coefs.m128_f32 [pos] =
			static_cast <float> (coef_arr [i]);
	}
}



/*
==============================================================================
Name: process_sample
Description:
	Upsamples (x2) the input sample, generating two output samples.
Input parameters:
	- input: The input sample.
Output parameters:
	- out_0: First output sample.
	- out_1: Second output sample.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	Upsampler2xSse <NC>::process_sample (float &out_0, float &out_1, float input)
{
	assert (&out_0 != 0);
	assert (&out_1 != 0);

	enum {	CURR_CELL	=  NBR_STAGES * sizeof (_filter [0])	};

	StageDataSse *	filter_ptr = &_filter [0];

	__asm
	{
		mov				edx, filter_ptr
		movss				xmm0, input
		shufps			xmm0, [edx + CURR_CELL + 1*16], 040h
		movaps			xmm2, [edx +         0 + 1*16]
	}
	StageProcSse <NBR_STAGES>::process_sample_pos ();
	__asm
	{
		mov				edi, out_0
		movaps			[edx + CURR_CELL + 1*16], xmm0
		mov				ecx, out_1

		shufps			xmm0, xmm0, 0E3h
		movss				[edi], xmm0
		shufps			xmm0, xmm0, 0E2h
		movss				[ecx], xmm0
	}
	// The latest shufps/movss instruction pairs can be freely inverted
}



/*
==============================================================================
Name: process_block
Description:
	Upsamples (x2) the input sample block.
	Input and output blocks may overlap, see assert() for details.
Input parameters:
	- in_ptr: Input array, containing nbr_spl samples.
	- nbr_spl: Number of input samples to process, > 0
Output parameters:
	- out_0_ptr: Output sample array, capacity: nbr_spl * 2 samples.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	Upsampler2xSse <NC>::process_block (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (out_ptr != 0);
	assert (in_ptr != 0);
	assert (out_ptr >= in_ptr + nbr_spl || in_ptr >= out_ptr + nbr_spl);
	assert (nbr_spl > 0);

	enum {	CURR_CELL	=  NBR_STAGES * sizeof (_filter [0])	};

	StageDataSse *	filter_ptr = &_filter [0];

	__asm
	{
		mov				esi, in_ptr
		mov				edi, out_ptr
		mov				eax, nbr_spl
		mov				edx, filter_ptr
		lea				esi, [esi + eax*4]
		lea				edi, [edi + eax*8 - 8]
		neg				eax

	loop_sample:

		movss				xmm0, [esi + eax*4]
		shufps			xmm0, [edx + CURR_CELL + 1*16], 040h
		movaps			xmm2, [edx +         0 + 1*16]
	}
#if defined (_MSC_VER) && ! defined (NDEBUG)
	__asm push			eax
#endif
	StageProcSse <NBR_STAGES>::process_sample_pos ();
#if defined (_MSC_VER) && ! defined (NDEBUG)
	__asm pop			eax
#endif
	__asm
	{
		inc				eax
		movaps			[edx + CURR_CELL + 1*16], xmm0

		shufps			xmm0, xmm0, 0E3h
		movss				[edi + eax*8    ], xmm0
		shufps			xmm0, xmm0, 0E2h
		movss				[edi + eax*8 + 4], xmm0

		jl					loop_sample
	}
	// The latest shufps/movss instruction pairs can be freely inverted
}



/*
==============================================================================
Name: clear_buffers
Description:
	Clears filter memory, as if it processed silence since an infinite amount
	of time.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	Upsampler2xSse <NC>::clear_buffers ()
{
	for (int i = 0; i < NBR_STAGES + 1; ++i)
	{
		_filter [i]._mem = _mm_setzero_ps ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



#endif	// hiir_Upsampler2xSse_CODEHEADER_INCLUDED

#undef hiir_Upsampler2xSse_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
