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
#include <commonFun.h>
#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <nuttx/analog/adc.h>
#include <nuttx/analog/ioctl.h>

static void init(python_block *block)
{
  int * intPar = block->intPar;
  int fd = intPar[2];

  if(fd==0){
    fd = open(block->str, O_RDONLY);
    if(fd<0) {
      fprintf(stderr,"Error opening device: %s\n", block->str);
      exit(1);
    }
  }

  intPar[2] = fd;
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar = block->intPar;
  double *y = block->y[0];
  int ret;
  int n_ch = intPar[0];
  int res = intPar[1];
  int fd = intPar[2];
  int readsize = n_ch*sizeof(struct adc_msg_s);
  int nbytes;
  
  struct adc_msg_s sample[n_ch];

#ifdef CONFIG_EXAMPLES_ADC_SWTRIG
  ret = ioctl(fd, ANIOC_TRIGGER, 0);
  if (ret < 0)
    {
      int errcode = errno;
      fprintf(stderr,"ERROR: ANIOC_TRIGGER ioctl failed: %d\n", errcode);
      close(fd);
      exit(1);
    }
#endif
      
  nbytes = read(fd, sample, readsize);

  if (nbytes <= 0)
    {
      int errval = errno;
      if (errval != EINTR)
        {
	        fprintf(stderr,"ERROR: read %s failed: %d\n", block->str, errval);
	        close(fd);
	        exit(1);
        }
    }

  int nsamples = nbytes / sizeof(struct adc_msg_s);
  if (nsamples != n_ch)
    {
      fprintf(stderr, "ERROR: number of channels %d is not equal to number of samples %d\n",
              n_ch, nsamples);
    }
  else
    {
      for (int i = 0; i < n_ch; i++)
        {
          y[i] = maprD2D((double) sample[i].am_data/(res - 1), realPar[0], realPar[1]);
        }
    }
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;

  close(intPar[2]);
}

void nuttx_ADC(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */
    end(block);
  }
  else if (flag ==CG_INIT){   /* initialisation */
    init(block);
  }
}


