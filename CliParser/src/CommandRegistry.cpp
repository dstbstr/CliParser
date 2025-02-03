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