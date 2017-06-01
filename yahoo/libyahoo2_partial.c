#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "yahoolib2_partial.h"
#include "md5.h"
#include "sha.h"
#include "yahoo_util.h"	// only for FREE
#include "yahoo_fn.h"
#define snprintf _snprintf
extern char *yahoo_crypt(char *key, char *salt);

static char base64digits[] = 	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789._";
static void to_y64(unsigned char *out, const unsigned char *in, int inlen)
/* raw bytes in quasi-big-endian order to base 64 string (NUL-terminated) */
{
	for (; inlen >= 3; inlen -= 3)
		{
			*out++ = base64digits[in[0] >> 2];
			*out++ = base64digits[((in[0]<<4) & 0x30) | (in[1]>>4)];
			*out++ = base64digits[((in[1]<<2) & 0x3c) | (in[2]>>6)];
			*out++ = base64digits[in[2] & 0x3f];
			in += 3;
		}
	if (inlen > 0)
		{
			unsigned char fragment;

			*out++ = base64digits[in[0] >> 2];
			fragment = (in[0] << 4) & 0x30;
			if (inlen > 1)
				fragment |= in[1] >> 4;
			*out++ = base64digits[fragment];
			*out++ = (inlen < 2) ? '-' 
					: base64digits[(in[1] << 2) & 0x3c];
			*out++ = '-';
		}
	*out = '\0';
}

// libyahoo2 name:
//static void yahoo_process_auth_pre_0x0b(struct yahoo_input_data *yid, 
//		const char *seed, const char *sn)

// result6 and result96 must be at least 25 characters long
// user is yd->user
// pw is yd->pw
void GetYahooStringsPreVer11(const char *seed,const char *sn,
	char *user,char *pw,unsigned char *result6,unsigned char *result96)
{	
	/* So, Yahoo has stopped supporting its older clients in India, and 
	 * undoubtedly will soon do so in the rest of the world.
	 * 
	 * The new clients use this authentication method.  I warn you in 
	 * advance, it's bizzare, convoluted, inordinately complicated.  
	 * It's also no more secure than crypt() was.  The only purpose this 
	 * scheme could serve is to prevent third part clients from connecting 
	 * to their servers.
	 *
	 * Sorry, Yahoo.
	 */
	
	md5_byte_t result[16];
	md5_state_t ctx;
	char *crypt_result;
	unsigned char *password_hash = malloc(25);
	unsigned char *crypt_hash = malloc(25);
	unsigned char *hash_string_p = malloc(50 + strlen(sn));
	unsigned char *hash_string_c = malloc(50 + strlen(sn));
	
	char checksum;
	
	int sv;
	
	//unsigned char *result6 = malloc(25);
	//unsigned char *result96 = malloc(25);

	sv = seed[15];
	sv = (sv % 8) % 5;

	md5_init(&ctx);
	md5_append(&ctx, (md5_byte_t *)pw, strlen(pw));
	md5_finish(&ctx, result);
	to_y64(password_hash, result, 16);
	
	md5_init(&ctx);
	crypt_result = yahoo_crypt(pw, "$1$_2S43d5f$");  
	md5_append(&ctx, (md5_byte_t *)crypt_result, strlen(crypt_result));
	md5_finish(&ctx, result);
	to_y64(crypt_hash, result, 16);
	free(crypt_result);

	switch (sv) {
	case 0:
		checksum = seed[seed[7] % 16];
		snprintf((char *)hash_string_p, strlen(sn) + 50,
			"%c%s%s%s", checksum, password_hash, user, seed);
		snprintf((char *)hash_string_c, strlen(sn) + 50,
			"%c%s%s%s", checksum, crypt_hash, user, seed);
		break;
	case 1:
		checksum = seed[seed[9] % 16];
		snprintf((char *)hash_string_p, strlen(sn) + 50,
			"%c%s%s%s", checksum, user, seed, password_hash);
		snprintf((char *)hash_string_c, strlen(sn) + 50,
			"%c%s%s%s", checksum, user, seed, crypt_hash);
		break;
	case 2:
		checksum = seed[seed[15] % 16];
		snprintf((char *)hash_string_p, strlen(sn) + 50,
			"%c%s%s%s", checksum, seed, password_hash, user);
		snprintf((char *)hash_string_c, strlen(sn) + 50,
			"%c%s%s%s", checksum, seed, crypt_hash, user);
		break;
	case 3:
		checksum = seed[seed[1] % 16];
		snprintf((char *)hash_string_p, strlen(sn) + 50,
			"%c%s%s%s", checksum, user, password_hash, seed);
		snprintf((char *)hash_string_c, strlen(sn) + 50,
			"%c%s%s%s", checksum, user, crypt_hash, seed);
		break;
	case 4:
		checksum = seed[seed[3] % 16];
		snprintf((char *)hash_string_p, strlen(sn) + 50,
			"%c%s%s%s", checksum, password_hash, seed, user);
		snprintf((char *)hash_string_c, strlen(sn) + 50,
			"%c%s%s%s", checksum, crypt_hash, seed, user);
		break;
	}
		
	md5_init(&ctx);  
	md5_append(&ctx, (md5_byte_t *)hash_string_p, strlen((char *)hash_string_p));
	md5_finish(&ctx, result);
	to_y64(result6, result, 16);

	md5_init(&ctx);  
	md5_append(&ctx, (md5_byte_t *)hash_string_c, strlen((char *)hash_string_c));
	md5_finish(&ctx, result);
	to_y64(result96, result, 16);


	//FREE(result6);
	//FREE(result96);
	FREE(password_hash);
	FREE(crypt_hash);
	FREE(hash_string_p);
	FREE(hash_string_c);

}

/*
 * New auth protocol cracked by Cerulean Studios and sent in to Gaim
 */
//static void yahoo_process_auth_0x0b(struct yahoo_input_data *yid, const char *seed, const char *sn)

// resp_6 and resp_96 must be 100 chars
void GetYahooStringsVer11(const char *seed,const char *sn,
	char *pw,char *res6,char *res96)
{
	//struct yahoo_packet *pack = NULL;
	//struct yahoo_data *yd = yid->yd;

	md5_byte_t         result[16];
	md5_state_t        ctx;

	SHA_CTX            ctx1;
	SHA_CTX            ctx2;

	char *alphabet1 = "FBZDWAGHrJTLMNOPpRSKUVEXYChImkwQ";
	char *alphabet2 = "F0E1D2C3B4A59687abcdefghijklmnop";

	char *challenge_lookup = "qzec2tb3um1olpar8whx4dfgijknsvy5";
	char *operand_lookup = "+|&%/*^-";
	char *delimit_lookup = ",;";

	unsigned char *password_hash = malloc(25);
	unsigned char *crypt_hash = malloc(25);
	char *crypt_result = NULL;
	unsigned char pass_hash_xor1[64];
	unsigned char pass_hash_xor2[64];
	unsigned char crypt_hash_xor1[64];
	unsigned char crypt_hash_xor2[64];
	unsigned char chal[7];
	char resp_6[100];
	char resp_96[100];

	unsigned char digest1[20];
	unsigned char digest2[20];
	unsigned char magic_key_char[4];
	const unsigned char *magic_ptr;

	unsigned int  magic[64];
	unsigned int  magic_work=0;

	char comparison_src[20];

	int x, j, i;
	int cnt = 0;
	int magic_cnt = 0;
	int magic_len;
	int depth =0, table =0;

	memset(&pass_hash_xor1, 0, 64);
	memset(&pass_hash_xor2, 0, 64);
	memset(&crypt_hash_xor1, 0, 64);
	memset(&crypt_hash_xor2, 0, 64);
	memset(&digest1, 0, 20);
	memset(&digest2, 0, 20);
	memset(&magic, 0, 64);
	memset(&resp_6, 0, 100);
	memset(&resp_96, 0, 100);
	memset(&magic_key_char, 0, 4);

	/* 
	 * Magic: Phase 1.  Generate what seems to be a 30 
	 * byte value (could change if base64
	 * ends up differently?  I don't remember and I'm 
	 * tired, so use a 64 byte buffer.
	 */

	magic_ptr = (unsigned char *)seed;

	while (*magic_ptr != (int)NULL) {
		char *loc;

		/* Ignore parentheses.  */

		if (*magic_ptr == '(' || *magic_ptr == ')') {
			magic_ptr++;
			continue;
		}

		/* Characters and digits verify against 
		   the challenge lookup.
		*/

		if (isalpha(*magic_ptr) || isdigit(*magic_ptr)) {
			loc = strchr(challenge_lookup, *magic_ptr);
			if (!loc) {
				/* This isn't good */
				continue;
			}

			/* Get offset into lookup table and lsh 3. */

			magic_work = loc - challenge_lookup;
			magic_work <<= 3;

			magic_ptr++;
			continue;
		} else {
			unsigned int local_store;

			loc = strchr(operand_lookup, *magic_ptr);
			if (!loc) {
				/* Also not good. */
				continue;
			}

			local_store = loc - operand_lookup;

			/* Oops; how did this happen? */
			if (magic_cnt >= 64) 
				break;

			magic[magic_cnt++] = magic_work | local_store;
			magic_ptr++;
			continue;
		}
	}

	magic_len = magic_cnt;
	magic_cnt = 0;

	/* Magic: Phase 2.  Take generated magic value and 
	 * sprinkle fairy dust on the values. */

	for (magic_cnt = magic_len-2; magic_cnt >= 0; magic_cnt--) {
		unsigned char byte1;
		unsigned char byte2;

		/* Bad.  Abort.
		 */
		if (magic_cnt >= magic_len) {
			//WARNING(("magic_cnt(%d)  magic_len(%d)", magic_cnt, magic_len))
			break;
		}

		byte1 = magic[magic_cnt];
		byte2 = magic[magic_cnt+1];

		byte1 *= 0xcd;
		byte1 ^= byte2;

		magic[magic_cnt+1] = byte1;
	}

	/* Magic: Phase 3.  This computes 20 bytes.  The first 4 bytes are used as our magic 
	 * key (and may be changed later); the next 16 bytes are an MD5 sum of the magic key 
	 * plus 3 bytes.  The 3 bytes are found by looping, and they represent the offsets 
	 * into particular functions we'll later call to potentially alter the magic key. 
	 * 
	 * %-) 
	 */ 
	
	magic_cnt = 1; 
	x = 0; 
	
	do { 
		unsigned int     bl = 0;  
		unsigned int     cl = magic[magic_cnt++]; 
		
		if (magic_cnt >= magic_len) 
			break; 
		
		if (cl > 0x7F) { 
			if (cl < 0xe0)  
				bl = cl = (cl & 0x1f) << 6;  
			else { 
				bl = magic[magic_cnt++];  
                              cl = (cl & 0x0f) << 6;  
                              bl = ((bl & 0x3f) + cl) << 6;  
			}  
			
			cl = magic[magic_cnt++];  
			bl = (cl & 0x3f) + bl;  
		} else 
			bl = cl;  
		
		comparison_src[x++] = (bl & 0xff00) >> 8;  
		comparison_src[x++] = bl & 0xff;  
	} while (x < 20); 

	/* Dump magic key into a char for SHA1 action. */
	
		
	for(x = 0; x < 4; x++) 
		magic_key_char[x] = comparison_src[x];

	/* Compute values for recursive function table! */
	memcpy( chal, magic_key_char, 4 );
        x = 1;
	for( i = 0; i < 0xFFFF && x; i++ )
	{
		for( j = 0; j < 5 && x; j++ )
		{
			chal[4] = i;
			chal[5] = i >> 8;
			chal[6] = j;
			md5_init( &ctx );
			md5_append( &ctx, chal, 7 );
			md5_finish( &ctx, result );
			if( memcmp( comparison_src + 4, result, 16 ) == 0 )
			{
				depth = i;
				table = j;
				x = 0;
			}
		}
	}

	/* Transform magic_key_char using transform table */
	x = magic_key_char[3] << 24  | magic_key_char[2] << 16 
		| magic_key_char[1] << 8 | magic_key_char[0];
	x = yahoo_xfrm( table, depth, x );
	x = yahoo_xfrm( table, depth, x );
	magic_key_char[0] = x & 0xFF;
	magic_key_char[1] = x >> 8 & 0xFF;
	magic_key_char[2] = x >> 16 & 0xFF;
	magic_key_char[3] = x >> 24 & 0xFF;

	/* Get password and crypt hashes as per usual. */
	md5_init(&ctx);
	md5_append(&ctx, (md5_byte_t *)pw,  strlen(pw));
	md5_finish(&ctx, result);
	to_y64(password_hash, result, 16);

	md5_init(&ctx);
	crypt_result = yahoo_crypt(pw, "$1$_2S43d5f$");  
	md5_append(&ctx, (md5_byte_t *)crypt_result, strlen(crypt_result));
	md5_finish(&ctx, result);
	to_y64(crypt_hash, result, 16);

	/* Our first authentication response is based off 
	 * of the password hash. */

	for (x = 0; x < (int)strlen((char *)password_hash); x++) 
		pass_hash_xor1[cnt++] = password_hash[x] ^ 0x36;

	if (cnt < 64) 
		memset(&(pass_hash_xor1[cnt]), 0x36, 64-cnt);

	cnt = 0;

	for (x = 0; x < (int)strlen((char *)password_hash); x++) 
		pass_hash_xor2[cnt++] = password_hash[x] ^ 0x5c;

	if (cnt < 64) 
		memset(&(pass_hash_xor2[cnt]), 0x5c, 64-cnt);

	shaInit(&ctx1);
	shaInit(&ctx2);

	/* The first context gets the password hash XORed 
	 * with 0x36 plus a magic value
	 * which we previously extrapolated from our 
	 * challenge. */

	shaUpdate(&ctx1, pass_hash_xor1, 64);
	if (j >= 3)
		ctx1.sizeLo = 0x1ff;
	shaUpdate(&ctx1, magic_key_char, 4);
	shaFinal(&ctx1, digest1);

	 /* The second context gets the password hash XORed 
	  * with 0x5c plus the SHA-1 digest
	  * of the first context. */

	shaUpdate(&ctx2, pass_hash_xor2, 64);
	shaUpdate(&ctx2, digest1, 20);
	shaFinal(&ctx2, digest2);

	/* Now that we have digest2, use it to fetch 
	 * characters from an alphabet to construct
	 * our first authentication response. */

	for (x = 0; x < 20; x += 2) {
		unsigned int    val = 0;
		unsigned int    lookup = 0;
		char            byte[6];

		memset(&byte, 0, 6);

		/* First two bytes of digest stuffed 
		 *  together.
		 */

		val = digest2[x];
		val <<= 8;
		val += digest2[x+1];

		lookup = (val >> 0x0b);
		lookup &= 0x1f;
		if (lookup >= strlen(alphabet1))
			break;
		sprintf(byte, "%c", alphabet1[lookup]);
		strcat(resp_6, byte);
		strcat(resp_6, "=");

		lookup = (val >> 0x06);
		lookup &= 0x1f;
		if (lookup >= strlen(alphabet2))
			break;
		sprintf(byte, "%c", alphabet2[lookup]);
		strcat(resp_6, byte);

		lookup = (val >> 0x01);
		lookup &= 0x1f;
		if (lookup >= strlen(alphabet2))
			break;
		sprintf(byte, "%c", alphabet2[lookup]);
		strcat(resp_6, byte);

		lookup = (val & 0x01);
		if (lookup >= strlen(delimit_lookup))
			break;
		sprintf(byte, "%c", delimit_lookup[lookup]);
		strcat(resp_6, byte);
	}

	/* Our second authentication response is based off 
	 * of the crypto hash. */

	cnt = 0;
	memset(&digest1, 0, 20);
	memset(&digest2, 0, 20);

	for (x = 0; x < (int)strlen((char *)crypt_hash); x++) 
		crypt_hash_xor1[cnt++] = crypt_hash[x] ^ 0x36;

	if (cnt < 64) 
		memset(&(crypt_hash_xor1[cnt]), 0x36, 64-cnt);

	cnt = 0;

	for (x = 0; x < (int)strlen((char *)crypt_hash); x++) 
		crypt_hash_xor2[cnt++] = crypt_hash[x] ^ 0x5c;

	if (cnt < 64) 
		memset(&(crypt_hash_xor2[cnt]), 0x5c, 64-cnt);

	shaInit(&ctx1);
	shaInit(&ctx2);

	/* The first context gets the password hash XORed 
	 * with 0x36 plus a magic value
	 * which we previously extrapolated from our 
	 * challenge. */

	shaUpdate(&ctx1, crypt_hash_xor1, 64);
	if (j >= 3)
		ctx1.sizeLo = 0x1ff;
	shaUpdate(&ctx1, magic_key_char, 4);
	shaFinal(&ctx1, digest1);

	/* The second context gets the password hash XORed 
	 * with 0x5c plus the SHA-1 digest
	 * of the first context. */

	shaUpdate(&ctx2, crypt_hash_xor2, 64);
	shaUpdate(&ctx2, digest1, 20);
	shaFinal(&ctx2, digest2);

	/* Now that we have digest2, use it to fetch 
	 * characters from an alphabet to construct
	 * our first authentication response.  */

	for (x = 0; x < 20; x += 2) {
		unsigned int val = 0;
		unsigned int lookup = 0;

		char byte[6];

		memset(&byte, 0, 6);

		/* First two bytes of digest stuffed 
		 *  together. */

		val = digest2[x];
		val <<= 8;
		val += digest2[x+1];

		lookup = (val >> 0x0b);
		lookup &= 0x1f;
		if (lookup >= strlen(alphabet1))
			break;
		sprintf(byte, "%c", alphabet1[lookup]);
		strcat(resp_96, byte);
		strcat(resp_96, "=");

		lookup = (val >> 0x06);
		lookup &= 0x1f;
		if (lookup >= strlen(alphabet2))
			break;
		sprintf(byte, "%c", alphabet2[lookup]);
		strcat(resp_96, byte);

		lookup = (val >> 0x01);
		lookup &= 0x1f;
		if (lookup >= strlen(alphabet2))
			break;
		sprintf(byte, "%c", alphabet2[lookup]);
		strcat(resp_96, byte);

		lookup = (val & 0x01);
		if (lookup >= strlen(delimit_lookup))
			break;
		sprintf(byte, "%c", delimit_lookup[lookup]);
		strcat(resp_96, byte);
	}

	/*
	pack = yahoo_packet_new(YAHOO_SERVICE_AUTHRESP, yd->initial_status, yd->session_id);
	yahoo_packet_hash(pack, 0, sn);
	yahoo_packet_hash(pack, 6, resp_6);
	yahoo_packet_hash(pack, 96, resp_96);
	yahoo_packet_hash(pack, 1, sn);
	yahoo_send_packet(yid, pack, 0);
	yahoo_packet_free(pack);
	*/

	memcpy(res6,&resp_6[0],sizeof(resp_6)/sizeof(resp_6[0]));
	memcpy(res96,&resp_96[0],sizeof(resp_6)/sizeof(resp_96[0]));
	free(password_hash);
	free(crypt_hash);
}

/*
static void yahoo_process_auth(struct yahoo_input_data *yid, struct yahoo_packet *pkt)
{
	char *seed = NULL;
	char *sn   = NULL;
	YList *l = pkt->hash;
	int m = 0;

	while (l) {
		struct yahoo_pair *pair = l->data;
		if (pair->key == 94)
			seed = pair->value;
		if (pair->key == 1)
			sn = pair->value;
		if (pair->key == 13)
			m = atoi(pair->value);
		l = l->next;
	}

	if (!seed) 
		return;

	switch (m) {
		case 0:
			yahoo_process_auth_pre_0x0b(yid, seed, sn);
			break;
		case 1:
			yahoo_process_auth_0x0b(yid, seed, sn);
			break;
		default:
			// call error
			WARNING(("unknown auth type %d", m));
			yahoo_process_auth_0x0b(yid, seed, sn);
			break;
	}
}
*/