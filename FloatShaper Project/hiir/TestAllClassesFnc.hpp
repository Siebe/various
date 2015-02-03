/*****************************************************************************

        TestAllClassesFnc.hpp
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



#if defined (hiir_TestAllClassesFnc_CURRENT_CODEHEADER)
	#error Recursive inclusion of TestAllClassesFnc code header.
#endif
#define	hiir_TestAllClassesFnc_CURRENT_CODEHEADER

#if ! defined (hiir_TestAllClassesFnc_CODEHEADER_INCLUDED)
#define	hiir_TestAllClassesFnc_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

// For an unknown reason, if the following files are not included first,
// MSVC 6.0 crashes (Internal Compiler Error)...
#include	"TestDownsampler2x.h"
#include	"TestPhaseHalfPi.h"
#include	"TestUpsampler2x.h"

#include	"AlignedObject.h"
#include	"Downsampler2xFpu.h"
#include	"fnc.h"
#include	"PhaseHalfPiFpu.h"
#include	"PolyphaseIir2Designer.h"
#include	"SweepingSine.h"
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

#include	<vector>

#include	<cstdio>


namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <int NC>
int	TestAllClassesFnc <NC>::perform_test (double transition_bw)
{
	assert (transition_bw > 0);
	assert (transition_bw < 0.5);

	using namespace std;

	int			ret_val = 0;

	// hiir::PolyphaseIir2Designer
	double			coef_arr [NBR_COEFS];
	printf ("Calculating %d filter coefficients... ", NBR_COEFS);
	fflush (stdout);
	PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		&coef_arr [0],
		NBR_COEFS,
		transition_bw
	);
	printf ("Done.\n");
	const double	stopband_at =
		PolyphaseIir2Designer::compute_atten_from_order_tbw (
			NBR_COEFS,
			transition_bw
		);
	printf ("Transition bandwidth: %f\n", transition_bw);
	printf ("Stopband attenuation: %f dB\n", stopband_at);
	printf ("\n");

	const float		fs = 44100;
	const long		len = round_int (fs * 20.0f);
	SweepingSine	ss (fs, 20, 22000, len);

	std::vector <float>	dest_0;
	std::vector <float>	dest_1;

	if (ret_val == 0)
	{
		// hiir::Downsampler2xFpu
		typedef	Downsampler2xFpu <NBR_COEFS>	TestedType;
		TestedType		dspl;
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw, dest_0, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::Upsampler2xFpu
		typedef	Upsampler2xFpu <NBR_COEFS>	TestedType;
		TestedType		dspl;
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw, dest_0, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::PhaseHalfPiFpu
		typedef	PhaseHalfPiFpu <NBR_COEFS>	TestedType;
		TestedType		dspl;
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "fpu", transition_bw, dest_0, dest_1
		);
	}

#if defined (HIIR_3DNOW)
	if (ret_val == 0)
	{
		// hiir::Downsampler2x3dnow
		typedef	Downsampler2x3dnow <NBR_COEFS>	TestedType;
		TestedType		dspl;
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "3dnow", transition_bw, dest_0, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::Upsampler2x3dnow
		typedef	Upsampler2x3dnow <NBR_COEFS>	TestedType;
		TestedType		dspl;
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "3dnow", transition_bw, dest_0, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::PhaseHalfPi3dnow
		typedef	PhaseHalfPi3dnow <NBR_COEFS>	TestedType;
		TestedType		dspl;
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "3dnow", transition_bw, dest_0, dest_1
		);
	}
#endif

#if defined (HIIR_SSE)
	if (ret_val == 0)
	{
		// hiir::Downsampler2xSse
		typedef	Downsampler2xSse <NBR_COEFS>	TestedType;
		AlignedObject <TestedType>	container;
		TestedType &	dspl = container.use ();
		ret_val = TestDownsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw, dest_0, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::Upsampler2xSse
		typedef	Upsampler2xSse <NBR_COEFS>	TestedType;
		AlignedObject <TestedType>	container;
		TestedType &	dspl = container.use ();
		ret_val = TestUpsampler2x <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw, dest_0, stopband_at
		);
	}

	if (ret_val == 0)
	{
		// hiir::PhaseHalfPiSse
		typedef	PhaseHalfPiSse <NBR_COEFS>	TestedType;
		AlignedObject <TestedType>	container;
		TestedType &	dspl = container.use ();
		ret_val = TestPhaseHalfPi <TestedType>::perform_test (
			dspl, coef_arr, ss, "sse", transition_bw, dest_0, dest_1
		);
	}
#endif

	return (ret_val);
}



template <int NC>
void	TestAllClassesFnc <NC>::perform_test_r (int &ret_val, double transition_bw)
{
	assert (&ret_val != 0);
	assert (transition_bw > 0);
	assert (transition_bw < 0.5);

	if (ret_val == 0)
	{
		ret_val = perform_test (transition_bw);
	}
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



#endif	// hiir_TestAllClassesFnc_CODEHEADER_INCLUDED

#undef hiir_TestAllClassesFnc_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
