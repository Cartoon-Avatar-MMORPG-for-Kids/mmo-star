// Copyright (C) 2020, Robin Klimonow. All rights reserved.

#pragma once

#include <cstddef>

namespace io
{
	struct ISink
	{
		virtual ~ISink() = default;

		virtual std::size_t Write(const char *src, std::size_t size) = 0;
		virtual std::size_t Overwrite(std::size_t position, const char *src, std::size_t size) = 0;
		virtual std::size_t Position() = 0;
		virtual void Flush() = 0;
	};
}
