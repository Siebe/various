/*****************************************************************************

        ClockCycleCounter.hpp
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



#if defined (hiir_ClockCycleCounter_CURRENT_CODEHEADER)
	#error Recursive inclusion of ClockCycleCounter code header.
#endif
#define	hiir_ClockCycleCounter_CURRENT_CODEHEADER

#if ! defined (hiir_ClockCycleCounter_CODEHEADER_INCLUDED)
#define	hiir_ClockCycleCounter_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



void	ClockCycleCounter::start ()
{
	_start = read_clock_counter ();
}



void	ClockCycleCounter::stop ()
{
	_end = read_clock_counter ();
	const Int64		duration = _end - _start;
	assert (duration >= 0);

	if (_best < 0)
	{
		_best = duration;
	}
	else if (duration < _best)
	{
		_best = duration;
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



ClockCycleCounter::Int64	ClockCycleCounter::read_clock_counter ()
{
	register Int64		clock_cnt;

#if defined (_MSC_VER)

	__asm
	{
		lea				edi, clock_cnt
		rdtsc
		mov				[edi    ], eax
		mov				[edi + 4], edx
	}

#elif defined (__GNUC__) && defined (__i386__)

	__asm__ __volatile__ ("rdtsc" : "=A" (clock_cnt));

#elif (__MWERKS__) && defined (__POWERPC__) 
	
	asm
	{
	loop:
		mftbu			clock_cnt@hiword
		mftb			clock_cnt@loword
		mftbu			r5
		cmpw			clock_cnt@hiword,r5
		bne			loop
	}
	
#endif

	return (clock_cnt);
}



}	// namespace hiir



#endif	// hiir_ClockCycleCounter_CODEHEADER_INCLUDED

#undef hiir_ClockCycleCounter_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
