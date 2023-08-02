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
struct sc_fw_ds {
	unsigned int op_code;
        unsigned int input_port;
	unsigned int output_port;
	unsigned int L_addr_reg;
        unsigned int H_addr_reg;
        unsigned int Rule_reg;
};

struct sc_access_ds {
	unsigned int op_code;
	unsigned int input_port;
	unsigned int output_port;
	unsigned int addr;
	unsigned int data;
};

struct sc_stat_ds {
	unsigned int port;
    unsigned int Total_passed;
    unsigned int Fifo_dropped;
    unsigned int Fw_dropped;
};

struct sc_direct_access_ds {
	unsigned int op_code;
	unsigned int bram_no;
	unsigned int addr;
	unsigned int data;
};
