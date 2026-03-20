# CLI PARSER
A simple extensible command line interface (CLI) parser for C++ projects.

## Quick Start
Main.cpp
```cpp
#include "commands.h"
#include <CliParser.h>
#include <iostream>

int main(int argc, char** argv) {
	InitializeCommands();
	CliParser::Run(argc, argv);
}
```
Commands.h
```cpp
#pragma once
void InitializeCommands();
```
Commands.cpp
```cpp
#include "commands.h"
#include <CliParser/CommandRegistry.h>
#include <iostream>

struct MyArgs : CliParser::IArgs {
	OPTION(std::string, 'i', Input, "Input file path");
	OPTION(std::optional<std::string>, 'o', Output, "Optional output file path", "./output.txt");
};

DEFAULT_COMMAND(MyArgs, args) {
	std::cout << "Input: " << args.Input << std::endl;
	std::cout << "Output: " << args.Output.value_or("Not provided") << std::endl;
}

// Preventing the linker from stripping out symbols
void InitializeCommands() {}
```

## Options
There are a few option types.  The default involves a type, a short name, a long name, and a description.
It is also possible to provide a default value, but providing a default does not make the option optional.
To mark an option as optional, use std::optional.

Another option type is LIMITED_OPTION, which allows a vector of valid values.
```cpp
std::vector<std::string> weekends = {"Saturday", "Sunday"};
struct MyArgs : CliParser::IArgs {
	LIMITED_OPTION(std::string, 'd', Day, "Day of the week", weekends);
};
```

A third type of option is a VALIDATED_OPTION, which takes a validation function which returns a bool.
```cpp
bool IsEven(int value, std::ostream& outErrors) {
	if (value % 2 != 0) {
		outErrors << value << " is not even.";
		return false;
	}
	return true;
}
struct MyArgs : CliParser::IArgs {
	VALIDATED_OPTION(int, 'n', Number, "An even number", IsEven);
};
```

## Commands
There are two kinds of commands, default and named.
A default command is executed if no command name is provided.
```cpp
DEFAULT_COMMAND(MyArgs, args) {
	// Command implementation
}
COMMAND(NamedCommand, MyArgs, args) {
	// Command implementation
}
```
The last argument is simply what you want the local variable holding your arguments to be called.

## Custom Types
It's possible to use your own custom types as options by providing overloads in ArgParse.
```cpp
enum struct Color {
	Red,
	Green,
	Blue
};

namespace CliParser::ArgParse {
	bool TryParse(std::string_view sv, Color& outResult, std::ostream& outErrors) {
		using enum Color;
		if (sv == "Red") outResult = Red;
		else if (sv == "Green") outResult = Green;
		else if (sv == "Blue") outResult = Blue;
		else {
			outErrors << "Failed to parse " << sv << " as Color";
			return false;
		}
		return true;
	}
	std::string ToString(const Color& value) {
		switch (value) {
		using enum Color;
		case Red: return "Red";
		case Green: return "Green";
		case Blue: return "Blue";
		}
		return "Unknown";
	}
}

struct MyArgs : CliParser::IArgs {
	OPTION(Color, 'c', FavoriteColor, "Your favorite color");
};
```

## Vectors
Vectors of built in types are supported.
Play strange games, win strange prizes though, so YMMV.
```cpp
struct MyArgs : CliParser::IArgs {
	OPTION(std::vector<int>, 'n', Numbers, "A list of numbers");
};
```
Populating the vector is done by repeating the option for each value.
`MyCommand -n 1 -n 2 -n 3`

## Flags
Flags are supported as boolean options.
Flags can be combined as short values.
```cpp
struct MyArgs : CliParser::IArgs {
	OPTION(std::optional<bool>, 'v', Verbose, "Enable verbose output");
	OPTION(std::optional<bool>, 'd', Debug, "Enable debug mode");
};
```
`MyCommand -vd` sets both Verbose and Debug to true.
Currently it only makes sense to use std::optional<bool> for flags.
Perhaps in the future the optional can be removed, and any flags not specified default to false.
It is possible to explicitly set a flag to false.
`MyCommand --Verbose false`

## Limitations
- Vectors of custom types are not supported
- Optional of custom types are not supported
