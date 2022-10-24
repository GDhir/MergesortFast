#include "omp.h"
#include <stdio.h>
void msort_seq(int a[], int b[], int lo, int hi);
void merge(int a[], int b[], int lo, int mid, int hi);
//void merge_par(int a[], int b[], int lo, int mid, int hi);
void merge_par_desc( int a[], int b[], int lo, int mid, int hi, int nelems );
void merge_par_asc( int a[], int b[], int lo, int mid, int hi, int nelems );

void merge_par_asc( int a[], int b[], int lo, int mid, int hi, int nelems ) {
  int h,i,j,k;
  h = lo;
  i = lo;
  j = mid+1;
  int iter = 0;

  while ( ( h <= mid ) && ( j <= hi ) && (iter < nelems) )
  { 
    if (a[h]<=a[j]) b[i++] = a[h++]; else b[i++] = a[j++]; 
    iter += 1;
  }

  if( iter < nelems ) {
    if ( h>mid )
      {  
        for( k = j; ( k <= hi) && (iter < nelems ) ; k++ ) {
          b[i++] = a[k]; 
          iter += 1;
        }
      }
    else
      {  
        for( k = h; ( k <= mid) && (iter < nelems ); k++ ) {
          b[i++] = a[k]; 
          iter += 1;
      }
    }
  }
}

void merge_par_desc( int a[], int b[], int lo, int mid, int hi, int nelems ) {
  int h,i,j,k;
  h = mid;
  i = hi;
  j = hi;
  int iter = 0;

  while ( ( h >= lo ) && ( j > mid ) && ( iter < nelems ) )
  { 
    if ( a[h] > a[j] ){
       b[i--] = a[h--]; 
    }
    else {
      b[i--] = a[j--]; 
    }
    iter += 1;
  }

  if( iter < nelems ) {

    if ( j > mid ) {  
        for( k = j; (k > mid) && (iter < nelems) ; k-- ) { 
          b[i--] = a[k];
          iter += 1; 
        }
    }
    else {  
        for( k = h; (k >= lo) && (iter < nelems) ; k--) { 
          b[i--] = a[k];
          iter += 1;
        } 
    }
  }

}

#define N (10000001)

void msort_par(int a[], int b[], int lo, int hi)
{
    int mid, n, nelems;
    if( hi - lo + 1 > 40000 )
      {
        n = hi - lo + 1;
        nelems = n/2;
        mid = (lo + hi) / 2;

        omp_set_nested(1);
        #pragma omp parallel num_threads(2)
        {
          #pragma omp sections
          {
            #pragma omp section 
            {
              msort_par(a, b, lo, mid);
            }
            #pragma omp section 
            {
              msort_par(a, b, mid + 1, hi);
            }
          }
          #pragma omp sections 
          {
              #pragma omp section
              merge_par_asc( a, b, lo, mid, hi, n - nelems );

              #pragma omp section
              merge_par_desc( a, b, lo, mid, hi, nelems );

          }
          #pragma omp for
          for( int k = lo ; k <= hi; k++ ) {
                a[k] = b[k];
          }
        }
      }
      else {
        msort_seq( a, b, lo, hi );
      }
}
