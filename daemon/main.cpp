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
#include "special.h"
#include "cfgread.h"
#include "list.h"

#include <pwd.h>
#include <csignal>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <syslog.h>

using namespace std;

ProcessManager* pm;

// SIGINT catch
void signal_callback(int signum)
{
	syslog(LOG_NOTICE, "Stopping ...");
	if (pm->stop())
		syslog(LOG_NOTICE, "Stopped");
}

// code to fork daemon
inline void to_daemon()
{
	pid_t pid, sid;

	pid = fork();
	if (pid < 0) exit(EXIT_FAILURE);
	if (pid > 0) exit(EXIT_SUCCESS);

	umask(0);

	sid = setsid();
	if (sid < 0) exit(EXIT_FAILURE);
	if (chdir("/") < 0) exit(EXIT_FAILURE);

	close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main()
{
	to_daemon();

	signal(SIGINT, signal_callback);

	// open log
	setlogmask(LOG_UPTO(LOG_NOTICE | LOG_INFO | LOG_ALERT));
	openlog("cpusaver", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);

	// default values
	int threshold = 7, percent = 5;
	end_type et = Suspend;
	LinkedList<specialend>* immunes = NULL;
	
	// forbidden list
	LinkedList<string>* forblist = new LinkedList<string>();
	forblist->addLast(new string("compiz"));
	forblist->addLast(new string("kwin"));
	forblist->addLast(new string("plasma-desktop"));
	forblist->addLast(new string("nautilus"));
	forblist->addLast(new string("xfwm"));

	// configure parameters and start monitoring
	passwd* pw = getpwuid(getuid());
	syslog(LOG_NOTICE, "Start ======================");
	if (!cfgread::readConfig(pw->pw_dir, threshold, et, percent, immunes, forblist))
	{
		syslog(LOG_ALERT, "Configuration file invalid; using default configuation.");
		cfgread::writeDefaultConfig(pw->pw_dir, threshold, et, percent, immunes, forblist);
	}
	pm = new ProcessManager(threshold, et, percent, immunes, forblist);

	// finishing and closing monitor
	bool status = pm->start();
	delete pm;
	syslog(LOG_NOTICE, "Finish =====================");
	closelog();
	
	if (status == false)
		return EXIT_FAILURE;
	return 0;
}
