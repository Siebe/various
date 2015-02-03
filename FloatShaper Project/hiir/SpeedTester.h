/*****************************************************************************

        SpeedTester.h
        Copyright (c) 2005 Laurent de Soras

Template parameters:
	- AP: Auxiliary class for processing. AP::TestedObject is the type of the
tested object.

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



#if ! defined (hiir_SpeedTester_HEADER_INCLUDED)
#define	hiir_SpeedTester_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"def.h"
#include	"SpeedTesterBase.h"



namespace hiir
{



template <class AP>
class SpeedTester
:	public SpeedTesterBase
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	typedef	AP	AuxProc;
	typedef	typename AuxProc::TestedObject	TestedObject;

						SpeedTester ();

	void				perform_test (const char *classname_0, const char *funcname_0);



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	enum {			NBR_COEFS	= TestedObject::NBR_COEFS	};

	TestedObject	_tested_object;	// Should be the first object (for alignment)



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						SpeedTester (const SpeedTester &other);
	SpeedTester &	operator = (const SpeedTester &other);
	bool				operator == (const SpeedTester &other);
	bool				operator != (const SpeedTester &other);

};	// class SpeedTester



}	// namespace hiir



#include	"SpeedTester.hpp"



#endif	// hiir_SpeedTester_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
