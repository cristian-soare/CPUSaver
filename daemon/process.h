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

#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <iosfwd>

#include "types.h"

struct window;
class ProcessDelayer;

template <typename tip>
class LinkedList;

// class process keeps all useful information about
// a process like its pid, cmdline, running state, window list and
// ending type. it also provides methos to kill, slowdown or
// suspend the process
class process
{

private:
	ProcessDelayer* pdelayer;
	bool slowdown(bool);
	bool pkill(bool);
	bool (process::*turn_select)(bool);
	end_type et;

public:
	uint pid;
	bool pon;
	LinkedList<window>* lw;
	std::string* name;

	process(ProcessDelayer*);
	~process();

	void setEndType(end_type);
	void pkill();
	bool suspend(bool);
	bool turn(bool);
	bool hasWid(uint);
	void addWindow(uint, uint);
	bool removeWindow(uint);

};

std::ostream& operator<<(std::ostream&, process&);
bool operator==(process&, process&);

#endif
