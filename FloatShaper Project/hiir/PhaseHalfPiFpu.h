/*****************************************************************************

        PhaseHalfPiFpu.h
        Copyright (c) 2005 Laurent de Soras

From the input signal, generates two signals with a pi/2 phase shift, using
FPU.

Template parameters:
	- NC: number of coefficients, > 0

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



#if ! defined (hiir_PhaseHalfPiFpu_HEADER_INCLUDED)
#define	hiir_PhaseHalfPiFpu_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"Array.h"
#include	"def.h"



namespace hiir
{



template <int NC>
class PhaseHalfPiFpu
{

	// Template parameter check, not used
	typedef	int	ChkTpl1 [(NC > 0) ? 1 : -1];

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {			NBR_COEFS	= NC	};

						PhaseHalfPiFpu ();

	void				set_coefs (const double coef_arr []);

	hiir_FORCEINLINE void
						process_sample (float &out_0, float &out_1, float input);
	void				process_block (float out_0_ptr [], float out_1_ptr [], const float in_ptr [], long nbr_spl);

	void				clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {			NBR_PHASES	= 2	};

	typedef	Array <float, NBR_COEFS>	HyperGluar;

	class Memory
	{
	public:
		HyperGluar		_x;
		HyperGluar		_y;
	};

	typedef	Array <Memory, NBR_PHASES>	MemoryBiPhase;

	HyperGluar		_coef;
	MemoryBiPhase	_mem;
	float				_prev;
	int				_phase;			// 0 or 1



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool				operator == (const PhaseHalfPiFpu &other);
	bool				operator != (const PhaseHalfPiFpu &other);

};	// class PhaseHalfPiFpu



}	// namespace hiir



#include	"PhaseHalfPiFpu.hpp"



#endif	// hiir_PhaseHalfPiFpu_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
