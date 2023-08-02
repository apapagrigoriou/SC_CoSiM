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
//HSOC Vector Template.

#ifndef HSOC_VECTOR_H_
#define HSOC_VECTOR_H_

template <class T> class hsoc_vector {
	struct contained_struct {
		T data;
		unsigned int pos;
		contained_struct *next;
		contained_struct *preview;
	};
 private:
	//properties
	contained_struct *m_last_element;
	contained_struct *m_element;
	unsigned int m_size;

 public:
	sc_mutex protect_data;
	 
	//constructor
	inline hsoc_vector() {
		m_size = 0;
		m_element = NULL;
		m_last_element = NULL;
	};
	//distructor
	inline ~hsoc_vector() {
		clear();
	};
	
  // methods
	inline void push_back(const T& data) {
		protect_data.lock();
		contained_struct *new_element = new contained_struct;
		new_element->data = data;
		new_element->next = NULL;
		new_element->pos = m_size;
		m_size++;
		if (m_last_element != NULL){
			m_last_element->next = new_element;
			new_element->preview = m_last_element;
			m_last_element = new_element;
		}else{
			new_element->preview = NULL;
			m_last_element = new_element;
			m_element = new_element;
		}
		protect_data.unlock();
		return; 
	};
	
	inline void pop_back() {
		if (m_element == NULL)
			return;
		protect_data.lock();
		contained_struct *del_element = m_last_element;
		m_last_element = m_last_element->preview;
		m_last_element->next = NULL;
		m_size--;
		delete del_element;
		protect_data.unlock();
		return;
	};
	
	inline unsigned int size() {
		return(m_size);
	};
	
	inline bool empty() {
		if (m_element == NULL)
			return(true);
		else
			return(false);
	};

	inline T* operator[] (unsigned int pos){
		contained_struct *cur_element = m_element;
		while (cur_element != NULL){
			if (cur_element->pos == pos)
				return(&(cur_element->data));
			cur_element = cur_element->next;
		}
		return(NULL);
	}

	inline void clear() {
		protect_data.lock();
		contained_struct *cur_element = m_element;
		while (cur_element != NULL){
			contained_struct *tmp_element = cur_element;
			cur_element = cur_element->next;
			delete tmp_element;
			tmp_element = NULL;
			m_size--;
		}
		m_last_element = NULL;
		m_element = NULL;
		protect_data.unlock();
	};

};
#endif /* HSOC_VECTOR_H_ */
 
