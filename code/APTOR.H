///////////////////////////////////////////////////////////////////////////
/// Filename:      APTOR.H                                              ///
/// Long filename: APTOR header file                                    ///
/// File type:     C++ header                                           ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  APTOR.CPP, APTCOMP.EXE                               ///
/// Uses:          NAMEREF2, CRYPT                                      ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: Header file for APTOR class: handles all interfaces       ///
///           into an APX file.                                         ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 7/18/95                      Revision: 0.1                    ///
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
/// SYNTAX:                                                             ///
///////////////////////////////////////////////////////////////////////////
/// APTOR_file a();                                                     ///
/// APTOR_file a("family.apx", "family.map");                           ///
/// APTOR_file a("family.apx", "family.mpx");                           ///
/// int good = a.open("family.apx", "family.map");                      ///
/// int good = a.open("family.apx", "family.mpx");                      ///
/// a.close();                                                          ///
/// if (a.ok()) cout << "Ready for input!\n";                           ///
/// cout << a[T_ROOM_11]; (***)                                         ///
/// cout << a[32];                                                      ///
/// cout << a["ROOM_11"];                                               ///
/// char b[100]; a.copy(b, T_ROOM_11); (***)                            ///
/// char b[100]; a.copy(b, 32);                                         ///
/// char b[100]; a.copy(b, "ROOM_11");                                  ///
///////////////////////////////////////////////////////////////////////////
/// (***) : Assumes a .def file has been included.                      ///
///////////////////////////////////////////////////////////////////////////

#ifndef APTOR_H
#define APTOR_H

#ifndef __FSTREAM_H
#include <fstream.h>
#endif

#ifndef CRYPT_H
#include "crypt.h"
#endif

#ifndef NAMEREF2_H
#include "nameref2.h"
#endif

#ifndef FALSE
#include "flag.h"
#endif

#define MAX_BLOCK_LEN 8192
#define MAX_BLOCKS 16384

///////////////////////////////////////////////////////////////////////////
/// APTOR_file                                                          ///
///////////////////////////////////////////////////////////////////////////

class APTOR_file
{
protected:
  cryptor *c;
  name_ref *map;
  ifstream *apx;

  char *buf;

  flag string_tokens;                           // Can strings be tokens? (.map)
  flag ok_f;                                    // Can blocks be read?

  int n_blocks;

  int token(const char *name)
    { return map->token(name); };
  long address(int n)
    { return map->get_address(n); };
  long address(const char *name)
    { return map->get_address(name); };
  void seek(int n)
    { apx->seekg(address(n)); };
  void seek(const char *name)
    { apx->seekg(address(name)); };

  int allocate_buf(void);

  int read_map(ifstream &m);
  int read_mpx(ifstream &m);

public:
// CONSTRUCTORS / DESTRUCTOR
  APTOR_file(void);
  APTOR_file(const char *apx_name,
             const char *map_name)
    { APTOR_file();
      ok_f = open(apx_name, map_name); };
  ~APTOR_file(void);
// FILE ACCESS
  int open(const char *apx_name,
           const char *map_name);
  void close(void);
  int ok(void)
    { return ok_f; };
// BLOCK ACCESS
  const char *operator[](int token);
  const char *operator[](const char *name)
    { return operator[](token(name)); };

  char *copy(char *dest,
             int token,
             int max_len = -1);
  char *copy(char *dest,
             const char *name,
             int max_len = -1)
    { return copy(dest, token(name), max_len); };
// DEBUGGING
  const char *name(int token);
  int n(void)
  {
    return n_blocks;
  };
};

#endif                                                    // APTOR_H


