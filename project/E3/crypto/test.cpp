#include "hash.h"

#include <iostream>

using namespace std;

struct caceta1 {
	char text[33];
};

bool validate(crypto::byte_t first[], crypto::byte_t second[]) {
	for (int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
		if (first[i] != second[i]) {
			return false;
		}
	}
	return true;
}

int main ()
{
	using crypto::Sha256;

	const char* txt = "hey, I have a secret (not drugs).";
	caceta1 cct = caceta1();
	memcpy(cct.text, txt, 33);

	{
		BYTE buf1[SHA256_BLOCK_SIZE];
		Sha256 hasher = Sha256();
		hasher.update(7);
		hasher.digest(buf1);

		cout << "Buff1: ";
		for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
			cout << (int)buf1[i];
		}

		crypto::byte_t right_buf_1[] = {
			232,97,63,90,91,201,249,254,237,163,42,142,124,128,182,157,212,135,
			142,71,182,169,23,35,251,21,235,132,35,107,106,43,};

		cout << " ("
		     << ("false\0true" + 6 *
			      validate(buf1,
		                right_buf_1))
			 << ")\n";
	}


	{
		BYTE buf1[SHA256_BLOCK_SIZE];
		Sha256 hasher = Sha256();
		hasher.update(cct);
		hasher.digest(buf1);

		cout << "Buff2: ";
		for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
			cout << (int)buf1[i];
		}

		crypto::byte_t right_buf_1[] = {
			123,77,37,50,124,181,245,100,52,158,162,216,2,2,94,106,23,174,51,
			233,98,183,5,54,211,237,22,226,114,84,29,70,
		};

		cout << " ("
		     << ("false\0true" + 6 *
			      validate(buf1,
		                right_buf_1))
			 << ")\n";
	}
}