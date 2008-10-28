/*
  pdbfs - mount the Protein Data Bank as a filesystem with FUSE
  Copyright (C) 2008 by Miorel-Lucian Palii
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _UTIL_H
#define _UTIL_H 1

#include "config.h"

#define DIGIT_COUNT ('9' - '0' + 1)
#define LETTER_COUNT ('z' - 'a' + 1)

#define PDB_NUM_BASE (DIGIT_COUNT + LETTER_COUNT)

#define PDB_ID_LEN 4
#define PDB_ID_LEN_S "4"
#define ENTGZ_LEN (PDB_ID_LEN + 10)

#define PDB_COUNT (PDB_NUM_BASE * PDB_NUM_BASE * PDB_NUM_BASE * PDB_NUM_BASE)

int pdbid_to_num(const char* pdbid, unsigned int* num);
int num_to_pdbid(const unsigned int* num, char* pdbid);

int path_to_num(const char* path, unsigned int* num);

void pdbid_to_entgz(const char* pdbid, char* entgz);

#endif /* _UTIL_H */
