#ifndef IOT_INTEGRITY_HASH_H
#define IOT_INTEGRITY_HASH_H

namespace crypto {

typedef unsigned char byte_t;
typedef unsigned int word_t;

const size_t SHA256_BLOCK_SIZE = 32;

/****************************** MACROS ******************************/
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

/**************************** VARIABLES *****************************/
static const word_t k[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static const word_t DEFAULT_STATE[8] = {
	0x6a09e667,
	0xbb67ae85,
	0x3c6ef372,
	0xa54ff53a,
	0x510e527f,
	0x9b05688c,
	0x1f83d9ab,
	0x5be0cd19,
};


class Sha256 {
public:
    Sha256():
    	data_length(0),
		bit_length(0)
    {
		for (int i = 0; i < 8; ++i) {
			state[i] = DEFAULT_STATE[i];
		}
	}

    void update(const unsigned char* input, size_t length) {
		for (word_t i = 0; i < length; ++i) {
			data[data_length] = input[i];
			++data_length;
			if (data_length == 64) {
				transform();
				bit_length += 512;
				data_length = 0;
			}
		}
    }

    void digest(unsigned char destiny[]) {
    	// sha256_final(&hasher, destiny);
		{
			word_t i = data_length;

			// Pad whatever data is left in the buffer.
			if (data_length < 56) {
				data[i] = 0x80;
				++i;
				while (i < 56) {
					data[i] = 0x00;
					++i;
				}
			}
			else {
				data[i++] = 0x80;
				while (i < 64) {
					data[i++] = 0x00;
				}
				transform();
				memset(data, 0, 56);
			}
		}

		// Append to the padding the total message's length in bits and transform.
		bit_length += data_length * 8;
		for (int i = 0; i < 8; ++i) {
			data[63 - i] = bit_length >> (i*8);
		}
		transform();

		// Since this implementation uses little endian byte ordering and SHA uses big endian,
		// reverse all the bytes when copying the final state to the output hash.
		for (word_t i = 0; i < 4; ++i) {
			for (int j = 0; j < 8; ++j) {
				destiny[i + 4*j] = (state[j] >> (24 - i * 8)) & 0x000000ff;
			}
		}
    }

    static const size_t DATA_LENGTH = 64;

private:
	void transform() {
		word_t a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

		for (i = 0, j = 0; i < 16; ++i, j += 4) {
			m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
		}
		for ( ; i < 64; ++i) {
			m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];
		}

		a = state[0];
		b = state[1];
		c = state[2];
		d = state[3];
		e = state[4];
		f = state[5];
		g = state[6];
		h = state[7];

		for (i = 0; i < 64; ++i) {
			t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
			t2 = EP0(a) + MAJ(a,b,c);
			h = g;
			g = f;
			f = e;
			e = d + t1;
			d = c;
			c = b;
			b = a;
			a = t1 + t2;
		}

		state[0] += a;
		state[1] += b;
		state[2] += c;
		state[3] += d;
		state[4] += e;
		state[5] += f;
		state[6] += g;
		state[7] += h;
	}

	byte_t data[DATA_LENGTH];
	word_t data_length;
	unsigned long long bit_length;
	word_t state[8];
};

}

#endif