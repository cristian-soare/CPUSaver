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

#include "process.h"

#include "window.h"
#include "list.h"
#include "pdelayer.h"

#include <csignal>
#include <string>
#include <fstream>

using namespace std;

typedef LinkedList<window>::iterator iter;

// constructor which receives pointer to processdelayer instance
// so that the process instance can add itself later to the
// delayer list
process::process(ProcessDelayer* pd)
{
	lw = new LinkedList<window>();
	pon = true;
	name = NULL;
	pid = 0;
	pdelayer = pd;
}

// destructor
process::~process()
{
	delete lw;
	delete name;
}

// setter of ending type 
void process::setEndType(end_type pet)
{
	et = pet;
	if (et == Suspend) turn_select = &process::suspend;
	else if (et == SlowDown) turn_select = &process::slowdown;
	else if (et == Kill) turn_select = &process::pkill;
}

// kills process if not immune
void process::pkill()
{
	if (et == Immune)
		return;
	kill(pid, SIGKILL);
}

// kills process
// signature with bool paramter is used to make the method
// compatible with the function pointer turn_select
bool process::pkill(bool on = false)
{
	kill(pid, SIGKILL);
	return false;
}

// suspens or continues the process
// its logic is inverted:
// false - suspends process
// true - continues the process
// this is done so that it can allign with the turn(bool) function
// which subsequently calls suspend if Suspend end_type is selected
bool process::suspend(bool on)
{
	if (on) kill(pid, SIGCONT);
	else	kill(pid, SIGSTOP);

	return true;
}

// adds process to the delayer list
bool process::slowdown(bool on)
{
	if (on == false) pdelayer->addProcess(this);
	else pdelayer->removeProcess(this);

	return true;
}

// turns a process on or off depending on its
// ending type (Suspend, Kill, SlowDown, Immune)
bool process::turn(bool on)
{
	if (et == Immune || pon == on)
		return true;

	pon = on;
	return (this->*turn_select)(on);
}

// checks if process has a window with window id pwid
bool process::hasWid(uint pwid)
{
	iter it;
	for (it = lw->begin(); it != lw->begin() || it.cycle(); ++it)
		if (*(*it) == pwid)
			return true;
	return false;
}

// removes window with window id (id) from windows list
bool process::removeWindow(uint id)
{
	iter it;
	for (it = lw->begin(); it != lw->begin() || it.cycle(); ++it)
		if (*(*it) == id)
		{
			delete lw->removeNode(it.getNode());
			if (lw->getSize() == 0)
				return true;
		}
	return false;
}

// adds window with id (id) and parent id (parent)
// to the windows list of the process
void process::addWindow(uint id, uint parent)
{
	lw->addFirst(new window(id, parent));
}

// output overload to make printing easier
ostream& operator<<(ostream& out, process& p)
{
	out << dec << p.pid << ": " << hex << *(p.lw) << ' ' << *(p.name) << " on state: " << p.pon;
	return out;
}

// checks if pids of two processes are equal
bool operator==(process& a, process& b)
{
	return a.pid == b.pid;
}
