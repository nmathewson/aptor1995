///////////////////////////////////////////////////////////////////////////
/// Filename:      COMMON.H                                             ///
/// Long filename: Common routines for APTCOMP code files               ///
/// File type:     C++ header                                           ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  all aptcomp code files.                              ///
/// Uses:          ---                                                  ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis:      This file contains all code and macros to be shared  ///
///                between APTCOMP files.                               ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 6/30/95                      Revision: 0.2                    ///
///////////////////////////////////////////////////////////////////////////

#ifndef COMMON_H
#define COMMON_H

// MACRO CONSTANTS FOR ALL FILES

#define MAX_FNAME_LEN 12
#define APTCOMP_VERSION 0.2
#define APTCOMP_VERSION_STR "0.2"
#define MAX_NAME_LEN 16
#define MAX_ARG_LEN 16
#define MAX_BLOCKS 16384
#define DEFAULT_BLOCKS 4096
#define MIN_BLOCKS 16
#define MAX_BLOCK_SIZE 8192

#define ORDERED3(x,y,z)  (((x) <= (y)) && ((y) <= (z)))

// DEBUGGING MACRO
#define DEBUGGING 0
extern int verbose;

// SHARED TYPES

///////////////////////////////////////////////////////////////////////////
/// enum input_file_type                                                ///
/// What kind of an input file (inf) are we dealing with?               ///
///////////////////////////////////////////////////////////////////////////

enum input_file_type
{
  NO_INF,                               // No inf specified.
  INLINE_INF,                           // Inf contains inline APTORcodes.
  APTORCODE_INF,                        // Inf is completely APTORcode.
  MULTIPLE_INF                          // Inf is a LIST of files.
};

///////////////////////////////////////////////////////////////////////////
/// struct command_line                                                 ///
/// Holds parsed command line.                                          ///
///////////////////////////////////////////////////////////////////////////

struct command_line
{
  int is_verbose;                       // Write verbose messages?
  char source_name[MAX_FNAME_LEN+1];    // Name of source file.
};

#endif


