This is Jennie's Peled final project for Computer Systems, Fall 2023. Below is an overview of the relevant methods and my thinking behind them.

int my_utf8_encode(char *input, char *output) = a method to encode any character into UTF8
First, I determine how many bytes are needed in UTF8 based on what I learned online. I then need to take the binary from unicode and get it properly formatted in UTF8 based on the rules online.
For one byte, we don't need to do anything special. But for bytes that are 2-4, we need special leading bits. I first handle everything besides the beginning, meaning the parts that need to be formatted as 10xxxxxx. 
We always want to preserve the 6 bits from unicode, which we can do by ANDing with a mask of 0011 1111 (x3F). We also need to make sure these bits end up in the proper spot, so we shift by 6 times the number of bytes minus what we're currently handling.
For the first two bits, we need to make sure that they are 10, which we get using 1000 0000 (x80). 
Once we've handled all of this, in a for loop based on the number of bytes, we handle the beginning, which needs the number of 1s equal to the number of bytes.
Therefore, we make a mask of 1s (xFF) in all the places we need 1s (meaning shift it to the right spots), and then move this all the way to the start.
At the end, we put a null terminating character.  
Credit to ChatGPT, which I consulted in helping me with understanding how to shift the bits properly - it helped me calculate (6 * (bytes - 1))

int my_utf8_decode(char *input, char *output) = a method to decode a UTF8 character back into Unicode
Like before, we need to know how many bytes it is, which we do using masks again. If it starts with 10 (from x80 again), we know it is 1 byte. If not, we check if it starts as 110, 1110, or 11110 for how many bytes by ANDing with respective masks.
Once we know that, we want to get rid of the leading bits that we added when encoding. So first, we get rid of the first bits by shifting back the other way with the xFF mask. 
Then, we again use a for loop to handle those with 10xxxxxx. We want the last 6 bits only, so again we use 0011 1111 (x3f) and again shift 6 to put them in the right spots.
At the end, we put a null terminating character. 

int my_utf8_check(char *string) = a method to check whether a string is valid UTF8
To see if it is a valid string, I check how the byte begins. Depending on how many bytes it seems to need, I make sure that all the continuation bytes begin with 10xxxxxx. If at any point that is not true, the string is not valid UTF8.
I found this fairly simple, by using masks for the leading bits respective to the number of 1s needing, and using x80 mask for all continuation bits. 

char *my_utf8_charat(char *string, int index) = a method to return the UTF8 char at that index
As I know from my earlier methods, we can check whether a byte is a start byte. If it is, we know it is the start of a character which may or may not be what we are looking for.
To figure that out, I decrement the index each time we see a start byte, so that by the time the index is 0, I am at the right spot for the character the index specified.
At that point, all I need to do is return the character at that index. 