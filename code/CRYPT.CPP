///////////////////////////////////////////////////////////////////////////
/// Filename:       CRYPT.CPP                                           ///
/// Long filename:  Low-strength encryption, code file                  ///
/// File type:      CPP code                                            ///
/// Project:        APTOR                                               ///
///////////////////////////////////////////////////////////////////////////
/// For use with:   APTOR                                               ///
///                 crypt.h                                             ///
/// Uses:           crypt.cpp                                           ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: Code file for low-strength, high-volume encryption.       ///
///           Intended for deterrance of "prying eyes," not serious     ///
///           assaults.                                                 ///
///////////////////////////////////////////////////////////////////////////
/// Author: N. A. A. Mathewson                                          ///
/// Date: 6/26/95                      Revision: 0.1                    ///
///////////////////////////////////////////////////////////////////////////

#include "crypt.h"
#include <string.h>
#include <iostream.h>
#include <time.h>                               // used by randomize...
#include <stdlib.h>

void cryptor::setkey(const char *str)
{
  reset();                                      // Setup index
  key_length = strlen(str);                     //   & key_length

  if (key_length == 0)                          // Boundschecking on str.
  {                                             // Str = ""  ==> no encrypt.
    key_length = 1;
    key[0] = NULLCH;
    return;
  };

  if (key_length > MAX_KEY_LENGTH)              // Strlen>MAX  ==> truncate.
    key_length = MAX_KEY_LENGTH;

  strncpy(keytext, str, key_length);            // Copy string into keytext.
  keytext[key_length] = NULLCH;

  mong_key();
}

void cryptor::mong_key(void)
{
  strcpy(key, keytext);                         // Copy string into key.

                                                // Loop to encrypt string
  for(int i = 0;                                //    in a set way. Namely,
      i < key_length;                           //    xor each keybyte with
      i++)                                      //    basevalue, incremented
  {                                             //    by incvalue each time.
    key[i] = (key[i] ^ BASE_CRYPT) & AND_CRYPT;
    if (key[i] == NULLCH)
      key[i] = 1;
  };
}

void cryptor::keygen(int len)
{
  if (len == 0)
  {
    key_length = 1;
    key[0] = keytext[0] = NULLCH;
    return;
  };

  if (len > MAX_KEY_LENGTH)
    key_length = MAX_KEY_LENGTH;
  else
    key_length = len;

  randomize();

  for(int i = 0; i < len; i++)
    keytext[i] = 'A' + random(25);

  keytext[i] = NULLCH;

  mong_key();
}

void cryptor::encrypt(char *str)
{
  reset();

  while(*str)
  {
    *str = operator()(*str);
    str++;
  };
}

cryptor &cryptor::send(const char *str,
                       ostream &os)
{
  reset();

  while(*str)
  {
    os.put( operator()(*str) );
    str++;
  };

  return *this;
}

cryptor &cryptor::get(char *buf,
                      int max_length,
                      istream &is,
                      int &more,
                      char terminator)
{
  is.get(buf, max_length, terminator);
  if (is.peek() == terminator)
  {
    is.get();
    more = 0;
  }
  else
    more = 1;

  encrypt(buf);

  return *this;
}

