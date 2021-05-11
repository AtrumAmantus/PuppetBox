#pragma once

namespace PB
{
	template <class T>
	class ITree
	{
	public:
		virtual ITree<T>* get(T value) = 0;
		virtual T value() = 0;
		virtual void has(T value) = 0;
		virtual void add(T value) = 0;
		virtual void remove(T value) = 0;
	};
}