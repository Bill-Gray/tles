#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "watdefs.h"
#include "date.h"

/* A utility probably of use only to me.  The TLEs in this directory
have names YYYYMMDD.tle;  each file should be used,  roughly speaking,
over a time span extending halfway to its neighboring file.
'tle_list.txt' shows how this works.  Maintaining those date spans
without messing them up can be annoying.

Run as './range 20*.tle',  this will figure out the ranges over which
the TLEs ought to be used and output them in the form desired by
'tle_list.txt'.  We rely here on the above command causing main() to
get the TLE names as command line arguments sorted in correct order...
which,  on most *nix/BSD* systems,  ought to be true.  You do have to
modify the first line to deal with 'all_tle.txt',  but that's
relatively easy.  Compile with

gcc -Wall -Wextra -pedantic -o range range.c -I ~/include ~/lunar/liblunar.a -lm

*/

static char *make_date_text( const long jd, char *buff)
{
   full_ctime( buff, (double)jd, FULL_CTIME_DATE_ONLY | FULL_CTIME_YMD
                  | FULL_CTIME_MONTHS_AS_DIGITS | FULL_CTIME_LEADING_ZEROES);
   buff[4] = buff[7] = '-';
   return( buff);
}

int main( const int argc, const char **argv)
{
   int i, month, day;
   long year, *jds = (long *)calloc( argc, sizeof( long));

   for( i = 1; i < argc; i++)
      {
      long dmy = atol( argv[i]);

      day = (int)( dmy % 100L);
      month = (int)( (dmy / 100L) % 100L);
      year = dmy / 10000L;
      jds[i] = dmy_to_day( day, month, year, CALENDAR_GREGORIAN);
      }
   for( i = argc - 1; i > 0; i--)
      {
      char t1[20], t2[20], buff[100];
      long jd_start = 2436204L, jd_end = 3000000L;
      int n_tles = 0;
      FILE *ifile = fopen( argv[i], "rb");

      assert( ifile);
      if( i > 1)
         jd_start = (jds[i - 1] + jds[i]) / 2;
      if( i < argc - 1)
         jd_end = (jds[i + 1] + jds[i]) / 2;
      printf( "# Range: %s %s\n", make_date_text( jd_start, t1),
                                  make_date_text( jd_end, t2));
      while( fgets( buff, sizeof( buff), ifile))
         if( *buff == '1' && buff[1] == ' ' && buff[7] == 'U'
                  && buff[23] =='.' && strlen( buff) > 69)
            n_tles++;
      fclose( ifile);
      printf( "# TLEs expected: %d\n", n_tles);
      printf( "# Include old_tles/%s\n\n", argv[i]);
      }

   printf( "# We don't actually expect to get TLEs from tle_list.txt itself:\n");
   printf( "# TLEs expected: 0\n");
   return( 0);
}
