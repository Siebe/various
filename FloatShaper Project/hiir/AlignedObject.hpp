/*****************************************************************************

        AlignedObject.hpp
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



#if defined (hiir_AlignedObject_CURRENT_CODEHEADER)
	#error Recursive inclusion of AlignedObject code header.
#endif
#define	hiir_AlignedObject_CURRENT_CODEHEADER

#if ! defined (hiir_AlignedObject_CODEHEADER_INCLUDED)
#define	hiir_AlignedObject_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	<cassert>



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class T>
AlignedObject <T>::AlignedObject ()
:	_obj_zone ()
,	_obj_ptr (0)
{
	// Check, not used
	typedef	int	ChkType1 [(sizeof (long) >= sizeof (void *)) ? 1 : -1];

	// Pointer alignment
	long				ptr = reinterpret_cast <long> (&_obj_zone [0]);
	ptr = (ptr + ALIGNMENT-1) & -ALIGNMENT;
	_obj_ptr = reinterpret_cast <EmbeddedType *> (ptr);

	// Object creation
	new (_obj_ptr) EmbeddedType;
}



template <class T>
AlignedObject <T>::~AlignedObject ()
{
	if (_obj_ptr != 0)
	{
		_obj_ptr->~T ();
	}
}



template <class T>
typename AlignedObject <T>::EmbeddedType &	AlignedObject <T>::use ()
{
	assert (_obj_ptr != 0);

	return (*_obj_ptr);
}



template <class T>
const typename AlignedObject <T>::EmbeddedType &	AlignedObject <T>::use () const
{
	assert (_obj_ptr != 0);

	return (*_obj_ptr);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



#endif	// hiir_AlignedObject_CODEHEADER_INCLUDED

#undef hiir_AlignedObject_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
