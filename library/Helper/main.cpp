/*
CoSiM Helper Functions/Classes
Copyright (C) TEI Crete

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// C++ Testbench: main
//
// Description: Regression test of the fifo class template of the HSoC library
//
// Authors: HSoC Team (C) - 24 October 2014
// Initial version: Antonis Papagrigoriou
// Changes: 
//
// Copyright: See LICENSE file that comes with this distribution
//
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include <systemc.h>

#include <string>

#include "config.h"
#include "hsoc_array.h"
#include "hsoc_array_of_array.h"
#include "hsoc_queue.h"
#include "hsoc_vector.h"

int sc_main (int argc, char* argv[]) {
	hsoc_queue<unsigned int> int_queue(2);
	int_queue.push(60);
	int_queue.push(61);
	if(int_queue.front())
		cout << *(int_queue.front()) << endl;
	int_queue.pop();
	if(int_queue.front())
		cout << *(int_queue.front()) << endl;
	int_queue.pop();
	if(int_queue.front())
		cout << *(int_queue.front()) << endl;
	int_queue.pop();
	hsoc_array<unsigned int> int_array(10, 0);
	int_array[0] = 20;
	int_array[1] = 21;
	cout << int_array[0]  << endl;
	cout << int_array[1]  << endl;
	hsoc_array_of_array<unsigned int> int_multi_array(10, 10);
	int_multi_array[0][1] = 40;
	int_multi_array[1][2] = 41;
	cout << int_multi_array[0][1] << endl;
	cout << int_multi_array[1][2] << endl;
	hsoc_vector<unsigned int> int_vector;
	for (unsigned int i=0; i<10; i++){
		int_vector.push_back(i);
	}
	for (unsigned int i=0; i<int_vector.size(); i++){
		cout << *(int_vector[i]) << endl;
	}
	int_vector.pop_back();
	int_vector.pop_back();
	for (unsigned int i=0; i<int_vector.size(); i++){
		cout << *(int_vector[i]) << endl;
	}
	
	int_vector.empty();
	
	cout << int_vector.size() << endl;
	return 0;// Terminate simulation
 }
