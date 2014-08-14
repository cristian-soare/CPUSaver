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

#include "cfgread.h"

#include "special.h"
#include "list.h"

#include <fstream>

using namespace std;

// readConfig reads the configuration file from the path 'path'
bool cfgread::readConfig(char* path, int& threshold, end_type& et, 
	int& per, LinkedList<specialend>*& immunes, LinkedList<string>*& forblist)
{
	string s(path);
	s += "/.cpusconfig";

	ifstream f(s.c_str());
	if (f.good())
	{
		while (f >> s)
		{
			if (s == "Threshold")
			{
				if (!(f >> threshold) || threshold < 3)
					return false;
			}
			else if (s == "EndType")
			{
				string s2;
				if (!(f >> s2))
					return false;
				if (s2 == "Suspend")
					et = Suspend;
				else if (s2 == "Kill")
					et = Kill;
				else if (s2 == "SlowDown")
					et = SlowDown;
				else
					return false;
			}
			else if (s == "SpecialList")
			{
				int ni;
				if (!(f >> ni) || ni < 0)
					return false;
				if (ni)
				{
					immunes = new LinkedList<specialend>();
					for (int i = 0; i < ni; i++)
					{
						specialend* spend = new specialend();
						f >> *spend;
						immunes->addLast(spend);
					}
				}
				else
					immunes = NULL;
			}
			else if (s == "ForbiddenList")
			{
				int ni;
				if (!(f >> ni) || ni < 0)
					return false;
				delete forblist;
				if (ni)
				{
					forblist = new LinkedList<string>();
					for (int i = 0; i < ni; i++)
					{
						string* sforb = new string();
						f >> *sforb;
						forblist->addLast(sforb);
					}
				}
				else
					forblist = NULL;
			}
			else if (s == "PercentSD")
				if (!(f >> per) || per < 1 || per > 9)
					return false;
		}
		f.close();
		return true;
	}
	f.close();
	return false;
}

// writeDefaultConfig initializes the parameters with the default values
// and creates a config file located at the path 'path'
void cfgread::writeDefaultConfig(char* path, int& t, end_type& et, 
	int& per, LinkedList<specialend>*& immunes, LinkedList<string>*& forblist)
{
	string s(path);
	s += "/.cpusconfig";

	ofstream g(s.c_str());
	g << "Threshold 7\n";
	g << "EndType Suspend\n";
	g << "ImmuneList 0\n";
	g << "PercentSD 5\n";

	t = 7;
	et = Suspend;
	per = 5;
	immunes = NULL;
	
	delete forblist;
	forblist = new LinkedList<string>();
	forblist->addLast(new string("compiz"));
	forblist->addLast(new string("kwin"));
	forblist->addLast(new string("plasma-desktop"));
	forblist->addLast(new string("nautilus"));
	forblist->addLast(new string("xfwm"));

	g.close();
}
