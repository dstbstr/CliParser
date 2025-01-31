#include "TestCommon.h"
#include "CliParser/CommandRegistry.h"

namespace CliParser {
	struct SampleArgs : public IArgs {
		OPTION(std::string, 's', str, Required::True, "An example string");
		OPTION(int, 'i', num, Required::False, "An example number", 42);
	};

	COMMAND(SampleCommand, SampleArgs, args) {
		outResult = args.num;
	}

	TEST(CommandRegistryTest, Commands_AfterCommandMacro_IncludesSampleCommand) {
		ASSERT_TRUE(GetCommands().contains("SampleCommand"));
	}

	TEST(CommandRegistryTest, ArgParsers_AfterCommandMacro_IncludesSampleCommand) {
		ASSERT_TRUE(GetArgParsers().contains("SampleCommand"));	
	}

	TEST(CommandRegistryTest, CommandNames_AfterCommandMacro_IncludesCommandName) {
		auto names = GetCommandNames();
		ASSERT_THAT(names, ::testing::Contains("SampleCommand"));
	}
}