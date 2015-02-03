/*****************************************************************************

        fnc.hpp
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



#if defined (hiir_fnc_CURRENT_CODEHEADER)
	#error Recursive inclusion of fnc code header.
#endif
#define	hiir_fnc_CURRENT_CODEHEADER

#if ! defined (hiir_fnc_CODEHEADER_INCLUDED)
#define	hiir_fnc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>
#include	<cmath>

namespace std { }



namespace hiir
{



int	round_int (double x)
{
	using namespace std;

	return (static_cast <int> (floor (x + 0.5)));
}



int	ceil_int (double x)
{
	using namespace std;

	return (static_cast <int> (ceil (x)));
}



template <class T>
T	ipowp (T x, long n)
{
	assert (n >= 0);

	T					z (1);
	while (n != 0)
	{
		if ((n & 1) != 0)
		{
			z *= x;
		}
		n >>= 1;
		x *= x;
	}

	return (z);
}



}	// namespace hiir



#endif	// hiir_fnc_CODEHEADER_INCLUDED

#undef hiir_fnc_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
