/*
CoSiM Demo Using the Firewall Module as DuT 
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
#ifndef MEM_BUS_IF_H_INCLUDED
#define MEM_BUS_IF_H_INCLUDED

#include <systemc>
#include <tlm>

//memory (AxiLike) bus channel Interface
template<typename _SCCOSIM_DT> class mem_bus_if : public sc_interface
{
	public:
		mem_bus_if(const char* name_ = ""){}
		virtual _SCCOSIM_DT& read(unsigned addr)=0;
		virtual void read(unsigned addr, _SCCOSIM_DT *data)=0;
		virtual void burst_read(unsigned addr, int size, _SCCOSIM_DT *data)=0;
		virtual void write(unsigned addr, _SCCOSIM_DT *data)=0;
		virtual void burst_write(unsigned addr, int size, _SCCOSIM_DT *data)=0;
};

//memory (AxiLike) bus channel port
template<typename _SCCOSIM_DT> class mem_bus_port : public sc_port<mem_bus_if<_SCCOSIM_DT> >
{
		typedef mem_bus_if<_SCCOSIM_DT>						if_type;
		typedef sc_port<if_type, 1,SC_ONE_OR_MORE_BOUND>    base_type;
		typedef mem_bus_port<_SCCOSIM_DT>     				this_type;
		typedef if_type										in_if_type;
		typedef base_type									in_port_type;
	public:
		mem_bus_port() {}
		explicit mem_bus_port( const char* name_ ) { }
		void reset() {}
		_SCCOSIM_DT &read(unsigned addr)
		{
			return (*this)->read(addr);
		}
		void burst_read(unsigned addr, int size, _SCCOSIM_DT *data)
		{
			(*this)->burst_read(addr, size, data);
		}

		void write(unsigned addr, _SCCOSIM_DT *data)
		{
			(*this)->write(addr, data);
		}
		void burst_write(unsigned addr, int size, _SCCOSIM_DT *data)
		{
			(*this)->burst_write(addr, size, data);
		}
};


//memory (AxiLike) bus channel
template <typename data_type> class mem_bus_chn  :public mem_bus_if<data_type> ,public sc_module
{
	private:
		data_type  *mem;
		std::string name;
		unsigned int m_start_addr;
		unsigned int m_end_addr;
	public:
		SC_HAS_PROCESS(mem_bus_chn);
		mem_bus_chn(sc_module_name _name,unsigned int start_addr = 0 ,unsigned int end_addr = 1024) : 
		     sc_module(_name), name(_name), m_start_addr(start_addr), m_end_addr(end_addr)
		{
			sc_assert(m_start_addr <= m_end_addr);
			unsigned int size = (m_end_addr-m_start_addr+1);
			mem = new data_type [size];
			for (unsigned int i = 0; i < size; ++i)
				mem[i] = i;
		}
		data_type& read(unsigned addr)
		{
			return mem[addr-m_start_addr];
		}
		void read(unsigned addr, data_type *data)
		{
			*data = mem[addr-m_start_addr];
		}
		void burst_read(unsigned addr, int size, data_type *data)
		{
			for(int i=0; i<size; i++)
			{
				data[i] = mem[addr-m_start_addr+i];
			}
		}
		void write(unsigned addr, data_type *data)
		{
			mem[addr-m_start_addr] = *data;
		}
		void burst_write(unsigned addr, int size, data_type *data)
		{
			for(int i=0; i<size; i++)
			{
				mem[addr-m_start_addr+i] = data[i];
			}
		}
};

#endif // MEM_BUS_IF_H_INCLUDED
