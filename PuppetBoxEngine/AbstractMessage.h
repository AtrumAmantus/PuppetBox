#pragma once

namespace PB
{
	class AbstractMessage
	{
	public:
		AbstractMessage(void* data) : data_(data) {};
	private:
		void* data_;
	};
}