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

#include "config.h"
#include "util.h"

#include <string.h>
#include <stdlib.h>

int pdbid_to_num(const char* pdbid, unsigned int* num) {
  const char* c;
  int ret = 1;
  *num = 0;
  for(c = pdbid; c != pdbid + PDB_ID_LEN; ++c) {
    if('0' <= (*c) && (*c) <= '9')
      (*num) = (*num) * PDB_NUM_BASE + (*c) - '0';
    else if('a' <= (*c) && (*c) <= 'z')
      (*num) = (*num) * PDB_NUM_BASE + (*c) - 'a' + DIGIT_COUNT;
    else {
      ret = 0;
      break;
    }
  }
  return ret;
}

int num_to_pdbid(const unsigned int* num, char* pdbid) {
  unsigned int n = *num;
  int ret = n >= 0 && n < PDB_COUNT;
  int i, m;
  if(ret) for(pdbid += (i = PDB_ID_LEN - 1); i >= 0; --i) {
      m = n % PDB_NUM_BASE;
      n /= PDB_NUM_BASE;
      *(pdbid--) = (char) (m + (m >= DIGIT_COUNT ? 'a' - DIGIT_COUNT : '0'));
    }
  return ret;
}

int path_to_num(const char* path, unsigned int* num) {
  int ret = 1;	
  switch(strlen(++path)) {
  case 0:
    *num = PDB_COUNT;
    break;
  case (PDB_ID_LEN + 4):
    if(strcmp(path + PDB_ID_LEN, ".pdb") == 0) ret = pdbid_to_num(path, num);
    break;
  default:
    ret = 0;
  }
  return ret;
}

void pdbid_to_entgz(const char* pdbid, char* entgz) {
  strcpy(entgz, "pdb");
  strncpy(entgz + 3, pdbid, PDB_ID_LEN);
  strcpy(entgz + 3 + PDB_ID_LEN, ".ent.gz");
}
