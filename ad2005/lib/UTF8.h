#ifndef UTF8_H
#define UTF8_H

int MultibyteToUTF8(const char* src,int nbyte,char** dest);
int UNICODE_encode(const char* src,int bytelen,unsigned short** unicode);
int UTF8_encode(const unsigned short* src,int srclen,char** multibyte);


int UTF8ToMultibyte(const char* src,int nbyte,char** dest);
int UNICODE_decode(const unsigned short* src,int srclen,char** multibyte);
int UTF8_decode(const char* src,int bytelen,unsigned short** unicode);

#endif