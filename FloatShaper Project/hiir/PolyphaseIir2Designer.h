/*****************************************************************************

        PolyphaseIir2Designer.h
        Copyright (c) 2005 Laurent de Soras

Compute coefficients for 2-path polyphase IIR filter, half-band filter or
Pi/2 phaser.

                        -2
               a     + z
         N/2-1  2k+1
A0 (z) = Prod  ------------
         k = 0           -2
               1 + a    z
                    2k+1

                             -2
                      a   + z
          -1 (N-1)/2   2k
A1 (z) = z  . Prod   ----------
              k = 0           -2
                      1 + a  z
                           2k

        1
H (z) = - (A0 (z) + A1 (z))
        2

Sum of A0 and A1 gives a low-pass filter.
Difference of A0 and A1 gives the complementary high-pass filter.

For the Pi/2 phaser, product form is (a - z^-2) / (1 - az^-2)
Sum and difference of A0 and A1 have a Pi/2 phase difference.

References:

*	Polyphase Two-Path Filter Designer in Java
	Artur Krukowski
	http://www.cmsa.wmin.ac.uk/~artur/Poly.html

*	Digital Signal Processing Schemes for Efficient Interpolation and Decimation
	Valenzuela and Constantinides
	IEE Proceedings, Dec 1983

*	A Hilbert-Transformer Frequency Shifter for Audio
	Scott Wardle
	http://www.iua.upf.es/dafx98/papers/WAR19.PS

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



#if ! defined (hiir_PolyphaseIir2Designer_HEADER_INCLUDED)
#define	hiir_PolyphaseIir2Designer_HEADER_INCLUDED

#if defined (_MSC_VER)
	#pragma once
	#pragma warning (4 : 4250) // "Inherits via dominance."
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



namespace hiir
{



class PolyphaseIir2Designer
{

/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

public:

	static int		compute_nbr_coefs_from_proto (double attenuation, double transition);
	static double	compute_atten_from_order_tbw (int nbr_coefs, double transition);

	static int		compute_coefs (double coef_arr [], double attenuation, double transition);
	static void		compute_coefs_spec_order_tbw (double coef_arr [], int nbr_coefs, double transition);




/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

protected:



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

	static void		compute_transition_param (double &k, double &q, double transition);
	static int		compute_order (double attenuation, double q);
	static double	compute_atten (double q, int order);
	static double	compute_coef (int index, double k, double q, int order);
	static double	compute_acc_num (double q, int order, int c);
	static double	compute_acc_den (double q, int order, int c);



/*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

						PolyphaseIir2Designer ();
						~PolyphaseIir2Designer ();
						PolyphaseIir2Designer (const PolyphaseIir2Designer &other);
	PolyphaseIir2Designer &
						operator = (const PolyphaseIir2Designer &other);
	bool				operator == (const PolyphaseIir2Designer &other);
	bool				operator != (const PolyphaseIir2Designer &other);

};	// class PolyphaseIir2Designer



}	// namespace hiir



#endif	// hiir_PolyphaseIir2Designer_HEADER_INCLUDED



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
