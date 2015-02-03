/*****************************************************************************

        PhaseHalfPiSse.hpp
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



#if defined (hiir_PhaseHalfPiSse_CURRENT_CODEHEADER)
	#error Recursive inclusion of PhaseHalfPiSse code header.
#endif
#define	hiir_PhaseHalfPiSse_CURRENT_CODEHEADER

#if ! defined (hiir_PhaseHalfPiSse_CODEHEADER_INCLUDED)
#define	hiir_PhaseHalfPiSse_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"StageProcSse.h"

#include	<mmintrin.h>

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
PhaseHalfPiSse <NC>::PhaseHalfPiSse ()
:	_filter ()
,	_prev (0)
,	_phase (0)
{
   for (int phase = 0; phase < NBR_PHASES; ++phase)
   {
		for (int i = 0; i < NBR_STAGES + 1; ++i)
		{
			_filter [phase] [i]._coefs = _mm_setzero_ps ();
		}
		if ((NBR_COEFS & 1) != 0)
		{
			const int		pos = (NBR_COEFS ^ 1) & (STAGE_WIDTH - 1);
			_filter [phase] [NBR_STAGES]._coefs.m128_f32 [pos] = 1;
		}
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
void	PhaseHalfPiSse <NC>::set_coefs (const double coef_arr [])
{
	assert (coef_arr != 0);

   for (int phase = 0; phase < NBR_PHASES; ++phase)
   {
		for (int i = 0; i < NBR_COEFS; ++i)
		{
			const int		stage = (i / STAGE_WIDTH) + 1;
			const int		pos = (i ^ 1) & (STAGE_WIDTH - 1);
			_filter [phase] [stage]._coefs.m128_f32 [pos] =
				static_cast <float> (coef_arr [i]);
		}
	}
}



/*
==============================================================================
Name: process_sample
Description:
	 From one input sample, generates two samples with a pi/2 phase shift.
Input parameters:
	- input: The input sample.
Output parameters:
	- out_0: Output sample, ahead.
	- out_1: Output sample, late.
Throws: Nothing
==============================================================================
*/

template <int NC>
hiir_FORCEINLINE void	PhaseHalfPiSse <NC>::process_sample (float &out_0, float &out_1, float input)
{
   assert (&out_0 != 0);
   assert (&out_1 != 0);

	enum {	CURR_CELL	=  NBR_STAGES * sizeof (_filter [0] [0])	};

	StageDataSse *	filter_ptr = &_filter [_phase] [0];
   float				prev = _prev;

	__asm
	{
		mov				edx, filter_ptr
		movss				xmm7, input
		movss				xmm0, prev
		unpcklps			xmm0, xmm7
		shufps			xmm0, [edx + CURR_CELL + 1*16], 044h
		movaps			xmm2, [edx +         0 + 1*16]
	}
	StageProcSse <NBR_STAGES>::process_sample_neg ();
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

	_prev = input;
	_phase = 1 - _phase;
}



/*
==============================================================================
Name: process_block
Description:
   From a block of samples, generates two blocks of samples, with a pi/2
	phase shift between these signals.
	Input and output blocks may overlap, see assert() for details.
Input parameters:
	- in_ptr: Input array, containing nbr_spl samples.
	- nbr_spl: Number of input samples to process, > 0
Output parameters:
	- out_0_ptr: Output sample array (ahead), capacity: nbr_spl samples.
	- out_1_ptr: Output sample array (late), capacity: nbr_spl samples.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	PhaseHalfPiSse <NC>::process_block (float out_0_ptr [], float out_1_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (out_0_ptr != 0);
	assert (out_1_ptr != 0);
	assert (in_ptr != 0);
	assert (out_0_ptr <= in_ptr || out_0_ptr >= in_ptr + nbr_spl);
	assert (out_1_ptr <= in_ptr || out_1_ptr >= in_ptr + nbr_spl);
	assert (out_0_ptr + nbr_spl <= out_1_ptr || out_1_ptr + nbr_spl <= out_0_ptr);
	assert (nbr_spl > 0);

   float          prev = _prev;

	enum {	CURR_CELL	=  NBR_STAGES * sizeof (_filter [0] [0])	};

	StageDataSse *	filter1_ptr = &_filter [    _phase] [0];
	StageDataSse *	filter2_ptr = &_filter [1 - _phase] [0];

	__asm
	{
      push           ebx

		mov				esi, in_ptr
		mov				edi, out_0_ptr
      mov            ecx, out_1_ptr
		mov				eax, nbr_spl
		mov				edx, filter1_ptr
      mov            ebx, filter2_ptr
		lea				esi, [esi + eax*4]
		lea				edi, [edi + eax*4 - 4]
		lea				ecx, [ecx + eax*4 - 4]
		neg				eax
		movss				xmm0, prev

	loop_sample:

		movss				xmm7, [esi + eax*4]
		unpcklps			xmm0, xmm7
		shufps			xmm0, [edx + CURR_CELL + 1*16], 044h
		movaps			xmm2, [edx +         0 + 1*16]
	}
#if defined (_MSC_VER) && ! defined (NDEBUG)
	__asm push			eax
	__asm push			ecx
#endif
	StageProcSse <NBR_STAGES>::process_sample_neg ();
#if defined (_MSC_VER) && ! defined (NDEBUG)
	__asm pop			ecx
	__asm pop			eax
#endif
	__asm
	{
		inc				eax
		movaps			[edx + CURR_CELL + 1*16], xmm0

		shufps			xmm0, xmm0, 0E3h
		movss				[edi + eax*4], xmm0
		shufps			xmm0, xmm0, 0E2h
		movss				[ecx + eax*4], xmm0

      xchg           edx, ebx
		movss				xmm0, xmm7

		jl					loop_sample

		movss				prev, xmm7

      pop            ebx
	}

   _phase = (_phase + nbr_spl) & 1;
   _prev = prev;
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
void	PhaseHalfPiSse <NC>::clear_buffers ()
{
   for (int phase = 0; phase < NBR_PHASES; ++phase)
   {
		for (int i = 0; i < NBR_STAGES + 1; ++i)
		{
			_filter [phase] [i]._mem = _mm_setzero_ps ();
		}
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



#endif	// hiir_PhaseHalfPiSse_CODEHEADER_INCLUDED

#undef hiir_PhaseHalfPiSse_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
