///////////////////////////////////////////////////////////////////////////
/// Filename:      INTRFACE.H                                           ///
/// Long filename: APTCOMP interface header file                        ///
/// File type:     C++ header                                           ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  intrface.h                                           ///
/// Uses:          ---                                                  ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: Header file for INTRFACE.CPP                              ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 6/30/95                      Revision: 0.2                    ///
///////////////////////////////////////////////////////////////////////////

#ifndef COMMON_H
#include "common.h"
#endif

#ifndef INTRFACE_H
#define INTRFACE_H

enum error_type
{
  NO_ERROR = 0                  ,
  INTERNAL = 1                  ,
  BAD_FILENAME                  ,   // Bad filename syntax
  BAD_SWITCH                    ,   // Bad command-line switch
  NO_SOURCE                     ,   // Sourcefile not specified
  WILDCARDS_USED                ,   // Wildcards in a filename
  UNREC_TOKEN                   ,   // Bad token in a file.
  LINE_TOO_LONG                 ,
  VAL_OUT_OF_BOUNDS             ,
  TOO_MANY_ARGS                 ,
  TOO_FEW_ARGS                  ,
  NOT_INLINE                    ,   // Misrecognized inline code.
  OUT_OF_MEMORY                 ,
  READ_ERROR                    ,
  WRITE_ERROR                   ,
  FILE_NOT_FOUND                ,
  MULTIPLE_MAKE                 ,   // MAKE errors
  MAKE_EXPECTED                 ,
  MULTIPLE_BLOCKS               ,
  BLOCKS_LATE                   ,
  USE_EXPECTED                  ,
  ENDFILE_EXPECTED              ,       // Code errors
  ENDFILE_WO_BF                 ,
  END_WO_BEGIN                  ,
  BLOCK_INTERRUPTED             ,
  END_EXPECTED                  ,
  BLOCK_TOO_LONG                ,
  DUPLICATE_NAMES
};

enum terminate_type
{
  TERMINAL = 1,
  NONTERMINAL = 0
};

void display_intro(void);
    // Displays introductory messages.

void display_help(int which_help);
    // Displays help messages, command-line options, etc.  Exits APTCOMP.
    // 1 is for command-line help; 2 is for APTORcode help.

void display_switches(const command_line &cline);
    // Tells the user what to expect from the command-line given.

void error(error_type err, int terminal, const char *text = "",
           const char *in = "");
void error(error_type err, int terminal, const char *text,
           long line, const char *fname);
    // Displays an error message specified by err. If terminal is true,
    // it exits with exitcode err. If text is specified, it should contain
    // the example of "what went wrong." The second prototype deals with
    // errors "In line ### of FILENAME.EXT"

#endif

