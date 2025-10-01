#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <zlib.h>
#include "watdefs.h"
#include "date.h"
#include "afuncs.h"

/* A utility probably of use only to me.  The TLEs in 'old_tles'
have names YYYYMMDD.tle or YYYYMMDD.tle.gz.  Each file should be
used,  roughly speaking, over a time span extending halfway to its
neighboring file. 'tle_list.txt' shows how this works.  Maintaining
those date spans without messing them up can be annoying.

Run as './range *.tle.gz' in the 'old_tles' directory,  this will figure
out the ranges over which the TLEs ought to be used and output them in the
form desired by 'tle_list.txt'.  We rely here on the above command causing
main() to get the TLE names as command line arguments sorted in correct
order... which,  on most *nix/BSD* systems,  ought to be true.   */

static char *make_date_text( const long jd, char *buff)
{
   full_ctime( buff, (double)jd, FULL_CTIME_DATE_ONLY | FULL_CTIME_YMD
                  | FULL_CTIME_MONTHS_AS_DIGITS | FULL_CTIME_LEADING_ZEROES);
   buff[4] = buff[7] = '-';
   return( buff);
}

int main( const int argc, const char **argv)
{
   int i, j;
   long *jds = (long *)calloc( argc, sizeof( long));

   for( i = j = 1; i < argc; i++)
      {
      const long dmy = atol( argv[i]);

      if( dmy > 19570000 && dmy < 20570000 && !memcmp( argv[i] + 8, ".tle", 4))
         {
         int month, day;
         long year;

         day = (int)( dmy % 100L);
         month = (int)( (dmy / 100L) % 100L);
         year = dmy / 10000L;
         jds[j] = dmy_to_day( day, month, year, CALENDAR_GREGORIAN);
         argv[j] = argv[i];
         j++;
         }
      else
         printf( "# Ignoring '%s'\n", argv[i]);
      }
   for( i = 1; i < j; i++)
      {
      char t1[20], t2[20], buff[100], *tptr;
      long jd_start = 2436204L, jd_end;
      int n_tles = 0;
      gzFile ifile = gzopen( argv[i], "rb");
      const long JD_1970 = 2440587;  /* 1969 Dec 31.5 = JD 2440587 */

      assert( ifile);
      if( i > 1)
         jd_start = (jds[i - 1] + jds[i]) / 2;
      if( i < argc - 1)
         jd_end = (jds[i + 1] + jds[i]) / 2;
      else     /* assume 'all_tle.txt' is for 'right now' */
         jd_end = (JD_1970 + (long)time( NULL) / seconds_per_day + jds[i]) / 2;
      printf( "# Range: %s %s\n", make_date_text( jd_start, t1),
                                  make_date_text( jd_end, t2));
      while( gzgets( ifile, buff, sizeof( buff)))
         if( *buff == '1' && buff[1] == ' ' && buff[7] == 'U'
                  && buff[23] =='.' && strlen( buff) > 69)
            n_tles++;
      gzclose( ifile);
      printf( "# TLEs expected: %d\n", n_tles);
      strcpy( buff, argv[i]);
      tptr = strstr( buff, ".gz");
      if( tptr)
         *tptr = '\0';
      printf( "# Include old_tles/%s\n\n", buff);

      if( i == argc - 1)
         {
         printf( "# Range: %s %s\n", make_date_text( jd_end, t2), "2049-01-01");
         printf( "# Include all_tle.txt\n\n");
         }
      }

   printf( "# We don't actually expect to get TLEs from tle_list.txt itself:\n");
   printf( "# TLEs expected: 0\n");
   free( jds);
   return( 0);
}
