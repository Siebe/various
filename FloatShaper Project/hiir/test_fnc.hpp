/*****************************************************************************

        test_fnc.hpp
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



#if defined (hiir_test_fnc_CURRENT_CODEHEADER)
	#error Recursive inclusion of test_fnc code header.
#endif
#define	hiir_test_fnc_CURRENT_CODEHEADER

#if ! defined (hiir_test_fnc_CODEHEADER_INCLUDED)
#define	hiir_test_fnc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace hiir
{



template <typename T>
T	get_min (T a, T b)
{
	return ((a < b) ? a : b);
}



template <typename T>
T	get_max (T a, T b)
{
	return ((a < b) ? b : a);
}



}	// namespace hiir



#endif	// hiir_test_fnc_CODEHEADER_INCLUDED

#undef hiir_test_fnc_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
