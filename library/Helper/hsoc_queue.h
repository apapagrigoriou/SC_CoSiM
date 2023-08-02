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
// Copyright (c) 1998 by Bruno R. Preiss, P.Eng.  All rights reserved.
//
//==============================================================================

#ifndef HSOC_QUEUE_H
#define HSOC_QUEUE_H

#define HSOC_Q_INCREASMENT_STEP 10
#include <string>
using namespace std;

#include <systemc.h>

template <class T>
class hsoc_queue
{
private:
	T *data;
	unsigned int head;
	unsigned int tail;
	unsigned int q_size;
	inline void change_array_size(uint64 newsize){   
		T* const newData = new T [newsize];
		if (newData == 0){
			cout << "Cannot Allocate memory" << endl;
		}
		uint64 i = head;
		uint64 j = 0;
		for (; i<tail; i++,j++){
			newData[j] = data[i];
		}
		tail = j;     
		q_size = newsize;
		head = 0;
		delete [] data;
		data = newData;
	};

public:
	sc_mutex protect_data;
	
	inline hsoc_queue(){
		data = new T [0];
		head = 0;
		tail = 0;
		q_size = 0;
	};
	
	inline hsoc_queue(uint64 qsize){
		data = new T [qsize];
		head = 0;
		tail = 0;
		q_size = qsize;
		
	};

	inline ~hsoc_queue(){
		delete [] data;
	};

	inline void push(T qdata){
		protect_data.lock();
		if (tail >= q_size){
			change_array_size(q_size+HSOC_Q_INCREASMENT_STEP);
		}
		data[tail++] = qdata;
		protect_data.unlock();
	};
	
	inline void pop(){
		protect_data.lock();
		if (head < tail){
			head++;
			if (head == tail){
				head = tail = 0; // queue is empty
			}
		}
		protect_data.unlock();
	};
	
	inline uint64 size() const{
		return tail-head;           
	};
    
	inline bool empty(){
		if (tail == head) 
			return(true);
		else
			return(false);
	}
	//Carefull data canot used directly since the are allocationg constantly
	inline T* front(){
		protect_data.lock();
		T *ret = NULL;
		if (head < tail){
			ret = &(data[head]);
		}
		protect_data.unlock();
		return(ret);
	};

	inline T* pop_front(){
		protect_data.lock();
		T *ret = NULL;
		if (head < tail){
			ret = &(data[head]);
			head++;
			if (head == tail){
				head = tail = 0; // queue is empty
			}
		}
		protect_data.unlock();
		return(ret);
	};

	
	inline T const& operator [] (uint64 position) const{
		uint64 const offset = position + head;
		if (offset >= tail){
			cout << "hsoc_queue[] Invalid position " <<  position << endl;
		}
		return data [offset];
	};

	inline T& operator [] (uint64 position){
		uint64 const offset = position + head;
		if (offset >= tail){
			cout << "hsoc_queue[] Invalid position " <<  position << endl;
		}
		return data [offset];
	};

	inline hsoc_queue& operator = (hsoc_queue const& otherqueue){
		protect_data.lock();
		delete [] data;
		head = 0;
		tail = otherqueue.size();
		q_size = otherqueue.size();
		data = new T [q_size];
		for (uint64 i = 0; i < q_size; ++i){
			data [i] = otherqueue[i];
		}
		protect_data.unlock();
		return(*this);
	}
	
};

#endif //HSOC_QUEUE_H
