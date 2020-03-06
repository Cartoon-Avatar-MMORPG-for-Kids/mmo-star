// Copyright (C) 2020, Robin Klimonow. All rights reserved.

#pragma once

#include "frame.h"


namespace mmo
{
	/// This class inherits the Frame class and extends it by some button logic.
	class Button
		: public Frame
	{
	public:
		/// Triggered when the button was clicked.
		signal<void()> Clicked;

	public:
		explicit Button(const std::string& type, const std::string& name);
		virtual ~Button() = default;
	};
}
