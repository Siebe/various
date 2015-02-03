/*****************************************************************************

        AlignedObject.h
        Copyright (c) 2005 Laurent de Soras

Template parameters:
	- T: Object to construct. Should have T::T() and T::~T()

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



#if ! defined (hiir_AlignedObject_HEADER_INCLUDED)
#define	hiir_AlignedObject_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace hiir
{



template <class T>
class AlignedObject
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	T	EmbeddedType;

	enum {			ALIGNMENT	= 16	};	// Must be a power of 2

						AlignedObject ();
						~AlignedObject ();

	inline EmbeddedType &
						use ();
	inline const EmbeddedType &
						use () const;



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	typedef	char	Zone [sizeof (EmbeddedType) + ALIGNMENT-1];

	Zone				_obj_zone;
	EmbeddedType *	_obj_ptr;



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						AlignedObject (const AlignedObject &other);
	AlignedObject&	operator = (const AlignedObject &other);
	bool				operator == (const AlignedObject &other);
	bool				operator != (const AlignedObject &other);

};	// class AlignedObject



}	// namespace hiir



#include	"AlignedObject.hpp"



#endif	// hiir_AlignedObject_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
