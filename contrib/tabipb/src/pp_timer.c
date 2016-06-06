/*
 * pp_timer.c
 *
 *  Created on: Apr 23, 2011
 *      Author: ferosh
 */

/* Return 1 if the difference is negative, otherwise 0.  */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

  void timer_start(char *name);
  void timer_end(void);


int my_timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}

struct timeval pp_tvBegin, pp_tvEnd, pp_tvDiff;
char *name;
int someconstant=0;

void timer_start(char *n){
	   gettimeofday(&pp_tvBegin, NULL);
	   name =n;
}

void timer_end(void){
    gettimeofday(&pp_tvEnd, NULL);
   my_timeval_subtract(&pp_tvDiff, &pp_tvEnd, &pp_tvBegin);
   if(someconstant!=0)
   	printf("%d %ld.%06ld \n",someconstant,pp_tvDiff.tv_sec, pp_tvDiff.tv_usec);
   else
   	printf("%s %ld.%06ld\n ",name,pp_tvDiff.tv_sec, pp_tvDiff.tv_usec);

}


