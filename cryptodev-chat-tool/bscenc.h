
#define DATA_SIZE       256
#define BLOCK_SIZE      16
#define KEY_SIZE	16  /* AES128 */


/* Insist until all of the data has been read */
//ssize_t insist_read(int fd, void *buf, size_t cnt);

int fill_urandom_buf(unsigned char *buf, size_t cnt);


unsigned char* test_encrypt(int cfd, unsigned char shared_key[], unsigned char shared_iv[], 
                            char src[]);

unsigned char* test_decrypt(int cfd, unsigned char shared_key[],unsigned char shared_iv[],
                         unsigned char encrypted_data[]);

