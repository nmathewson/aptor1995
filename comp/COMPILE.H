///////////////////////////////////////////////////////////////////////////
/// Filename:      COMPILE.H                                            ///
/// Long filename: Main compilation header                              ///
/// File type:     C++ header file                                      ///
/// Project:       APTCOMP (APTOR)                                      ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  COMPILE.CPP                                          ///
/// Uses: INTRFACE, NAMEREF, PARSING, CRYPT, COMMON                     ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: Routines to handle parsing directives and generate final  ///
///           .apx, .map, and .mpx files.  Handles all fstream calls.   ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 7/10/95                      Revision: 0.1                    ///
///////////////////////////////////////////////////////////////////////////

#ifndef __FSTREAM_H
#include <fstream.h>
#endif

#ifndef INTRFACE_H
#include "intrface.h"
#endif

#ifndef NAMEREF_H
#include "nameref.h"
#endif

#ifndef COMPILE_H
#define COMPILE_H

///////////////////////////////////////////////////////////////////////////
/// MASTER FUNCTION                                                     ///
///////////////////////////////////////////////////////////////////////////
void handle_make(const char *source_name);
  // Given the name of an .apm source file, opens the file, parses it as a
  // makefile, parses all APTORCode files, creates an .APX file and any other
  // files specified in the makefile, closes all files, handles all errors,
  // echos all messages.


///////////////////////////////////////////////////////////////////////////
/// MAP GENERATION FUNCTIONS                                            ///
///////////////////////////////////////////////////////////////////////////

int save_to_map(ostream &map, name_ref &names);
  // Given a full, sorted name_ref, and a newly-opened TEXT ofstream,
  //  pointing to an empty file, stores the name_ref into the file
  //  in .map format. Returns any error conditions that arise.
int save_to_def(ostream &def, name_ref &names, const char *sym,
                const char *def_name, const char *apx_name);
  // Given a full, sorted name_ref, and a newly-opened TEXT ofstream,
  //  pointing to an empty file, stores the name_ref into the file
  //  in .def format, for inclusion as a header file. SYM is the symbol
  //  used to prevent multiple includes. def_name is the name of the .def
  //  file, and apx_name is the name of the .apx file.
int save_to_mpx(ostream &mpx, name_ref &names);
  // Given a full, sorted name_ref, and a newly-opened BIN ofstream,
  //  pointing to an empty file, stores the name_ref into the file
  //  in .mpx format. Returns any error conditions that arise.

#endif

