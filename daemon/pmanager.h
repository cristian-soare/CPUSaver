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

#ifndef PMANAGER_H
#define PMANAGER_H

#include "types.h"

#include <X11/Xlib.h>
#include <pthread.h>
#include <string>
#include <iosfwd>

class process;
class ProcessDelayer;
struct specialend;

template <typename tip>
class LinkedList;

class ProcessManager
{

private:
	LinkedList<process>* lproc;
	LinkedList<specialend>* imlist;
	LinkedList<std::string>* forblist;
	ProcessDelayer* pdelayer;
	end_type et;
	int threshold;

	void getProcesses(LinkedList<process>*);
	bool isValidWindowId(Window);
	bool isSpecial(process*);
	bool isForbidden(process*);
	void addProcess(LinkedList<process>*, uint, uint);
	bool removeByWindowId(LinkedList<process>*, uint);
	uint getSingle(Window, char*);
	bool bringForth(Window);
	void adapt();
	std::string* getCmdLine(uint);
	uint getParent(uint);

	Display* display;
	Window rootwin;
	long event_mask;

public:
	ProcessManager(int, end_type, int, LinkedList<specialend>*, LinkedList<std::string>*);
	~ProcessManager();

	bool ongoing;

	bool start();
	bool stop();

};

#endif
