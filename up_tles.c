#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

/* Code to update a TLE file.  With ETBOs and maneuvering objects,
I frequently find that I've got an existing TLE file that just needs
to be corrected/extended after a certain date,  to accommodate new
data.  In such a case,  I'll run a set of TLEs for the desired time
span and use this program to merge the 'new' file in with the
existing file of TLE.

   This assumes you've got the new TLEs in /tmp/z.tle and are updating
tles in the file specified by argv[1].  Compile with

gcc -Wall -Wextra -pedantic -Werror -o up_tles up_tles.c

   This will merge in the new/corrected tles from /tmp/z.tle,
add in a comment about the orbital elements used in the update,
and extend the arc if the TLEs in /tmp/z.tle run beyond what was
originally present.  It'll warn you against certain common errors
(mismatched IDs,  etc.),  but caution is advised. */

typedef struct
   {
   double mjd_start, mjd_end, step;
   char norad_id[6], cospar_id[9];
   char date_start[12], date_end[12];
   } tle_info_t;

static int get_tle_info( tle_info_t *t, FILE *ifile)
{
   char buff[200];

   memset( t, 0, sizeof( tle_info_t));
   while( fgets( buff, sizeof( buff), ifile))
      {
      if( !memcmp( buff, "# Ephem range:", 14))
         sscanf( buff + 14, "%lf %lf %lf", &t->mjd_start, &t->mjd_end, &t->step);
      else if( !memcmp( buff, "# Ephemeris start:", 18))
         memcpy( t->date_start, buff + 19, 11);
      else if( !memcmp( buff, "# Ephemeris end:  ", 18))
         memcpy( t->date_end, buff + 19, 11);
      else if( !memcmp( buff, "# MJD ", 6) && fgets( buff, sizeof( buff), ifile)
                                           && fgets( buff, sizeof( buff), ifile))
            {
            assert( buff[0] == '1');
            assert( buff[1] == ' ');
            memcpy( t->norad_id, buff + 2, 5);
            assert( buff[7] == 'U');
            memcpy( t->cospar_id, buff + 9, 8);
            fseek( ifile, 0L, SEEK_SET);
            return( 0);
            }
      }
   assert( 0);
   return( -1);
}

int main( const int argc, const char **argv)
{
   FILE *add_file = fopen( "/tmp/z.tle", "rb"), *ifile, *ofile;
   char buff[200];
   tle_info_t add, orig;
   time_t t0 = time( NULL);

   assert( add_file);
   get_tle_info( &add, add_file);
   printf( "Addendum runs from %s = MJD %.3f to %s = MJD %.3f\n", add.date_start,
                      add.mjd_start, add.date_end, add.mjd_end);
   printf( "Addendum IDs are '%s', '%s'\n",
                      add.norad_id, add.cospar_id);
   if( argc != 2)
      {
      fprintf( stderr, "You have to specify the name of the 'current' .tle file\n"
                       "as a command-line argument.  '/tmp/z.tle' will be used\n"
                       "to update it (if identifiers and time spans match properly).\n");
      return( -1);
      }
   ifile = fopen( argv[1], "rb");
   if( !ifile)
      {
      fprintf( stderr, "TLE file '%s' not opened : ", argv[1]);
      perror( NULL);
      return( -2);
      }
   get_tle_info( &orig, ifile);
   printf( "Original runs from %s = MJD %.3f to %s = MJD %.3f\n", orig.date_start,
                      orig.mjd_start, orig.date_end, orig.mjd_end);
   printf( "Original IDs are '%s', '%s'\n",
                      orig.norad_id, orig.cospar_id);
   assert( add.mjd_start > orig.mjd_start);
   if( add.mjd_end != orig.mjd_end)
      printf( "**** END DATES DON'T MATCH ****\n");
   assert( add.step == orig.step);
   assert( !strcmp( orig.cospar_id, add.cospar_id));
   assert( !strcmp( orig.norad_id, add.norad_id));
   ofile = fopen( "up.tle", "wb");
   assert( ofile);
   while( fgets( buff, sizeof( buff), ifile))
      {
      if( !memcmp( buff, "# Ephemeris end:  ", 18))
         memcpy( buff + 19, add.date_end, 11);
      else if( !memcmp( buff, "# Ephem range:", 14))
         {
         snprintf( buff + 28, 13, "%.6f", add.mjd_end);
         buff[40] = ' ';
         }
      else if( !memcmp( buff, "# 1 NoradU COSPAR", 17))
         {
         char ibuff[200];
         int got_elements = 0;

         fprintf( ofile, "# Elements after %s :\n#\n", add.date_start);
         while( !got_elements && fgets( ibuff, sizeof( ibuff), add_file))
            if( !memcmp( ibuff, "# Orbital elements:", 19))
               {
               while( strlen( ibuff) > 4 && fgets( ibuff, sizeof( ibuff), add_file))
                   fprintf( ofile, "%s", ibuff);
               got_elements = 1;
               }
         assert( got_elements);
         }
      else if( !memcmp( buff, "# MJD ", 6) && atof( buff + 6) == add.mjd_start)
         {
         int got_it = 0;

         fprintf( ofile, "%s", buff);
         fseek( ifile, 0L, SEEK_END);
         while( !got_it && fgets( buff, sizeof( buff), add_file))
            got_it = (!memcmp( buff, "# MJD ", 6) && atof( buff + 6) == add.mjd_start);
         assert( got_it);
         while( fgets( buff, sizeof( buff), add_file))
            fprintf( ofile, "%s", buff);
         break;
         }

      fprintf( ofile, "%s", buff);
      }
   fprintf( ofile, "Updated %.24s UTC\n",
                               asctime( gmtime( &t0)));
   fclose( ofile);
   fclose( ifile);
   fclose( add_file);
   if( rename( argv[1], "old.tle"))
      {
      fprintf( stderr, "'%s' -> 'old.tle' ", argv[1]);
      perror( "Rename failed\n");
      }

   if( rename( "up.tle", argv[1]))
      {
      fprintf( stderr, "'up.tle' -> '%s' ", argv[1]);
      perror( "Rename failed\n");
      }

   return( 0);
}
