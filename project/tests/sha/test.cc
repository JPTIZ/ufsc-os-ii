#include <stdio.h>
#include <string.h>
#include "sha256.cc"

int main (int argc, char *argv[])
{
	BYTE text1[] = {"hey, I have a secret (not drugs)."};
	BYTE text2[] = {"hey, I have a secret (not drugs)."};
	BYTE text3[] = {"hey, I have another secret (totally drugs)."};

	BYTE buf1[SHA256_BLOCK_SIZE];
	BYTE buf2[SHA256_BLOCK_SIZE];
	BYTE buf3[SHA256_BLOCK_SIZE];
	SHA256_CTX ctx;
	int idx;
	int pass = 1;

	sha256_init(&ctx);
	sha256_update(&ctx, text1, strlen((char*)text1));
	sha256_final(&ctx, buf1);

	sha256_init(&ctx);
	sha256_update(&ctx, text2, strlen((char*)text2));
	sha256_final(&ctx, buf2);

	sha256_init(&ctx);
	sha256_update(&ctx, text3, strlen((char*)text3));
	sha256_final(&ctx, buf3);

	printf("Buff1: ");
	for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
		printf("%d", buf1[i]);
	}
	printf("\nBuff2: ");
	for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
		printf("%d", buf2[i]);
	}
	printf("\nBuff3: ");
	for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
		printf("%d", buf3[i]);
	}
	printf("\n");

	if (!memcmp(buf1, buf2, SHA256_BLOCK_SIZE) && memcmp(buf1, buf3, SHA256_BLOCK_SIZE) && memcmp(buf2, buf3, SHA256_BLOCK_SIZE))
		printf("Success\n");
}