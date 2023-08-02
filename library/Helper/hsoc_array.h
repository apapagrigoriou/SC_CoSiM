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
//HSOC Array Template.
// Based on initial Array created by  Marcello Coppola, Stephane Curaba, Miltos Grammatikakis, 
// Giuseppe Marrucia, and Francesco Papariello. 
// which was the C++ code from Program 4.1 of
//   "Data Structures and Algorithms
//    with Object-Oriented Design Patterns in C++"
//   by Bruno R. Preiss.
//   Copyright (c) 1998 by Bruno R. Preiss, P.Eng.  All rights reserved.
//
//==============================================================================

#ifndef HSOC_ARRAY_H
#define HSOC_ARRAY_H

#include <string>
using namespace std;

template <class T> 
class hsoc_array {

private:
	T* data;
	uint64 base;
	uint64 length;
	
public:
	sc_mutex protect_data;
  inline hsoc_array () {
		data  = (new T [0]);
		base  = 0;
		length = 0;
	};

	inline hsoc_array (uint64 n, uint64 m = 0){
		data  = (new T [n]);
		base  = m;
		length = n;
	};
	
  inline hsoc_array (hsoc_array const& array){
		protect_data.lock();
		data  = (new T [array.length]);
		base  = array.Base();
		length = array.Length();
		for (uint64 i = 0; i < length; ++i){
			data [i] = array.Data()[i];
		}
		protect_data.unlock();
	};

  inline ~hsoc_array (){
		delete [] data;
	};

  inline hsoc_array& operator = (hsoc_array const& array){
		protect_data.lock();
		delete [] data;
		data  = (new T [array.Length()]);
		base  = array.Base();
		length = array.Length();
		for (uint64 i = 0; i < length; ++i){
			data [i] = array.Data()[i];
		}
		protect_data.unlock();
		return(*this);
	}
	
  inline T const* Data () const{
		return data;
	};

  inline uint64 Base () const {
		return base;
	};
	
  inline uint64 Length () const{
		return length;
	};

  inline void SetBase (uint64 newBase){
		base = newBase;
	};

  inline void SetLength (uint64 newLength){
		protect_data.lock();
		T* const newData = new T [newLength];
		uint64 const min = length < newLength ? length : newLength;
		for (uint64 i = 0; i < min; ++i){
			newData [i] = data [i];
		}
		delete [] data;
		data = newData;
		length = newLength;
		protect_data.unlock();
	};

	inline T const& operator [] (uint64 position) const{
		uint64 const offset = position - base;
		if (offset >= length){
			cout << "hsoc_array[] Invalid position " <<  position << endl;
    }
    return data [offset];
	};

	inline T& operator [] (uint64 position){
		uint64 const offset = position - base;
		if (offset >= length){
			cout << "hsoc_array[] Invalid position " <<  position << endl;
		}
		return data [offset];
	};

	inline void SetNoOffsetData(uint64 position, T const& value){
		if (position >= length){
			cout << "hsoc_array.SetNoOffsetData() Invalid position " <<  position << endl;
		}
		data[position] = value;
	};

	inline int64 get_id(T key) const {
		for (uint64 i=0;i<length;i++){
			if (data[i]== key){
				return i;
			}
		}
		return(0);
	}

	inline void fill (T value) {
		protect_data.lock();
		for (uint64 i=0;i<length;i++){
			data[i] = value;
		}
		protect_data.unlock();
	};
	
};

#endif   /* HSOC_ARRAY_H */
