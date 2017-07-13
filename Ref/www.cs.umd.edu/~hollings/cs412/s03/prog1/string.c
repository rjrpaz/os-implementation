// String library
// Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
// $Revision: 5.0 $

// This is free software.  You are permitted to use,
// redistribute, and modify it as specified in the file "COPYING".

// NOTE:
// These are slow and simple implementations of a subset of
// the standard C library string functions.

#include "string.h"
#include "malloc.h"
#include "screen.h"

void* memset( void* s, int c, size_t n )
{
    unsigned char* p = (unsigned char*) s;

    while ( n > 0 ) {
	*p++ = (unsigned char) c;
	--n;
    }

    return s;
}

void* memcpy( void *dst, const void* src, size_t n )
{
    unsigned char* d = (unsigned char*) dst;
    const unsigned char* s = (const unsigned char*) src;

    while ( n > 0 ) {
	*d++ = *s++;
	--n;
    }

    return dst;
}

size_t strlen( const char* s )
{
    size_t len = 0;
    while ( *s++ != '\0' )
	++len;
    return len;
}

int strcmp( const char* s1, const char* s2 )
{
    while ( 1 ) {
	int cmp = *s1 - *s2;
	if ( cmp != 0 || *s1 == '\0' || *s2 == '\0' )
	    return cmp;
	++s1;
	++s2;
    }
}

int strncmp( const char* s1, const char* s2, int limit )
{
    int i = 0;
    while ( i < limit ) {
	int cmp = *s1 - *s2;
	if ( cmp != 0 || *s1 == '\0' || *s2 == '\0' )
	    return cmp;
	++s1;
	++s2;
	++i;
    }

    // limit reached and equal
    return 0;
}
char *strcat(char *s1, char *s2)
{
    char *t1;

    t1 = s1;
    while (*s1) s1++;
    while(*s2) *s1++ = *s2++;
    *s1 = '\0';

    return t1;
}

char *strcpy(char *dest, const char *src)
{
    char *ret;

    ret = dest;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return ret;
}

char *strdup(char *s1)
{
    char *ret;

    ret = Malloc_Atomic(strlen(s1) + 1);
    strcpy(ret, s1);

    return ret;
}

int atoi(char *buf) 
{
    int ret = 0;

    while (*buf >= '0' && *buf <= '9') {
       ret *= 10;
       ret += *buf - '0';
       buf++;
    }

    return ret;
}
