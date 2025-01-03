#include "../game/q_shared.h"
#include "q_string.h"


/*
============================================================================

LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/


int Q_isprint(int c)
{
    if (c >= 0x20 && c <= 0x7E)
        return (1);
    return (0);
}

int Q_islower(int c)
{
    if (c >= 'a' && c <= 'z')
        return (1);
    return (0);
}

int Q_isupper(int c)
{
    if (c >= 'A' && c <= 'Z')
        return (1);
    return (0);
}

int Q_isalpha(int c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return (1);
    return (0);
}

char* Q_strrchr(const char* string, int c)
{
    char cc = c;
    char *s;
    char *sp=(char *)0;

    s = (char*)string;

    while (*s)
    {
        if (*s == cc)
            sp = s;
        s++;
    }

    if (cc == 0)
        sp = s;

    return sp;
}

/*
=============
Q_strncpyz

Safe strncpy that ensures a trailing zero
=============
*/
void Q_strncpyz( char *dest, const char *src, int destsize )
{
    // bk001129 - also NULL dest
    if (!dest) {
        Com_Error(ERR_FATAL, "Q_strncpyz: NULL dest");
    }
    if ( !src ) {
        Com_Error(ERR_FATAL, "Q_strncpyz: NULL src");
    }
    if (destsize < 1) {
        Com_Error(ERR_FATAL,"Q_strncpyz: destsize < 1"); 
    }

    strncpy(dest, src, destsize-1);
    dest[destsize-1] = 0;
}

qboolean Q_strgtr(const char* s0, const char* s1)
{
    int l0, l1, i;

    l0 = strlen(s0);
    l1 = strlen(s1);

    if (l1 < l0) {
        l0 = l1;
    }

    for (i = 0; i < l0; i++) {
        if (s1[i] > s0[i]) {
            return qtrue;
        }
        if (s1[i] < s0[i]) {
            return qfalse;
        }
    }

    return qfalse;
}

int Q_stricmpn(const char* s1, const char* s2, int n)
{
    int c1, c2;

    // bk001129 - moved in 1.17 fix not in id codebase
    if (s1 == NULL) {
        if (s2 == NULL)
            return 0;
        else
            return -1;
    }
    else if (s2 == NULL)
        return 1;

    do {
        c1 = *s1++;
        c2 = *s2++;

        if (!n--) {
            return 0;		// strings are equal until end point
        }

        if (c1 != c2) {
            if (c1 >= 'a' && c1 <= 'z') {
                c1 -= ('a' - 'A');
            }
            if (c2 >= 'a' && c2 <= 'z') {
                c2 -= ('a' - 'A');
            }
            if (c1 != c2) {
                return c1 < c2 ? -1 : 1;
            }
        }
    } while (c1);

    return 0;		// strings are equal
}

int Q_strncmp (const char *s1, const char *s2, int n)
{
    int	c1, c2;

    do {
        c1 = *s1++;
        c2 = *s2++;

        if (!n--) {
            return 0;		// strings are equal until end point
        }

        if (c1 != c2) {
            return c1 < c2 ? -1 : 1;
        }
    } while (c1);

    return 0;		// strings are equal
}

int Q_stricmp (const char* s1, const char* s2)
{
    return (s1 && s2) ? Q_stricmpn (s1, s2, 99999) : -1;
}


char* Q_strlwr(char* s1)
{
    char* s;

    s = s1;
    while (*s) {
        *s = tolower(*s);
        s++;
    }

    return s1;
}

char* Q_strupr(char* s1)
{
    char* s;

    s = s1;
    while (*s) {
        *s = toupper(*s);
        s++;
    }

    return s1;
}


// never goes past bounds or leaves without a terminating 0
void Q_strcat(char* dest, int size, const char* src)
{
    int l1;

    l1 = strlen(dest);
    if (l1 >= size) {
        Com_Error(ERR_FATAL, "Q_strcat: already overflowed");
    }

    Q_strncpyz(dest + l1, src, size - l1);
}


int Q_PrintStrlen(const char* string)
{
    int len;
    const char* p;

    if(!string) {
        return 0;
    }

    len = 0;
    p = string;
    while(*p) {
        if(Q_IsColorString(p)) {
            p += 2;
            continue;
        }
        p++;
        len++;
    }

    return len;
}


char* Q_CleanStr(char* string)
{
    char* d;
    char* s;
    int	c;

    s = string;
    d = string;
    while ((c = *s) != 0) {
        if (Q_IsColorString(s)) {
            s++;
        }		
        else if (c >= 0x20 && c <= 0x7E) {
            *d++ = c;
        }
        s++;
    }

    *d = '\0';

    return string;
}


void Q_CDECL Com_sprintf(char* dest, int size, const char* fmt, ...)
{
    int len;
    va_list argptr;
    char bigbuffer[32000];	// big, but small enough to fit in PPC stack

    va_start (argptr,fmt);
    len = vsprintf (bigbuffer,fmt,argptr);
    va_end (argptr);
    if (len >= sizeof(bigbuffer)) {
        Com_Error( ERR_FATAL, "Com_sprintf: overflowed bigbuffer" );
    }
    if (len >= size) {
        Com_Printf ("Com_sprintf: overflow of %i in %i\n", len, size);
#ifdef	_DEBUG
        __asm {
            int 3;
        }
#endif
    }

    Q_strncpyz (dest, bigbuffer, size );
}

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char* Q_CDECL va(char *format, ...)
{
    va_list argptr;
    static char string[2][32000];	// in case va is called by nested functions
    static int index = 0;
    char* buf;

    buf = string[index & 1];
    index++;

    va_start (argptr, format);
    vsprintf (buf, format,argptr);
    va_end (argptr);

    return buf;
}

/*
============
Com_StringContains
============
*/
const char* Com_StringContains(const char* str1, const char* str2, int casesensitive)
{
    int len, i, j;

    len = strlen(str1) - strlen(str2);

    for (i = 0; i <= len; i++, str1++) {
        for (j = 0; str2[j]; j++) {
            if (casesensitive) {
                if (str1[j] != str2[j]) {
                    break;
                }
            }
            else {
                if (toupper(str1[j]) != toupper(str2[j])) {
                    break;
                }
            }
        }
        if (!str2[j]) {
            return str1;
        }
    }

    return NULL;
}

/*
============
Com_Filter
============
*/
int Com_Filter(const char* filter, const char* name, int casesensitive)
{
    char buf[MAX_TOKEN_CHARS];
    const char* ptr;
    int i, found;

    while (*filter) {
        if (*filter == '*') {
            filter++;
            for (i = 0; *filter; i++) {
                if (*filter == '*' || *filter == '?') break;
                buf[i] = *filter;
                filter++;
            }
            buf[i] = '\0';
            if (strlen(buf)) {
                ptr = Com_StringContains(name, buf, casesensitive);
                if (!ptr) return qfalse;
                name = ptr + strlen(buf);
            }
        }
        else if (*filter == '?') {
            filter++;
            name++;
        }
        else if (*filter == '[' && *(filter + 1) == '[') {
            filter++;
        }
        else if (*filter == '[') {
            filter++;
            found = qfalse;
            while (*filter && !found) {
                if (*filter == ']' && *(filter + 1) != ']') break;
                if (*(filter + 1) == '-' && *(filter + 2) && (*(filter + 2) != ']' || *(filter + 3) == ']')) {
                    if (casesensitive) {
                        if (*name >= *filter && *name <= *(filter + 2)) found = qtrue;
                    }
                    else {
                        if (toupper(*name) >= toupper(*filter) &&
                            toupper(*name) <= toupper(*(filter + 2))) found = qtrue;
                    }
                    filter += 3;
                }
                else {
                    if (casesensitive) {
                        if (*filter == *name) found = qtrue;
                    }
                    else {
                        if (toupper(*filter) == toupper(*name)) found = qtrue;
                    }
                    filter++;
                }
            }
            if (!found) return qfalse;
            while (*filter) {
                if (*filter == ']' && *(filter + 1) != ']') break;
                filter++;
            }
            filter++;
            name++;
        }
        else {
            if (casesensitive) {
                if (*filter != *name) return qfalse;
            }
            else {
                if (toupper(*filter) != toupper(*name)) return qfalse;
            }
            filter++;
            name++;
        }
    }
    
    return qtrue;
}

/*
============
Com_FilterPath
============
*/
int Com_FilterPath(char* filter, char* name, int casesensitive)
{
    int i;
    char new_filter[MAX_QPATH];
    char new_name[MAX_QPATH];

    for (i = 0; i < MAX_QPATH - 1 && filter[i]; i++) {
        if (filter[i] == '\\' || filter[i] == ':') {
            new_filter[i] = '/';
        }
        else {
            new_filter[i] = filter[i];
        }
    }
    new_filter[i] = '\0';
    for (i = 0; i < MAX_QPATH - 1 && name[i]; i++) {
        if (name[i] == '\\' || name[i] == ':') {
            new_name[i] = '/';
        }
        else {
            new_name[i] = name[i];
        }
    }
    new_name[i] = '\0';
    return Com_Filter(new_filter, new_name, casesensitive);
}

/*
================
Com_HashString

================
*/
#define FILE_HASH_SIZE		1024
int Com_HashString(const char* fname)
{
    int i;
    long hash;
    char letter;

    hash = 0;
    i = 0;
    while (fname[i] != '\0') {
        letter = tolower(fname[i]);
        if (letter == '.') break;				// don't include extension
        if (letter == '\\') letter = '/';		// damn path names
        hash += (long)(letter) * (i + 119);
        i++;
    }

    hash &= (FILE_HASH_SIZE - 1);
    
    return hash;
}

/*
============
Com_SkipPath
============
*/
char* Com_SkipPath(char* pathname)
{
    char* last;

    last = pathname;
    while (*pathname)
    {
        if (*pathname=='/')
            last = pathname+1;
        pathname++;
    }

    return last;
}

/*
============
Com_StripExtension
============
*/
void Com_StripExtension(const char* in, char* out)
{
    while (*in && *in != '.') {
        *out++ = *in++;
    }

    *out = 0;
}


/*
==================
Com_DefaultExtension
==================
*/
void Com_DefaultExtension (char* path, int maxSize, const char* extension)
{
    char oldPath[MAX_QPATH];
    char* src;

    //
    // if path doesn't have a .EXT, append extension
    // (extension should include the .)
    //
    src = path + strlen(path) - 1;

    while (*src != '/' && src != path) {
        if (*src == '.') {
            return;                 // it has an extension
        }
        src--;
    }

    Q_strncpyz(oldPath, path, sizeof(oldPath));
    Com_sprintf(path, maxSize, "%s%s", oldPath, extension);
}
