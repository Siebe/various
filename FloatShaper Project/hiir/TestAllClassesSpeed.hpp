/*****************************************************************************

        TestAllClassesSpeed.hpp
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



#if defined (hiir_TestAllClassesSpeed_CURRENT_CODEHEADER)
	#error Recursive inclusion of TestAllClassesSpeed code header.
#endif
#define	hiir_TestAllClassesSpeed_CURRENT_CODEHEADER

#if ! defined (hiir_TestAllClassesSpeed_CODEHEADER_INCLUDED)
#define	hiir_TestAllClassesSpeed_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"AlignedObject.h"
#include	"Downsampler2xFpu.h"
#include	"PhaseHalfPiFpu.h"
#include	"SpeedTester.h"
#include	"test_conf.h"
#include	"Upsampler2xFpu.h"

#if defined (HIIR_3DNOW)
#include	"Downsampler2x3dnow.h"
#include	"PhaseHalfPi3dnow.h"
#include	"Upsampler2x3dnow.h"
#endif

#if defined (HIIR_SSE)
#include	"Downsampler2xSse.h"
#include	"PhaseHalfPiSse.h"
#include	"Upsampler2xSse.h"
#endif

#include	<cstdio>

namespace std { }



namespace hiir
{



template <class TO>
class AuxProc11
{
public:
	typedef	TO	TestedObject;
	static hiir_FORCEINLINE void	process_block (hiir::SpeedTesterBase &tester, TO &tested_obj)
	{
		tested_obj.process_block (
			&tester._dest_arr [0] [0],
			&tester._src_arr [0] [0],
			tester._block_len
		);
	}
};

template <class TO>
class AuxProc12
{
public:
	typedef	TO	TestedObject;
	static hiir_FORCEINLINE void	process_block (hiir::SpeedTesterBase &tester, TO &tested_obj)
	{
		tested_obj.process_block (
			&tester._dest_arr [0] [0],
			&tester._dest_arr [1] [0],
			&tester._src_arr [0] [0],
			tester._block_len
		);
	}
};

template <class TO>
class AuxProc12Split
{
public:
	typedef	TO	TestedObject;
	static hiir_FORCEINLINE void	process_block (hiir::SpeedTesterBase &tester, TO &tested_obj)
	{
		tested_obj.process_block_split (
			&tester._dest_arr [0] [0],
			&tester._dest_arr [1] [0],
			&tester._src_arr [0] [0],
			tester._block_len
		);
	}
};



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
void	TestAllClassesSpeed <NC>::perform_test ()
{
	// Downsampler
	{
		SpeedTester <AuxProc11 <Downsampler2xFpu <NBR_COEFS> > >	st;
		st.perform_test ("Downsampler2xFpu", "process_block");
	}
#if defined (HIIR_3DNOW)
	{
		SpeedTester <AuxProc11 <Downsampler2x3dnow <NBR_COEFS> > >	st;
		st.perform_test ("Downsampler2x3dnow", "process_block");
	}
#endif
#if defined (HIIR_SSE)
	{
		typedef	SpeedTester <AuxProc11 <Downsampler2xSse <NBR_COEFS> > >	TestType;
		AlignedObject <TestType>	container;
		TestType &	st = container.use ();
		st.perform_test ("Downsampler2xSse", "process_block");
	}
#endif

	// Downsampler, split
	{
		SpeedTester <AuxProc12Split <Downsampler2xFpu <NBR_COEFS> > >	st;
		st.perform_test ("Downsampler2xFpu", "process_block_split");
	}
#if defined (HIIR_3DNOW)
	{
		SpeedTester <AuxProc12Split <Downsampler2x3dnow <NBR_COEFS> > >	st;
		st.perform_test ("Downsampler2x3dnow", "process_block_split");
	}
#endif
#if defined (HIIR_SSE)
	{
		typedef	SpeedTester <AuxProc12Split <Downsampler2xSse <NBR_COEFS> > >	TestType;
		AlignedObject <TestType>	container;
		TestType &	st = container.use ();
		st.perform_test ("Downsampler2xSse", "process_block_split");
	}
#endif

	// Upsampler
	{
		SpeedTester <AuxProc11 <Upsampler2xFpu <NBR_COEFS> > >	st;
		st.perform_test ("Upsampler2xFpu", "process_block");
	}
#if defined (HIIR_3DNOW)
	{
		SpeedTester <AuxProc11 <Upsampler2x3dnow <NBR_COEFS> > >	st;
		st.perform_test ("Upsampler2x3dnow", "process_block");
	}
#endif
#if defined (HIIR_SSE)
	{
		typedef	SpeedTester <AuxProc11 <Upsampler2xSse <NBR_COEFS> > >	TestType;
		AlignedObject <TestType>	container;
		TestType &	st = container.use ();
		st.perform_test ("Upsampler2xSse", "process_block");
	}
#endif

	// PhaseHalfPi
	{
		SpeedTester <AuxProc12 <PhaseHalfPiFpu <NBR_COEFS> > >	st;
		st.perform_test ("PhaseHalfPiFpu", "process_block");
	}
#if defined (HIIR_3DNOW)
	{
		SpeedTester <AuxProc12 <PhaseHalfPi3dnow <NBR_COEFS> > >	st;
		st.perform_test ("PhaseHalfPi3dnow", "process_block");
	}
#endif
#if defined (HIIR_SSE)
	{
		typedef	SpeedTester <AuxProc12 <PhaseHalfPiSse <NBR_COEFS> > >	TestType;
		hiir::AlignedObject <TestType>	container;
		TestType &	st = container.use ();
		st.perform_test ("PhaseHalfPiSse", "process_block");
	}
#endif

	using namespace std;

	printf ("\n");
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



#endif	// hiir_TestAllClassesSpeed_CODEHEADER_INCLUDED

#undef hiir_TestAllClassesSpeed_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
