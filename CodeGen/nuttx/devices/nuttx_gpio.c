/*
COPYRIGHT (C) 2025  Michal Lenc (michallenc@seznam.cz)

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

#include <errno.h>
#include <fcntl.h>
#include <nuttx/config.h>
#include <pyblock.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <nuttx/ioexpander/gpio.h>

static void init(python_block *block) {
  int *intPar = block->intPar;
  int ret;
  int fd;

  fd = open(block->str, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Error opening device: %s: %d\n", block->str, errno);
    exit(1);
  }

  ret = ioctl(fd, GPIOC_PINTYPE, (unsigned long)((uintptr_t)&intPar[0]));
  if (ret < 0) {
    fprintf(stderr, "Error reading pintype from %s: %d\n", block->str, errno);
    close(fd);
    exit(1);
  }

  if (block->nout > 0 && intPar[0] < GPIO_OUTPUT_PIN) {
    fprintf(stderr, "GPIO %s is not configured as an output pin.\n",
            block->str);
    close(fd);
    exit(1);
  }

  intPar[1] = fd;
}

static void inout(python_block *block) {
  int *intPar = block->intPar;
  double *u;
  double *y;
  int pintype = intPar[0];
  bool outvalue;
  bool invalue;

  /* Write output only if the pin is configured as an output pin. */
  if (pintype >= GPIO_OUTPUT_PIN && block->nin > 0) {
    outvalue = u[0] > realPar[0] ? true : false;
    ret = ioctl(fd, GPIOC_WRITE, (unsigned long)outvalue);
    if (ret < 0) {
      fprintf(stderr, "Error writing value %u from %s: %d\n",
              (unsigned int)outvalue, devpath, errno);
      close(intPar[0]);
      exit(1);
    }
  }

  if (block->nout > 0) {
    ret = ioctl(fd, GPIOC_READ, (unsigned long)((uintptr_t)&invalue));
    if (ret < 0) {
      fprintf(stderr, "ERROR: Failed to read value from %s: %d\n", block->str,
              errno);
      close(intPar[0]);
      exit(1);
    }

    y[0] = invalue ? 1 : 0;
  }
}

static void end(python_block *block) {
  int *intPar = block->intPar;
  close(intPar[1]);
}

void nuttx_gpio(int flag, python_block *block) {
  if (flag == CG_OUT) { /* set/get value */
    inout(block);
  } else if (flag == CG_END) { /* termination */
    end(block);
  } else if (flag == CG_INIT) { /* initialisation */
    init(block);
  }
}