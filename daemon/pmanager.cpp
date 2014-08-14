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

#include "pmanager.h"

#include "list.h"
#include "pdelayer.h"
#include "process.h"
#include "special.h"

#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <syslog.h>	

using namespace std;

// LinkedList iterator type definition
typedef LinkedList<process>::iterator iter;

// get pid and foreground macros that use the same getSingle function
#define getPid(w) getSingle(w, (char*)"_NET_WM_PID")
#define getForegroundWID(w) getSingle(w, (char*)"_NET_ACTIVE_WINDOW")

// retusn command line (string), knowing application pid
string* ProcessManager::getCmdLine(uint pid)
{
	ostringstream oss;
	oss << pid;

	string proc = "/proc/";
	proc += oss.str();
	proc += "/cmdline";

	ifstream f(proc.c_str());
	string path;
	f >> path;
	
	size_t pos = 0, actpos = 0;
	while (true)
	{
		pos = path.find('/', actpos);
		if (pos != string::npos)
			actpos = pos + 1;
		else
			break;
	}

	return new string(path.substr(actpos, path.length() - actpos).c_str());
}

// returns parent window id from the current window id
uint ProcessManager::getParent(uint id)
{
	Window rr, pr, *cr; uint ncr;
    XQueryTree(display, id, &rr, &pr, &cr, &ncr);
    XFree(cr);
    return pr;
}

// returns (uint) single property (cmd) from the window (win)
uint ProcessManager::getSingle(Window win, char* cmd)
{
	XSelectInput(display, win, 0);
    Atom a = XInternAtom(display, cmd, true);
    Atom type; int format; ulong nitems, bytes_after; uchar* data;

    int status = XGetWindowProperty(display, win, a, 0L, (~0L), false, AnyPropertyType, 
                                    &type, &format, &nitems, &bytes_after, &data);
	
	uint ret = 0;
	if (nitems) ret = *((uint*)data);
	XFree(data);
	XSelectInput(display, rootwin, event_mask);
	return (status == Success) ? ret : 0;
}

// checks if process is part of the special handling list
// and if so, sets its ending type (Suspend/Kill/SlowDown/Immune)
bool ProcessManager::isSpecial(process* p)
{
	LinkedList<specialend>::iterator it;
	if (imlist && imlist->getSize() > 0)
		for (it = imlist->begin(); it != imlist->begin() || it.cycle(); ++it)
			if (*((*it)->name) == *p->name)
			{
				p->setEndType((*it)->et);
				return true;
			}
	return false;
}

// checks if process is forbidden to interact with
// this is the case of window compositors or managers 
// (like compiz, plasma-dekstop, compiz, etc)
// which are mapped as normal windows, but should not be touched
bool ProcessManager::isForbidden(process* p)
{
	LinkedList<string>::iterator it;
	if (forblist && forblist->getSize() > 0)
		for (it = forblist->begin(); it != forblist->begin() || it.cycle(); ++it)
			if (*(*it) == *p->name)
				return true;
	return false;
}

// adds process with window id (id) and parent id (parent) to list lp
// uses getPid and getCmdLine to get pid and cmdline properties of process
void ProcessManager::addProcess(LinkedList<process>* lp, uint id, uint parent)
{
	process* p = new process(pdelayer);
	p->pid = getPid(id);
	p->name = getCmdLine(p->pid);

	if (isForbidden(p))
	{
		delete p;
		return;
	}

	process* pfound;
	if ((pfound = lp->findNode(p)) == NULL)
	{
		p->addWindow(id, parent);
		if (isSpecial(p) == false)
			p->setEndType(et);
		lp->addFirst(p);
	}
	else
	{
		pfound->addWindow(id, parent);
		delete p;
	}
}

// removes window wid and if its process has no more windows
// it removes the process itself from the process list
bool ProcessManager::removeByWindowId(LinkedList<process>* lp, uint wid)
{
	iter it;
	for (it = lp->begin(); it != lp->begin() || it.cycle(); ++it)
		if ((*it)->removeWindow(wid))
		{
			delete lp->removeNode(it.getNode());
			return true;
		}
	return false;
}

// gets a list of running processes from open windows list
// and stores them in the linked list lp
void ProcessManager::getProcesses(LinkedList<process>* lp)
{
	Atom a = XInternAtom(display, "_NET_CLIENT_LIST", true);
	Atom type; int format; ulong nitems, bytes_after; uchar* data;
    int status = XGetWindowProperty(display, rootwin, a, 0L, (~0L), false, AnyPropertyType, 
    	&type, &format, &nitems, &bytes_after, &data);

    if (status == Success && nitems)
    {
    	uint* wids = (uint*)data;
    	for (ulong i = 0; i < nitems; i++)
    		addProcess(lp, wids[i], getParent(wids[i]));
    }

    XFree(data);
}

// checks if window id (wid) belongs to an actual open window
bool ProcessManager::isValidWindowId(Window wid)
{
	XSelectInput(display, wid, 0);
	Atom a = XInternAtom(display, "_NET_WM_WINDOW_TYPE", true);
	Atom type; int format; ulong nitems, bytes_after; uchar* data;
	int status = XGetWindowProperty(display, wid, a, 0L, (~0L), false, AnyPropertyType,
		&type, &format, &nitems, &bytes_after, &data);

	bool wok = false;
	if (status == Success && nitems == 1)
	{
		Atom wtype = *((Atom*)data);
		wok = strcmp(XGetAtomName(display, wtype), "_NET_WM_WINDOW_TYPE_NORMAL") == 0;
	}

	XFree(data);
	XSelectInput(display, rootwin, event_mask);
	return wok;
}

// searches for the process which has a window with wid id
// and brings it at the top of the list, giving the process
// full access to the cpu if it was suspended
bool ProcessManager::bringForth(Window wid)
{
	iter it;
	uint count = 0;
	for (it = lproc->begin(); it != lproc->begin() || it.cycle(); ++it)
		if (count++ && (*it)->hasWid(wid))
		{
			process* p = lproc->removeNode(it.getNode());
			p->turn(true);
			lproc->addFirst(p);
			return true;
		}
	return false;
}

// turns on processes with index lower than threshold
// and off processes with index greater than threshold
void ProcessManager::adapt()
{
	if (lproc->getSize() == 0)
		return;
	
	iter it;
	int count;
	for (it = lproc->begin(), count = 0; it != lproc->begin() || it.cycle(); ++it, count++)
		if ((*it)->turn(count < threshold) == false)
		{
			Node<process>* node = it.getNode();
			count--; --it;
			delete lproc->removeNode(node);
		}
}

// constructor
ProcessManager::ProcessManager(int t = 10, end_type pet = Suspend, 
	int per = 5, LinkedList<specialend>* pimlist = NULL, LinkedList<string>* pforb = NULL)
{
	lproc = new LinkedList<process>();
	pdelayer = new ProcessDelayer(per);
	threshold = t;
	et = pet;
	display = XOpenDisplay(NULL);
	rootwin = DefaultRootWindow(display);
	event_mask = SubstructureNotifyMask | PropertyChangeMask;
	ongoing = true;
	imlist = pimlist;
	forblist = pforb;
}

// destructor
ProcessManager::~ProcessManager()
{
	delete pdelayer;
	delete lproc;

	XFree(display);
	if (imlist) delete imlist;
	if (forblist) delete forblist;
}

// starts monitoring windows and their processes using X11 events
bool ProcessManager::start()
{
	getProcesses(lproc);
	rootwin = DefaultRootWindow(display);

	if (lproc->getSize() > threshold)
	{
		syslog(LOG_ERR, "ERROR ! Number of windows already greater than threshold !");
		return false;
	}

    XEvent event;
    while (ongoing)
    {
    	if (!ongoing)
    		break;

    	XNextEvent(display, &event);

    	if (event.type == PropertyNotify)
    	{
    		uint fgw = getForegroundWID(rootwin);
    		if (fgw)
    		{
    			if (pdelayer->active) pthread_mutex_lock(&pdelayer->mutex);

	    		bringForth(fgw);
	    		adapt();

				if (pdelayer->active) pthread_mutex_unlock(&pdelayer->mutex);
	    	}
    	}
    	else if (event.type == MapNotify)
    	{
    		XMapEvent* mapevent = (XMapEvent*)&event;
    		Window w = (Window)(mapevent->window);

    		if (isValidWindowId(w))
    		{
    			if (pdelayer->active) pthread_mutex_lock(&pdelayer->mutex);

    			addProcess(lproc, w, getParent(w));
    			adapt();

    			if (pdelayer->active) pthread_mutex_unlock(&pdelayer->mutex);

	    		syslog(LOG_INFO, "Mapped: %x %d", (uint)w, getPid(w));
	    	}
    	}
    	else if (event.type == ReparentNotify)
    	{
    		XReparentEvent* repevent = (XReparentEvent*)&event;
    		Window w = (Window)(repevent->window);
    		Window pw = (Window)(repevent->parent);
    		if (isValidWindowId(w))
    		{
    			if (pdelayer->active) pthread_mutex_lock(&pdelayer->mutex);

    			addProcess(lproc, w, pw);
    			adapt();

    			if (pdelayer->active) pthread_mutex_unlock(&pdelayer->mutex);

    			syslog(LOG_INFO, "Reparented: %x %d", (uint)w, getPid(w));
    		}
    	}
    	else if (event.type == UnmapNotify)
    	{
    		XUnmapEvent* unmapevent = (XUnmapEvent*)&event;
    		Window w = (Window)(unmapevent->window);

    		if (pdelayer->active) pthread_mutex_lock(&pdelayer->mutex);

    		if (removeByWindowId(lproc, w))
    		{
    			adapt();
    			syslog(LOG_INFO, "Unmapped: %x", (uint)w);
    		}

    		if (pdelayer->active) pthread_mutex_unlock(&pdelayer->mutex);
    	}

    }

    return true;
}

// stops any kind of monitoring, turns on any suspended process
// and prepares the program for ending
bool ProcessManager::stop()
{
	ongoing = false;
	if (pdelayer->active && pdelayer->clear())
		syslog(LOG_NOTICE, "Joined");

	// continue any suspended processes
	iter it;
	for (it = lproc->begin(); it != lproc->begin() || it.cycle(); ++it)
		(*it)->suspend(true);

	return pdelayer->thr_finished;
}
