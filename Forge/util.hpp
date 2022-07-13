#pragma once

#include <string>
#include <vector>
#include <source_location>
#include "sdk.hpp"
#include "report.hpp"

namespace sdk
{
	namespace signatures
	{
		constexpr char const* integer = "I";
		constexpr char const* long_integer = "J";
		constexpr char const* byte = "B";
		constexpr char const* boolean = "Z";
		constexpr char const* floating = "F";
		constexpr char const* short_integer = "S";
		constexpr char const* double_floating = "D";
		constexpr char const* string = "Ljava/lang/String;";
		constexpr char const* object = "Ljava/lang/Object;";
	}

	template<typename T>
	concept signatureable = std::_Is_any_of_v<T, int, long, short, char, bool, float, double>;

	template<signatureable T>
	constexpr char const* signature = std::is_same_v<T, int> ? signatures::integer :
		std::is_same_v<T, long> ? signatures::long_integer :
		std::is_same_v<T, char> ? signatures::byte :
		std::is_same_v<T, short> ? signatures::short_integer :
		std::is_same_v<T, bool> ? signatures::boolean :
		std::is_same_v<T, std::string> ? signatures::string : signatures::object;

	template<typename T>
	using signature_type = typename std::conditional_t<std::is_same_v<T, int>, jint, typename std::conditional_t<std::is_same_v<T, long>, jlong,
		typename std::conditional_t<std::is_same_v<T, short>, jshort, typename std::conditional_t<std::is_same_v<T, bool>, jboolean,
		typename std::conditional_t<std::is_same_v<T, float>, jfloat, typename std::conditional_t<std::is_same_v<T, double>, jdouble, jobject>>>>>>;

	jclass const inline find_class(std::string const& name, std::source_location const& source = std::source_location::current()) noexcept
	{
		jclass const value = sdk::get_env()->FindClass(name.data());
		if (value == nullptr) [[unlikely]]
		{
			sdk::report_error(mixins::connect("Failed to find class: ", name), source);
		}

		return value;
	}

	jfieldID const inline find_field(jclass const type, std::string const& name, std::string const& signature, 
		std::source_location const& source = std::source_location::current()) noexcept
	{
		jfieldID const value = sdk::get_env()->GetFieldID(type, name.data(), signature.data());
		if (value == nullptr) [[unlikely]]
		{
			sdk::report_error(mixins::connect("Failed to find non-static field: ", name, " Signature: ", signature), source);
		}

		return value;
	}

	template<signatureable T>
	jfieldID const inline find_field(jclass const type, std::string const& name, std::source_location const& source = std::source_location::current()) noexcept
	{
		jfieldID const value = sdk::get_env()->GetFieldID(type, name.data(), sdk::signature<T>);
		if (value == nullptr) [[unlikely]]
		{
			sdk::report_error(mixins::connect("Failed to find non-static field: ", name, " Signature: ", sdk::signature<T>), source);
		}

		return value;
	}


	jfieldID const inline find_static_field(jclass const type, std::string const& name, std::string const& signature, 
		std::source_location const& source = std::source_location::current()) noexcept
	{
		jfieldID const value = sdk::get_env()->GetStaticFieldID(type, name.data(), signature.data());
		if (value == nullptr) [[unlikely]]
		{
			sdk::report_error(mixins::connect("Failed to static find field: ", name, " Signature: ", signature), source);
		}

		return value;
	}

	template<typename T>
	jfieldID const inline find_static_field(jclass const type, std::string const& name, std::source_location const& source = std::source_location::current()) noexcept
	{
		jfieldID const value = sdk::get_env()->GetStaticFieldID(type, name.data(), sdk::signature<T>);
		if (value == nullptr) [[unlikely]]
		{
			sdk::report_error(mixins::connect("Failed to static find field: ", name, " Signature: ", sdk::signature<T>), source);
		}

		return value;
	}

	std::vector<jfieldID> const inline get_class_fields(jclass const type) noexcept
	{
		jint field_count{};
		jfieldID* fields{};
		sdk::report_jvmti_error(sdk::get_envti()->GetClassFields(type, &field_count, &fields));

		std::vector<jfieldID> v;
		v.resize(field_count);
		std::copy(fields, fields + field_count, v.data());

		delete[] fields;

		return v;
	}

	std::vector<jclass> const inline get_loaded_classes() noexcept
	{
		jint count{};
		jclass* classes{};
		sdk::report_jvmti_error(sdk::get_envti()->GetLoadedClasses(&count, &classes));

		std::vector<jclass> v;
		v.resize(count);
		std::copy(classes, classes + count, v.data());
		
		delete[] classes;

		return v;
	}

	auto const inline get_field_name(jclass const type, jfieldID const field_id) noexcept
	{
		struct field_name
		{
			char* name;
			char* signature;
			char* generic;

			~field_name()
			{
				delete[] name;
				delete[] signature;
				delete[] generic;
			}
		} name{};

		sdk::get_envti()->GetFieldName(type, field_id, &name.name, &name.signature, &name.generic);
		return name;
	}

	auto const inline get_class_name(jclass const type) noexcept
	{
		struct class_name
		{
			char* signature;
			char* generic;

			~class_name()
			{
				delete[] signature;
				delete[] generic;
			}
		} name{};
		
		sdk::get_envti()->GetClassSignature(type, &name.signature, &name.generic);

		return name;
	}

	class type
	{
	private:
		jclass _type;

	public:
		inline type(jclass const type, std::source_location const& source = std::source_location::current()) noexcept : _type{ type }
		{
			if (type == nullptr) [[unlikely]]
			{
				sdk::report_error("type is null", source);
			}
		}

		inline type(std::string const& name, std::source_location const& source = std::source_location::current()) noexcept : _type{ sdk::find_class(name, source) }
		{
			
		}

		template<typename T>
		inline jfieldID const get_field_id(std::string const& name, std::source_location const& source = std::source_location::current()) const noexcept
		{
			return sdk::find_field<T>(_type, name, source);
		}

		inline jfieldID const get_field_id(std::string const& name, std::string const& signature ,std::source_location const& source = std::source_location::current()) const noexcept
		{
			return sdk::find_field(_type, name, signature, source);
		}

		template<typename T>
		inline jfieldID const get_static_field_id(std::string const& name, std::source_location const& source = std::source_location::current()) const noexcept
		{
			return sdk::find_static_field<T>(_type, name, source);
		}

		inline jfieldID const get_static_field_id(std::string const& name, std::string const& signature, std::source_location const& source = std::source_location::current()) const noexcept
		{
			return sdk::find_static_field(_type, name, signature, source);
		}
		
		template<signatureable T>
		inline signature_type<T> const get_field(jfieldID const field_id) const noexcept
		{
			if constexpr (std::is_same_v<T, int>)
			{
				return sdk::get_env()->GetStaticIntField(_type, field_id);
			}
			else if constexpr (std::is_same_v<T, long>)
			{
				return sdk::get_env()->GetStaticLongField(_type, field_id);
			}
			else if constexpr (std::is_same_v<T, float>)
			{
				return sdk::get_env()->GetStaticFloatField(_type, field_id);
			}
			else if constexpr (std::is_same_v<T, double>)
			{
				return sdk::get_env()->GetStaticDoubleField(_type, field_id);
			}
			else if constexpr (std::is_same_v<T, char>)
			{
				return sdk::get_env()->GetStaticCharField(_type, field_id);
			}
			else if constexpr (std::is_same_v<T, short>)
			{
				return sdk::get_env()->GetStaticShortField(_type, field_id);
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				return sdk::get_env()->GetStaticBooleanField(_type, field_id);
			}
			else
			{
				return sdk::get_env()->GetStaticObjectField(_type, field_id);
			}
		}

		template<signatureable T>
		inline signature_type<T> const get_field(std::string const& name, std::source_location const& source = std::source_location::current()) const noexcept
		{
			return get_field<T>(get_static_field_id<T>(name, source));
		}
	};
}