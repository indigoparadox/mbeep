
#include "mbeep.h"

#ifdef PLATFORM_XLIB
#include <time.h>
#include <alsa/asoundlib.h>
#elif defined( PLATFORM_DOS )
#include <dos.h>
#endif /* PLATFORM_XLIB */

#ifdef PLATFORM_XLIB
snd_pcm_t* g_pcm = NULL;
#endif /* PLATFORM_XLIB */

void beep( int freq_hz, int duration_ms, uint8_t flags ) {
#ifdef PLATFORM_XLIB
   uint8_t buffer[2400];
   int32_t retval = 0,
      i = 0,
      j = 0;

   /* Lots of constants here pulled from various posts and tweaked until they
    * kinda sound right-ish on our PC. Needs work, research, and refinement.
    * Also needs #ifdef porting for other platforms.
    */

   if( NULL == g_pcm ) {
      if( snd_pcm_open( &g_pcm, "default", 0, 0 ) ) {
         fprintf( stderr, "could not open sound device\n" );
         return;
      }
      snd_pcm_set_params( g_pcm, 1, 3, 1, 8000, 1, 20000 );
   }

   /* Duration here kinda chosen at random by what sounds right-ish. */
   for( i = 0 ; duration_ms / 150 > i ; i++ ) {
      snd_pcm_prepare( g_pcm );
      for( j = 0 ; 2400 > j ; j++ ) {
         buffer[j] = freq_hz > 0 ? (255 * j * freq_hz / 8000) : 0;
      }
      retval = snd_pcm_writei( g_pcm, buffer, 2400 );
      if( 0 > retval ) {
         snd_pcm_recover( g_pcm, retval, 0 );
      }
   }
#elif defined( PLATFORM_DOS )
   uint32_t freq_division = 0;
   uint8_t prev_keyb_status = 0;

   /* TODO: Needs cleanup with constants for port numbers, bitmasks, etc. */

   freq_division = 1193180 / freq_hz;

   /* Set the programmable interrupt timer to the given frequency. */
   outp( 0x43, 0xb6 );
   outp( 0x42, (uint8_t)freq_division );
   outp( 0x42, (uint8_t)(freq_division >> 8) );

   /* Set keyboard controller status to enable PC speaker. */
   prev_keyb_status = inp( 0x61 );
   if( prev_keyb_status != (prev_keyb_status | 0x03) ) {
      outp( 0x61, (prev_keyb_status | 0x03) );
   }

   delay( duration_ms );

   /* Turn off the PC speaker. */
   prev_keyb_status = inp( 0x61 );
   outp( 0x61, (prev_keyb_status & 0xfc) );

   /* TODO: Reset the PIT to its previous frequency. */

#endif /* PLATFORM_XLIB || PLATFORM_DOS */
}


