/*
COPYRIGHT (C) 2021  Roberto Bucher (roberto.bucher@supsi.ch)

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
#include <commonFun.h>
#include <nuttx/config.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <nuttx/sensors/dhtxx.h>

#define VERB 1

static int fd;

static void init(python_block *block)
{
  fd = open(block->str, O_RDWR);    /* Open the device */
  if (fd < 0)
    {
      fprintf(stderr, "ERROR: Failed to open the device %s\n", block->str);
      close(fd);
      exit(1);
    }
}

static void inout(python_block *block)
{
  double *y = block->y[0];

  struct dhtxx_sensor_data_s data;
  int ret;

  ret = read(fd, &data, sizeof(struct dhtxx_sensor_data_s));
  if (ret < 0)
    {
      fprintf(stderr, "Read error %d\n", data.status);
    }
  else
    {
#ifdef VERB
      printf("Humidity = %2.2f %%, temperature = %2.2f C\n",
              data.hum, data.temp);
#endif
      y[0] = (double)data.hum;
      y[1] = (double)data.temp;
      printf("hum %f, temp %f", y[0], y[1]);
    }
}

static void end(python_block *block)
{
  close(fd);
}

void nuttx_DHTXX(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init(block);
  }
}
