#include "TestCommon.h"

#include "CliParser/Option.h"
#include <sstream>
#include <ranges>

namespace CliParser {
	struct ExampleArgs : public IArgs {
		ExampleArgs() : IArgs(), str("unset"), fp(3.14f) {}

		OPTION(std::string, 's', str, Required::True, "An example string");
		OPTION(float, 'f', fp, Required::False, "An example float");
	};

	template<typename TArgs>
	struct BaseOptionTest : ::testing::Test {
		TArgs Args;
		std::stringstream Errors;
		std::vector<std::string_view> ArgValues;
		void MakeArgs(std::string_view argString) {
			for (const auto& part : argString | std::views::split(' ')) {
				ArgValues.emplace_back(part.begin(), part.end());
			}
		}

		void AssertSuccess() {
			auto success = Args.TryParse(ArgValues, Errors);
			ASSERT_EQ(Errors.str(), "");
			ASSERT_TRUE(success);
		}

	};
	struct OptionTest : BaseOptionTest<ExampleArgs> {};

	TEST_F(OptionTest, TryParse_WithRequiredLongOption_ReturnsTrue) {
		MakeArgs("--str hello");
		AssertSuccess();
		ASSERT_EQ(Args.str, "hello");
	}
	TEST_F(OptionTest, TryParse_WithRequiredShortOption_ReturnsTrue) {
		MakeArgs("-s hello");
		AssertSuccess();
		ASSERT_EQ(Args.str, "hello");
	}
	TEST_F(OptionTest, TryParse_WithBothOptions_ReturnsTrue) {
		MakeArgs("-s hello -f 1.62");
		AssertSuccess();
		ASSERT_EQ(Args.fp, 1.62f);
	}
	TEST_F(OptionTest, TryParse_WithDuplicateOption_ReturnsTrue) {
		MakeArgs("-s hello -s world");
		AssertSuccess();
		ASSERT_EQ(Args.str, "world");
	}
	TEST_F(OptionTest, TryParse_WithMissingRequiredOption_ReturnsFalse) {
		MakeArgs("-f 3.14");
		ASSERT_FALSE(Args.TryParse(ArgValues, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("str"));
	}
	TEST_F(OptionTest, TryParse_WithoutArgName_ReturnsFalse) {
		MakeArgs("42");
		ASSERT_FALSE(Args.TryParse(ArgValues, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("42"));
	}
	TEST_F(OptionTest, TryParse_WithTrailingArgument_ReturnsFalse) {
		MakeArgs("-s hello world");
		ASSERT_FALSE(Args.TryParse(ArgValues, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("world"));
	}

	enum struct SomeEnum { One, Two, Three };

	namespace ArgParse {
		bool TryParse(std::string_view sv, SomeEnum& outResult, std::ostream& outErrors) {
			if (sv == "One") outResult = SomeEnum::One;
			else if (sv == "Two") outResult = SomeEnum::Two;
			else if (sv == "Three") outResult = SomeEnum::Three;
			else {
				outErrors << "Failed to parse " << sv << " as SomeEnum";
				return false;
			}
			return true;
		}
		std::string ToString(const SomeEnum& value) {
			switch (value) {
			case SomeEnum::One: return "One";
			case SomeEnum::Two: return "Two";
			case SomeEnum::Three: return "Three";
			}
			return "Unknown";
		}
	}

	struct EnumOption : IArgs {
		OPTION(SomeEnum, 'e', val, Required::True, "An example enum");
	};

	struct EnumTest : BaseOptionTest<EnumOption> {};

	TEST_F(EnumTest, TryParse_WithValidEnum_ReturnsTrue) {
		MakeArgs("-e Two");
		AssertSuccess();
		ASSERT_EQ(Args.val, SomeEnum::Two);
	}
	TEST_F(EnumTest, TryParse_WithInvalid_ReturnsFalse) {
		MakeArgs("-e Four");
		ASSERT_FALSE(Args.TryParse(ArgValues, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("Four"));
	}

	bool MustBeEven(const int& i, std::ostream& outErrors) {
		if (i % 2 != 0) {
			outErrors << i << " is not even";
			return false;
		}
		return true;
	}

	struct ValidatedValuesOption : IArgs {
		VALIDATED_OPTION(int, 'i', num, Required::True, "An even number", MustBeEven);
	};

	struct ValidatedOptionTest : BaseOptionTest<ValidatedValuesOption> {};

	TEST_F(ValidatedOptionTest, TryParse_WithEvenNumber_ReturnsTrue) {
		MakeArgs("-i 42");
		AssertSuccess();
		ASSERT_EQ(Args.num, 42);
	}
	TEST_F(ValidatedOptionTest, TryParse_WithOddNumber_ReturnsFalse) {
		MakeArgs("-i 13");
		ASSERT_FALSE(Args.TryParse(ArgValues, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("13"));
	}

	std::vector<std::string> Weekends() { 
		return { "Saturday", "Sunday" };
	}
	struct LimitedValuesOption : IArgs {
		LIMITED_OPTION(std::string, 'd', day, Required::True, "A day of the week", Weekends);
	};
	struct LimitedOptionTest : BaseOptionTest<LimitedValuesOption> {};

	TEST_F(LimitedOptionTest, TryParse_WithValidValue_ReturnsTrue) {
		MakeArgs("-d Sunday");
		AssertSuccess();
		ASSERT_EQ(Args.day, "Sunday");
	}
	TEST_F(LimitedOptionTest, TryParse_WithInvalidValue_ReturnsFalse) {
		MakeArgs("-d Monday");
		ASSERT_FALSE(Args.TryParse(ArgValues, Errors));
		ASSERT_THAT(Errors.str(), HasSubstr("Monday"));
	}

	struct DefaultValuesOption : IArgs {
		OPTION(int, 'i', num, Required::False, "An integer", 42);
	};
	struct DefaultOptionTest : BaseOptionTest<DefaultValuesOption> {};
	TEST_F(DefaultOptionTest, TryParse_WithNoValue_ReturnsTrue) {
		MakeArgs("");
		AssertSuccess();
		ASSERT_EQ(Args.num, 42);
	}
}