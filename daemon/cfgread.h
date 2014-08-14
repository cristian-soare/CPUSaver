//////////////////////////////////////////////////////////////////////////
//																		//
// CPUSaver (C)2013, Cristian-Valeriu Soare 							//
// author: Cristian-Valeriu Soare 										//
// e-mail: soare_cristian16@yahoo.com 									//
// supervisor: Conf. Dr. Ing. Florin Pop								//
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

#ifndef CFGREAD_H
#define CFGREAD_H

#include "types.h"

#include <string>

struct specialend;

template <typename tip>
class LinkedList;

// functions that deal with the configuration file
namespace cfgread
{
	bool readConfig(char*, int&, end_type&, int&, 
		LinkedList<specialend>*&, LinkedList<std::string>*&);
	void writeDefaultConfig(char*, int&, end_type&, int&, 
		LinkedList<specialend>*&, LinkedList<std::string>*&);
}

#endif