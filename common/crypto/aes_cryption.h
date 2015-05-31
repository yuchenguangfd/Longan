/*
 * aes_cryption.h
 * Created on: May 28, 2015
 * Author: yuchenguang
 */

#ifndef COMMON_CRYPTO_AES_CRYPTION_H
#define COMMON_CRYPTO_AES_CRYPTION_H

#include "common/lang/types.h"
#include <string>

namespace longan {

class AESCryption {
public:
	AESCryption(const std::string& key);
	std::string Encrypt(const std::string& content);
	std::string Decrypt(const std::string& content);
private:
	void Encrypt16Byte(uint8* bytes);
	void Decrypt16Byte(uint8* bytes);
	void ExpanKey(const std::string& key);
	void AddRoundKey(uint8 state[][4], uint8 k[][4]);
	void SubBytes(uint8 state[][4]);
	void ShiftRows(uint8 state[][4]);
	void MixColumns(uint8 state[][4]);
	uint8 FFmul(uint8 a, uint8 b);
	void InvShiftRows(uint8 state[][4]);
	void InvSubBytes(uint8 state[][4]);
	void InvMixColumns(uint8 state[][4]);
private:
	uint8 mSbox[256];
	uint8 mInvSbox[256];
	uint8 mW[11][4][4];
};

} //~ namespace longan

#endif /* COMMON_CRYPTO_AES_CRYPTION_H */
