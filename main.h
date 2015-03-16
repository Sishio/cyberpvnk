/*
Czech_mate by Daniel
This file is part of Czech_mate.

Czech_mate is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License Version 2 as published by
the Free Software Foundation, 

Czech_mate is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Czech_mate.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MAIN_H
	#define MAIN_H
	#include "cstdio"
	#define TERMINATE -127
	#define DEBUG_SEPERATOR 1
	#define MULTITHREAD 1
	/*
	Multithreading isn't safe in the slightest, at least not in the current state. Forcing no multithreading would
	lower the data overhead since there are no locks to set and is the safest choice for stability. If you don't
	absolutely need more than one thread, please leave this off.
	*/
#endif
