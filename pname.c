/*
 * src/tutorial/pname.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"
#include "string.h"
#include "utils/hashutils.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */

PG_MODULE_MAGIC;

typedef struct PersonName
{
    int struct_size;
    int family_end;
    int given_start;
	char name[FLEXIBLE_ARRAY_MEMBER];
} PersonName;

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

static void
pname_abs_error_internal(char * str)
{
     ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input syntax for type %s: \"%s\"",
                    "PersonName", str)));
}

static struct PersonName *createPName(struct PersonName *s, char a[], int family_end, int given_start, bool space)
{
    // Allocating memory according to user provided
    // array of characters
    int given_len = strlen(a) - given_start;
    int new_size = family_end + 1 + given_len +1;
    s = palloc( sizeof(*s) + sizeof(char) * new_size);

    memcpy(s->name, a, family_end);
    s->name[family_end]=',';
    strcpy(s->name+family_end+1, a+given_start);
    s->name[family_end+1+given_len]='\0';
    // Assigning size according to size of stud_name
    // which is a copy of user provided array a[].
    s->struct_size =
        (sizeof(*s) + sizeof(char) * strlen(s->name));
    s->family_end = family_end;
    if (space)
        given_start-=1;
    s->given_start = given_start;

    SET_VARSIZE(s, sizeof(*s) + sizeof(char) * new_size);

    return s;
}

static bool
is_upper_alphabet(char c)
{
     if(c >= 'A' && c <= 'Z')
        return true;
     return false;
}

static bool
is_lower_alphabet(char c)
{
     if(c >= 'a' && c <= 'z')
        return true;
     return false;
}

static bool
is_alphabet(char c)
{
     return is_upper_alphabet(c) || is_lower_alphabet(c);
}

static bool
is_allowed_symbol(char c)
{
     if( c == '-' || c == '\'')
        return true;
     return false;
}

PG_FUNCTION_INFO_V1(pname_in);

Datum
pname_in(PG_FUNCTION_ARGS)
{
	char	*str = PG_GETARG_CSTRING(0);
	PersonName   *result;
    bool space = false;
    char *ptr = strchr(str, ',');
    int family_size = ptr-str;
    int given_size = strlen(str) - family_size-1;
    char * pch;
    char* tempstr = palloc(strlen(str)+1);
    char prev;

    if(given_size < 2 || family_size < 2)
        pname_abs_error_internal(str);

    if(*(ptr-1) == ' ' || str[0] == ' ' || str[strlen(str)-1] == ' ')
        pname_abs_error_internal(str);

    if(strchr(ptr+1, ',')!= NULL)
        pname_abs_error_internal(str);

    if(*(ptr+1) == ' '){
        ptr+=1;
        given_size -=1;
        space = true;
    }

    if(*(ptr+1) == ' ')
        pname_abs_error_internal(str);

    strcpy(tempstr, str);
    pch = strtok (tempstr," ,");
    while (pch != NULL)
    {
        if(strlen(pch)<2)
            pname_abs_error_internal(str);

        if(!is_upper_alphabet(pch[0]))
            pname_abs_error_internal(str);

        prev = '\0';
        for(int i = 1; i < strlen(pch); i++){
            if(!(is_alphabet(pch[i]) || is_allowed_symbol(pch[i]))){
                if(!(is_allowed_symbol(prev) && is_upper_alphabet(pch[i])))
                    pname_abs_error_internal(str);
            }
            prev = pch[i];
        }

        pch = strtok (NULL, " ,");
    }

	result = createPName(result, str, family_size, ptr - str + 1, space);

	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pname_out);

Datum
pname_out(PG_FUNCTION_ARGS)
{
	PersonName    *pname = (PersonName *) PG_GETARG_POINTER(0);
	char	 *result;

	result = psprintf("%s", pname->name);
	PG_RETURN_CSTRING(result);
}

/*****************************************************************************
 * Operator class for defining B-tree index
 *
 * It's essential that the comparison operators and support function for a
 * B-tree index opclass always agree on the relative ordering of any two
 * data values.  Experience has shown that it's depressingly easy to write
 * unintentionally inconsistent functions.  One way to reduce the odds of
 * making a mistake is to make all the functions simple wrappers around
 * an internal three-way-comparison function, as we do here.
 *****************************************************************************/

#define Mag(c)	((c)->x*(c)->x + (c)->y*(c)->y)

static char *
get_family(PersonName * pname)
{
	return psprintf("%.*s", pname->family_end, pname->name);
}

static char *
get_given(PersonName * pname)
{
    int size = strlen(pname->name) - pname->given_start;
	return psprintf("%.*s", size, pname->name + pname->given_start);
}

static char *
get_show(PersonName * p)
{
    char *given = get_given(p);
    char *ptr = strchr(given, ' ');
    int g_size = strlen(given);
    if (ptr!=NULL)
        g_size = ptr - given;
	return psprintf("%.*s %s", g_size, given, get_family(p));
}

static int
pname_abs_cmp_internal(PersonName * a, PersonName * b)
{
	int result = strcmp(get_family(a),get_family(b));
    if (result != 0)
        return result;
    return strcmp(get_given(a), get_given(b));
}

static int
pname_hash_internal(PersonName * p)
{
    int h = hash_any((const unsigned char *)p->name, strlen(p->name));
	return h;
}


PG_FUNCTION_INFO_V1(hash1);

Datum
hash1(PG_FUNCTION_ARGS)
{
	PersonName    *pname = (PersonName *) PG_GETARG_POINTER(0);
	PG_RETURN_INT32(pname_hash_internal(pname));
}

PG_FUNCTION_INFO_V1(family);

Datum
family(PG_FUNCTION_ARGS)
{
	PersonName    *pname = (PersonName *) PG_GETARG_POINTER(0);
	PG_RETURN_CSTRING(get_family(pname));
}

PG_FUNCTION_INFO_V1(given);

Datum
given(PG_FUNCTION_ARGS)
{
	PersonName    *pname = (PersonName *) PG_GETARG_POINTER(0);
	PG_RETURN_CSTRING(get_given(pname));
}

PG_FUNCTION_INFO_V1(show);

Datum
show(PG_FUNCTION_ARGS)
{
	PersonName    *pname = (PersonName *) PG_GETARG_POINTER(0);
	PG_RETURN_CSTRING(get_show(pname));
}

PG_FUNCTION_INFO_V1(pname_abs_lt);

Datum
pname_abs_lt(PG_FUNCTION_ARGS)
{
	PersonName    *a = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *b = (PersonName *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) < 0);
}

PG_FUNCTION_INFO_V1(pname_abs_le);

Datum
pname_abs_le(PG_FUNCTION_ARGS)
{
	PersonName    *a = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *b = (PersonName *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) <= 0);
}

PG_FUNCTION_INFO_V1(pname_abs_eq);

Datum
pname_abs_eq(PG_FUNCTION_ARGS)
{
	PersonName    *a = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *b = (PersonName *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) == 0);
}

PG_FUNCTION_INFO_V1(pname_abs_ne);

Datum
pname_abs_ne(PG_FUNCTION_ARGS)
{
	PersonName    *a = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *b = (PersonName *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) != 0);
}

PG_FUNCTION_INFO_V1(pname_abs_ge);

Datum
pname_abs_ge(PG_FUNCTION_ARGS)
{
	PersonName    *a = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *b = (PersonName *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) >= 0);
}

PG_FUNCTION_INFO_V1(pname_abs_gt);

Datum
pname_abs_gt(PG_FUNCTION_ARGS)
{
	PersonName    *a = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *b = (PersonName *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) > 0);
}

PG_FUNCTION_INFO_V1(pname_abs_cmp);

Datum
pname_abs_cmp(PG_FUNCTION_ARGS)
{
	PersonName    *a = (PersonName *) PG_GETARG_POINTER(0);
	PersonName    *b = (PersonName *) PG_GETARG_POINTER(1);

	PG_RETURN_INT32(pname_abs_cmp_internal(a, b));
}

