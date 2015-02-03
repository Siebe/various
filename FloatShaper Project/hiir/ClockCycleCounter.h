/*****************************************************************************

        ClockCycleCounter.h
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



#if ! defined (hiir_ClockCycleCounter_HEADER_INCLUDED)
#define	hiir_ClockCycleCounter_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"def.h"



namespace hiir
{



class ClockCycleCounter
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

#if defined (_MSC_VER)

	typedef	__int64	Int64;

#elif defined (__MWERKS__) || defined (__GNUC__)

	typedef	long long	Int64;

#elif defined (__BEOS__)

	typedef	int64	Int64;

#else

	#error No 64-bit integer type defined for this compiler !

#endif

						ClockCycleCounter ();
	virtual			~ClockCycleCounter () {}

	hiir_FORCEINLINE void
						start ();
	hiir_FORCEINLINE void
						stop ();
	double			get_best_score (long nbr_spl) const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static hiir_FORCEINLINE Int64
						read_clock_counter ();

	Int64				_start;
	Int64				_end;
	Int64				_best;

	static long		_clk_mul;
	static bool		_init_flag;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						ClockCycleCounter (const ClockCycleCounter &other);
	ClockCycleCounter &
						operator = (const ClockCycleCounter &other);
	bool				operator == (const ClockCycleCounter &other);
	bool				operator != (const ClockCycleCounter &other);

};	// class ClockCycleCounter



}	// namespace hiir



#include	"ClockCycleCounter.hpp"



#endif	// hiir_ClockCycleCounter_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
