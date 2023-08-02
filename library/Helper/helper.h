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
// C++ Interface: config
//
// Description: Helper functions of the HSoC library (string tokenization and trimming)
//
// Authors: HSoC Team (C) - 24 October 2014
// Initial version: Antonis Papagrigoriou
// Changes: Antonis Papagrigoriou, Polydoros Petrakis, Miltos Grammatikakis
//
// Copyright: See LICENSE file that comes with this distribution
//
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#ifndef HELPER_H_
#define HELPER_H_

#include <string>
using namespace std;

//String Function Definitions
string trimstring(string m_string);
string trim(string m_string);
void strim(string &s_string);
int tokenize(string str_line, string tokenizer, string *tokens);


#endif /* HELPER_H_ */
