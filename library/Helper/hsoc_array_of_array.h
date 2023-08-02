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
//HSOC Array of Array Template.
// Based on initial Array created by  Marcello Coppola, Stephane Curaba, Miltos Grammatikakis, 
// Giuseppe Marrucia, and Francesco Papariello. 
// which was the C++ code from Program 4.1 of
//   "Data Structures and Algorithms
//    with Object-Oriented Design Patterns in C++"
//   by Bruno R. Preiss.
//   Copyright (c) 1998 by Bruno R. Preiss, P.Eng.  All rights reserved.
//
//==============================================================================

#ifndef HSOC_ARRAY_OF_ARRAY_H
#define HSOC_ARRAY_OF_ARRAY

#include <string>
using namespace std;
#include "hsoc_array.h"

template <class T>
class hsoc_array_of_array
{
private:
	T **data;
	uint64 size;
	uint64 length;

public:
	sc_mutex protect_data;
	
	inline hsoc_array_of_array () {
		data = new T*[1];
		data[0] = new T[0];
		size = 1;
		length = 0;
	};

	inline hsoc_array_of_array (uint64 n, uint64 m){
		data = new T*[n];
		for ( uint64 i = 0; i < n; i++ ){
			data[i] = new T[m];
		}
		size  = n;
		length = m;
	};
	
  inline hsoc_array_of_array (hsoc_array_of_array const& array){
		protect_data.lock();
		length = array.Length();
		size = array.size();
		data = new T*[size];
		for (uint64 i = 0; i < size; i++ ){
			data[i] = new T[length];
			for ( uint64 j = 0; j < length; j++){
				data[i][j] = array.Data()[i][j];
			}
		}
		protect_data.unlock();
	};

  inline ~hsoc_array_of_array (){
		for ( uint64 i = 0; i < size; i++ ){
			delete [] data[i];
		}
		delete [] data;
	};

  inline hsoc_array_of_array& operator = (hsoc_array_of_array const& array){
		protect_data.lock();
		for ( uint64 i = 0; i < size; i++ ){
			delete [] data[i];
		}
		delete [] data;
		size  = array.Size();
		length = array.Length();
		data = new T*[size];
		for ( uint64 i = 0; i < size; i++ ){
			data[i] = new T[length];
			for (uint64 j = 0; j < length; j++){
				data[i][j] = array.Data()[i][j];
			}
		}
		protect_data.unlock();
		return(*this);
	}
	
  inline T const** Data () const{
		return data;
	};


	inline uint64 Size () const {
		return size;
	};
	
  inline uint64 Length () const{
		return length;
	};

  inline void ReSize (uint64 n, uint64 m){
		protect_data.lock();
		T **newData = new T*[n];
		for ( uint64 i = 0; i < n; i++ ){
			newData[i] = new T[m];
		}
		uint64 const minS = size < n ? size : n;
		uint64 const minL = length < m ? length : m;
		for (uint64 i = 0; i < minS; ++i){
			for (uint64 j = 0; j < minL; ++j){
				newData [i][j] = data [i][j];
			}
		}
		for ( uint64 i = 0; i < size; i++ ){
			delete [] data[i];
		}
		delete [] data;
		data = newData;
		length = m;
		size = n;
		protect_data.unlock();
	};

	inline T* const operator [] (uint64 position) const{
		if (position >= size){
			cout << "hsoc_array_of_array[] Invalid position " <<  position << endl;
    }
    return data [position];
	};
	
	inline void fill (T value) {
		for (uint64 i=0;i<size;i++){
			for (uint64 j=0;j<length;j++){
				data[i][j] = value;
			}
		}
	};
	
};

#endif //HSOC_ARRAY_OF_ARRAY
