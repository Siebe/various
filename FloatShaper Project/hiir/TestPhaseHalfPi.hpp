/*****************************************************************************

        TestPhaseHalfPi.hpp
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



#if defined (hiir_TestPhaseHalfPi_CURRENT_CODEHEADER)
	#error Recursive inclusion of TestPhaseHalfPi code header.
#endif
#define	hiir_TestPhaseHalfPi_CURRENT_CODEHEADER

#if ! defined (hiir_TestPhaseHalfPi_CODEHEADER_INCLUDED)
#define	hiir_TestPhaseHalfPi_CODEHEADER_INCLUDED



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"BlockSplitter.h"
#include	"FileOp.h"
#include	"ResultCheck.h"
#include	"SweepingSine.h"

#include	<cstdio>



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



template <class TO>
int	TestPhaseHalfPi <TO>::perform_test (TO &phaser, const double coef_arr [NBR_COEFS], const SweepingSine &ss, const char *type_0, double transition_bw, std::vector <float> &dest_0, std::vector <float> &dest_1)
{
	assert (&phaser != 0);
	assert (coef_arr != 0);
	assert (&ss != 0);
	assert (&type_0 != 0);
	assert (transition_bw > 0);
	assert (transition_bw < 0.5);
	assert (&dest_0 != 0);
	assert (&dest_1 != 0);

	using namespace std;

	printf (
		"Test: PhaseHalfPi, %s implementation, %d coefficients.\n",
		type_0,
		NBR_COEFS
	);

	const long		len = ss.get_len ();
	std::vector <float>	src (len);
	printf ("Generating sweeping sine... ");
	fflush (stdout);
	ss.generate (&src [0]);
	printf ("Done.\n");

	phaser.set_coefs (coef_arr);
	phaser.clear_buffers ();

	dest_0.resize (len, 0);
	dest_1.resize (len, 0);

	printf ("Phasing... ");
	fflush (stdout);
	BlockSplitter	bs (64);
	for (bs.start (len); bs.is_continuing (); bs.set_next_block ())
	{
		const long		b_pos = bs.get_pos ();
		const long		b_len = bs.get_len ();
		phaser.process_block (&dest_0 [b_pos], &dest_1 [b_pos], &src [b_pos], b_len);
	}
	printf ("Done.\n");

	int			ret_val = ResultCheck::check_phase (
		ss,
		transition_bw,
		&dest_0 [0],
		&dest_1 [0]
	);

	char			filename_0 [255+1];
	sprintf (filename_0, "phaser_%02d_%s.raw", TestedType::NBR_COEFS, type_0);
	hiir::FileOp::save_raw_data_16_stereo (filename_0, &dest_0 [0], &dest_1 [0], len, 1);
	printf ("\n");

	return (ret_val);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



#endif	// hiir_TestPhaseHalfPi_CODEHEADER_INCLUDED

#undef hiir_TestPhaseHalfPi_CURRENT_CODEHEADER



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
