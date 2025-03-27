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
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

extern int fdSerial;
extern int32_t pins[];

extern unsigned char BuffOut[4];

#define MAXLEN 4
#define ANALOGOUT 6
#define OUTPUT 1

static void init(python_block *block)
{
  int * intPar    = block->intPar;

  BuffOut[0] = 0;
  BuffOut[1] = intPar[0];
  int16_t *value = (int16_t *) &BuffOut[2];
  *value =  OUTPUT;
  write(fdSerial, BuffOut, MAXLEN);
  usleep(50000);
}

static void inout(python_block *block)
{
  double * realPar = block->realPar;
  int * intPar    = block->intPar;
  double *u = block->u[0];
  double val;

  BuffOut[0] = ANALOGOUT;
  BuffOut[1] = intPar[0];

  val = u[0];
  if (val>realPar[1]) val = realPar[1];
  if (val<realPar[0]) val = realPar[0];

  int16_t *value = (int16_t *) &BuffOut[2];
  *value = (int16_t) 4095*mapD2wD(val, realPar[0], realPar[1]);
  
  if (*value!=pins[intPar[0]]){
      pins[intPar[0]] = *value;
      write(fdSerial, BuffOut, MAXLEN);
    }
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  BuffOut[0] = ANALOGOUT;
  BuffOut[1] = intPar[0];
  int16_t *value = (int16_t *) &BuffOut[2];
  
  * value = 0;
  write(fdSerial, BuffOut, MAXLEN);
}

void ar2inoAO(int flag, python_block *block)
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


