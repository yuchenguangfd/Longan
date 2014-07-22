/*
 * random_unittest.cpp
 * Created on: Jul 21, 2014
 * Author: chenguangyu
 */

#include "random.h"
#include "common/math/math.h"
#include "common/math/statistic.h"
#include <gtest/gtest.h>

using namespace longan;

const int BIG = 10000000;

TEST(RandomTest, IsPseudoRandom) {
	const int seed = 123456;
	const int len = 20;
	int seq1[len], seq2[len];
	Random& rnd = Random::Instance();
	rnd.ChangeSeed(seed);
	for (int i = 0; i < len; ++i) {
		seq1[i] = rnd.NextInt();
	}
	rnd.ChangeSeed(seed);
	for (int i = 0; i < len; ++i) {
		seq2[i] = rnd.NextInt();
	}
	for (int i = 0; i < len; ++i) {
		ASSERT_EQ(seq1[i], seq2[i]);
	}
}

TEST(RandomTest, UniformInt) {
	Random& rnd = Random::Instance();
	int a = 10, b = 200;
	double mean = (a + b)/2;
	double var = Math::Sqr(a - b)/12;
	RunningStatistic stat;
	for (int i = 0; i < BIG; ++i) {
		stat.Accumulate(rnd.Uniform(a, b));
	}
	EXPECT_LT(Math::RelativeError(mean, stat.GetMean()), 0.01);
	EXPECT_LT(Math::RelativeError(var, stat.GetVariance()), 0.01);
}

TEST(RandomTest, UniformDouble) {
	Random& rnd = Random::Instance();
	double a = 1.0, b = 20.0;
	double mean = (a + b) / 2;
	double var = Math::Sqr(a - b) / 12;
	RunningStatistic stat;
	for (int i = 0; i < BIG; ++i) {
		stat.Accumulate(rnd.Uniform(a, b));
	}
	EXPECT_LT(Math::RelativeError(mean, stat.GetMean()),  0.01);
	EXPECT_LT(Math::RelativeError(var, stat.GetVariance()), 0.01);
}

TEST(Random, GaussStandard) {
	Random& rnd = Random::Instance();
	RunningStatistic stat;
	for (int i = 0; i < BIG; ++i) {
		stat.Accumulate(rnd.Gauss());
	}
    EXPECT_LE(Math::Abs(stat.GetMean()), 0.01);
    EXPECT_LE(Math::RelativeError(1.0, stat.GetStandardDeviation()), 0.01);
}

TEST(Random, Gauss) {
	Random& rnd = Random::Instance();
	double mean = 10.0;
	double std = 2.0;
	RunningStatistic stat;
	for (int i = 0; i < BIG; ++i) {
	    stat.Accumulate(rnd.Gauss(mean, std));
	}
    EXPECT_LT(Math::RelativeError(mean, stat.GetMean()), 0.01);
    EXPECT_LT(Math::RelativeError(std, stat.GetStandardDeviation()), 0.01);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
