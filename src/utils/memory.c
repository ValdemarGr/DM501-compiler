#include <stdio.h>
#include <stdlib.h>
#if !__APPLE__
#include <malloc.h>
#endif

void *Malloc(unsigned n)
{
  void *p;
  if(!(p = malloc(n)))
  {
    fprintf(stderr,"Malloc(%d) failed.\n",n);
    fflush(stderr);
    abort();
  }
  return p;
}
