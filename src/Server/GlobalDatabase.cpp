// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Erewhon Server" project
// For conditions of distribution and use, see copyright notice in LICENSE

#include <Server/GlobalDatabase.hpp>
#include <iostream>

namespace ewn
{
	void GlobalDatabase::PrepareStatements(DatabaseConnection& conn)
	{
		try
		{
			PrepareStatement(conn, "CreateSpaceship", "INSERT INTO spaceship(name, script, owner_id, last_update_date) VALUES(LOWER($2), $3, $1, NOW())", { DatabaseType::Int32, DatabaseType::Text, DatabaseType::Text });
			PrepareStatement(conn, "DeleteSpaceship", "DELETE FROM spaceship WHERE owner_id = $1 AND name = LOWER($2)", { DatabaseType::Int32, DatabaseType::Text });
			PrepareStatement(conn, "FindAccountByLogin", "SELECT id, password, password_salt FROM account WHERE login=LOWER($1);", { DatabaseType::Text });
			PrepareStatement(conn, "FindSpaceshipByOwnerIdAndName", "SELECT script FROM spaceship WHERE owner_id = $1 AND name=LOWER($2);", { DatabaseType::Int32, DatabaseType::Text });
			PrepareStatement(conn, "LoadAccount", "SELECT login, display_name, permission_level FROM account WHERE id=$1;", { DatabaseType::Int32 });
			PrepareStatement(conn, "RegisterAccount", "INSERT INTO account(login, display_name, password, password_salt, email, creation_date) VALUES (LOWER($1), $1, $2, $3, $4, NOW());", { DatabaseType::Text, DatabaseType::Text, DatabaseType::Text, DatabaseType::Text });
			PrepareStatement(conn, "UpdateLastLoginDate", "UPDATE account SET last_login_date=NOW() WHERE id=$1", { DatabaseType::Int32 });
			PrepareStatement(conn, "UpdatePermissionLevel", "UPDATE account SET permission_level=$2 WHERE id=$1", { DatabaseType::Int32, DatabaseType::Int16 });
		}
		catch (const std::exception& e)
		{
			std::cerr << "Failed to prepare statements: " << e.what() << std::endl;
			throw;
		}
	}
}
