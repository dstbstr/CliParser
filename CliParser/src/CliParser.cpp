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
		const auto& commandName = [&args]() -> std::string {
			if (args.empty() || args[0][0] == '-' || args[0][0] == '/') {
				return "";
			}
			return args[0];
		}();

		auto& commands = GetCommands();
		if (!commands.contains(commandName)) {
			if(commandName.empty()) {
				outErrors << "No command specified and default command not set" << std::flush;
			} else {
				outErrors << "Unknown command: " << commandName << std::flush;
			}
			return 1;
		}
		auto& command = commands.at(commandName);
		auto argParser = GetArgParsers().at(commandName)();
		const auto remainingArgs = [&args, &commandName]() -> std::vector<std::string_view> {
			if(commandName.empty()) {
				return std::vector<std::string_view>(args.begin(), args.end());
			}
			return std::vector<std::string_view>(args.begin() + 1, args.end());
		}();
		if (!argParser->TryParse(remainingArgs, outErrors)) {
			return 1;
		}

		int result = SuccessCode;
		(*command)(argParser.get(), result);

		return result;
	}
}