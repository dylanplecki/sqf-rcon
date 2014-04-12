#include "stdafx.h"
#include "package.h"

namespace sqfrcon
{
	std::atomic<size_t> package::pid_counter = 0;

	bool package::partial(std::string& container, size_t amount)
	{
		container = this->contents.substr(this->placeholder, amount);
		this->placeholder += amount;
		return (this->placeholder >= this->contents.size());
	}
}