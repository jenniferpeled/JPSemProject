This is Jennie's Peled final project for Computer Systems, Fall 2023. Below is an overview of the relevant methods and my thinking behind them.

int my_utf8_encode_helper = a method to encode a single character into UTF8
First, I determine how many bytes are needed in UTF8 based on what I learned online. I then need to take the binary from unicode and get it properly formatted in UTF8 based on the rules online.
For one byte, we don't need to do anything special. But for bytes that are 2-4, we need special leading bits. I first handle everything besides the beginning, meaning the parts that need to be formatted as 10xxxxxx. 
We always want to preserve the 6 bits from unicode, which we can do by ANDing with a mask of 0011 1111 (x3F). We also need to make sure these bits end up in the proper spot, so we shift by 6 times the number of bytes minus what we're currently handling.
For the first two bits, we need to make sure that they are 10, which we get using 1000 0000 (x80). 
Once we've handled all of this, in a for loop based on the number of bytes, we handle the beginning, which needs the number of 1s equal to the number of bytes.
Therefore, we make a mask of 1s (xFF) in all the places we need 1s (meaning shift it to the right spots), and then move this all the way to the start.
Credit to ChatGPT, which I consulted in helping me with understanding how to shift the bits properly - it helped me calculate (6 * (bytes - 1))

int my_utf8_encode = a method to encode full strings into UTF8
First, I differentiate each character by its starting with \u, and find the length of the current character. 
Then, I handle each character separately, calling the helper method above, then moving to the next character.
I also included handling of non unicode code points, and of course ended with a null terminating character.

int my_utf8_decode = a method to decode a UTF8 character back into Unicode
Like before, we need to know how many bytes it is, which we do using masks again. If it starts with 10 (from x80 again), we know it is 1 byte. If not, we check if it starts as 110, 1110, or 11110 for how many bytes by ANDing with respective masks.
Once we know that, we want to get rid of the leading bits that we added when encoding. So first, we get rid of the first bits by shifting back the other way with the xFF mask. 
Then, we again use a for loop to handle those with 10xxxxxx. We want the last 6 bits only, so again we use 0011 1111 (x3f) and again shift 6 to put them in the right spots.
At the end, we put a null terminating character. 

int my_utf8_check = a method to check whether a string is valid UTF8
To see if it is a valid string, I check how the byte begins. Depending on how many bytes it seems to need, I make sure that all the continuation bytes begin with 10xxxxxx. If at any point that is not true, the string is not valid UTF8.
I found this fairly simple, by using masks for the leading bits respective to the number of 1s needing, and using x80 mask for all continuation bits. 

int my_utf8_strlen = a method to return the length, based on characters, of a string
I incremented the length for each leading byte I saw, finding leading bytes with the same logic and the utf8 check. 
I would then jump ahead that amount of bytes, and jump ahead for any continuation bits. Since I only increment length by 1 for eaching leading byte, this gives us the amount of characters.

int my_builtin_strlen = a method to return the length, based on bytes, of a string
Since we could not use built in string methods, I wrote my own to mimic the built in string length for some functions and testing. It works similarly to the strlen above, but increments the length with the number of bytes.

char *my_utf8_charat = a method to return the UTF8 char at that index
I start with knowing from past experience how to get an element at an index where you cannot directly index in. Within that loop, I need to know how many bytes the character is, because I want the index to be character-based, meaning the user can't index into the second byte of a character bc they probably did not mean to do that.
So, I check how many bytes, then jump ahead that amount in the string, then increment my counter against the index. Once I exit the loop, we are at the right character.
At that point, we just need to return the character at that index. However, I realized through testing that we need to add a null terminating character, otherwise the rest of the string will be returned as well, so I reallocated space for the character with a null terminating space at the end.

int my_utf8_strcmp = a method to compare two strings
First, I find how many bytes the current character of each string is. Then, we compare the characters of that byte length by subtracting them. If that does not result in 0, they are already not equal. If they are not the same number of bytes, they are also not equal. We then move to the next byte and continue this check. At the end of our while loop, if they are both empty, they were equal (and if not, not).

char* my_utf8_concat = a method to concatenate two strings
First, I find the lengths of each original string, and use that information to allocate a new string of the combined lengths plus a null char. 
Then, it was simple to insert all the letters in the proper spot of the resulting char array. I then null terminated and returned the new string.

char* my_utf8_last = a method to return the last character of a string
First, I used my fake built in strlen to find the length of the input. Then, I backed up from the end of the string until I got to the start of the last byte, meaning before these last continuation bytes.
Then, I found the length of the last character and allocated space for it in a new resulting array. I then copied in the last character and added a null terminating char.
