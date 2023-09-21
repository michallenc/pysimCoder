/*
  COPYRIGHT (C) 2023 Michal Lenc (michallenc@seznam.cz)

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

#ifndef CANDEV_H
#define CANDEV_H

#include <stdbool.h>
#include <stdint.h>

bool can_get_msg_from_id(int id, uint64_t *value);
bool can_register_msg(int id);
bool can_send_msg(int id, int dlc, uint64_t value);
bool can_device_open(int canfd_on, char *devname);
bool can_device_close(void);

#endif /* CANDEV_H */