#include "CliParser/CliParser.h"

namespace CliParser {
	int Run(int argc, char* argv[], std::ostream& outErrors) {
		std::vector<std::string> args;
		args.reserve(argc - 1);
		for (int i = 1; i < argc; ++i) {
			args.emplace_back(argv[i]);
		}
		return Run(args, outErrors);
	}

	int Run(const std::vector<std::string>& args, std::ostream& outErrors) {
		if(args.empty()) {
			outErrors << "No command specified";
			return 1;
		}
		const auto& commandName = args[0];
		auto& commands = GetCommands();
		if (!commands.contains(commandName)) {
			outErrors << "Unknown command: " << commandName;
			return 1;
		}
		auto& command = commands.at(commandName);
		auto argParser = GetArgParsers().at(commandName)();
		auto remainingArgs = std::vector<std::string_view>(args.begin() + 1, args.end());
		if (!argParser->TryParse(remainingArgs, outErrors)) {
			return 1;
		}

		int result = SuccessCode;
		(*command)(argParser.get(), result);

		return result;
	}
}