///////////////////////////////////////////////////////////////////////////
/// Filename:      APTOR.CPP                                            ///
/// Long filename: APTOR_file class implementation file                 ///
/// File type:     C++ class implementation                             ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  APTOR.H                                              ///
/// Uses:          CRYPTOR, NAMEREF                                     ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis:  Implementation for APTOR_file class.  Exits on unrecover-///
///            -able error conditions.                                  ///
///////////////////////////////////////////////////////////////////////////
/// Author:  N.A.A. Mathewson                                           ///
/// Date: 9/19/95                      Revision: 1.0                    ///
///////////////////////////////////////////////////////////////////////////

#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "nameref2.h"
#include "crypt.h"
#include "aptor.h"

#define MAX_LINE_LEN 255

//////////////////////////
// INTERNAL PROTOTYPES  //
//////////////////////////

char *skip_wspace(char *line);
char *skip_to_wspace(char *line);
void word_copy(char *d, const char *c);

//////////////////////////
// INTERNAL, NONMEMBERS //
//////////////////////////

void word_copy(char *d, const char *c)
{
  while ((*c) && ! isspace (*c))
    *(d++) = *(c++);

  *d = '\0';
}

char *skip_wspace(char *line)
{
  while (isspace(*line))
    ++line;

  return line;
}

char *skip_to_wspace(char *line)
{
  while ((*line) && ! isspace(*line))
    ++line;

  return line;
}

//////////////////////
// INTERNAL MEMBERS //
//////////////////////

int APTOR_file::allocate_buf(void)
{
  if (buf)
    return 1;

  buf = new char[MAX_BLOCK_LEN];

  if (! buf)
    exit(1);

  return 0;
}

int APTOR_file::read_map(ifstream &m)
{
  char lineb[MAX_LINE_LEN+1];
  char word[MAX_LINE_LEN+1];
  long l;

  char *line;

  line = lineb;

  m.getline(line, MAX_LINE_LEN);            // EXTRACT 'MAP'.

  m.getline(line, MAX_LINE_LEN);            // EXTRACT #records.
  line = skip_wspace(line);
  n_blocks = atoi(line);

  if ((n_blocks < 1) ||
      (n_blocks > MAX_BLOCKS))
  {
    n_blocks = 0;
    return 1;
  };

  map = new name_ref();
  if (! map)
    return 1;

  map->set_memory(FALSE,                    // NOT small model.
                  n_blocks+1);

  for(int i = 0; i < n_blocks; ++i)
  {
    line = lineb;
    m.getline(line, MAX_LINE_LEN);
    line = skip_wspace(
           skip_to_wspace(
           skip_wspace(line) ) );           // Skip to name.

    word_copy(word, line);

    line = skip_wspace(
           skip_to_wspace(line) );          // Skip to address.

    l = atol(line);
    if (l == 0L)
      return 1;

    if (map->add(word, l))
      return 1;
  };

  ok_f = TRUE;

  return 0;
}

int APTOR_file::read_mpx(ifstream &m)
{
  char ch;
  long l;
  m.read(&ch, sizeof(char));                // Skip '~'

  m.read((char *) &n_blocks, sizeof(int));

  if ((n_blocks < 1) ||
      (n_blocks > MAX_BLOCKS))
  {
    n_blocks = 0;
    return 1;
  };

  map = new name_ref();
  if (! map)
    return 1;

  map->set_memory(TRUE,                     // Small memory!
                  n_blocks);

  for(int i = 0; i < n_blocks; ++i)
  {
    m.read((char *) &l, sizeof(long));
    map->add(l);
  };

  ok_f = TRUE;

  return 0;
}

////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS //
////////////////////////////////

APTOR_file::APTOR_file(void)
{
  c   = NULL;
  map = NULL;
  apx = NULL;
  buf = NULL;

  string_tokens = FALSE;
  ok_f          = FALSE;

  n_blocks      = 0;
}

APTOR_file::~APTOR_file(void)
{
  if (apx)
  {
    apx->close();
    delete apx;
  };

  if (c)
    delete c;
  if (map)
    delete map;
  if (buf)
    delete [] buf;
}

/////////////////
// FILE ACCESS //
/////////////////

int APTOR_file::open(const char *apx_name,
                     const char *map_name)
{
  char ch;

// OPEN FILES

  if (apx)
    close();

  ok_f = FALSE;

  apx = new ifstream(apx_name);
  if (! apx->good())
    return 1;

  ifstream map_file(map_name);
  if (! map_file.good())
    return 1;

// READ MAP FILE

  ch = map_file.get();

  if (ch == '~')
  {
    map_file.close();
    map_file.open(map_name, ios::binary);
    ok_f = ! read_mpx(map_file);
  }
  else
  {
    ok_f = ! read_map(map_file);
  };

  map_file.close();

  if (! ok_f)
    return 1;

// READ KEY

  char key[MAX_KEY_LENGTH+1];

  apx->get(key, MAX_KEY_LENGTH, '\0').get();

  c = new cryptor(key);

  if (! c)
  {
    ok_f = FALSE;
    return 1;
  };

  return 0;
}

void APTOR_file::close(void)
{
  if (apx)
  {
    apx->close();
    delete apx;
  };

  if (c)
    delete c;
  if (map)
    delete map;
  if (buf)
    delete [] buf;

  n_blocks      = 0;
  ok_f          = FALSE;
  string_tokens = FALSE;

  c   = NULL;
  map = NULL;
  buf = NULL;
  apx = NULL;
}

//////////////////
// BLOCK ACCESS //
//////////////////

const char *APTOR_file::operator[](int token)
{
  if ((0 > token) ||
      (token >= n_blocks))
    return NULL;

  if (! buf)
    allocate_buf();

  return copy(buf, token, MAX_BLOCK_LEN-1);
}

char *APTOR_file::copy(char *dest, int token, int max_len)
{
  if ((0 > token) ||
      (token >= n_blocks))
    return NULL;
  seek(token);

  if ((max_len < 0) ||
      (max_len > (MAX_BLOCK_LEN - 1)))
    max_len = MAX_BLOCK_LEN - 1;

  int more;                             // UNUSED...

  c->get(dest, max_len, *apx, more);

  return dest;
}

const char *APTOR_file::name(int token)
{
  if ((0 > token ) ||
      (token >= n_blocks))
    return NULL;

  if (! ok_f)
    return NULL;

  return map->name(token);
};

