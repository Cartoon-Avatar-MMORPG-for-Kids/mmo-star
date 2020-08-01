// Copyright (C) 2020, Robin Klimonow. All rights reserved.

#pragma once

#include "container_source.h"

#include <string>

namespace io
{
	typedef ContainerSource<std::string> StringSource;
	typedef ContainerSource<std::wstring> WStringSource;
}
