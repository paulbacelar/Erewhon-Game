// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Erewhon Server" project
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef EREWHON_SERVER_DATABASETRANSACTION_HPP
#define EREWHON_SERVER_DATABASETRANSACTION_HPP

#include <Nazara/Prerequisites.hpp>
#include <Server/Database/DatabaseTypes.hpp>
#include <string>
#include <variant>
#include <vector>

namespace ewn
{
	class DatabaseResult;

	class DatabaseTransaction
	{
		public:
			struct PreparedStatement;
			struct QueryStatement;
			struct Statement;
			using StatementVector = std::vector<Statement>;
			using TransactionOperator = std::function<DatabaseResult(DatabaseTransaction& transaction, DatabaseResult result)>;
			using const_iterator = typename StatementVector::const_iterator;
			using size_type = typename StatementVector::size_type;

			DatabaseTransaction() = default;
			DatabaseTransaction(const DatabaseTransaction&) = delete;
			DatabaseTransaction(DatabaseTransaction&&) noexcept = default;
			~DatabaseTransaction() = default;

			inline std::size_t AppendQuery(std::string query, TransactionOperator transactionOperator = nullptr);
			inline std::size_t AppendPreparedStatement(std::string statementName, std::initializer_list<DatabaseValue> parameters, TransactionOperator transactionOperator = nullptr);
			inline std::size_t AppendPreparedStatement(std::string statementName, std::vector<DatabaseValue> parameters, TransactionOperator transactionOperator = nullptr);
			inline std::size_t AppendPreparedStatement(std::string statementName, const DatabaseValue* parameters, std::size_t parameterCount, TransactionOperator transactionOperator = nullptr);

			inline std::size_t GetBeginResultIndex();
			inline std::size_t GetCommitResultIndex();

			// STL API
			inline const_iterator begin() const;
			inline bool empty() const;
			inline const_iterator end() const;
			inline size_type size() const;

			inline Statement& operator[](std::size_t i);
			inline const Statement& operator[](std::size_t i) const;

			DatabaseTransaction& operator=(const DatabaseTransaction&) = delete;
			DatabaseTransaction& operator=(DatabaseTransaction&&) noexcept = default;

			struct PreparedStatement
			{
				std::string statementName;
				std::vector<DatabaseValue> parameters;
			};

			struct QueryStatement
			{
				std::string query;
			};

			struct Statement
			{
				std::variant<QueryStatement, PreparedStatement> statement;
				TransactionOperator operatorFunc;
			};

		private:
			std::vector<Statement> m_statements;
	};
}

#include <Server/Database/DatabaseTransaction.inl>

#endif // EREWHON_SERVER_DATABASETRANSACTION_HPP
