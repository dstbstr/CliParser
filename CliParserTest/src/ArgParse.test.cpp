#include "TestCommon.h"
#include "CliParser/ArgParse.h"

namespace CliParser::ArgParse {
	struct BoolTest : public ::testing::Test {
		bool Result{false};
		std::stringstream Errors{};
	};

	TEST_F(BoolTest, TryParse_WithTrue_ReturnsTrue) {
		ASSERT_TRUE(TryParse("true", Result, Errors));
		ASSERT_TRUE(Result);
	}
	TEST_F(BoolTest, TryParse_With1_ReturnsTrue) {
		ASSERT_TRUE(TryParse("1", Result, Errors));
		ASSERT_TRUE(Result);
	}
	TEST_F(BoolTest, TryParse_WithY_ReturnsTrue) {
		ASSERT_TRUE(TryParse("y", Result, Errors));
		ASSERT_TRUE(Result);
	}
	TEST_F(BoolTest, TryParse_WithOn_ReturnsTrue) {
		ASSERT_TRUE(TryParse("on", Result, Errors));
		ASSERT_TRUE(Result);
	}

	TEST_F(BoolTest, TryParse_WithFalse_ReturnsTrue) {
		ASSERT_TRUE(TryParse("false", Result, Errors));
		ASSERT_FALSE(Result);
	}
	TEST_F(BoolTest, TryParse_With0_ReturnsTrue) {
		ASSERT_TRUE(TryParse("0", Result, Errors));
		ASSERT_FALSE(Result);
	}
	TEST_F(BoolTest, TryParse_WithN_ReturnsTrue) {
		ASSERT_TRUE(TryParse("n", Result, Errors));
		ASSERT_FALSE(Result);
	}
	TEST_F(BoolTest, TryParse_WithOff_ReturnsTrue) {
		ASSERT_TRUE(TryParse("off", Result, Errors));
		ASSERT_FALSE(Result);
	}

	TEST_F(BoolTest, TryParse_WithInvalid_ReturnsFalse) {
		ASSERT_FALSE(TryParse("invalid", Result, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("invalid"));
	}
	TEST_F(BoolTest, TryParse_WithNumberAbove1_ReturnsFalse) {
		ASSERT_FALSE(TryParse("3", Result, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("3"));
	}
	TEST_F(BoolTest, TryParse_WithNegativeNumber_ReturnsFalse) {
		ASSERT_FALSE(TryParse("-1", Result, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("-1"));
	}

	struct FloatTest : public ::testing::Test {
		float Result{0};
		std::stringstream Errors{};
	};
	TEST_F(FloatTest, TryParse_WithZero_ReturnsTrue) {
		ASSERT_TRUE(TryParse("0", Result, Errors));
		ASSERT_FLOAT_EQ(Result, 0);
	}
	TEST_F(FloatTest, TryParse_WithPi_ReturnsTrue) {
		ASSERT_TRUE(TryParse("3.14159265", Result, Errors));
		ASSERT_FLOAT_EQ(Result, 3.14159265f);
	}
	TEST_F(FloatTest, TryParse_WithNegative_ReturnsTrue) {
		ASSERT_TRUE(TryParse("-3.14", Result, Errors));
		ASSERT_FLOAT_EQ(Result, -3.14f);
	}
	TEST_F(FloatTest, TryParse_WithNegativeZero_ReturnsTrue) {
		ASSERT_TRUE(TryParse("-0", Result, Errors));
		ASSERT_FLOAT_EQ(Result, 0);
	}
	TEST_F(FloatTest, TryParse_WithZeroPointZero_ReturnsTrue) {
		ASSERT_TRUE(TryParse("0.0", Result, Errors));
		ASSERT_FLOAT_EQ(Result, 0);
	}
	TEST_F(FloatTest, TryParse_WithPointZero_ReturnsTrue) {
		ASSERT_TRUE(TryParse(".0", Result, Errors));
		ASSERT_FLOAT_EQ(Result, 0);
	}
	TEST_F(FloatTest, TryParse_WithZeroPoint_ReturnsTrue) {
		ASSERT_TRUE(TryParse("0.", Result, Errors));
		ASSERT_FLOAT_EQ(Result, 0);
	}

	TEST_F(FloatTest, TryParse_WithEmptyString_ReturnsFalse) {
		ASSERT_FALSE(TryParse("", Result, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("Empty"));
	}
	TEST_F(FloatTest, TryParse_WithNonNumbers_ReturnsFalse) {
		ASSERT_FALSE(TryParse("invalid", Result, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("invalid"));
	}
	TEST_F(FloatTest, TryParse_EndingInGarbage_ReturnsFalse) {
		ASSERT_FALSE(TryParse("3.14garbage", Result, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("3.14garbage"));
	}
	TEST_F(FloatTest, TryPrase_StartingInGarbage_ReturnsFalse) {
		ASSERT_FALSE(TryParse("garbage3.14", Result, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("garbage3.14"));
	}
}
