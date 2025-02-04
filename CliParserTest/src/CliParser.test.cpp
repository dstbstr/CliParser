#include "TestCommon.h"
#include "CliParser/CliParser.h"

#include <vector>
#include <sstream>
#include <ranges>

namespace CliParser {
	struct EmptyArgs : IArgs {};

	struct Command1Args : IArgs {
		OPTION(std::string, 's', str, "An example string");
		OPTION(std::optional<int>, 'i', num, "An example number", 42);
	};

	struct Command2Args : IArgs {
		OPTION(float, 'f', fp, "An example float");
		OPTION(std::optional<char>, 'c', character, "An example character", 'a');
	};

	struct DefaultArgs : IArgs {
		OPTION(int, 'r', result, "The result of the command", 0);
	};

	COMMAND(Command1, Command1Args, args) {
		if(args.num != 42) {
			outResult = static_cast<int>(args.str.size());
		}
	}
	COMMAND(Command2, Command2Args, args) {
		outResult = args.character.has_value() 
			? static_cast<int>(*args.character) 
			: 0;
	}
	COMMAND(EmptyCommand, EmptyArgs, args) {
		outResult = 42;
		(void)args;
	}
	DEFAULT_COMMAND(DefaultArgs, args) {
		outResult = args.result;
	}

	struct CliParserTest : public ::testing::Test {
		std::stringstream Errors{};
		std::vector<std::string> Args{};

		void MakeArgs(const std::string& argStr) {
			for (const auto& part : argStr | std::views::split(' ')) {
				Args.emplace_back(part.begin(), part.end());
			}
		}
	};

	TEST_F(CliParserTest, Run_WithValidCommandAndArgs_IsSuccess) {
		MakeArgs("Command1 -s hello");
		auto result = CliParser::Run(Args, Errors);
		ASSERT_EQ(result, SuccessCode);
		ASSERT_TRUE(Errors.str().empty());
	}
	TEST_F(CliParserTest, Run_WithValidCommandAndArgs_RunsCommand) {
		MakeArgs("Command1 -s hello -i 24");
		auto result = CliParser::Run(Args, Errors);
		ASSERT_EQ(result, static_cast<int>(strlen("hello")));
	}
	TEST_F(CliParserTest, Run_WithOtherCommand_IsSuccess) {
		MakeArgs("Command2 -f 3.14");
		auto result = CliParser::Run(Args, Errors);
		ASSERT_EQ(result, static_cast<int>('a'));
	}
	TEST_F(CliParserTest, Run_WithNoArgs_IsAnError) {
		auto result = CliParser::Run(Args, Errors);
		ASSERT_NE(result, SuccessCode);
	}

	TEST_F(CliParserTest, Run_WithNoArgs_WritesError) {
		CliParser::Run(Args, Errors);
		ASSERT_FALSE(Errors.str().empty());
	}

	TEST_F(CliParserTest, Run_WithUnknownCommand_IsAnError) {
		MakeArgs("UnknownCommand");
		auto result = CliParser::Run(Args, Errors);
		ASSERT_NE(result, SuccessCode);
	}

	TEST_F(CliParserTest, Run_WithUnknownCommand_WritesError) {
		MakeArgs("UnknownCommand");
		CliParser::Run(Args, Errors);
		ASSERT_THAT(Errors.str(), HasSubstr("UnknownCommand"));
	}

	TEST_F(CliParserTest, Run_WithMissingArgs_IsAnError) {
		MakeArgs("Command1");
		auto result = CliParser::Run(Args, Errors);
		ASSERT_NE(result, SuccessCode);
		ASSERT_FALSE(Errors.str().empty());
	}

	TEST_F(CliParserTest, Run_WithBadArgs_IsAnError) {
		MakeArgs("Command1 -s hello -i notanumber");
		auto result = CliParser::Run(Args, Errors);
		ASSERT_NE(result, SuccessCode);
		ASSERT_FALSE(Errors.str().empty());
	}

	TEST_F(CliParserTest, Run_WithDefaultCommand_IsSuccess) {
		MakeArgs("-r 42");
		auto result = CliParser::Run(Args, Errors);
		ASSERT_EQ(result, 42);
	}

	TEST_F(CliParserTest, Run_EmptyCommand_IsSuccess) {
		MakeArgs("EmptyCommand");
		auto result = CliParser::Run(Args, Errors);
		ASSERT_EQ(result, 42);
		ASSERT_TRUE(Errors.str().empty());
	}
}