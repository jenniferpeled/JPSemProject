#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int my_utf8_check(char *input);
int my_utf8_encode(char *input, char *output);
int my_utf8_decode(char *input, char *output);
int my_utf8_strlen(char *input);
char* my_utf8_charat(char *input, int index);
int my_utf8_strcmp(char *str1, char *str2);
char* my_utf8_concat(char *str1, char *str2);
char* my_utf8_last(char *input);

int decoding_tests(char *input, char *expected);
int encoding_tests(char *input, char *expected);
int valid_utf8_tests(char *input);
int charat_tests(char *input, int index, char *expected);
int strlen_tests(char *input, int expected);
int strcmp_tests(char *str1, char *str2);
int strconcat_tests(char *str1, char *str2, char *expected);
int lastchar_tests(char *input, char *expected);


int main() {
    //printf("\nEncoding Tests:\n");
    //encoding_tests("05D0", "\xd7\x90");
    //encoding_tests("0024", "\x24");
    //encoding_tests("00A8", "\xc2\xa8");

    // if i pass in the emoji itself though, it fails!! so what do i do
    //encoding_tests("1F618", "\xF0\x9F\x98\x98");

    //printf("\nDecoding Tests:\n");
    //decoding_tests("\xd7\x90", "05D0");
    //decoding_tests("\x24", "0024");
    //decoding_tests("\xc2\xa8", "00A8");
    //decoding_tests("\xF0\x9F\x98\x98", "1F618");

    //printf("\nValid UTF8 Tests:\n");
    //valid_utf8_tests("א");
    //valid_utf8_tests("אריה");
    //valid_utf8_tests("Jennie");
    //valid_utf8_tests("\\uD83D\\uDE18");

    //printf("\nChar At Tests:\n");
    //charat_tests("My name is Jennie", 0, "M");
    //charat_tests("Hello, 😘 world!", 7, "😘");

    //printf("\nString Comparison Tests:\n");
    //strcmp_tests("hello", "hello");
    //strcmp_tests("abc", "abcdef");

    //printf("\nString Concatenation Tests:\n");
    //strconcat_tests( "Hello, ", "😘", "Hello, 😘");

    //printf("\nLast Character Tests:\n")
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
    char* result = my_utf8_concat(str1, str2);
    if (strcmp(result, expected) == 0) {
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.");
    }
    return 0;
}

int strcmp_tests(char *str1, char *str2){
    int result = my_utf8_strcmp(str1, str2);
    if (result == 0){
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.");
    }
    return 0;
}

int strlen_tests(char *input, int expected){

}

int charat_tests(char *input, int index, char *expected){
    char *result = my_utf8_charat(input, index);

    if (strcmp(result, expected) == 0) {
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.");
    }
    return 0;
}

int encoding_tests(char *input, char *expected){
    char utf8[10];
    int bytes = my_utf8_encode(input, utf8);

    if (memcmp(utf8, expected, bytes) == 0) {
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.\n");
    }
    return 0;
}

int decoding_tests(char *input, char *expected){
    char unicode[10];
    my_utf8_decode(input, unicode);
    if (strcmp(unicode, expected) == 0) {
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.\n");
    }
    return 0;
}

int valid_utf8_tests(char *input){
    int result = my_utf8_check(input);
    if (result == 0){
        printf("Test passed!\n");
    }
    else {
        printf("Test failed.\n");
    }
}


int my_utf8_encode(char *input, char *output){
    /* start by finding the number of bytes. based on that, we need to fill in the binary pattern with the hex binary values
     * how? like checking for valid utf, we're gonna use masks to fill in the 10xxxxxx's
     * then we'll handle the leading bits and final null terminating
     */

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
        size_t shift = 6 * (bytes - i -1);
        output[i] = (char)(0x80 | ((uInput >> shift) & 0x3F));

    }
    // now, for the start of the bits, we need 1s in the beginning equal to the number of bytes
    // how? take a mask of all 1s and shift it to put the 1s in the digits we need based on bytes (like for 2 bytes it needs to start w 11)
    // so start by taking FF shifted left by 8 (cuz num bits) minus the num bytes
    // and then we need to shift it to be at the start since these are the leading bits by doing 6 (cuz 10xxxxxx) times the number of bytes, which will get our new bits pushed to the front
    output[0] = (char)((0xFF << (8 - bytes)) | (uInput >> (6 * (bytes - 1))));
    // null terminating character
    output[bytes] = '\0';

    return bytes;

}

int my_utf8_decode(char *input, char *output) {
    /* so now, we want to get it back to how it was BEFORE we got it into utf8 formatting, like w leading bits
     * so we need to know how many bytes (cuz then we know  how many leading bits need to be lost
     * then,
     *
     */
    size_t i = 0;

    while (input[i] != '\0') {
        unsigned char currentByte = (unsigned char) input[i++];

        // find number of bytes like in utf8 check
        int bytes;
        if ((currentByte & 0x80) == 0) {
            bytes = 1;
        } else if ((currentByte & 0xE0) == 0xC0) {
            bytes = 2;
        } else if ((currentByte & 0xF0) == 0xE0) {
            bytes = 3;
        } else if ((currentByte & 0xF8) == 0xF0) {
            bytes = 4;
        }

        // we want to mask off the leading bits that we added in encoding by ANDing but preserving j the bits we want
        // so and with mask that has 1s in bits we want and 0s in the other - FF but shift by how many bytes so we ignore that leading
        unsigned int codePoint = currentByte & (0xFF >> bytes);

        // and then we want to mask off the rest of the leading bits, as in the 10s, so we mask w 00111111 which is 3f
        // but we want to insert these 6 bits in the right spot so we have to shift the current before adding it
        for (int k = 1; k < bytes; k++) {
            codePoint = (codePoint << 6) | (input[i++] & 0x3F);
        }

        // wasn't sure how to get it back into output but found this method which stores output in char buffer
        sprintf(output, "%04X", codePoint);
    }
    return i;
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
        // Check the number of bytes in the UTF-8 character
        int bytes;
        if ((*input & 0x80) == 0) {
            bytes = 1;
        } else if ((*input & 0xE0) == 0xC0) {
            bytes = 2;
        } else if ((*input & 0xF0) == 0xE0) {
            bytes = 3;
        } else if ((*input & 0xF8) == 0xF0) {
            bytes = 4;
        } else {
            return -1;
        }

        length++;
        input += bytes;

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

        // then go to next byte
        str1 += bytes1;
        str2 += bytes2;
    }

    // if at the while loop they arent both empty then one was longer and they were not equal
    if (*str1 != *str2){
        return -1;
    }

    // if we go to this point, then the strings are equal
    return 0;
}

char* my_utf8_concat(char *str1, char *str2){
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    // allocate space in new string of the combined lengths plus null terminating str
    char* result = (char*)malloc((len1 + len2 + 1) * sizeof(char));

    strcpy(result, str1);
    strcpy(result + len1, str2);

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