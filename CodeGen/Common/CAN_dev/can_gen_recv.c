/*
COPYRIGHT (C) 2009  Roberto Bucher (roberto.bucher@supsi.ch)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#include <pyblock.h>

#include <candev.h>
#include <stdlib.h>
#include <unistd.h>

static void init(python_block *block) {
  if (can_device_open(0, block->str))
    exit(1);
  can_register_msg(block->intPar[3]);
}

static void inout(python_block *block) {
  uint8_t DATA[8];
  double *y;

  uint16_t *index;
  uint8_t *subindex;
  uint32_t *data_index;

  DATA[0] = 0x40;
  index = (uint16_t *)&DATA[1];
  subindex = (uint8_t *)&DATA[3];
  *index = block->intPar[1];
  *subindex = block->intPar[2];

  uint64_t value = 0x11223344556677;

  can_send_msg(block->intPar[0], 8, value);

  y = block->y[0];
  y[0] = 0;
}

static void end(python_block *block) {}

void can_gen_recv(int flag, python_block *block) {
  if (flag == CG_OUT) { /* get input */
    inout(block);
  } else if (flag == CG_END) { /* termination */
    end(block);
  } else if (flag == CG_INIT) { /* initialisation */
    init(block);
  }
}
