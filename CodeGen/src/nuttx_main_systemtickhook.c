#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <nuttx/semaphore.h>
#include <nuttx/config.h>

#ifdef HAVE_MLOCK
#include <sys/mman.h>
#endif

#ifdef CANOPEN
void canopen_synch(void);
#endif

#define XNAME(x,y)  x##y
#define NAME(x,y)   XNAME(x,y)

int NAME(MODEL,_init)(void);
int NAME(MODEL,_isr)(double);
int NAME(MODEL,_end)(void);
double NAME(MODEL,_get_tsamp)(void);

#define NSEC_PER_SEC  1000000000
#define USEC_PER_SEC	1000000

static volatile int end = 0;
static double T = 0.0;
static double Tsamp;
static int nTick_per_Tsamp;

/* Options presettings */
static char rtversion[] = "0.9";
static int prio = 99;
static int verbose = 0;
static int wait = 0;
static int extclock = 0;
double FinalTime = 0.0;

extern sem_t g_waitsem;

#ifdef CLOCK_MEASURE
static int timespecDiffUs(struct timespec t1, struct timespec t2)
{
	int diff;
	diff = 1000000 * ((int) t1.tv_sec - (int) t2.tv_sec);
	diff += ((int) t1.tv_nsec - (int) t2.tv_nsec) / 1000;
	return diff;
}
#endif

double get_run_time()
{
  return(T);
}

double get_Tsamp()
{
  return(Tsamp);
}

int get_priority_for_com(void)
{
  if (prio < 0)
    {
      return -1;
    }
  else
    {
      return prio - 1;
    }
}

static inline void tsnorm(struct timespec *ts)
{
  while (ts->tv_nsec >= NSEC_PER_SEC) {
    ts->tv_nsec -= NSEC_PER_SEC;
    ts->tv_sec++;
  }
}

static inline double calcdiff(struct timespec t1, struct timespec t2)
{
  double diff;
  diff = 1.0 * ((long) t1.tv_sec - (long) t2.tv_sec);
  diff += 1e-9*t1.tv_nsec - 1e-9*t2.tv_nsec;
  return (diff);
}

static void *rt_task(void *p)
{
  struct timespec t_next, t_current, t_isr, T0;
#ifdef CLOCK_MEASURE
  struct timespec measuretime;
#endif
  struct sched_param param;
  static int nTick = 0;
  static int cumulative = 0;
  int ret;

  param.sched_priority = prio;
  if(sched_setscheduler(0, SCHED_FIFO, &param)==-1){
    perror("sched_setscheduler failed");
    exit(-1);
  }

#ifdef HAVE_MLOCK
  mlockall(MCL_CURRENT | MCL_FUTURE);
#endif

  Tsamp = NAME(MODEL,_get_tsamp)();
  
  nTick_per_Tsamp = (int) (Tsamp/CONFIG_USEC_PER_TICK*1000000);

  t_isr.tv_sec =  0L;
  t_isr.tv_nsec = (long)(1e9*Tsamp);
  tsnorm(&t_isr);

  T=0;

  NAME(MODEL,_init)();
  sem_init(&g_waitsem, 0, 0);
  

#ifdef CANOPEN
  canopen_synch();
#endif

  /* get current time */
  clock_gettime(CLOCK_MONOTONIC,&t_current);
#ifdef CLOCK_MEASURE
  measuretime = t_current;
#endif
  T0 = t_current;
  
  while (!end)
    {
      while (sem_wait(&g_waitsem) >= 0) 
        {
          cumulative += 1;
          if (nTick == 0)
            {
              /* periodic task */
              T = calcdiff(t_current,T0);
              NAME(MODEL,_isr)(T);

#ifdef CANOPEN
              canopen_synch();
#endif

              if ((FinalTime >0) && (T >= FinalTime)) 
                {
                  break;
                }

              t_next.tv_sec = t_current.tv_sec + t_isr.tv_sec;
              t_next.tv_nsec = t_current.tv_nsec + t_isr.tv_nsec;
              tsnorm(&t_next);

              /* Check if Overrun */
              clock_gettime(CLOCK_MONOTONIC,&t_current);
              
#ifdef CLOCK_MEASURE
              if (timespecDiffUs(t_current, measuretime) > USEC_PER_SEC)
                {
                  fprintf(stderr, "%d\n", cumulative);
                  cumulative = 0;
                  measuretime = t_current;
                }
#endif

              if (t_current.tv_sec > t_next.tv_sec ||
                  (t_current.tv_sec == t_next.tv_sec && t_current.tv_nsec > t_next.tv_nsec))
                {
                  int usec = (t_current.tv_sec - t_next.tv_sec) * 1000000 + (t_current.tv_nsec -
                                 t_next.tv_nsec)/1000;
                  fprintf(stderr, "Base rate overrun by %d us\n", usec);
                  t_next = t_current;
                }
              t_current = t_next;
            }
          nTick = (nTick + 1) % nTick_per_Tsamp;
        }
    }
  NAME(MODEL,_end)();
  pthread_exit(0);
}

void endme(int n)
{
  end = 1;
}

void print_usage(void)
{
  puts(  "\nUsage:  'RT-model-name' [OPTIONS]\n"
         "\n"
         "OPTIONS:\n"
         "  -h  print usage\n"
	 "  -f <final time> set the final time of the execution\n"
	 "  -v  verbose output\n"
	 "  -p <priority>  set rt task priority (default 99)\n"
	 "  -e  external clock\n"
	 "  -w  wait to start\n"
	 "  -V  print version\n"
	 "\n");
}

static void proc_opt(int argc, char *argv[])
{
  int i;
  while((i=getopt(argc,argv,"ef:hp:vVw"))!=-1){
    switch(i){
    case 'h':
      print_usage();
      exit(0);
      break;
    case 'p':
      //prio = atoi(optarg);
      break;
    case 'v':
      verbose = 1;
      break;
    case 'w':
      wait = 1;
      break;
    case 'V':
      printf("Version %s\n",rtversion);
      exit(0);
      break;
    case 'f':
/*
      if (strstr(optarg, "inf")) {
        FinalTime = 0.0;
      } else if ((FinalTime = atof(optarg)) <= 0.0) {
        printf("-> Invalid final time.\n");
        exit(1);
      }
*/
      break;
    }
  }
}

int main(int argc,char** argv)
{
  pthread_t thrd;
  int fd;
  int uid;

  proc_opt(argc, argv);

  signal(SIGINT,endme);
  signal(SIGKILL,endme);

  pthread_create(&thrd,NULL,rt_task,NULL);

  pthread_join(thrd,NULL);
  return(0);
}
