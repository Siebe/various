/*****************************************************************************

        Downsampler2x3dnow.h
        Copyright (c) 2005 Laurent de Soras

Downsamples by a factor 2 the input signal, using 3DNow! instruction set.

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



#if ! defined (hiir_Downsampler2x3dnow_HEADER_INCLUDED)
#define	hiir_Downsampler2x3dnow_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"Array.h"
#include	"def.h"
#include	"StageData3dnow.h"



namespace hiir
{



template <int NC>
class Downsampler2x3dnow
{

	// Template parameter check, not used
	typedef	int	ChkTpl1 [(NC > 0) ? 1 : -1];

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	enum {			NBR_COEFS	= NC	};

						Downsampler2x3dnow ();

	void				set_coefs (const double coef_arr []);

	hiir_FORCEINLINE float
						process_sample (const float in_ptr [2]);
	void				process_block (float out_ptr [], const float in_ptr [], long nbr_spl);

	hiir_FORCEINLINE void
						process_sample_split (float &low, float &high, const float in_ptr [2]);
	void				process_block_split (float out_l_ptr [], float out_h_ptr [], const float in_ptr [], long nbr_spl);

	void				clear_buffers ();



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {			STAGE_WIDTH	= 2	};
	enum {			NBR_STAGES = (NBR_COEFS + STAGE_WIDTH - 1) / STAGE_WIDTH	};

	typedef	Array <StageData3dnow, NBR_STAGES + 1>	Filter;	// Stage 0 contains only input memory

	Filter			_filter;		// Should be the first member (thus easier to align)



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	bool				operator == (const Downsampler2x3dnow &other);
	bool				operator != (const Downsampler2x3dnow &other);

};	// class Downsampler2x3dnow



}	// namespace hiir



#include	"Downsampler2x3dnow.hpp"



#endif	// hiir_Downsampler2x3dnow_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
