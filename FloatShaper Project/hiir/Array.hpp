/*****************************************************************************

        Array.hpp
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



#if defined (hiir_Array_CURRENT_CODEHEADER)
	#error Recursive inclusion of Array code header.
#endif
#define	hiir_Array_CURRENT_CODEHEADER

#if ! defined (hiir_Array_CODEHEADER_INCLUDED)
#define	hiir_Array_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T, long LEN>
Array <T, LEN>::Array (const Array <T, LEN> &other)
{
	for (long pos = 0; pos < LENGTH; ++pos)
	{
		_data [pos] = other._data [pos];
	}
}



template <class T, long LEN>
Array <T, LEN> &	Array <T, LEN>::operator = (const Array <T, LEN> &other)
{
	for (long pos = 0; pos < LENGTH; ++pos)
	{
		_data [pos] = other._data [pos];
	}

	return (*this);
}



template <class T, long LEN>
const typename Array <T, LEN>::Element &	Array <T, LEN>::operator [] (long pos) const
{
	assert (pos >= 0);
	assert (pos < LENGTH);

	return (_data [pos]);
}



template <class T, long LEN>
typename Array <T, LEN>::Element &	Array <T, LEN>::operator [] (long pos)
{
	assert (pos >= 0);
	assert (pos < LENGTH);

	return (_data [pos]);
}



template <class T, long LEN>
long	Array <T, LEN>::size ()
{
	return (LENGTH);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



#endif	// hiir_Array_CODEHEADER_INCLUDED

#undef hiir_Array_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
