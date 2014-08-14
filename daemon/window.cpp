//////////////////////////////////////////////////////////////////////////
//																		//
// CPUSaver (C)2013, Cristian-Valeriu Soare 							//
// author: Cristian-Valeriu Soare 										//
// e-mail: soare_cristian16@yahoo.com 									//
// supervisor: Conf. Dr. Eng. Florin Pop								//
// Politehnica University of Bucharest, 								//
// Faculty of Automatic Control and Computer Science					//
// 																		//
// This file is part of CPUSaver.										//
// 																		//
// CPUSaver is free software: you can redistribute it and/or modify		//
// it under the terms of the GNU General Public License as published by	//
// the Free Software Foundation, either version 3 of the License, or 	//
// (at your option) any later version.									//
// 																		//
// CPUSaver is distributed in the hope that it will be useful,			//
// but WITHOUT ANY WARRANTY; without even the implied warranty of		//
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 		//
// GNU General Public License for more details.							//
// 																		//
// You should have received a copy of the GNU General Public License 	//
// along with CPUSaver.  If not, see <http://www.gnu.org/licenses/>.	//
//																		//
//////////////////////////////////////////////////////////////////////////

#include "window.h"

#include <fstream>

using namespace std;

// constructor
window::window(uint pid, uint pparent)
{
	id = pid;
	parent = pparent;
}

// output stream overload used to make printing easier
ostream& operator<<(ostream& out, window& w)
{
	out << hex << "id: " << w.id << " parent: " << w.parent << ' ';
	return out;
}

// checks if either the id or parent of the window
// is equal to the window id (wid)
bool operator==(window& w, uint wid)
{
	return w.id == wid || w.parent == wid;
}
