///////////////////////////////////////////////////////////////////////////
/// Filename:       CRYPT.H                                             ///
/// Long filename:  Low-strength encryption, header file                ///
/// File type:      CPP Header                                          ///
/// Project:        APTOR                                               ///
///////////////////////////////////////////////////////////////////////////
/// For use with:   APTOR                                               ///
///                 crypt.cpp                                           ///
/// Uses:           ---                                                 ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: Object header for low-strength, high-volume encryption.   ///
///           Intended for deterance of "prying eyes," not serious      ///
///           assaults.                                                 ///
///////////////////////////////////////////////////////////////////////////
/// Author:   N. A. A. Mathewson                                        ///
/// Date: 6/26/95                      Revision: 0.1                    ///
///////////////////////////////////////////////////////////////////////////

#ifndef CRYPT_H                             // Prevent multiple #include
#define CRYPT_H

#ifndef __IOSTREAM_H
#include <iostream.h>
#endif

#define BASE_CRYPT '\x07'
#define INC_CRYPT  '\x03'
#define AND_CRYPT '\x0F'
#define MAX_KEY_LENGTH 32
#define NULLCH '\x00'

///////////////////////////////////////////////////////////////////////////
/// class cryptor                                                       ///
///////////////////////////////////////////////////////////////////////////
/// This class implements a ridiculously silly algorythm for encrypting ///
/// strings: XOR each datum with a preset character, until all is done. ///
/// It can encrypt characters, or entire strings.                       ///
///   Note: To reset the index into the key, use member function reset. ///
/// Do this at the start of each string.                                ///
///////////////////////////////////////////////////////////////////////////
/// USAGE: cryptor c("key");                                            ///
///        c.setkey("key");                                             ///
///        cout << c.getkey();                                          ///
///        c.keygen(12);      (Generates random 12-charater key (def=20)///
///        cout << c('x');                                              ///
///        char ch = c('w');                                            ///
///        c.reset();                                                   ///
///        c.send("Encrypt me!", os);                                   ///
///        c.get(buffer, 80, is, is_there_more);                        ///
///        c.get(buffer, 80, is, is_there_more, '\0');                  ///
///        c.setkey("");                   (encryption off.)            ///
///////////////////////////////////////////////////////////////////////////

class cryptor
{
protected:
  char key[MAX_KEY_LENGTH + 1];
  char keytext[MAX_KEY_LENGTH + 1];
  int index;
  int key_length;
  void mong_key(void);                        // Called by setkey, keygen
public:
  cryptor(void)
  {
    setkey("");
  };
  cryptor(char *str)
  {
    setkey(str);
  };
  ~cryptor(void)
  {
  };
  void reset(void)
  {
    index = 0;
  };
  char operator()(char ch)
  {
    index %= key_length;
    return (ch >= '\x10') ? (ch) ^ (key[index++])
                          : (ch);
  };
  void setkey(const char *str);
  void keygen(int len = 20);
  const char *getkey(void)
  {
    return keytext;
  };
  void encrypt(char *str);
  cryptor &send(const char *str,
                ostream &os);
  cryptor &get(char *buf,
               int max_length,
               istream &is,
               int &more,
               char terminator = NULLCH);
};

#endif                                      // CRYPT_H

