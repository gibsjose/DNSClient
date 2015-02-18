#include "Record.hpp"

void Record::EncodeName(void) {
    std::vector<std::string> tokens = StringUtilities::SplitString(rawName, ".");
    std::ostringstream oss;

    //Allocate bytes for name (calloc to clear)
    name = (char *)calloc(rawName.size() + 1, sizeof(char));
    char * p = name;

    for(int i = 0; i < tokens.size(); i++) {
        std::string & token = tokens.at(i);
        oss << token.size();
        oss << token;

        //Copy the size of the token (as an unsigned char)
        unsigned char l = (unsigned char)token.size();
        memcpy(p, &l, sizeof(char));
        p += sizeof(char);

        //Copy the actual token (minus the NULL byte)
        memcpy(p, token.c_str(), token.size());
        p += token.size();
    }

    oss << "0";

    //Terminate with a '0'
    unsigned char l = 0;
    memcpy(p, &l, sizeof(char));

    displayName = oss.str();
}

void Record::EncodeName(const std::string & rawName) {
    this->rawName = rawName;
    this->EncodeName();
}

std::string Record::DecodeString(const char * data, const char ** name) {
    //Convert between (00000011)www(00000110)google(00000011)com(00000000) and www.google.com
    unsigned int len = strlen(*name);

    // Make a copy of the name pointer so that the name pointer can be left to point to the next part
    // of this packet's data right after the name's bytes even if it contained a "pointer" in the
    // name.

    const char * p = *name;
    char * tmp = (char *)malloc(MAX_DNS_NAME_LEN);
    memset(tmp, 0, MAX_DNS_NAME_LEN);

    unsigned char didFollowPointer = 0;

    while(1) {
        // Test if this short is a pointer.
        unsigned short potentialOffset = 0;
        memcpy(&potentialOffset, p, sizeof(potentialOffset));
        potentialOffset = SWAP16(potentialOffset);

        // If the two most significant bits are set in the 2 byte short, this is
        // a byte offset "pointer"
        if( (potentialOffset & 0xC000) != 0)
        {
            if(!didFollowPointer)
            {
                // Move the name pointer right after the pointer's last byte.
                (*name) = p + sizeof(potentialOffset);

                // Mark that a pointer was followed.
                didFollowPointer = 1;
            }

            // Clear the top two bits and set the pointer equal to the data + offset.
            p = data + (potentialOffset & ~0xC000);

        }
        else
        {
            //This is not a pointer since the top two bits of the short are not set.

            //Concatenate *p bytes to the temp string.
            strncat(tmp, (p + 1), *p);

            //Move the pointer forward n + 1 bytes to go past the n byte string.
            p += (*p + 1);

            //Only if we haven't followed a "pointer" name should follow p.
            if(!didFollowPointer)
            {
                (*name) = p;
            }

            //If the next length is 0, this is the end of the string.
            if(*p == 0)
            {
                //since p (and therefore *name) now points at 0, marking the end of the string, move it 1 byte farther
                // so that subsequent reads get the next chunk of data
                if(!didFollowPointer)
                {
                    (*name)++;
                }

                break;
            }

            //If this is not the last string concatenated, append a period.
            strncat(tmp, ".", 1);
        }
    }

    std::string returnString(tmp);

    //Free tmp
    free(tmp);

    return returnString;
}

void Record::DecodeName(const char * data, const char ** name)
{
    //Encode to set display name
    this->EncodeName(this->DecodeString(data, name));
}

std::string Record::DecodeType(const unsigned short recordType) {
    if(recordType == TYPE_A) {
        return "A";
    } else if(recordType == TYPE_CNAME) {
        return "CNAME";
    } else {
        return std::to_string(recordType);
    }
}

std::string Record::DecodeClass(const unsigned short recordClass) {
    if(recordClass == CLASS_IN) {
        return "Internet (IN)";
    } else {
        return std::to_string(recordClass);
    }
}

void Record::Print(void) {
    std::cout << "\t\tRaw Name --> " << rawName << std::endl;
    std::cout << "\t\tEncoded Name --> " << displayName << std::endl;
    std::cout << "\t\tType --> " << DecodeType(recordType) << std::endl;
    std::cout << "\t\tClass --> " << DecodeClass(recordClass) << std::endl;
}

size_t Record::Size(void) {
    size_t size = 0;

    size += strlen(this->name) + 1;
    size += sizeof(this->recordType);
    size += sizeof(this->recordClass);

    return size;
}

char * Record::GetData(void) {
    //Malloc the required amount of space
    size_t dataLen = this->Size();
    data = (char *)malloc(dataLen);
    char * p = data;

    memcpy(p, this->name, strlen(this->name) + 1);
    p += strlen(this->name) + 1;

    unsigned short recordType = SWAP16(this->recordType);
    unsigned short recordClass = SWAP16(this->recordClass);

    memcpy(p, &(recordType), sizeof(this->recordType));
    p += sizeof(this->recordType);

    memcpy(p, &(recordClass), sizeof(this->recordClass));

    return data;
}
