#include "CliParser/CommandRegistry.h"
#include <ranges>

namespace CliParser {
	std::unordered_map<std::string, std::unique_ptr<CommandWrapper>>& GetCommands() {
		static std::unordered_map<std::string, std::unique_ptr<CommandWrapper>> commands;
		return commands;
	}

	std::unordered_map<std::string, std::function<std::unique_ptr<IArgs>()>>& GetArgParsers() {
		static std::unordered_map<std::string, std::function<std::unique_ptr<IArgs>()>> argParsers;
		return argParsers;
	}

	std::vector<std::string> GetCommandNames() {
		return GetCommands() | std::views::keys | std::ranges::to<std::vector>();
	}

}
//
//namespace {
//	using ArgParsers = std::unordered_map<std::string, CliParser::CommandRegistry::ArgParseFunc>;
//	ArgParsers& GetArgParsers() {
//		static ArgParsers argParsers;
//		return argParsers;
//	}
//}
//
//namespace CliParser::CommandRegistry {
//	CommandMap& GetCommandMap() {
//		static CommandMap commandMap;
//		return commandMap;
//	}
//	void RegisterCommand(const std::string& name, ArgParseFunc argParser, CommandFunc command) {
//		GetCommandMap()[name] = command;
//		GetArgParsers()[name] = argParser;
//	}
//
//	int Run(int argc, char* argv[]) {
//		std::vector<std::string> args;
//		args.reserve(argc);
//		for (int i = 1; i < argc; ++i) {
//			args.emplace_back(argv[i]);
//		}
//		return Run(args);
//	}
//	int Run(const std::vector<std::string>& args) {
//		if (args.empty()) return 1;
//		auto& commandMap = GetCommandMap();
//		if (!commandMap.contains(args[0])) return 1;
//		auto& command = commandMap.at(args[0]);
//		auto argParser = GetArgParsers().at(args[0]);
//
//
//		int result = 0;
//		command->second(args, result);
//		return result;
//	}
//}