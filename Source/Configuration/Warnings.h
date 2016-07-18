/*
    Initial author: (https://github.com/)Convery for Ayria.se
    License: LGPL 3.0
    Started: 2016-7-14
    Notes:
        Disables common warnings, use with care.
*/

#pragma once

// Warning about constant overflows, signed type being used as unsigned.
#pragma warning(disable: 4307)

// Warning about unused parameters.
#pragma warning(disable: 4100)

// Warning about symbols being redeclared, release | debug mismatch.
#pragma warning(disable: 4565)

// Warning about comments in type declarations.
#pragma warning(disable: 4138)

// Deprecated standard library functions.
#pragma warning(disable:4996)
