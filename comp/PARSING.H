///////////////////////////////////////////////////////////////////////////
/// Filename:      PARSING.H                                            ///
/// Long filename: APTCOMP Parsing routines header                      ///
/// File type:     C++ Header                                           ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  PARSING.CPP                                          ///
/// Uses:          COMMON, INTRFACE                                     ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis:  Header for APTCOMP file parsing routines.                ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 6/30/95                      Revision: 0.2                    ///
///////////////////////////////////////////////////////////////////////////

#ifndef COMMON_H
#include "common.h"
#endif

#ifndef INTRFACE_H
#include "intrface.h"
#endif

#ifndef PARSING_H
#define PARSING_H

///////////////////////////////////////////////////////////////////////////
/// STRUCTURES / ENUMERATIONS                                           ///
///////////////////////////////////////////////////////////////////////////

enum make_directive_type
{
  BAD_DRC = -1,
  NO_DRC = 0,
  BLOCKS_DRC,
  MAKE_DRC,
  USE_DRC
};

enum APTORCode_command_type
{
  BAD_ACMD = -1,
  NO_ACMD = 0,
  BEGIN_ACMD, END_ACMD,
  BEGINFILE_ACMD, ENDFILE_ACMD
};

enum make_flag_type
{
  BAD_MF = -1,
  NO_MF = 0,
  ALL_MF,
  APX_MF,
  MAP_MF,
  DEF_MF,
  MPX_MF
};

struct make_flags
{
  int all     : 1;
  int apx     : 1;
  int map     : 1;
  int def     : 1;
  int mpx     : 1;
  int         : 11;
};

struct make_directive
{
  make_directive_type dir;              // What directive is involved?
  char sarg[MAX_ARG_LEN];               // String argument
  union
  {
    unsigned narg;                      // Numerical argument
    input_file_type inf;                // INF argument
    make_flags mf;                      // Make flags
  };
  error_type err;
};

struct APTORCode_command
{
  APTORCode_command_type cmd;           // What directive is involved?
  char sarg[MAX_ARG_LEN];               // String argument
  error_type err;
};

///////////////////////////////////////////////////////////////////////////
/// COMMAND LINE PARSING                                                ///
///////////////////////////////////////////////////////////////////////////

void parse_cline(command_line &cline, int argc, char *argv[]);
    // Parses a command-line.

///////////////////////////////////////////////////////////////////////////
/// SHARED PARSING                                                      ///
///////////////////////////////////////////////////////////////////////////

int parse_filename(char *d, const char *s);
    // Copies a filename from s to d, reformatting it. Assumes at least
    // MAX_FNAME_LEN+1 char's free at d. Returns 1 for errors. d[0] must
    // be null, or an error occurs.

///////////////////////////////////////////////////////////////////////////
/// SOURCE PARSING                                                      ///
///////////////////////////////////////////////////////////////////////////

void parse_make_line(make_directive &m, const char *line);
    // Parses the line into m.

void parse_APTORCode_line(APTORCode_command &c, const char *line,
                          input_file_type inf);
    // Parses the line of type inf into c.

int is_APTORCode_line(const char *line);
    // Returns whether line is to be interpreted as a line of APTORCode,
    // in the context of an INLINE_INF.

inline int dbl_slash(const char *s)
{
  return ((s[0] == '/') && (s[1] == '/'));
}

int tighten_line(char *line);
  // Remove whitespace from the end of a line, add newline character
  //   (if no '\'). Returns new length of line.

#endif

