#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int my_utf8_encode(char *input, char *output);
int my_utf8_encode_helper(const char *input, char *output);
int my_utf8_decode(char *input, char *output);
int my_utf8_check(char *input);
int my_utf8_strlen(char *input);
char* my_utf8_charat(char *input, int index);
int my_utf8_strcmp(char *str1, char *str2);
char* my_utf8_concat(char *str1, char *str2);
char* my_utf8_last(char *input);

int encoding_tests(char *input, char *expected);
int decoding_tests(char *input, char *expected);
int valid_utf8_tests(char *input, int expected);
int strlen_tests(char *input, int expected);
int charat_tests(char *input, int index, char *expected);
int strcmp_tests(char *str1, char *str2, int expected);
int strconcat_tests(char *str1, char *str2, char *expected);
int lastchar_tests(char *input, char *expected);


int main() {
    //printf("\nEncoding Tests:\n");
    //encoding_tests("\\u0024", "\x24");
    //encoding_tests("\\u00A8", "\xc2\xa8");
    //encoding_tests("\\u05D0\\u05E8\\u05D9\\u05D4", "אריה");
    //encoding_tests("Hello \\u05D0\\u05E8\\u05D9\\u05D4", "Hello אריה");
    //encoding_tests("\\u1F618", "😘");


    //printf("\nDecoding Tests:\n");
    //decoding_tests("\xd7\x90", "\\u05D0");
    //decoding_tests("\xd7\x90\xd7\xa8\xd7\x99\xd7\x94", "\\u05D0\\u05E8\\u05D9\\u05D4");
    //decoding_tests("\xc2\xa8", "\\u00A8");
    //decoding_tests("\xF0\x9F\x98\x98", "\\u1F618");
    //decoding_tests("Hello \xd7\x90\xd7\xa8\xd7\x99\xd7\x94", "Hello \\u05D0\\u05E8\\u05D9\\u05D4");
    //decoding_tests("\xd7\x90\xd7\xa8\xd7\x99\xd7\x94 Hello", "\\u05D0\\u05E8\\u05D9\\u05D4 Hello");
    //decoding_tests("\x24", "$");
    //decoding_tests("j", "j");

    //printf("\nValid UTF8 Tests:\n");
    //valid_utf8_tests("אריה", 0);
    //valid_utf8_tests("\\uD83D\\uDE18", 0);
    //valid_utf8_tests("\xB2\xA3", -1);
    //valid_utf8_tests("\xC2\x80", 0);
    //valid_utf8_tests("\\uD83D אריה jennie", 0);

    //printf("\nChar At Tests:\n");
    //charat_tests("My name is Jennie", 0, "M");
    //charat_tests("Hello, 😘 Jennie!", 7, "😘");

    //printf("\nString Length Tests:\n");
    //strlen_tests("אריה", 4);
    //strlen_tests("arieh", 5);
    //strlen_tests("😘", 1);
    //strlen_tests("\xC2\x80", 1);

    //printf("\nString Comparison Tests:\n");
    //strcmp_tests("hello", "hello", 0);
    //strcmp_tests("😘", "😘", 0);
    //strcmp_tests("abc", "abcdef", -1);

    //printf("\nString Concatenation Tests:\n");
    //strconcat_tests( "Hello, ", "😘", "Hello, 😘");
    //strconcat_tests("€", "€", "€€");

    //printf("\nLast Character Tests:\n");
    //lastchar_tests("Hello, 世界", "界");


    return 0;

}

int lastchar_tests(char *input, char *expected){
    char* result = my_utf8_last(input);
    if (memcmp(result, expected, strlen(result)) == 0) {
        printf("Test passed!\n");
    } else {
        printf("Test failed.");
    }
    return 0;
}

int strconcat_tests(char *str1, char *str2, char *expected){
    char *result = my_utf8_concat(str1, str2);

    if (strcmp(result, expected) == 0) {
        printf("Test passed!\n");
    } else {
        printf("Test failed.\n");
    }

    return 0;
}

int strcmp_tests(char *str1, char *str2, int expected){
    int result = my_utf8_strcmp(str1, str2);
    if (result == expected){
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.\n");
    }
    return 0;
}

int strlen_tests(char *input, int expected){
    int result = my_utf8_strlen(input);
    if (result == expected){
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.\n");
    }
    return 0;
}

int charat_tests(char *input, int index, char *expected){
    char *result = my_utf8_charat(input, index);

    if (strcmp(result, expected) == 0) {
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.\n");
    }
    return 0;
}

int valid_utf8_tests(char *input, int expected){
    int result = my_utf8_check(input);
    if (result == expected){
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.\n");
    }
}

int decoding_tests(char *input, char *expected){
    char unicode[100];

    // realized I had an issue with not null terminating the array (previous tests were seeping in), so this makes sure its null terminating
    memset(unicode, 0, sizeof(unicode));
    my_utf8_decode(input, unicode);

    if (strcmp(unicode, expected) == 0) {
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.\n");
    }

    return 0;
}

int encoding_tests(char *input, char *expected) {
    char utf8[100];
    my_utf8_encode_helper(input, utf8);

    if (strcmp(utf8, expected) == 0) {
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.\n");
    }
    return 0;
}

int my_utf8_encode(char *input, char *output){
    /* start by finding the number of bytes. based on that, we need to fill in the binary pattern with the hex binary values
     * how? like checking for valid utf, we're gonna use masks to fill in the 10xxxxxx's
     * then we'll handle the leading bits and final null terminating
     */

    // ignore the \u
    input += 2;

    // hex needs to be an int for the bit manipulation
    unsigned int uInput = strtoul(input, NULL, 16);

    int bytes;
    if (uInput <= 0x7F){
        bytes = 1;
    }
    else if (uInput <= 0x7FF){
        bytes = 2;
    }
    else if (uInput <= 0xFFFF){
        bytes = 3;
    }
    else if (uInput <= 0x10FFFF) {
        bytes = 4;
    }
    else{
        return -1;
    }

    // one byte we're chilling and just give it back
    if (uInput <= 0x7F) {
        output[0] = (char)uInput;
        output[1] = '\0';
        return 1;
    }

    for (int i = 1; i < bytes; i++){
        // for bytes 2-4, we need the 6 bits for 10xxxxxx
        // calculate the shift based on number of bytes, bc each 10xxxxx is 6 we do 6 * that number
        // for the mask, we want 6 bits as in 0011 1111 which is 0x3f
        // and we want the first 2 bits to be 10 as in 1000 0000 which is 0x80 (do or so we dont mess with the rest)
        // and obv at the end we convert the hex back into char
        size_t shift = 6 * (bytes - i -1);
        output[i] = (char)(0x80 | ((uInput >> shift) & 0x3F));
    }
    // now, for the start of the bits, we need 1s in the beginning equal to the number of bytes
    // how? take a mask of all 1s and shift it to put the 1s in the digits we need based on bytes (like for 2 bytes it needs to start w 11)
    // so start by taking FF shifted left by 8 (cuz num bits) minus the num bytes
    // and then we need to shift it to be at the start since these are the leading bits by doing 6 (cuz 10xxxxxx) times the number of bytes, which will get our new bits pushed to the front
    output[0] = (char)((0xFF << (8 - bytes)) | (uInput >> (6 * (bytes - 1))));

    return bytes;

}

// helper method for encode - handling multiple characters
int my_utf8_encode_helper(const char *input, char *output) {
    while (*input != '\0') {
        if (*input == '\\' && *(input + 1) == 'u') {
            // we're gonna handle each \u character separately
            // first we're gonna need to know how long this unicode char was for when we move along the input string
            // so, again ignore the \u, then move along as long as its not the end, not exceeding 5 hex digits like we said, and still a hex digit
            int sequenceLength = 2;
            while (*(input + sequenceLength) != '\0' && sequenceLength < 7 &&
                   (isxdigit(*(input + sequenceLength)))) {
                sequenceLength++;
            }

            // now lets handle each character
            int bytes = my_utf8_encode(input, output);
            if (bytes < 0) {
                return -1;
            }
            output += bytes;
            // and then move to the next character
            input += sequenceLength;
        }

        // handling for non unicode code points just in case teachers r tricky
        else {
            *output++ = *input++;
        }
    }
    // add null terminating character
    *output = '\0';

    return 0;
}

int my_utf8_decode(char *input, char *output) {
    /* so now, we want to get it back to how it was BEFORE we got it into utf8 formatting, like w leading bits
     * so we need to know how many bytes (cuz then we know how many leading bits need to be lost
     * then, we need to mask off all the parts that we had previously added
     */
    size_t i = 0;
    size_t currentOut = 0;

    while (input[i] != '\0') {
        unsigned char currentByte = (unsigned char)input[i++];

        // find number of bytes like in utf8 check
        int bytes;
        if ((currentByte & 0x80) == 0) {
            // if it was one byte, we're chilling and just put it back out
            bytes = 1;
            output[currentOut++] = (char)currentByte;
            continue;
        } else if ((currentByte & 0xE0) == 0xC0) {
            bytes = 2;
        } else if ((currentByte & 0xF0) == 0xE0) {
            bytes = 3;
        } else if ((currentByte & 0xF8) == 0xF0) {
            bytes = 4;
        }

        // we want to mask off the leading bits that we added in encoding by ANDing but preserving j the bits we want
        // so AND with mask that has 1s in bits we want and 0s in the other - FF but shift by how many bytes there are so we ignore that amt leading 1s
        unsigned int codePoint = currentByte & (0xFF >> bytes);

        // and then we want to mask off the rest of the leading bits, as in the 10s, so we mask w 00111111 which is 3f
        // but we want to insert these 6 bits in the right spot so we have to shift the current before adding it
        for (int k = 1; k < bytes; k++) {
            codePoint = (codePoint << 6) | (input[i++] & 0x3F);
        }

        // wasn't sure how to get it back into output but found this method which stores output in char buffer (later added the index of where the output was add to handle multiple chars)
        currentOut += sprintf(output + currentOut, "\\u%04X", codePoint);

    }
    return currentOut;
}

int my_utf8_check(char *input) {
    while (*input != '\0') {
        /* see how many bytes are needed. based on that, make sure it fits the binary pattern for UTF8
         * map out the first bits to see how many bytes (by mapping with 1s in those places and 0s in the rest
         * then check that the following ones are correct
         * return false if at any point it breaks the pattern
         * */
        unsigned char currentByte = (unsigned char) (*input);

        // single byte (automatically valid) = 0xxxxxxx
        if (currentByte <= 0x7F) {
            input++;
        }

            // two bytes = 110xxxxx 10xxxxxx
            // start by mapping first 3 bits to 110
        else if ((currentByte & 0xE0) == 0xC0) {
            unsigned char nextByte = (unsigned char)(*(input + 1));
            // check whether next byte maps to 10
            if ((nextByte & 0xC0) != 0x80){
                return -1;
            }
            else{
                input += 2;
            }
        }

            // three bytes = 1110xxxx 10xxxxxx 10xxxxxx
            // start by mapping first 4 bits to 1110
        else if ((currentByte & 0xF0) == 0xE0) {
            unsigned char nextByte = (unsigned char) (*(input + 1));
            unsigned char nextNextByte = (unsigned char) (*(input + 2));
            if (((nextByte & 0xC0) != 0x80) || ((nextNextByte & 0xC0) != 0x80)){
                return -1;
            }
            else{
                input += 3;
            }
        }

            // four bytes = 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            // start by mapping first 5 bits to 11110
        else if ((currentByte & 0xF8) == 0xF0) {
            unsigned char nextByte = (unsigned char) (*(input + 1));
            unsigned char nextNextByte = (unsigned char) (*(input + 2));
            unsigned char nextNextNextByte = (unsigned char) (*(input + 3));

            if (((nextByte & 0xC0) != 0x80) || ((nextNextByte & 0xC0) != 0x80) || ((nextNextNextByte & 0xC0) != 0x80)) {
                return -1;
            }
            else {
                input += 4;
            }
        }
            // if it did not fit any pattern, it is obviously not UTF8
        else {
            return -1;
        }
    }
    return 0;
}
int my_utf8_strlen(char *input){
    size_t length = 0;

    while (*input != '\0') {
        // determine number of bytes in that character and increment length for a leading byte
        int bytes;
        if ((*input & 0x80) == 0) {
            bytes = 1;
            length++;
        } else if ((*input & 0xE0) == 0xC0) {
            bytes = 2;
            length++;
        } else if ((*input & 0xF0) == 0xE0) {
            bytes = 3;
            length++;
        } else if ((*input & 0xF8) == 0xF0) {
            bytes = 4;
            length++;
        }

        // then jump ahead that amount
        input += bytes;
        // and jump ahead for continuation bits
        while ((*input & 0xC0) == 0x80) {
            input++;
        }
    }
    return length;

}
char *my_utf8_charat(char *input, int index){
    /* for this method, we have however many utf8 characters. so to know we're at one character means making sure we see a leading bit
     * each leading bit counts as a character that we have now seen, so we find the right index based off the number of leading bits we are seeing
     */
    int curr = 0;
    while (index > 0) {
        // if it's a leading byte, decrement the index so we know to move along
        if ((input[curr] <= 0x7F) || ((input[curr] & 0xE0) == 0xC0) || ((input[curr] & 0xF0) == 0xE0) || ((input[curr] & 0xF8) == 0xF0)){
            index--;
        }
        curr++;
    }
    // once we fall out of the loop, index = 0 which means we found the right place so return that element
    // however, we need to add a null terminating string or it will return the rest of the string from that element on
    // so see how long the character is based on continuation bits (10xxxxxx)
    int i = 1;
    while ((input[curr+i] & 0xC0) == 0x80) {
        i++;
    }

    // need to remake the char* with extra space for null terminating character
    char *character = (char *)malloc(i + 1);
    // and then fill in with the character
    for (int j = 0; j < i; j++) {
        character[j] = input[curr + j];
    }
    character[i] = '\0';

    return character;

}
int my_utf8_strcmp(char *str1, char *str2) {
    while (*str1 != '\0' && *str2 != '\0'){
        // we need to know how many bytes the current chars are
        int bytes1 = 0;
        if ((*str1 & 0x80) == 0) {
            bytes1 = 1;
        } else if ((*str1 & 0xE0) == 0xC0) {
            bytes1 = 2;
        } else if ((*str1 & 0xF0) == 0xE0) {
            bytes1 = 3;
        } else if ((*str1 & 0xF8) == 0xF0) {
            bytes1 = 4;
        }
        int bytes2 = 0;
        if ((*str2 & 0x80) == 0) {
            bytes2 = 1;
        } else if ((*str2 & 0xE0) == 0xC0) {
            bytes2 = 2;
        } else if ((*str2 & 0xF0) == 0xE0) {
            bytes2 = 3;
        } else if ((*str1 & 0xF8) == 0xF0) {
            bytes2 = 4;
        }

        // then, we compare the character of that byte length by subtracting them, anything but 0 means not equal
        for (int i = 0; i < bytes1 && i < bytes2; i++) {
            int subtracted = (unsigned char)str1[i] - (unsigned char)str2[i];
            if (subtracted != 0) {
                return -1;
            }
        }

        if (bytes1 != bytes2){
            return -1;
        }

        // then go to next byte
        str1 += bytes1;
        str2 += bytes2;
    }

    // if at the end of the while loop theyre both empty, we can return 0
    if (*str1 == '\0' && *str2 == '\0'){
        return 0;
    }
    // otherwise, one was longer so they cant be equal
    return -1;
}

char* my_utf8_concat(char *str1, char *str2){
    size_t len1 = 0;
    size_t len2 = 0;

    // get lengths for when we allocate memory for new str, cant use strlen
    while (str1[len1] != '\0'){
        len1++;
    }
    while (str2[len2] != '\0'){
        len2++;
    }

    // allocate space in new string of the combined lengths plus null terminating str
    char* result = (char*)malloc((len1 + len2 + 1) * sizeof(char));

    // then insert all the letters
    for (size_t i = 0; i < len1; i++){
        result[i] = str1[i];
    }
    for (size_t j = 0; j < len2; j++){
        result[len1 + j] = str2[j];
    }

    // then insert null terminating string
    result[len1+ len2] = '\0';

    return result;
}

char* my_utf8_last(char *input) {
    size_t length = strlen(input);

    // we start at the end and then back up until we see the start of the continuation bits of this last byte
    char* beg_of_end = input + length - 1;
    while ((*beg_of_end & 0xC0) == 0x80) {
        beg_of_end--;
    }

    // find size of last char and allocate memory for it
    size_t len_last = input + length - beg_of_end;
    char* last_char = (char*)malloc((len_last + 1) * sizeof(char));

    memcpy(last_char, beg_of_end, len_last);
    last_char[len_last] = '\0';
    return last_char;

}