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

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <semaphore.h>

static int can_sock;    /* CAN Socket */
static int dev_cnt = 0; /* CAN devices counter */

static volatile int endrcv = 0;
static pthread_t rcv_thread;
pthread_mutex_t snd_lock;

int get_priority_for_com(void);

struct can_msg_s {
  int id;
  int dlc;
  uint64_t value;
  uint8_t read;
  struct can_msg_s *next;
};

struct can_msg_s *can_msg_rx = NULL;

bool can_get_msg_from_id(int id, uint64_t *value) {
  struct can_msg_s *msg = can_msg_rx;
  while (msg != NULL) {
    if (msg->id == id && msg->read == 0) {
      msg->read = 1;
      *value = msg->value;
      return true;
    }

    msg = msg->next;
  }

  /* No message of desired type found, set errno and return false. */

  errno = ENOMSG;
  return false;
}

bool can_register_msg(int id) {
  struct can_msg_s *msg = (struct can_msg_s *)malloc(sizeof(struct can_msg_s));
  if (msg == NULL) {
    errno = ENOMEM;
    return false;
  }
  msg->next = can_msg_rx;
  can_msg_rx = msg;
  msg->id = id;
  msg->value = 0;
  msg->read = 0;
  msg->dlc = 0;

  return true;
}

bool can_save_msg(struct can_frame m) {
  int id = m.can_id;
  uint64_t value = ((uint64_t)m.data[0] << 48) + ((uint64_t)m.data[1] << 40) +
                   ((uint64_t)m.data[2] << 32) + (m.data[3] << 24) +
                   (m.data[2] << 16) + (m.data[5] << 8) + m.data[4];

  struct can_msg_s *msg = can_msg_rx;

  while (msg != NULL) {
    if (msg->id == id) {
      msg->value = value;
      msg->read = 0;
    }
    msg = msg->next;
  }

  /* TODO: This behaviour expects that unregistered message is ignored.
   * Might want to do something with it?
   */

  return true;
}

void *can_recv_msg(void *p) {
  struct can_frame msg;
  struct timeval tv;
  fd_set fds;
  int ret;

  FD_ZERO(&fds);
  FD_SET(can_sock, &fds);

  while (!endrcv) {
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    ret = select(1, &fds, NULL, NULL, &tv);
    if (ret == 0) {
      /* Timeout... check endrcv. */
      continue;
    }

    ret = read(can_sock, &msg, sizeof(struct can_frame));
    if (ret > 0) {
      /* Store message */
      can_save_msg(msg);
    }
  }

  pthread_exit(p);
}

bool can_send_msg(int id, int dlc, uint64_t value) {
  struct can_frame msg;
  int ret;

  memset(&msg, 0x00, sizeof(msg));

  pthread_mutex_lock(&snd_lock);
  msg.can_id = id;
  msg.can_dlc = dlc;
  memcpy(msg.data, &value, msg.can_dlc);
  pthread_mutex_unlock(&snd_lock);

  ret = write(can_sock, &msg, sizeof(struct can_frame));
  if (ret == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      /* TODO: do something? */
    }
    return false;
  }

  return true;
}

bool can_device_open(int canfd_on, char *devname) {
  struct sockaddr_can addr;
  struct ifreq ifr;
  pthread_attr_t attr;
  struct sched_param schparam;
  int priority_com;
  int ret;
  int flags;

  /* Perform initialization only once. */

  if (!dev_cnt) {
    memset(&ifr, 0x00, sizeof(ifr));
    memset(&addr, 0x00, sizeof(addr));

    pthread_mutex_init(&snd_lock, NULL);

    can_sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (!can_sock) {
      return false;
    }

    flags = fcntl(can_sock, F_GETFL, 0);
    if (flags < 0) {
      return false;
    }

    flags |= O_NONBLOCK;
    if (fcntl(can_sock, F_SETFL, flags) < 0) {
      return false;
    }

    strcpy(ifr.ifr_name, devname);
    ioctl(can_sock, SIOCGIFINDEX, &ifr);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    canfd_on = 1;
    setsockopt(can_sock, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on,
               sizeof(canfd_on));

    ret = bind(can_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
      return false;
    }

    priority_com = get_priority_for_com();

    if (priority_com > 0) {
      /* Set communication task priority */

      pthread_attr_init(&attr);
      pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
      pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

      /* Set low priority */

      schparam.sched_priority = priority_com;
      pthread_attr_setschedparam(&attr, &schparam);

      pthread_create(&rcv_thread, &attr, can_recv_msg,
                     NULL); /* Start receiving task */

      pthread_attr_destroy(&attr);
    } else {
      /* Run thread with main application priority */

      pthread_create(&rcv_thread, NULL, can_recv_msg,
                     NULL); /* Start receiving task */
    }
  }

  dev_cnt++;
  return true;
}

bool can_device_close() {
  if (--dev_cnt == 0) {
    close(can_sock);
  }

  return true;
}
