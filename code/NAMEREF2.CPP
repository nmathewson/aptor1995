///////////////////////////////////////////////////////////////////////////
/// Filename:      NAMEREF2.CPP                                         ///
/// Long filename: name_ref class implementation (for APTOR)            ///
/// File type:     C++ code                                             ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  APTOR.H                                              ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: This class is used by APTOR to store its Address-table.   ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 7/18/95                      Revision: 0.1                    ///
///////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include "nameref2.h"

typedef char *char_p;

int name_ref::allocate(void)
{
  if (longs_only)
    return 0;

  mem_p[mem_used] = new char[mem_block_size];

  if (! mem_p[mem_used])
    return 1;

  ++mem_used;
  mem_pos = 0U;

  return 0;
}

name_ref::name_ref(void)
{
  ref_p = mem_p = NULL;
  addresses = NULL;
  mem_pos = mem_block_size = 0U;
  max_refs = n_refs = mem_used = 0;
  longs_only = FALSE;
}

int name_ref::set_mem_big(int names, unsigned block_size)
{
  if (ref_p)
    return 1;

  longs_only = FALSE;

  mem_block_size = block_size;
  max_refs = names;
  ref_p = new char_p[names];
  mem_p = new char_p[( names * (MAX_NAME_LEN + sizeof(long))
                       / mem_block_size) ];
  addresses = NULL;

  if (! mem_p)
    return 2;
  if (! ref_p)
    return 2;

  return allocate();
}

int name_ref::set_mem_small(int names)
{
  if (ref_p)
    return 1;

  longs_only = TRUE;

  mem_block_size = 0;
  max_refs = names;
  ref_p = NULL;
  mem_p = NULL;
  addresses = new long[names+1];

  if (! addresses)
    return 2;

  return 0;
}

const char *name_ref::name(int n)
{
  if (longs_only)
    return NULL;

  if ((n >= n_refs) || (n < 0))
    return NULL;

  return str(n);
}

int name_ref::token(const char *s)
{
  if (longs_only)
    return -1;

  int c, low, high, mid;

  low = 0;
  high = n_refs - 1;
  c = strcmp(s, str(low));

  if (c < 0)
    return -1;
  if (c == 0)
    return low;

  c = strcmp(s, str(high));
  if (c > 0)
    return -1;
  if (c == 0)
    return high;

  mid = (low + high) / 2;
  do
  {
    c = strcmp(s, str(mid));
    if (c == 0)
      return mid;
    if (c > 0)
      low = mid;
    if (c < 0)
      high = mid;
    mid = (low + high) / 2;
  }
  while ( (low != mid) && (high != mid) );

  return -1;
}

long name_ref::get_address(int n)
{
  if ((n >=0 ) && (n < n_refs))
  {
    if (longs_only)
      return addresses[n];
    else
      return *(num(n));
  }
  else
    return -1L;
}

int name_ref::add(const char *s, long l)
{
  if (longs_only)
    return add(l);

  if ( (mem_block_size - mem_pos) < (strlen(s) + (sizeof(long) + 2)) )
    if (allocate())
      return 1;

  if (n_refs == max_refs)
    return 1;

  ref_p[n_refs] = & mem_p[mem_used - 1][mem_pos]; // Where to store it?
  *(num(n_refs)) = l;                             // Store l.
  strcpy(str(n_refs), s);                         // Store s.
  mem_pos += 1 + sizeof(long) + strlen(s);        // Increment mem_pos.

  ++n_refs;

  return 0;
}

int name_ref::add(long l)
{
  if (! longs_only)
    return add("unnamed", l);

  if (n_refs == max_refs)
    return 1;

  addresses[n_refs] = l;
  ++n_refs;

  return 0;
}

name_ref::~name_ref(void)
{
  if (ref_p)
    delete [] ref_p;
  if (mem_p)
    for(int i = 0; i < mem_used; i++)
      delete [] mem_p[i];
  if (mem_p)
    delete [] mem_p;
  if (addresses)
    delete [] addresses;
}

