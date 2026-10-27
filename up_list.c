#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/* This code is used to add a set of TLEs to 'tle_list.txt'. You
specify the filename of a TLE set on the command line;  that file is
read in and its time range and NORAD and COSPAR (international) IDs
are extracted.  The code then reads 'tle_list.txt' and looks for the line
starting with

  note : next artsat will be 79857,  25000ZYH

   It inserts the lines for the new TLE file,  specifying date range,
IDs,  and file name,  and then emits the 'note :' file,  suitably updated
to the previous NORAD number and COSPAR letter designation (for the above,
79856,  25000 ZYG).

   At present,  output is to /tmp/tle_list.txt.  Compile with

cc -Wall -Wextra -pedantic -O3 -o up_list up_list.c   */

/* The letter part of international designations is an uppercase letter,
with O and I skipped.  So... */

static char decrement_intl_id( const char c)
{
   if( c == 'A')        /* wrap around */
      return( 'Z');
   else if( c == 'P')
      return( 'N');
   else if( c == 'J')
      return( 'H');
   else
      return( (char)( c - 1));
}

int main( const int argc, const char **argv)
{
   FILE *ifile = fopen( argv[1], "rb"), *ofile;
   char buff[300], range[50], ids[50];

   assert( argc == 2);
   assert( ifile);
   strcpy( range, "# Range: ---------- ----------");
   *ids = '\0';
   while( !*ids && fgets( buff, sizeof( buff), ifile))
      if( !memcmp( buff, "# Ephemeris ", 12))
         {
         char *loc = NULL;
         const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
         const char *monthptr;
         int month;

         if( buff[12] == 's')
            loc = range + 9;
         if( buff[12] == 'e')
            loc = range + 20;
         assert( loc);
         memcpy( loc, buff + 26, 4);         /* year */
         memcpy( loc + 8, buff + 19, 2);     /* day */
         if( loc[8] == ' ')
            loc[8] = '0';
         buff[25] = '\0';
         monthptr = strstr( months, buff + 22);
         assert( monthptr);
         month = (int)( monthptr - months) / 3 + 1;
         assert( month > 0 && month < 13);
         sprintf( loc + 5, "%02d", month);
         loc[4] = loc[7] = '-';
         }
      else if( buff[0] == '1' && buff[1] == ' ' && buff[7] == 'U')
         {
         strcpy( ids, "# ID: -----  YYMMMLLL");
         memcpy( ids + 6, buff + 2, 5);            /* NORAD ID */
         memcpy( ids + 13, buff + 9, 8);           /* COSPAR (int'l) ID */
         printf( "%s\n%s\n", range, ids);
         }
   fclose( ifile);
   ifile = fopen( "tle_list.txt", "rb");
   assert( ifile);
   ofile = fopen( "/tmp/tle_list.txt", "wb");
   assert( ofile);
   *buff = '\0';
   while( fgets( buff, sizeof( buff), ifile) && memcmp( buff, "  note : next", 13))
      fputs( buff, ofile);
   fprintf( ofile, "%s\n%s\n", range, ids);
   fprintf( ofile, "# Include %s\n\n", argv[1]);
   sprintf( buff + 29, "%d", atoi( buff + 29) - 1);  /* decrement NORAD number */
   buff[34] = ',';
   buff[44] = decrement_intl_id( buff[44]);
   if( buff[44] == 'Z')       /* wrapped around */
      {
      buff[43] = decrement_intl_id( buff[43]);
      if( buff[43] == 'Z')                   /* wraparound.  _Very_ unlikely to */
         buff[42] = decrement_intl_id( buff[42]);  /* happen; we don't get that */
      }                                          /* many bits of junk per year. */
   fputs( buff, ofile);
   while( fgets( buff, sizeof( buff), ifile))
      fputs( buff, ofile);
   fclose( ifile);
   fclose( ofile);
   return( 0);
}
