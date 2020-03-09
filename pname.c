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

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */

PG_MODULE_MAGIC;

typedef struct PersonName
{
    int struct_size;
	char name[];
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
                    "pname", str)));
}

static struct PersonName *createPName(struct PersonName *s, char a[])
{
    // Allocating memory according to user provided
    // array of characters

    ereport(WARNING,
    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
        errmsg("- input syntax for type")));

    s = palloc( sizeof(*s) + sizeof(char) * strlen(a));

    strcpy(s->name, a);
    // Assigning size according to size of stud_name
    // which is a copy of user provided array a[].
    s->struct_size =
        (sizeof(*s) + sizeof(char) * strlen(s->name));

ereport(WARNING,
    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
        errmsg("+ input syntax for type %s", s->name)));

    return s;
}


PG_FUNCTION_INFO_V1(pname_in);

Datum
pname_in(PG_FUNCTION_ARGS)
{
	char	*str = PG_GETARG_CSTRING(0);
	PersonName   *result;

    char *ptr = strchr(str, ',');
    int family_size = ptr-str;
    int given_size = strlen(str) - family_size-1;

    if(given_size < 2 || family_size < 2)
        pname_abs_error_internal(str);

    if(*(ptr-1) == ' ')
        pname_abs_error_internal(str);

    if(strchr(ptr+1, ',')!= NULL)
        pname_abs_error_internal(str);

    if(*(ptr+1) == ' '){
        ptr+=1;
        given_size -=1;
    }
    if(('a'<=str[0] && str[0]<= 'z') || ('a'<=ptr[1] && ptr[1]<= 'z'))
        pname_abs_error_internal(str);


    ereport(WARNING,
    (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
        errmsg("1 input syntax for type %d: \"%d\"",
            family_size, given_size)));

	result = createPName(result, str);

	    ereport(WARNING,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("2 input syntax for type %s: \"%s\"",
                    "pname", str)));

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

static int
pname_abs_cmp_internal(PersonName * a, PersonName * b)
{
	int result = strcmp(a->name, b->name);
    if (result != 0)
        return result;

    return strcmp(a->name, b->name);
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

