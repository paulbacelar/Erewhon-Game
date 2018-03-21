// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Erewhon Server" project
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef EREWHON_SERVER_DATABASETYPE_HPP
#define EREWHON_SERVER_DATABASETYPE_HPP

#include <Nazara/Prerequisites.hpp>
#include <json/json.hpp>
#include <string>
#include <variant>
#include <vector>

namespace ewn
{
	enum class DatabaseType
	{
		Binary,
		Bool,
		Char,
		Date,
		Double,
		FixedVarchar,
		Int16,
		Int32,
		Int64,
		Json,
		Single,
		Text,
		Time,
		Varchar
	};

	constexpr unsigned int GetDatabaseOid(DatabaseType type);
	template<typename T> constexpr DatabaseType GetDatabaseType();

	using DatabaseValue = std::variant<std::vector<Nz::UInt8>, bool, char, double, Nz::Int16, Nz::Int32, Nz::Int64, float, const char*, std::string, nlohmann::json>;
}

#include <Server/Database/DatabaseTypes.inl>

#endif // EREWHON_SERVER_DATABASETYPE_HPP
