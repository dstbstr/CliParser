#include "CliParser/CliParser.h"
#include <algorithm>
#include <ranges>

namespace {
	bool NeedsHelp(const std::vector<std::string>& args) {
		return std::any_of(args.begin(), args.end(), [](const auto& arg) {
			return arg == "-h" || arg.rfind("help") != arg.npos;
		});
	}

	void ShowHelp(const std::vector<std::string>& args, std::ostream& outStream) {
		auto PrintKnownCommands = [&outStream]() {
			const auto commands = CliParser::GetCommandNames();
			auto commandStr = commands 
				| std::views::transform([](const std::string& s) { return s == "" ? "<default>" : s; })
				| std::views::join_with('\n') 
				| std::ranges::to<std::string>();
			outStream << "Available commands:\n" << commandStr << std::flush;
		};
		if(args.empty()) {
			PrintKnownCommands();
		} else {
			auto commandName = args[0];
			if (commandName == "default" || commandName == "empty") commandName = "";
			auto& parsers = CliParser::GetArgParsers();
			if(parsers.contains(commandName)) {
				auto& parser = parsers.at(commandName);
				outStream << parser()->Describe() << std::flush;
			} else {
				PrintKnownCommands();
			}
		}
	}
}

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
		if(NeedsHelp(args)) {
			ShowHelp(args, outErrors);
			return 0;
		}

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