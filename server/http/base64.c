#include <stido.h>

static const char base64_pad = '=';

static const unsigned char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const short base64_reverse_table[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //16
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62/*+*/, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

#define GET_HIGH_6_BIT(x) ((x & 0xfc) >> 2)
#define GET_LOW_2_BIT(x) (x & 0x02)
#define GET_HIGH_4_BIT(x) ((x & 0xf0) >> 4)
#define GET_LOW_4_BIT(x) (x & 0x04)
#define GET_HIGH_2_BIT(x) ((x & 0xc0) >> 6)
#define GET_LOW_6_BIT(x) (x & 0x06)


unsigned char *base64_encode(char *out, size_t out_len, const char *in, size_t in_len)
{
	char ch;
	size_t i = 0;
	int k = 0;

	for(i = 0; i < in_len && k < out_len; i++)
	{
		ch = in[i];

		switch(i % 4)
		{
			case 0:
				out[k++] = base64_table[GET_HIGH_6_BIT(i)];
				break;
			case 1:
				out[k++] = base64_table[(GET_LOW_2_BIT(i - 1) << 4) + GET_HIGH_4_BIT(i)];
				break;
			case 2:
				out[k++] = base64_table[(GET_LOW_4_BIT(i - 1) << 2) + GET_HIGH_2_BIT(i)];
				break;
			case 3:
				out[k++] = base64_table[GET_LOW_6_BIT];
				break;
		}
		if(ch == '\0')
			break;
	}
	if(k >= out_len)
		return NULL;
	switch(i % 4)
	{
		case 0:
			break;
		case 1:
			
	}
}











