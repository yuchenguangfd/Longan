/*
 * aes_cryption_unittest.cpp
 * Created on: May 28, 2015
 * Author: chenguangyu
 */

#include "aes_cryption.h"
#include "random_string_generator.h"
#include <gtest/gtest.h>

using namespace longan;

TEST(AESCryptionTest, EncryptAndDecryptOK) {
	RandomStringGenerator keyRSG(16, true, true, true);
	for (int i = 0; i < 10; ++i) {
		std::string key = keyRSG.Generate();
		AESCryption aes(key);
		for (int len = 1; len < 100; ++len) {
			RandomStringGenerator contentRSG(len, true, true, true);
			for (int j = 0; j < 50; ++j) {
				std::string content = contentRSG.Generate();
				std::string result = aes.Encrypt(content);
				std::string result2 = aes.Decrypt(result);
				EXPECT_EQ(content, result2);
			}
		}
	}
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
