#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
 
#include <sys/types.h>
#include <sys/stat.h>

#include <crypto/cryptodev.h>
#include "bscenc.h"
/*
#define DATA_SIZE       256
#define BLOCK_SIZE      16
#define KEY_SIZE	16  // AES128 
*/

unsigned char* test_decrypt(int cfd, unsigned char shared_key[],unsigned char shared_iv[],
                        unsigned char encrypted_data[]);


/* Insist until all of the data has been read */
ssize_t insist_read(int fd, void *buf, size_t cnt)
{
        ssize_t ret;
        size_t orig_cnt = cnt;

        while (cnt > 0) {
                ret = read(fd, buf, cnt);
                if (ret < 0)
                        return ret;
                buf += ret;
                cnt -= ret;
        }

        return orig_cnt;
}

int fill_urandom_buf(unsigned char *buf, size_t cnt)
{
        int crypto_fd;
        int ret = -1;

        crypto_fd = open("/dev/urandom", O_RDONLY);
        if (crypto_fd < 0)
                return crypto_fd;

        ret = insist_read(crypto_fd, buf, cnt);
        close(crypto_fd);

        return ret;
}


unsigned char* test_encrypt(int cfd, unsigned char shared_key[], unsigned char shared_iv[], 
                            char src[])
{
    //unsigned char src[DATA_SIZE];
    //memcpy(src,str,sizeof(src));

	int i = -1;
	struct session_op sess;
	struct crypt_op cryp;
	struct {
		unsigned char 	in[DATA_SIZE],          //256
				encrypted[DATA_SIZE],
				decrypted[DATA_SIZE],
				iv[BLOCK_SIZE],
				key[KEY_SIZE];
	} data;

	memset(&sess, 0, sizeof(sess));
	memset(&cryp, 0, sizeof(cryp));
	

	/*
	 * Use random values for the encryption key,
	 * the initialization vector (IV), and the
	 * data to be encrypted
	 */
        
     
    memcpy(data.in, src, sizeof(data.in));
    //printf("data.in is %s\n",data.in);

    memcpy(data.iv, shared_iv, sizeof(data.iv));
    memcpy(data.key, shared_key, sizeof(data.key));

//	printf("\nOriginal data:(hex)\n");
//	for (i = 0; i < DATA_SIZE; i++)
//		printf("%x", data.in[i]);
//	printf("\n");

	/*
	 * Get crypto session for AES128
	 */
	sess.cipher = CRYPTO_AES_CBC;
	sess.keylen = KEY_SIZE;
	sess.key = data.key;

	if (ioctl(cfd, CIOCGSESSION, &sess)) {
		perror("ioctl(CIOCGSESSION)");
		return NULL;
	}

	/*
	 * Encrypt data.in to data.encrypted
	 */
	cryp.ses = sess.ses;
	cryp.len = sizeof(data.in);
	cryp.src = data.in;
	cryp.dst = data.encrypted;
	cryp.iv = data.iv;
	cryp.op = COP_ENCRYPT;

	if (ioctl(cfd, CIOCCRYPT, &cryp)) {
		perror("ioctl(CIOCCRYPT)");
		return NULL;
	}

//	printf("\nEncrypted data:\n");
//	for (i = 0; i < DATA_SIZE; i++) {
//		printf("%x", data.encrypted[i]);
//	}
    unsigned char *encrypted_msg;
    encrypted_msg = malloc(DATA_SIZE * sizeof(unsigned char));
    memcpy(encrypted_msg, data.encrypted, sizeof(data.encrypted));

	/* Finish crypto session */
	if (ioctl(cfd, CIOCFSESSION, &sess.ses)) {
		perror("ioctl(CIOCFSESSION)");
		return NULL;
	}
    return encrypted_msg;

}

unsigned char* test_decrypt(int cfd, unsigned char shared_key[],unsigned char shared_iv[],
                         unsigned char encrypted_data[])
{
    	
	int i = -1;
	struct session_op sess;
	struct crypt_op cryp;
	struct {
		unsigned char 	in[DATA_SIZE],          //256
				encrypted[DATA_SIZE],
				decrypted[DATA_SIZE],
				iv[BLOCK_SIZE],
				key[KEY_SIZE];
	} data;

	memset(&sess, 0, sizeof(sess));
	memset(&cryp, 0, sizeof(cryp));
	

	/*
	 * Get crypto session for AES128
	 */
	sess.cipher = CRYPTO_AES_CBC;
	sess.keylen = KEY_SIZE;
    memcpy(data.key, shared_key, sizeof(data.key));
	sess.key = data.key;

    //Initiate a session
	if (ioctl(cfd, CIOCGSESSION, &sess)) {
		perror("ioctl(CIOCGSESSION)");
		return NULL;
	}

	cryp.ses = sess.ses;            //session identifier, needed for closing it
	cryp.len = sizeof(data.encrypted);      //length of source data
	
    //data we want to decrypt
    memcpy(data.encrypted, encrypted_data, sizeof(data.encrypted));

    //initialization vector
    memcpy(data.iv, shared_iv, sizeof(data.iv));
	cryp.iv = data.iv;


	/*
	 * Decrypt data.encrypted to data.decrypted
	 */
	cryp.src = data.encrypted;
	cryp.dst = data.decrypted;
	cryp.op = COP_DECRYPT;
	if (ioctl(cfd, CIOCCRYPT, &cryp)) {
		perror("ioctl(CIOCCRYPT)");
		return NULL;
	}

//	printf("\nDecrypted data:\n");
//	for (i = 0; i < DATA_SIZE; i++) {
//		printf("%x", data.decrypted[i]);
//	}
	//printf("\n");
    //printf("Decrypted data: %s\n", data.decrypted);
    unsigned char *decrypted_msg;
    decrypted_msg = malloc(DATA_SIZE * sizeof(unsigned char));
    memcpy(decrypted_msg, data.decrypted, sizeof(data.decrypted));

	/* Finish crypto session */
	if (ioctl(cfd, CIOCFSESSION, &sess.ses)) {
		perror("ioctl(CIOCFSESSION)");
		return NULL;
	}
    return decrypted_msg;
}



//int main(void)
//{
//	int fd;
//
//	fd = open("/dev/crypto", O_RDWR);
//	if (fd < 0) {
//		perror("open(/dev/crypto)");
//		return 1;
//	}
//    /* encrypt string=> unsigned char[]
//     * decrypt unsigned char[] => unsigned char[]
//     */
//
//
//    char src[256] = "What's up friend,I hope you are doing well, cheers";
//    unsigned char iv[BLOCK_SIZE];
//    unsigned char key[KEY_SIZE];
//    //unsigned char src[256] = "What's up friend,I hope you are doing well, cheers";
//    
//    printf("message is %s\n",src);
//
//	if (fill_urandom_buf(iv, BLOCK_SIZE) < 0) {    //16
//		perror("getting data from /dev/urandom\n");
//		return 1;
//	}
//
//	if (fill_urandom_buf(key, KEY_SIZE) < 0) {     //16
//		perror("getting data from /dev/urandom\n");
//		return 1;
//	}
//    
//
//    printf("\n----------------------------------------------------\n\n");
//    unsigned char *encrypted_msg, *decrypted_msg;
//    encrypted_msg = test_encrypt(fd, key, iv, src);
//    if(!encrypted_msg) {
//        printf("Encryption failed\n");
//        exit(EXIT_FAILURE);
//    }
//    decrypted_msg = test_decrypt(fd, key, iv, encrypted_msg);
//    if(!decrypted_msg) {
//        printf("Decryption failed\n");
//        exit(EXIT_FAILURE);
//    }
//    printf("Decrypted msg in MAIN: %s\n", decrypted_msg);
//
//    free(encrypted_msg);
//    free(decrypted_msg);
//	if (close(fd) < 0) {
//		perror("close(fd)");
//		return 1;
//	}
//
//	return 0;
//}
