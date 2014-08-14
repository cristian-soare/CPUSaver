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

#ifndef PDELAYER_H
#define PDELAYER_H

#include <unistd.h>
#include <pthread.h>

#include "types.h"

template <typename tip>
class LinkedList;

class process;

// ProcessDelayer class retains a list
// of processes. Once the list has at least one process,
// the class slows it down, keeping its cpu usage at (percent)%
class ProcessDelayer
{

private:
	LinkedList<process>* lproc;
	int percent;
	pthread_t thr;
	static void* slow_down(void*);

public:
	pthread_mutex_t mutex;
	bool active, thr_finished;

	ProcessDelayer(int);
	~ProcessDelayer();

	void addProcess(process*);
	void removeProcess(process*);
	bool clear();
	
};

#endif
