#include "omp.h"
#include <stdio.h>
#include <math.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define CEILI(x, y) ((x - 1) / y + 1)

void msort_seq(int a[], int b[], int lo, int hi);
void merge(int a[], int b[], int lo, int mid, int hi);

void msort_par(int a[], int b[], int lo, int hi)
{
  int npairs, partsize;
  int n = hi - lo + 1;

#pragma omp parallel
  {
#pragma omp single
    {
      npairs = omp_get_num_threads();
      partsize = CEILI(n, npairs);
    }

#pragma omp for schedule(static, 1)
    for (int i = 0; i < npairs; i++)
    {

      msort_seq(a, b, i * partsize, MIN((i + 1) * partsize - 1, hi));
    }
  }

  npairs = CEILI(npairs, 2);
  int ilo, ihi, mid;
  int abort = 0;

#pragma omp parallel
  {
    while (!abort)
    {

#pragma omp single
      {
        partsize = partsize * 2;
      }

#pragma omp for schedule(auto) private(ilo, mid, ihi)
      for (int i = 0; i < npairs; i++)
      {

        ilo = i * partsize;

        if (i == npairs - 1)
        {

          ihi = MIN(hi, i * partsize + partsize - 1);

          if (ihi - ilo + 1 > partsize / 2)
          {

            mid = ilo + partsize / 2 - 1;

            merge(a, b, ilo, mid, ihi);
          }
        }
        else
        {

          ihi = i * partsize + partsize - 1;
          mid = (ilo + ihi) / 2;

          merge(a, b, ilo, mid, ihi);
        }
      }

#pragma omp single
      {
        if (npairs == 1)
        {
          abort = 1;
        }
        npairs = CEILI(npairs, 2);
      }
    }
  }
}
