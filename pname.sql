Drop TYPE if exists PersonName cascade;

CREATE FUNCTION pname_in(cstring)
   RETURNS PersonName
   AS '/srvr/z3290805/testing/pname'
   LANGUAGE C IMMUTABLE STRICT;

-- the output function 'pname_out' takes the internal representation and
-- converts it into the textual representation.

CREATE FUNCTION pname_out(PersonName)
   RETURNS cstring
   AS '/srvr/z3290805/testing/pname'
   LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE PersonName (
   input = pname_in,
   output = pname_out,
   internallength = VARIABLE,
   storage = plain
);

CREATE FUNCTION family(PersonName)
   RETURNS cstring
   AS '/srvr/z3290805/testing/pname'
   LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION given(PersonName)
   RETURNS cstring
   AS '/srvr/z3290805/testing/pname'
   LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION hash(PersonName)
   RETURNS cstring
   AS '/srvr/z3290805/testing/pname'
   LANGUAGE C IMMUTABLE STRICT;

-- now, we can create the type. The internallength specifies the size of the
-- memory block required to hold the type (we need two 8-byte doubles).

-----------------------------
-- Interfacing New Types with Indexes:
--	We cannot define a secondary index (eg. a B-tree) over the new type
--	yet. We need to create all the required operators and support
--      functions, then we can make the operator class.
-----------------------------

-- first, define the required operators
CREATE FUNCTION pname_abs_lt(PersonName, PersonName) RETURNS bool
   AS '/srvr/z3290805/testing/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_abs_le(PersonName, PersonName) RETURNS bool
   AS '/srvr/z3290805/testing/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_abs_eq(PersonName, PersonName) RETURNS bool
   AS '/srvr/z3290805/testing/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_abs_ge(PersonName, PersonName) RETURNS bool
   AS '/srvr/z3290805/testing/pname' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION pname_abs_gt(PersonName, PersonName) RETURNS bool
   AS '/srvr/z3290805/testing/pname' LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR < (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_abs_lt,
   commutator = > , negator = >= ,
   restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR <= (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_abs_le,
   commutator = >= , negator = > ,
   restrict = scalarlesel, join = scalarlejoinsel
);
CREATE OPERATOR = (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_abs_eq,
   commutator = = , negator = <> ,
   restrict = eqsel, join = eqjoinsel
);

CREATE OPERATOR === (
    LEFTARG = PersonName,
    RIGHTARG = PersonName,
    FUNCTION = pname_abs_eq,
    COMMUTATOR = ===
);


CREATE OPERATOR >= (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_abs_ge,
   commutator = <= , negator = < ,
   restrict = scalargesel, join = scalargejoinsel
);
CREATE OPERATOR > (
   leftarg = PersonName, rightarg = PersonName, procedure = pname_abs_gt,
   commutator = < , negator = <= ,
   restrict = scalargtsel, join = scalargtjoinsel
);

-- create the support function too
CREATE FUNCTION pname_abs_cmp(PersonName, PersonName) RETURNS int4
   AS '/srvr/z3290805/testing/pname' LANGUAGE C IMMUTABLE STRICT;


-- Using the new type:
--      user-defined types can be used like ordinary built-in types.
-----------------------------

CREATE FUNCTION hash1(PersonName)
    RETURNS int4
    AS '/srvr/z3290805/testing/pname'
    LANGUAGE C IMMUTABLE STRICT;


CREATE OPERATOR CLASS pname_abs_ops
    DEFAULT FOR TYPE PersonName USING hash AS
        OPERATOR        1       = ,
        FUNCTION        1       hash1(PersonName);


