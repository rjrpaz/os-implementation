#include <stdarg.h>
#include "libuser.h"

void Put_Char( int c )
{
    char buf[2];
    buf[0] = c;
    buf[1] = '\0';
    Print_String( buf );
}


// Helper for %d format in Print() function.
static void Format_D( char* buf, int val )
{
    char stack[16];
    int top = 0;
    Boolean negative;
    unsigned uval;

    // Convert to sign and unsigned magnitude.
    if ( val < 0 ) {
	negative = TRUE;
	uval = -val;
    }
    else {
	negative = FALSE;
	uval = val;
    }

    // Convert magnitude to decimal.  We do this in order of least
    // significant to most significant digit.
    do {
	int digit = uval % 10;
	stack[top++] = digit + '0';
	uval /= 10;
    }
    while ( uval > 0 );

    // Add leading minus sign if negative.
    if ( negative ) {
	*buf++ = '-';
    }

    // Put formatted characters in the output buffer.
    do {
	*buf++ = stack[--top];
    }
    while ( top > 0 );

    // Terminate the output buffer.
    *buf = '\0';
}

// Helper function for %x format in Print() function.
static void Format_X( char* buf, int val )
{
    int i;

    for ( i = 28; i >= 0; i -= 4 ) {
	int x = (val >> i) & 0xf;
	*buf++ = "0123456789abcdef"[x];
    }
    *buf = '\0';
}


// Formatted output to the screen.
// Supports a limited set of printf()-style format options.
void Print( const char* fmt, ... )
{
    char buf[64];
    va_list args;
    int ival;
    const char* sval;

    va_start( args, fmt );

    while ( *fmt != '\0' ) {
	switch ( *fmt ) {
	case '%':
	    ++fmt;
	    switch ( *fmt ) {
	    case 'd':
		ival = va_arg( args, int );
		Format_D( buf, ival );
		Print_String( buf );
		break;

	    case 'x':
		ival = va_arg( args, int );
		Format_X( buf, ival );
		Print_String( buf );
		break;

	    case 's':
		sval = va_arg( args, const char* );
		Print_String( sval );
		break;

	    case 'c':
		ival = va_arg( args, int );
		Put_Char( ival & 0xff );
		break;

	    default:
		Put_Char( *fmt );
		break;
	    }
	    break;

	default:
	    Put_Char( *fmt );
	    break;
	}

	++fmt;
    }
    va_end( args );
}

