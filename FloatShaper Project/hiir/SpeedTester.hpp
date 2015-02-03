/*****************************************************************************

        SpeedTester.hpp
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



#if defined (hiir_SpeedTester_CURRENT_CODEHEADER)
	#error Recursive inclusion of SpeedTester code header.
#endif
#define	hiir_SpeedTester_CURRENT_CODEHEADER

#if ! defined (hiir_SpeedTester_CODEHEADER_INCLUDED)
#define	hiir_SpeedTester_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"ClockCycleCounter.h"

#include	<cstdio>



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class AP>
SpeedTester <AP>::SpeedTester ()
:	_tested_object ()
{
	// Nothing
}



template <class AP>
void	SpeedTester <AP>::perform_test (const char *classname_0, const char *funcname_0)
{
	assert (classname_0 != 0);
	assert (funcname_0 != 0);

	using namespace std;

	printf (
		"Speed test, %-18s::%-19s, %2d coef:",
		classname_0,
		funcname_0,
		NBR_COEFS
	);

	// Prepares filter
	const double	transition_bw = 0.1 / NBR_COEFS;
	double			coef_arr [NBR_COEFS];
	hiir::PolyphaseIir2Designer::compute_coefs_spec_order_tbw (
		&coef_arr [0],
		NBR_COEFS,
		transition_bw
	);
	_tested_object.set_coefs (&coef_arr [0]);
	_tested_object.clear_buffers ();

	// Does the speed test
	ClockCycleCounter	clk;
	const long		nbr_tests = 1000;

	for (long test_cnt = 0; test_cnt < nbr_tests; ++test_cnt)
	{
		clk.start ();
		AuxProc::process_block (*this, _tested_object);
		clk.stop ();
	}

	// Displays result
	const double	clk_per_spl = clk.get_best_score (_block_len);
	printf ("%8.3f clk/spl\n", clk_per_spl);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



#endif	// hiir_SpeedTester_CODEHEADER_INCLUDED

#undef hiir_SpeedTester_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
