/*****************************************************************************

        Downsampler2xSse.hpp
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



#if defined (hiir_Downsampler2xSse_CURRENT_CODEHEADER)
	#error Recursive inclusion of Downsampler2xSse code header.
#endif
#define	hiir_Downsampler2xSse_CURRENT_CODEHEADER

#if ! defined (hiir_Downsampler2xSse_CODEHEADER_INCLUDED)
#define	hiir_Downsampler2xSse_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"StageProcSse.h"

#include	<xmmintrin.h>

#include	<cassert>



namespace hiir
{



static const float	Downsampler2xSse_half = 0.5f;



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*
==============================================================================
Name: ctor
Throws: Nothing
==============================================================================
*/

template <int NC>
Downsampler2xSse <NC>::Downsampler2xSse ()
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
void	Downsampler2xSse <NC>::set_coefs (const double coef_arr [])
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
   Downsamples (x2) one pair of samples, to generate one output sample.
Input parameters:
	- in_ptr: pointer on the two samples to decimate
Returns: Samplerate-reduced sample.
Throws: Nothing
==============================================================================
*/

template <int NC>
float	Downsampler2xSse <NC>::process_sample (const float in_ptr [2])
{
	assert (in_ptr != 0);

	enum {	CURR_CELL	=  NBR_STAGES * sizeof (_filter [0])	};

	StageDataSse *	filter_ptr = &_filter [0];
	float				result;

	__asm
	{
		mov				esi, in_ptr
		mov				edx, filter_ptr
		movups			xmm0, [esi]
		shufps			xmm0, [edx + CURR_CELL + 1*16], 044h
		movaps			xmm2, [edx +         0 + 1*16]
	}
	StageProcSse <NBR_STAGES>::process_sample_pos ();
	__asm
	{
		movaps			xmm2, xmm0
		movaps			[edx + CURR_CELL + 1*16], xmm0

		shufps			xmm0, xmm0, 080h
		addps				xmm0, xmm2
		shufps			xmm0, xmm0, 3
		mulss				xmm0, Downsampler2xSse_half
		movss				result, xmm0
	}

	return (result);
}



/*
==============================================================================
Name: process_block
Description:
   Downsamples (x2) a block of samples.
	Input and output blocks may overlap, see assert() for details.
Input parameters:
	- in_ptr: Input array, containing nbr_spl * 2 samples.
	- nbr_spl: Number of samples to output, > 0
Output parameters:
	- out_ptr: Array for the output samples, capacity: nbr_spl samples.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	Downsampler2xSse <NC>::process_block (float out_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (in_ptr != 0);
	assert (out_ptr != 0);
	assert (out_ptr <= in_ptr || out_ptr >= in_ptr + nbr_spl * 2);
	assert (nbr_spl > 0);

	enum {	CURR_CELL	=  NBR_STAGES * sizeof (_filter [0])	};

	StageDataSse *	filter_ptr = &_filter [0];
	static const float	half = 0.5f;

	__asm
	{
		mov				esi, in_ptr
		mov				edi, out_ptr
		mov				eax, nbr_spl
		mov				edx, filter_ptr
		lea				esi, [esi + eax*8]
		lea				edi, [edi + eax*4 - 4]
		neg				eax

	loop_sample:

		movups			xmm0, [esi + eax*8]
		shufps			xmm0, [edx + CURR_CELL + 1*16], 044h
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
		movaps			xmm2, xmm0
		movaps			[edx + CURR_CELL + 1*16], xmm0

		shufps			xmm0, xmm0, 080h
		addps				xmm0, xmm2
		inc				eax
		shufps			xmm0, xmm0, 003h
		mulss				xmm0, Downsampler2xSse_half
		movss				[edi + eax*4], xmm0

		jl					loop_sample
	}
}



/*
==============================================================================
Name: process_sample_split
Description:
   Split (spectrum-wise) in half a pair of samples. The lower part of the
   spectrum is a classic downsampling, equivalent to the output of
   process_sample().
   The higher part is the complementary signal: original filter response
   is flipped from left to right, becoming a high-pass filter with the same
   cutoff frequency. This signal is then critically sampled (decimation by 2),
   flipping the spectrum: Fs/4...Fs/2 becomes Fs/4...0.
Input parameters:
	- in_ptr: pointer on the pair of input samples
Output parameters:
	- low: output sample, lower part of the spectrum (downsampling)
	- high: output sample, higher part of the spectrum.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	Downsampler2xSse <NC>::process_sample_split (float &low, float &high, const float in_ptr [2])
{
	assert (&low != 0);
	assert (&high != 0);
	assert (in_ptr != 0);

	enum {	CURR_CELL	=  NBR_STAGES * sizeof (_filter [0])	};

	StageDataSse *	filter_ptr = &_filter [0];
	float				result;

	__asm
	{
		mov				esi, in_ptr
		mov				edx, filter_ptr
		mov				edi, low
		movups			xmm0, [esi]
		shufps			xmm0, [edx + CURR_CELL + 1*16], 044h
		movaps			xmm2, [edx +         0 + 1*16]
	}
	StageProcSse <NBR_STAGES>::process_sample_pos ();
	__asm
	{
		movaps			xmm2, xmm0
		movaps			[edx + CURR_CELL + 1*16], xmm0

		shufps			xmm0, xmm0, 080h
		mov				ecx, high
		addps				xmm0, xmm2
		shufps			xmm0, xmm0, 003h
		mulss				xmm0, Downsampler2xSse_half
		movss				[edi], xmm0
		shufps			xmm2, xmm2, 003h
		subss				xmm2, xmm0
		movss				[ecx], xmm2
	}

	return (result);
}



/*
==============================================================================
Name: process_block_split
Description:
   Split (spectrum-wise) in half a block of samples. The lower part of the
   spectrum is a classic downsampling, equivalent to the output of
   process_block().
   The higher part is the complementary signal: original filter response
   is flipped from left to right, becoming a high-pass filter with the same
   cutoff frequency. This signal is then critically sampled (decimation by 2),
   flipping the spectrum: Fs/4...Fs/2 becomes Fs/4...0.
	Input and output blocks may overlap, see assert() for details.
Input parameters:
	- in_ptr: Input array, containing nbr_spl * 2 samples.
	- nbr_spl: Number of samples for each output, > 0
Output parameters:
	- out_l_ptr: Array for the output samples, lower part of the spectrum
      (downsampling). Capacity: nbr_spl samples.
	- out_h_ptr: Array for the output samples, higher part of the spectrum.
      Capacity: nbr_spl samples.
Throws: Nothing
==============================================================================
*/

template <int NC>
void	Downsampler2xSse <NC>::process_block_split (float out_l_ptr [], float out_h_ptr [], const float in_ptr [], long nbr_spl)
{
	assert (in_ptr != 0);
	assert (out_l_ptr != 0);
	assert (out_l_ptr <= in_ptr || out_l_ptr >= in_ptr + nbr_spl * 2);
	assert (out_h_ptr != 0);
	assert (out_h_ptr <= in_ptr || out_h_ptr >= in_ptr + nbr_spl * 2);
	assert (out_h_ptr != out_l_ptr);
	assert (nbr_spl > 0);

	enum {	CURR_CELL	=  NBR_STAGES * sizeof (_filter [0])	};

	StageDataSse *	filter_ptr = &_filter [0];
	static const float	half = 0.5f;

	__asm
	{
		mov				esi, in_ptr
		mov				edi, out_l_ptr
		mov				ecx, out_h_ptr
		mov				eax, nbr_spl
		mov				edx, filter_ptr
		lea				esi, [esi + eax*8]
		lea				edi, [edi + eax*4 - 4]
		lea				ecx, [ecx + eax*4 - 4]
		neg				eax

	loop_sample:

		movups			xmm0, [esi + eax*8]
		shufps			xmm0, [edx + CURR_CELL + 1*16], 044h
		movaps			xmm2, [edx +         0 + 1*16]
	}
#if defined (_MSC_VER) && ! defined (NDEBUG)
	__asm push			eax
	__asm push			ecx
#endif
	StageProcSse <NBR_STAGES>::process_sample_pos ();
#if defined (_MSC_VER) && ! defined (NDEBUG)
	__asm pop			ecx
	__asm pop			eax
#endif
	__asm
	{
		movaps			xmm2, xmm0
		movaps			[edx + CURR_CELL + 1*16], xmm0

		shufps			xmm0, xmm0, 080h
		addps				xmm0, xmm2
		inc				eax
		shufps			xmm0, xmm0, 003h
		mulss				xmm0, Downsampler2xSse_half
		movss				[edi + eax*4], xmm0
		shufps			xmm2, xmm2, 003h
		subss				xmm2, xmm0
		movss				[ecx + eax*4], xmm2

		jl					loop_sample
	}
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
void	Downsampler2xSse <NC>::clear_buffers ()
{
	for (int i = 0; i < NBR_STAGES + 1; ++i)
	{
		_filter [i]._mem = _mm_setzero_ps ();
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



#endif	// hiir_Downsampler2xSse_CODEHEADER_INCLUDED

#undef hiir_Downsampler2xSse_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
