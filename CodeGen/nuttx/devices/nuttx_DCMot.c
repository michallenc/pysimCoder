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

#include <nuttx/timers/pwm.h>
#include <nuttx/ioexpander/gpio.h>

#define RANGE   65536

static int pwm1;
static int pwm2;
static int do1;
static int do2;
static struct pwm_info_s info;
static double freq;
char pwm1_dev[] = "dev/pwm1";
char pwm2_dev[] = "dev/pwm2";
char do1_dev[] = "dev/gpout0";
char do2_dev[] = "dev/gpout1";

/****************************************************************************
 * Name: init
 *
 * Description:
 *   Open the device and set characteristics of PWM.
 *
 ****************************************************************************/

static void init(python_block *block)
{
  double * realPar = block->realPar;
  int ret;

  pwm1 = open(pwm1_dev, O_RDONLY);
  if(pwm1 < 0)
    {
      fprintf(stderr, "Error opening device: %s\n", pwm1_dev);
      exit(1);
    }
  pwm2 = open(pwm2_dev, O_RDONLY);
  if(pwm2 < 0)
    {
      fprintf(stderr, "Error opening device: %s\n", pwm2_dev);
      close(pwm1);
      exit(1);
    }

  freq = realPar[0];

  info.duty = 0;

  info.frequency = freq;

  ret = ioctl(pwm1, PWMIOC_SETCHARACTERISTICS,
              (unsigned long)((uintptr_t) &info));
  if (ret < 0)
    {
      fprintf(stderr, "pwm_main: ioctl(PWMIOC_SETCHARACTERISTICS) failed\n");
      close(pwm1);
      close(pwm2);
      exit(1);
    }

  ret = ioctl(pwm2, PWMIOC_SETCHARACTERISTICS,
              (unsigned long)((uintptr_t) &info));
  if (ret < 0)
    {
      fprintf(stderr, "pwm_main: ioctl(PWMIOC_SETCHARACTERISTICS) failed\n");
      close(pwm1);
      close(pwm2);
      exit(1);
    }

  do1 = open(do1_dev, O_RDWR);
  if (do1 < 0)
    {
      fprintf(stderr, "ERROR: Failed to open %s\n", do1_dev);
      exit(1);
    }
  
  do2 = open(do2_dev, O_RDWR);
  if (do2 < 0)
    {
      fprintf(stderr, "ERROR: Failed to open %s\n", do2_dev);
      exit(1);
    }
  
  bool outvalue = true;
  ret = ioctl(do1, GPIOC_WRITE, (unsigned long)outvalue);
  if (ret < 0){
    fprintf(stderr,"ERROR: Failed to write value to %s\n", do1_dev);
    close(do1);
    exit(1);
  }
  ret = ioctl(do2, GPIOC_WRITE, (unsigned long)outvalue);
  if (ret < 0){
    fprintf(stderr,"ERROR: Failed to write value to %s\n", do2_dev);
    close(do2);
    exit(1);
  }
}

/****************************************************************************
 * Name: inout
 *
 * Description:
 *   Compute and set PWM duty cycle based on input value.
 *
 ****************************************************************************/

static void inout(python_block *block)
{
  int ret;
  double *u = block->u[0];
  double val  = u[0];
  info.frequency = freq;

  if (val < 0)
  {
    ret = ioctl(pwm2, PWMIOC_STOP, 0);
    if (ret < 0)
    {
        fprintf(stderr, "pwm_main: ioctl(PWMIOC_STOP) failed: %d\n", errno);
        close(pwm2);
        exit(1);
    }
    if (val < -3.3) val = -3.3;
    val = val * (-1);

    double value = mapD2wD(val, 0, 3.3);
    info.duty = (uint32_t) (value*RANGE);

    ret = ioctl(pwm1, PWMIOC_SETCHARACTERISTICS,
              (unsigned long)((uintptr_t) &info));
    if (ret < 0) {
        fprintf(stderr,"pwm_main: ioctl(PWMIOC_SETCHARACTERISTICS) failed: %d\n",
	    errno);
        close(pwm1);
        exit(1);
    }
    ret = ioctl(pwm1, PWMIOC_START, 0);
    if (ret < 0)
    {
      fprintf(stderr,"pwm_main: ioctl(PWMIOC_START) failed\n");
      close(pwm1);
      exit(1);
    }
  }
  else
  {
    ret = ioctl(pwm1, PWMIOC_STOP, 0);
    if (ret < 0)
    {
        fprintf(stderr, "pwm_main: ioctl(PWMIOC_STOP) failed: %d\n", errno);
        close(pwm1);
        exit(1);
    }
    if (val > 3.3) val = 3.3;

    double value = mapD2wD(val, 0, 3.3);
    info.duty = (uint32_t) (value*RANGE);

    ret = ioctl(pwm2, PWMIOC_SETCHARACTERISTICS,
              (unsigned long)((uintptr_t) &info));
    if (ret < 0) {
        fprintf(stderr,"pwm_main: ioctl(PWMIOC_SETCHARACTERISTICS) failed: %d\n",
	    errno);
        close(pwm2);
        exit(1);
    }
    ret = ioctl(pwm2, PWMIOC_START, 0);
    if (ret < 0)
    {
      fprintf(stderr,"pwm_main: ioctl(PWMIOC_START) failed\n");
      close(pwm2);
      exit(1);
    }
  }
}

/****************************************************************************
 * Name: end
 *
 * Description:
 *   Stop PWM pulse and close the device.
 *
 ****************************************************************************/

static void end(python_block *block)
{
}

/****************************************************************************
 * Name: nuttx_PWM
 *
 * Description:
 *   Call needed function based on input flag.
 *
 ****************************************************************************/

void nuttx_DCMot(int flag, python_block *block)
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
