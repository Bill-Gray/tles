#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Until October 2026,  we got Space-Track TLEs from the following sites.
The last two haven't actually worked for a while :

https://www.idb.com.au/files/TLE_DATA/ALL_TLE.ZIP
http://www.tle.info/data/ALL_TLE.ZIP
http://www.aus-city.com/data/ALL_TLE.ZIP

   The first file is still available,  but (as of October 2026) has started
dropping out some higher-altitude objects and everything with a six-digit
NORAD ID.  Fortunately,  the same site provides the data in JSON format :

https://www.idb.com.au/files/TLE_DATA/ALL_GP.JSON.ZIP

   The JSON file has all the objects we'd normally expect,  _and_ it is
relatively straightforward to extract the TLEs from it.  The following
code does just that.  See the 'get_sat' script for example use.   */

int main( void)
{
   FILE *ifile = fopen( "/tmp/ALL_GP.JSON", "rb");
   char *buff, *tptr;
   size_t len, bytes_read;

   if( !ifile)
      ifile = fopen( "ALL_GP.JSON", "rb");
   assert( ifile);
   fseek( ifile, 0L, SEEK_END);
   len = ftell( ifile);
   fseek( ifile, 0L, SEEK_SET);
   buff = tptr = (char *)malloc( len + 1);
   assert( buff);
   bytes_read = fread( buff, 1, len, ifile);
   if( bytes_read != len)
      {
      fprintf( stderr, "Read %ld of %ld bytes\n", (long)bytes_read, (long)len);
      return -1;
      }
   fclose( ifile);
   buff[len] = '\0';
   while( NULL != (tptr = strstr( tptr, "TLE_LINE")))
      {
      char *tptr2;

      tptr += 12;
      tptr2 = strchr( tptr, '"');
      *tptr2 = '\0';
      if( *tptr == '0')    /* object name line */
         {
         size_t i, j;

         tptr += 2;
         for( i = j = 0; tptr[i]; i++)
            if( tptr[i] != '\\')          /* remove backslashes */
               tptr[j++] = tptr[i];
         tptr[j] = '\0';
         }
      printf( "%s\n", tptr);
      tptr = tptr2 + 1;
      }
   free( buff);
   return( 0);
}
