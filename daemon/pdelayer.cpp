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

#include "pdelayer.h"

#include "process.h"
#include "list.h"

#include <syslog.h>

using namespace std;

// base 'PWM' frequency and iterator type definition
#define BASE_DELAY 10000
typedef LinkedList<process>::iterator iter;

// constructor
ProcessDelayer::ProcessDelayer(int pp = 5)
{
	percent = pp;
	lproc = new LinkedList<process>();
	lproc->setFreeResponsability(false);
	active = false;
	pthread_mutex_init(&mutex, NULL);
}

// destructor
ProcessDelayer::~ProcessDelayer()
{
	delete lproc;
}

// adds a process to the delaying list and if it is the first one,
// it starts a separate thread which handles the looping suspend
// algorithm which slows down the applications
void ProcessDelayer::addProcess(process* p)
{
	lproc->addLast(p);
	syslog(LOG_INFO, "Process %s added to delayer", p->name->c_str());

	if (active == false)
	{
		active = true;
		pthread_create(&thr, NULL, slow_down, (void*)this);
		syslog(LOG_NOTICE, "Thread created");
	}
}

// removes pocess from list and if after the operation the list 
// has no more elements, it terminates the separate thread and
// joins it with the master thread
void ProcessDelayer::removeProcess(process* p)
{
	iter it;
	for (it = lproc->begin(); it != lproc->begin() || it.cycle(); ++it)
		if (*it == p)
		{
			lproc->removeNode(it.getNode());
			break;
		}

	p->suspend(true);
	syslog(LOG_INFO, "Process %s removed from delayer", p->name->c_str());

	if (lproc->getSize() == 0)
	{
		active = false;
		pthread_mutex_unlock(&mutex);
		pthread_join(thr, NULL);
		syslog(LOG_NOTICE, "Thread ended");
	}
}

// clear the delaying process list
bool ProcessDelayer::clear()
{
	if (active)
	{
		active = false;
		pthread_join(thr, NULL);
	}

	return thr_finished;
}

// function that runs on a separate thread and handles the
// process slowing task, using repeated suspending and continuing a pid
void* ProcessDelayer::slow_down(void* arg)
{
	iter it;

	ProcessDelayer* pthis = (ProcessDelayer*)arg;
	pthis->thr_finished = false;

	while (pthis->active)
	{
		pthread_mutex_lock(&pthis->mutex);
		if (pthis->lproc->getSize() > 0)
			for (it = pthis->lproc->begin(); it != pthis->lproc->begin() || it.cycle(); ++it)
				(*it)->suspend(true);
		pthread_mutex_unlock(&pthis->mutex);
		usleep(BASE_DELAY * pthis->percent);

		pthread_mutex_lock(&pthis->mutex);
		if (pthis->lproc->getSize() > 0)
			for (it = pthis->lproc->begin(); it != pthis->lproc->begin() || it.cycle(); ++it)
				(*it)->suspend(false);
		pthread_mutex_unlock(&pthis->mutex);
		usleep(BASE_DELAY * (10 - pthis->percent));
	}

	pthis->thr_finished = true;

	return NULL;
}
