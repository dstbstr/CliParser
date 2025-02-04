#include "TestCommon.h"
#include "CliParser/CommandRegistry.h"

namespace CliParser {
	struct SampleArgs : public IArgs {
		OPTION(std::string, 's', str, "An example string");
		OPTION(std::optional<int>, 'i', num, "An example number", 42);
	};

	COMMAND(SampleCommand, SampleArgs, args) {
		outResult = args.num.value();
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