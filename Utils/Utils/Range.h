#pragma once

#include <type_traits>
#include <utility>


template <class iterator, class T>
constexpr bool is_iterator_of_v = std::is_same_v<T, decltype(std::declval<iterator>().operator*())> || std::is_convertible_v<decltype(std::declval<iterator>().operator*()), T>;

namespace Utils {
/*
template<class T>
struct C_Range
{
private:
	class iterBase
	{
	public:
		using value_type = T;
		using reference = T&;
		using difference_type = std::ptrdiff_t;

		template<class T_exterIter>
		iterBase(T_exterIter&& value)
		{
			*this = std::forward<T_exterIter>(value);
		}

		template<class T_exterIter>
		iterBase& operator=(T_exterIter&& value)
		{
			static_assert(is_iterator_of_v<iterBase, T>, "The T_exterIter has to be of same type with T");
			m_value.reset(new Model<T_exterIter>(std::forward<T_exterIter>(value)));
			return *this;
		}

		iterBase(iterBase&& other)
			: m_value(std::move(other.m_value))
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "iterBase(iterBase&& other)");
		}

		iterBase& operator++() { m_value->increase(); return *this; }
		bool operator!=(const iterBase& other) const
		{
			return false;
		}
		T operator*() const { return m_value->get(); }

	private:
		struct Concept
		{
			virtual ~Concept() = default;
			virtual void increase() = 0;
			//virtual bool isEqual(const Concept&) const = 0;
			virtual T get() const = 0;
		};

		template<class U>
		struct Model : public Concept
		{
			Model(U&& val)
				: m_Val(std::forward<U>(val))
			{
				CORE_LOG(E_Level::Error, E_Context::Core, "Model(U&& val)");
			}

			virtual void increase() { ++m_Val; }
			virtual T get() const
			{
				return *m_Val;
			}

			U m_Val;
		};
		std::unique_ptr<Concept> m_value;
	};
	static_assert(is_iterator_of_v<iterBase, T>, "The iterable has to be of same type with T");
public:

	template<class iterable>
	C_Range(iterable&& begin, iterable&& end)
		: m_Begin(std::forward<iterable>(begin))
		, m_End(std::move(end))
	{
		static_assert(is_iterator_of_v<iterable, T>, "The iterable has to be of same type with T");
	}

	~C_Range()
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "~C_Range()");
	}

	C_Range(C_Range&& other)
		: m_Begin(std::move(other.m_Begin))
		, m_End(std::move(other.m_End))
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "const C_Range&& other");
	}

	template<class iterable>
	C_Range(std::pair<iterable, iterable>&& range)
		: m_Begin(std::move(range.first))
		, m_End(std::move(range.second))
	{
		static_assert(is_iterator_of_v<iterable, T>, "The iterable has to be of same type with T");
	}

	iterBase& begin() {
		CORE_LOG(E_Level::Error, E_Context::Core, "begin()");
		return m_Begin;
	}
	iterBase& end() { return m_End; }

	const iterBase& cbegin() const { return m_Begin; }
	const iterBase& cend() const { return m_End; }

	//auto size() const { return std::distance(cbegin(), cend()); }


private:
	iterBase m_Begin;
	iterBase m_End;
};*/

template <class T> struct Range {
	Range(T&& begin, T&& end)
		: m_begin(std::forward<T>(begin))
		, m_end(std::forward<T>(end))
	{
	}

	Range(Range&& other)
		: m_begin(other.m_begin)
		, m_end(other.m_end)
	{
	}

	T& begin() { return m_begin; }
	T& end() { return m_end; }

	bool empty() const { return m_begin == m_end; }

private:
	T m_begin;
	T m_end;
};

} // namespace Utils
