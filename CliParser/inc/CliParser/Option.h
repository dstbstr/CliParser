#pragma once

#include "CliParser/ArgParse.h"

#include <string>
#include <string_view>
#include <functional>
#include <iosfwd>
#include <vector>

namespace CliParser {
	using LimitedValuesFunc = std::function<std::vector<std::string>()>;
	using NamedParams = std::vector<std::pair<std::string, std::string>>;
	template<typename T>
	using DefaultValueFunc = std::string(*)(const T&);
	template<typename T>
	using ArgParseFunc = bool(*)(std::string_view, T&, std::ostream&);
	template<typename T>
	using CustomValidatorFunc = std::function<bool(const T&, std::ostream&)>;

	enum struct Required { True, False };
	struct OptionWrapper {
		OptionWrapper(char shortName, std::string_view longName, Required required, std::string_view helpText, /*const std::string& defaultValue,*/ LimitedValuesFunc limitedValues)
			: ShortName(shortName)
			, LongName(longName)
			, IsRequired(required)
			, HelpText(helpText)
			, LimitedValues(limitedValues)
		{}
		virtual ~OptionWrapper() = default;

		virtual bool TryParse(std::string_view sv, std::ostream& outErrors) = 0;
		virtual std::string GetDefaultValue() = 0;

		char ShortName {' '};
		std::string LongName {"Unset"};
		Required IsRequired{ Required::False };
		std::string HelpText{ "No help text" };
		bool Populated{ false };
		LimitedValuesFunc LimitedValues{ nullptr };
	};

	template<typename T>
	struct Option : public OptionWrapper {
		Option(char shortName, std::string_view longName, Required required, std::string_view helpText, DefaultValueFunc<T> defaultValueFunc, T& backingField, ArgParseFunc<T> argParser, LimitedValuesFunc limitedValues, CustomValidatorFunc<T> customValidator)
			: OptionWrapper(shortName, longName, required, helpText, limitedValues)
			, BackingField(backingField)
			, ArgParser(argParser)
			, CustomValidator(customValidator)
			, DefaultValueFunc(defaultValueFunc)
		{
		}

		bool TryParse(std::string_view sv, std::ostream& outErrors) override {
			if(!Populated) {
				BackingField = {};
			}
			if (!ArgParser(sv, BackingField, outErrors)) {
				return false;
			}
			if(CustomValidator) {
				return CustomValidator(BackingField, outErrors);
			}
			return true;
		}

		std::string GetDefaultValue() override {
			return DefaultValueFunc(BackingField);
		}
		T& BackingField;
		ArgParseFunc<T> ArgParser{ nullptr };
		CustomValidatorFunc<T> CustomValidator{ nullptr };
		DefaultValueFunc<T> DefaultValueFunc;
	};

	class IArgs {
	public:
		IArgs() = default;
		IArgs(const IArgs&) = delete;
		IArgs& operator=(const IArgs&) = delete;

		virtual ~IArgs() = default;

		virtual bool TryParse(std::vector<std::string_view> args, std::ostream& outErrors);
		virtual bool TryParse(int argc, char* argv[], std::ostream& outErrors);
		virtual std::string Describe();

	private:
		template<typename T>
		friend struct IArgsRegistrar;

		std::vector<std::unique_ptr<OptionWrapper>> Options{};

		bool IsValueValid(const OptionWrapper& option, std::string_view value, std::ostream& outErrors);
		bool AreRequiredValuesPopulated(std::ostream& outErrors);

		virtual bool ValidateParsedArgs(std::ostream&) { return true; }
	};

	template<typename T>
	struct IArgsRegistrar {
		IArgsRegistrar(
			IArgs& collection,
			char shortName,
			std::string_view longName,
			Required required,
			std::string_view helpText,
			DefaultValueFunc<T> defaultValueFunc,
			T& backingField,
			ArgParseFunc<T> argParser,
			LimitedValuesFunc limitedValues,
			CustomValidatorFunc<T> customValidator) {
			collection.Options.push_back(std::make_unique<Option<T>>(
					shortName,
					longName,
					required,
					helpText,
					defaultValueFunc,
					backingField,
					argParser,
					limitedValues,
					customValidator
			));
		}
	};
}

#define _IMPL_OPTION(_type, _short_name, _long_name, _required, _helpText, _limitedFunc, _customValidator, ...) \
public: \
	_type _long_name{__VA_ARGS__}; \
private: \
	CliParser::IArgsRegistrar<_type> _long_name##Option{ \
		*this, \
		_short_name, \
		#_long_name, \
		_required, \
		_helpText, \
		static_cast<DefaultValueFunc<_type>>(CliParser::ArgParse::ToString), \
		_long_name, \
		static_cast<ArgParseFunc<_type>>(CliParser::ArgParse::TryParse), \
		_limitedFunc, \
		_customValidator\
 	}; \
public:

#define OPTION(_type, _short_name, _long_name, _required, _helpText, ...) _IMPL_OPTION(_type, _short_name, _long_name, _required, _helpText, nullptr, nullptr, __VA_ARGS__)
#define LIMITED_OPTION(_type, _short_name, _long_name, _required, _helpText, _limitedFunc, ...) _IMPL_OPTION(_type, _short_name, _long_name, _required, _helpText, _limitedFunc, nullptr, __VA_ARGS__)
#define VALIDATED_OPTION(_type, _short_name, _long_name, _required, _helpText, _customValidator, ...) _IMPL_OPTION(_type, _short_name, _long_name, _required, _helpText, nullptr, _customValidator, __VA_ARGS__)