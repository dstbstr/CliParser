#pragma once

#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

#include "CliParser/Option.h"

namespace CliParser {
	struct CommandWrapper {
		virtual ~CommandWrapper() = default;
		virtual void operator()(const IArgs*, int&) = 0;
	};

	template<typename ArgType>
	struct Cmd : CommandWrapper {
		std::function<void(const ArgType&, int&)> fn;
		void operator()(const IArgs* args, int& outResult) override {
			fn(*static_cast<const ArgType*>(args), outResult);
		}
	};

	std::unordered_map<std::string, std::unique_ptr<CommandWrapper>>& GetCommands();
	std::unordered_map<std::string, std::function<std::unique_ptr<IArgs>()>>& GetArgParsers();
	std::vector<std::string> GetCommandNames();

	template<typename ArgType>
	struct CommandRegistrar {
		CommandRegistrar(const char* name, std::function<void(const ArgType&, int&)> command) {
			auto cmd = std::make_unique<Cmd<ArgType>>();
			cmd->fn = command;
			GetCommands()[name] = std::move(cmd);
			GetArgParsers()[name] = []() { return std::make_unique<ArgType>(); };
		}
	};
}

#define COMMAND(_name, _argType, _argName) \
void _name(const _argType& _argName, int& outResult); \
CliParser::CommandRegistrar<_argType> _name##Registrar(#_name, _name); \
void _name(const _argType& _argName, [[maybe_unused]] int& outResult)

#define DEFAULT_COMMAND(_argType, _argName) \
void defaultCommand(const _argType& _argName, int& outResult); \
CliParser::CommandRegistrar<_argType> defaultCommandRegistrar("", defaultCommand); \
void defaultCommand(const _argType& _argName, [[maybe_unused]] int& outResult)