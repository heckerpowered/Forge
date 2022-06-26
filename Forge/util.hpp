#pragma once

#include <string>
#include <vector>
#include <source_location>
#include "sdk.hpp"
#include "report.hpp"

namespace sdk
{
	jclass inline find_class(std::string const& name, std::source_location const& source = std::source_location::current()) noexcept
	{
		auto value = sdk::get_env()->FindClass(name.data());
		if (value == nullptr)
		{
			sdk::report_error(mixins::connect("Failed to find class: ", name), source);
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
}