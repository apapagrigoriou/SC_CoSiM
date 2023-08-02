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
// C++ Implementation: helper functions
//
// Description: Helper functions of the HSoC library (string tokenization and trimming)
//
// Authors: HSoC Team (C) - 29 August 2013
// Initial version: Antonis Papagrigoriou
// Changes: Polydoros Petrakis, Miltos Grammatikakis
//
// Copyright: See LICENSE file that comes with this distribution
//
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "helper.h"

bool isWhitespace(const char& c){
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

//Cleans String from extra spaces and end of line characters
string trimstring(string m_string){
	if (m_string[0] == '"' || m_string[0] == '\"')
		m_string.erase(0, 1);
	while(isWhitespace(m_string[0]))
		m_string.erase(0, 1);
	basic_string<char, char_traits<char> >::iterator i = m_string.begin();
	if (i == m_string.end())
		return(m_string);
	i++;
	while(i != m_string.end()){
		if(isWhitespace(*(i-1))){
			if(isWhitespace(*i)){
				i = m_string.erase(i);
			}else{
				i++;
			}
		}else{
			i++;
		}
	}
	while (isWhitespace(m_string[(m_string.size()-1)]))
		m_string.erase((m_string.size()-1), 1);
	if (m_string[m_string.size()-1] == '"' || m_string[m_string.size()-1] == '\"')
		m_string.erase((m_string.size()-1), 1);
	return (m_string);
}

//Cleans String for extra with spaces and end of line characters
string trim(string m_string){
	while(isWhitespace(m_string[0]))
		m_string.erase(0, 1);
	basic_string<char, char_traits<char> >::iterator i = m_string.begin();
	if (i == m_string.end())
		return(m_string);
	i++;
	while(i != m_string.end()){
		if(isWhitespace(*(i-1))){
			if(isWhitespace(*i)){
				i = m_string.erase(i);
			}else{
				i++;
			}
		}else{
			i++;
		}
	}
	while (isWhitespace(m_string[(m_string.size()-1)]))
		m_string.erase((m_string.size()-1), 1);
	return (m_string);
}

void strim(string &s_string){
	while(isWhitespace(s_string[0]))
		s_string.erase(0, 1);
	basic_string<char, char_traits<char> >::iterator i = s_string.begin();
	if (i == s_string.end())
		return;
	i++;
	while(i != s_string.end()){
		if(isWhitespace(*(i-1))){
			if(isWhitespace(*i)){
				i = s_string.erase(i);
			}else{
				i++;
			}
		}else{
			i++;
		}
	}
	while (isWhitespace(s_string[(s_string.size()-1)]))
		s_string.erase((s_string.size()-1), 1);
	return;
}

int tokenize(string str_line, string tokenizer, string *tokens){
	if (str_line.empty())
		return(-1);
	if (tokenizer.empty())
		return(-2);
	//obtain the number of elements
	int counter = 0;
	int tocsize = tokenizer.size();
	string::size_type pos = str_line.find(tokenizer,0);
	string token = str_line.substr(0,pos);
	while (pos != string::npos){
		strim(token);
		counter++;
		string::size_type pos_start = pos+tocsize;
		pos = str_line.find(tokenizer,pos_start);
		if (pos != string::npos)
			token = str_line.substr(pos_start,(pos-pos_start));
		else
			token = str_line.substr(pos_start,pos);
	}
	strim(token);
	counter++;
	//fill the array
	tokens = new string[counter];
	counter = 0;
	pos = str_line.find(tokenizer,0);
	token = str_line.substr(0,pos);
	while (pos != string::npos){
		strim(token);
		tokens[counter] = token;
		counter++;
		string::size_type pos_start = pos+tocsize;
		pos = str_line.find(tokenizer,pos_start);
		if (pos != string::npos)
			token = str_line.substr(pos_start,(pos-pos_start));
		else
			token = str_line.substr(pos_start,pos);
	}
	strim(token);
	tokens[counter] = token;
	return(counter);
}
