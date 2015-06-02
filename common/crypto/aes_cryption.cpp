/*
 * aes_cryption.cpp
 * Created on: May 28, 2015
 * Author: yuchenguang
 */

#include "aes_cryption.h"
#include <algorithm>
#include <cassert>

namespace longan {

AESCryption::AESCryption(const std::string& key) {
	assert(key.length() == 16);
    uint8 sBox[256] = {
    		/* 0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f */
            0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76, /*0*/
            0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0, /*1*/
            0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15, /*2*/
            0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75, /*3*/
            0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84, /*4*/
            0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf, /*5*/
            0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8, /*6*/
            0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2, /*7*/
            0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73, /*8*/
            0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb, /*9*/
            0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79, /*a*/
            0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08, /*b*/
            0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a, /*c*/
            0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e, /*d*/
            0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf, /*e*/
            0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16  /*f*/
	};
    std::copy(sBox, sBox + 256, mSbox);
    uint8 invsBox[256] = {
           /*  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f  */
            0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb, /*0*/
            0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb, /*1*/
            0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e, /*2*/
            0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25, /*3*/
            0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92, /*4*/
            0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84, /*5*/
            0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06, /*6*/
            0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b, /*7*/
            0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73, /*8*/
            0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e, /*9*/
            0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b, /*a*/
            0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4, /*b*/
            0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f, /*c*/
            0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef, /*d*/
            0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61, /*e*/
            0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d  /*f*/
	};
    std::copy(invsBox, invsBox + 256, mInvSbox);
    ExpanKey(key);
}

void AESCryption::ExpanKey(const std::string& key) {
	uint8 rc[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			mW[0][r][c] = key[r + c * 4];
		}
	}
	for (int i = 1; i <= 10; ++i) {
		for (int j = 0; j < 4; ++j) {
			uint8 t[4];
			for (int r = 0; r < 4; ++r) {
				t[r] = (j != 0) ? mW[i][r][j - 1] : mW[i - 1][r][3];
			}
			if (j == 0) {
				uint8 temp = t[0];
				for (int r = 0; r < 3; ++r) {
					t[r] = mSbox[t[(r + 1) % 4]];
				}
				t[3] = mSbox[temp];
				t[0] ^= rc[i - 1];
			}
			for (int r = 0; r < 4; ++r) {
				mW[i][r][j] = mW[i - 1][r][j] ^ t[r];
			}
		}
	}
}

std::string AESCryption::Encrypt(const std::string& content) {
	std::string result;
	for (int begin = 0; begin < content.length(); begin += 16) {
		uint8 bytes[16];
		for (int i = 0; i < 16; ++i) {
			bytes[i] = (begin + i < content.length()) ? content[begin + i] : 0;
		}
		Encrypt16Byte(bytes);
		for (int i = 0; i < 16; ++i) {
			int high = (bytes[i] >> 4) & 0xF;
			result += ((high < 10) ? high + '0' : high - 10 + 'A');
			int low = bytes[i] & 0xF;
			result += ((low < 10) ? low + '0' : low - 10 + 'A');
		}
	}
	int padding = (content.length() % 16 == 0) ? 0 : 16 - content.length() % 16;
	result += ((padding < 10) ? padding + '0' : padding - 10 + 'A');
	return result;
}

void AESCryption::Encrypt16Byte(uint8* bytes) {
	uint8 state[4][4];
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			state[r][c] = bytes[r + c * 4];
		}
	}
	AddRoundKey(state, mW[0]);
	for (int i = 1; i <= 10; ++i) {
		SubBytes(state);
		ShiftRows(state);
		if (i != 10) {
			MixColumns(state);
		}
		AddRoundKey(state, mW[i]);
	}
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			bytes[c * 4 + r] = state[r][c];
		}
	}
}

std::string AESCryption::Decrypt(const std::string& content) {
	std::string result;
	for (int begin = 0; begin < content.length() - 1; begin += 32) {
		uint8 bytes[16];
		for (int i = 0; i < 16; ++i) {
			char ch1 = content[begin + i*2], ch2 = content[begin + i*2 + 1];
			int high = ((ch1 >= 'A') ? ch1 - 'A' + 10 : ch1 - '0');
			int low = ((ch2 >= 'A') ? ch2 - 'A' + 10 : ch2 - '0');
			bytes[i] = (((high << 4) | low) & 0xFF);
		}
		Decrypt16Byte(bytes);
		result += std::string(bytes, bytes + 16);
	}
	char ch = content[content.length() - 1];
	int padding = ((ch >= 'A') ? ch - 'A' + 10 : ch - '0');
	return result.substr(0, result.length() - padding);
}

void AESCryption::Decrypt16Byte(uint8 *bytes) {
	uint8 state[4][4];
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			state[r][c] = bytes[r + c * 4];
		}
	}
	AddRoundKey(state, mW[10]);
	for (int i = 9; i >= 0; --i) {
		InvShiftRows(state);
		InvSubBytes(state);
		AddRoundKey(state, mW[i]);
		if (i) {
			InvMixColumns(state);
		}
	}
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			bytes[c * 4 + r] = state[r][c];
		}
	}
}

void AESCryption::AddRoundKey(uint8 state[][4], uint8 k[][4]) {
	for (int c = 0; c < 4; ++c) {
		for (int r = 0; r < 4; ++r) {
			state[r][c] ^= k[r][c];
		}
	}
}

void AESCryption::SubBytes(uint8 state[][4]) {
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			state[r][c] = mSbox[state[r][c]];
		}
	}
}

void AESCryption::ShiftRows(uint8 state[][4]) {
	uint8 t[4];
	for (int r = 1; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			t[c] = state[r][(c + r) % 4];
		}
		for (int c = 0; c < 4; ++c) {
			state[r][c] = t[c];
		}
	}
}

void AESCryption::MixColumns(uint8 state[][4]) {
	uint8 t[4];
	for (int c = 0; c < 4; ++c) {
		for (int r = 0; r < 4; ++r) {
			t[r] = state[r][c];
		}
		for (int r = 0; r < 4; ++r) {
			state[r][c] = FFmul(0x02, t[r]) ^ FFmul(0x03, t[(r + 1) % 4])
					^ FFmul(0x01, t[(r + 2) % 4]) ^ FFmul(0x01, t[(r + 3) % 4]);
		}
	}
}

uint8 AESCryption::FFmul(uint8 a, uint8 b) {
	uint8 bw[4];
	uint8 res = 0;
	bw[0] = b;
	for (int i = 1; i < 4; ++i) {
		bw[i] = bw[i - 1] << 1;
		if (bw[i - 1] & 0x80) {
			bw[i] ^= 0x1b;
		}
	}
	for (int i = 0; i < 4; ++i) {
		if ((a >> i) & 0x01) {
			res ^= bw[i];
		}
	}
	return res;
}

void AESCryption::InvShiftRows(uint8 state[][4]) {
	uint8 t[4];
	for (int r = 1; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			t[c] = state[r][(c - r + 4) % 4];
		}
		for (int c = 0; c < 4; ++c) {
			state[r][c] = t[c];
		}
	}
}

void AESCryption::InvSubBytes(uint8 state[][4]) {
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			state[r][c] = mInvSbox[state[r][c]];
		}
	}
}

void AESCryption::InvMixColumns(uint8 state[][4]) {
	uint8 t[4];
	for (int c = 0; c < 4; ++c) {
		for (int r = 0; r < 4; ++r) {
			t[r] = state[r][c];
		}
		for (int r = 0; r < 4; ++r) {
			state[r][c] = FFmul(0x0e, t[r]) ^ FFmul(0x0b, t[(r + 1) % 4])
					^ FFmul(0x0d, t[(r + 2) % 4]) ^ FFmul(0x09, t[(r + 3) % 4]);
		}
	}
}

} //~ namespace longan
