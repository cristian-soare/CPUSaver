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

#include "special.h"

#include <string>
#include <fstream>

using namespace std;

specialend::specialend() {}

specialend::~specialend()
{
	delete name;
}

// input stream overload for specialend structure
// used to read more easily from the config file
istream& operator>>(istream& in, specialend& se)
{
	se.name = new string();
	in >> *se.name;

	string s;
	in >> s;
	if (s == "Suspend")
		se.et = Suspend;
	else if (s == "SlowDown")
		se.et = SlowDown;
	else if (s == "Kill")
		se.et = Kill;
	else if (s == "Immune")
		se.et = Immune;

	return in;
}
