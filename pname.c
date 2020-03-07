/*
 * src/tutorial/pname.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */

PG_MODULE_MAGIC;

typedef struct Pname
{
	char *		x;
	char *		y;
} Pname;


/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_in);

Datum
pname_in(PG_FUNCTION_ARGS)
{
	char	*str = PG_GETARG_CSTRING(0);
	char	*x = NULL,
            *y = NULL;
	Pname   *result;

    int given_size = ptr-str;
    int family_size = strlen(str) - given_size-1;

    bool error = false;
    if(given_size < 2 || family_size < 2)
        error = true;

    if(*(ptr-1) == ' ')
        error = true;

    if(strchr(ptr+1, ',')!= NULL)
        error = true;

    if(*(ptr+1) == ' '){
        ptr+=1;
        family_size -=1;
    }
    if(('a'<=str[0] && str[0]<= 'z') || ('a'<=ptr[1] && ptr[1]<= 'z'))
        error = true;

    if (error)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                errmsg("invalid input syntax for type %s: \"%s\"",
                    "pname", str)));

    x = (char *) palloc(sizeof(char)*(given_size+1));
    y = (char *) palloc(sizeof(char)*(family_size+1));

    memcpy(x, str, given_size);
    memcpy(y, ptr+1, family_size);
    *(x+given_size) = '\0';
    *(y+family_size) = '\0';

	result = (Pname *) palloc(sizeof(Pname));
	result->x = x;
	result->y = y;
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pname_out);

Datum
pname_out(PG_FUNCTION_ARGS)
{
	Pname    *pname = (Pname *) PG_GETARG_POINTER(0);
	char	   *result;

	result = psprintf("(%g,%g)", pname->x, pname->y);
	PG_RETURN_CSTRING(result);
}

/*****************************************************************************
 * Binary Input/Output functions
 *
 * These are optional.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_recv);

Datum
pname_recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);
	Pname    *result;

	result = (Pname *) palloc(sizeof(Pname));
	result->x = pq_getmsgfloat8(buf);
	result->y = pq_getmsgfloat8(buf);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(pname_send);

Datum
pname_send(PG_FUNCTION_ARGS)
{
	Pname    *pname = (Pname *) PG_GETARG_POINTER(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendfloat8(&buf, pname->x);
	pq_sendfloat8(&buf, pname->y);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 * New Operators
 *
 * A practical Pname datatype would provide much more than this, of course.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(pname_add);

Datum
pname_add(PG_FUNCTION_ARGS)
{
	Pname    *a = (Pname *) PG_GETARG_POINTER(0);
	Pname    *b = (Pname *) PG_GETARG_POINTER(1);
	Pname    *result;

	result = (Pname *) palloc(sizeof(Pname));
	result->x = a->x + b->x;
	result->y = a->y + b->y;
	PG_RETURN_POINTER(result);
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
pname_abs_cmp_internal(Pname * a, Pname * b)
{
	double		amag = Mag(a),
				bmag = Mag(b);

	if (amag < bmag)
		return -1;
	if (amag > bmag)
		return 1;
	return 0;
}


PG_FUNCTION_INFO_V1(pname_abs_lt);

Datum
pname_abs_lt(PG_FUNCTION_ARGS)
{
	Pname    *a = (Pname *) PG_GETARG_POINTER(0);
	Pname    *b = (Pname *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) < 0);
}

PG_FUNCTION_INFO_V1(pname_abs_le);

Datum
pname_abs_le(PG_FUNCTION_ARGS)
{
	Pname    *a = (Pname *) PG_GETARG_POINTER(0);
	Pname    *b = (Pname *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) <= 0);
}

PG_FUNCTION_INFO_V1(pname_abs_eq);

Datum
pname_abs_eq(PG_FUNCTION_ARGS)
{
	Pname    *a = (Pname *) PG_GETARG_POINTER(0);
	Pname    *b = (Pname *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) == 0);
}

PG_FUNCTION_INFO_V1(pname_abs_ge);

Datum
pname_abs_ge(PG_FUNCTION_ARGS)
{
	Pname    *a = (Pname *) PG_GETARG_POINTER(0);
	Pname    *b = (Pname *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) >= 0);
}

PG_FUNCTION_INFO_V1(pname_abs_gt);

Datum
pname_abs_gt(PG_FUNCTION_ARGS)
{
	Pname    *a = (Pname *) PG_GETARG_POINTER(0);
	Pname    *b = (Pname *) PG_GETARG_POINTER(1);

	PG_RETURN_BOOL(pname_abs_cmp_internal(a, b) > 0);
}

PG_FUNCTION_INFO_V1(pname_abs_cmp);

Datum
pname_abs_cmp(PG_FUNCTION_ARGS)
{
	Pname    *a = (Pname *) PG_GETARG_POINTER(0);
	Pname    *b = (Pname *) PG_GETARG_POINTER(1);

	PG_RETURN_INT32(pname_abs_cmp_internal(a, b));
}
