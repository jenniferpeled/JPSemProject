#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int my_utf8_check(char *string);
int my_utf8_encode(char *input, char *output);
int my_utf8_decode(char *input, char *output);
int my_utf8_strlen(char *string);
char *my_utf8_charat(char *string, int index);
int decoding_tests(char *input, char *expected);
int encoding_tests(char *input, char *expected);
int valid_utf8_tests(char *input);
unsigned int unicode_to_codepoint(const char *input);

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

    char *utf8_string = "Hello, 😘 world!";

    // Get the character at index 7
    char *character_at_7 = my_utf8_charat(utf8_string, 7);

    if (character_at_7 != NULL) {
        // Determine the length of the character
        int char_length = 1;
        while ((character_at_7[char_length] & 0xC0) == 0x80) {
            char_length++;
        }

        // Print the character up to its length
        printf("Character at index 7: %.*s\n", char_length, character_at_7);
    } else {
        printf("Index out of range or invalid string.\n");
    }


    return 0;

}

int encoding_tests(char *input, char *expected){
    char utf8[10];
    int bytes = my_utf8_encode(input, utf8);

    if (memcmp(utf8, expected, bytes) == 0) {
        printf("Test passed!\n");
    } else {
        printf("Test failed: Actual UTF-8 does not match expected.\n");
    }

}

int decoding_tests(char *input, char *expected){
    char unicode[10];
    my_utf8_decode(input, unicode);
    if (strcmp(unicode, expected) == 0) {
        printf("Test passed!\n");
    }
    else {
        printf("Test failed: Actual Unicode does not match expected.\n");
    }
    return 0;
}

int valid_utf8_tests(char *input){
    int result = my_utf8_check(input);
    if (result == 0){
        printf("Test passed!\n");
    }
    else {
        printf("Test failed: String is not valid UTF8.\n");
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

int my_utf8_check(char *string) {
    while (*string != '\0') {
        /* see how many bytes are needed. based on that, make sure it fits the binary pattern for UTF8
         * map out the first bits to see how many bytes (by mapping with 1s in those places and 0s in the rest
         * then check that the following ones are correct
         * return false if at any point it breaks the pattern
         * */
        unsigned char currentByte = (unsigned char) (*string);

        // single byte (automatically valid) = 0xxxxxxx
        if (currentByte <= 0x7F) {
            string++;
        }

            // two bytes = 110xxxxx 10xxxxxx
            // start by mapping first 3 bits to 110
        else if ((currentByte & 0xE0) == 0xC0) {
            unsigned char nextByte = (unsigned char)(*(string + 1));
            // check whether next byte maps to 10
            if ((nextByte & 0xC0) != 0x80){
                return -1;
            }
            else{
                string += 2;
            }
        }

            // three bytes = 1110xxxx 10xxxxxx 10xxxxxx
            // start by mapping first 4 bits to 1110
        else if ((currentByte & 0xF0) == 0xE0) {
            unsigned char nextByte = (unsigned char) (*(string + 1));
            unsigned char nextNextByte = (unsigned char) (*(string + 2));
            if (((nextByte & 0xC0) != 0x80) || ((nextNextByte & 0xC0) != 0x80)){
                return -1;
            }
            else{
                string += 3;
            }
        }

            // four bytes = 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            // start by mapping first 5 bits to 11110
        else if ((currentByte & 0xF8) == 0xF0) {
            unsigned char nextByte = (unsigned char) (*(string + 1));
            unsigned char nextNextByte = (unsigned char) (*(string + 2));
            unsigned char nextNextNextByte = (unsigned char) (*(string + 3));
            if (((nextByte & 0xC0) != 0x80) || ((nextNextByte & 0xC0) != 0x80) || ((nextNextNextByte & 0xC0) != 0x80)) {
                return -1;
            }
            else {
                string += 4;
            }
        }
            // if it did not fit any pattern, it is obviously not UTF8
        else {
            return -1;
        }
    }
    return 0;
}
int my_utf8_strlen(char *string){
    // count leading bits that we see (meaning when its like 110, 1110, etc)
    int len = 0;
    while (*string != '\0'){
        // so if it starts with 10, we know its continuing the pattern and dont add to length
        if ((*string & 0xC0) != 0x80){
            len++;
        }
        string++;
    }
    return len;
}
char *my_utf8_charat(char *string, int index){
    /* for this method, we have however many utf8 characters. so to know we're at one character means making sure we see a leading bit
     * each leading bit counts as a character that we have now seen, so we find the right index based off the number of leading bits we are seeing
     */
    int i = 0;

    while (index > 0) {
        // if it's a leading byte, decrement the index so we know to move along
        if ((string[i] <= 0x7F) || ((string[i] & 0xE0) == 0xC0) || ((string[i] & 0xF0) == 0xE0) || ((string[i] & 0xF8) == 0xF0)){
            index--;
        }
        i++;
    }
    // once we fall out of the loop, index = 0 which means we found the right place
    // so return pointer to that element
    return &string[i];

}