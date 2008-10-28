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
#include "rsync.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>

#define RSYNC_CMD RSYNC " -az --port=33444 "		\
  "rsync.wwpdb.org::ftp_data/structures/divided/pdb/"
gzFile pdbfs_get_file(const char* pdbid, struct stat* st) {
  char entgz[ENTGZ_LEN + 1];
  char cmd[strlen(RSYNC_CMD) + 3 + ENTGZ_LEN + 3];
  pdbid_to_entgz(pdbid, entgz);
  if(stat(entgz, st) != 0) {
    strcpy(cmd, RSYNC_CMD);
    strncpy(cmd + strlen(RSYNC_CMD), pdbid + 1, 2);
    strcpy(cmd + strlen(RSYNC_CMD) + 2, "/");
    strcat(cmd, entgz);
    strcat(cmd, " .");
    system(cmd);
    stat(entgz, st);
  }
  return gzopen(entgz, "rb");
}
