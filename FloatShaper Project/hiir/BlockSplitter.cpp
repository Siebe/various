/*****************************************************************************

        BlockSplitter.cpp
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



#if defined (_MSC_VER)
	#pragma warning (1 : 4130) // "'operator' : logical operation on address of string constant"
	#pragma warning (1 : 4223) // "nonstandard extension used : non-lvalue array converted to pointer"
	#pragma warning (1 : 4705) // "statement has no effect"
	#pragma warning (1 : 4706) // "assignment within conditional expression"
	#pragma warning (4 : 4786) // "identifier was truncated to '255' characters in the debug information"
	#pragma warning (4 : 4800) // "forcing value to bool 'true' or 'false' (performance warning)"
	#pragma warning (4 : 4355) // "'this' : used in base member initializer list"
#endif



/*\\\ INCLUDE FILES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#include	"BlockSplitter.h"

#include	<cassert>



namespace hiir
{



/*\\\ PUBLIC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



BlockSplitter::BlockSplitter (long max_block_len)
:	_max_block_len (max_block_len)
,	_total_len (0)
,	_pos (0)
,	_cur_len ()
,	_len_index (0)
{
	assert (max_block_len > 0);
}



void	BlockSplitter::start (long total_len)
{
	assert (total_len > 0);

	_total_len = total_len;
	_pos = 0;
	for (int i = 0; i < NBR_LEN; ++i)
	{
		_cur_len [i] = 1;
	}
	_len_index = 0;
}



bool	BlockSplitter::is_continuing () const
{
	return (_pos < _total_len);
}



void	BlockSplitter::set_next_block ()
{
	_pos += get_len ();

	++ _len_index;
	if (_len_index >= NBR_LEN)
	{
		_len_index = 0;
		bool				cont_flag = true;
		for (int i = 0; i < NBR_LEN && cont_flag; ++i)
		{
			++ _cur_len [i];
			if (_cur_len [i] > _max_block_len)
			{
				_cur_len [i] = 1;
			}
			else
			{
				cont_flag = false;
			}
		}
	}
}



long	BlockSplitter::get_pos () const
{
	return (_pos);
}



long	BlockSplitter::get_len () const
{
	long				len = _cur_len [_len_index];
	if (_pos + len > _total_len)
	{
		len = _total_len - _pos;
	}
	assert (len > 0);

	return (len);
}



/*\\\ PROTECTED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



}	// namespace hiir



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
