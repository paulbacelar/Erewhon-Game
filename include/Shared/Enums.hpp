// Copyright (C) 2018 Jérôme Leclercq
// This file is part of the "Erewhon Shared" project
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#ifndef EREWHON_SHARED_ENUMS_HPP
#define EREWHON_SHARED_ENUMS_HPP

#include <Nazara/Prerequisites.hpp>

namespace ewn
{
	enum class BotMessageType : Nz::UInt8
	{
		Error,
		Warning,
		Info
	};

	enum class LoginFailureReason : Nz::UInt8
	{
		AccountNotFound,
		PasswordMismatch,
		ServerError
	};

	enum class RegisterFailureReason : Nz::UInt8
	{
		EmailAlreadyTaken,
		LoginAlreadyTaken,
		ServerError
	};

	enum class UpdateSpaceshipFailureReason : Nz::UInt8
	{
		NotFound,
		ServerError
	};
}

#endif // EREWHON_SHARED_ENUMS_HPP
