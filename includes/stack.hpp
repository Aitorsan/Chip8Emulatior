#pragma once
#include <type_traits>

template <typename T, unsigned int N>
class stack
{   
public:
	using stack_type = stack<T, N>;
	using value_type = T;
	using reference_type = T & ;
	using const_ref = T const&;

	template <class = std::enable_if<std::is_fundamental<T>::value, T>::type>
	stack() 
		: s_size{ 0 }
		, block{}
		, sp{ block }
	{
	}

   stack ( const stack<value_type,N>& other)
		: s_size{other.s_size}
		, block{}
		, sp{ block }
	{
	   for (std::size_t i = 0; i < s_size; ++i )
	   {
		   *sp = other.block[i];
		   ++sp;
	   }
	}

	void push(const_ref value)
	{
		if (s_size < N)
		{
			*sp = value;
			++sp;
			++s_size;
		}
	}

	//WARNING: if the stack is empty an emtpy default value is returned
	value_type pop()
	{
		value_type  temp = value_type{};
		if (s_size >0)
		{
		  --s_size;
		  --sp;
	       temp = std::move(*sp);   
		}
		return temp; 
	}

	std::size_t size() { return s_size;}

	stack_type& operator=(const stack_type& other)
	{
		if (this == &other)
			return *this;

		s_size = other.s_size;
		sp = block;
		for (std::size_t i = 0; i < s_size;++i,++sp)
		{
			*sp = other.block[i];
		}
		return *this;
	}
private:
	std::size_t s_size;
	value_type block[N];
	value_type* sp;
};