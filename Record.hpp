#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include <iostream>
#include <vector>

#include "StringUtilities.h"

#if defined (__APPLE__)
    #include <netinet/in.h>
    #include <libkern/OSByteOrder.h>
    #define SWAP16(x) (_OSSwapInt16(x))
    #define SWAP32(x) (_OSSwapInt32(x))
#else
    #include <byteswap.h>
    #define SWAP16(x) (__bswap_16(x))
    #define SWAP32(x) (__bswap_32(x))
#endif

#define MAX_DNS_NAME_LEN 256

const short RD_FLAG = (short)(1 << 8);      //RD (Recursion desired) bit set
const short CLASS_IN = 1;                   //IN = Internet class
const short TYPE_A = 1;                     //A = IPv4 Addresses
const short TYPE_CNAME = 5;                 //CNAME = Cononical Name (alias)

class Record {
public:
    Record(void) {
        name = NULL;
        displayName.clear();
        rawName.clear();
        recordType = TYPE_A;
        recordClass = CLASS_IN;
        data = NULL;
    }

    ~Record(void) {
        if(name != NULL) {
            //free(name);
            name = NULL;
        }

        if(data != NULL) {
            free(data);
            data = NULL;
        }
    }

    std::string & GetRawName(void) { return rawName; }
    std::string & GetDisplayName(void) { return displayName; }
    char * GetName(void) { return name; }
    unsigned short GetType(void) { return recordType; }
    unsigned short GetClass(void) { return recordClass; }

    void SetRawName(const std::string & rawName) { this->rawName = rawName; }
    void SetType(const unsigned short recordType) { this->recordType = recordType; }
    void SetClass(const unsigned short recordClass) { this->recordClass = recordClass; }

    void EncodeName(void);
    void EncodeName(const std::string &);

    std::string DecodeString(const char *, const char **);
    void DecodeName(const char *, const char **);


    std::string DecodeType(const unsigned short);
    std::string DecodeClass(const unsigned short);

    void Print(void);

    size_t Size(void);

    char * GetData(void);

protected:
    char * name;                    //(00000011)www(00000110)google(00000011)com(00000000)
    std::string displayName;        //3www6google3com0
    std::string rawName;            //www.google.com
    unsigned short recordType;      //TYPE_A (IPv4) or TYPE_CNAME (Cannonical Name)
    unsigned short recordClass;     //CLASS_IN (Internet)
    char * data;                    //Serialized byte data
};

class QuestionRecord : public Record {};
