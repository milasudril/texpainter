//@	{
//@	  "targets":[{"name":"default_rng.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"default_rng.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UTILS_DEFAULTRNG_HPP
#define TEXPAINTER_UTILS_DEFAULTRNG_HPP

#include "pcg-cpp/include/pcg_random.hpp"
#include "./bytes_to_hex.hpp"

#define JSON_USE_IMPLICIT_CONVERSIONS 0

#include <nlohmann/json.hpp>
#include <bit>
#include <cstring>

namespace Texpainter::DefaultRng
{
	namespace detail
	{
		using RngImpl   = pcg64;
		using StateType = pcg64::state_type;
	}

	class SeedValue
	{
	public:
		constexpr explicit SeedValue(): m_value{} {}

		constexpr explicit SeedValue(detail::StateType val): m_value{val} {}

		constexpr auto value() const { return m_value; }

		constexpr bool operator==(SeedValue const& other) const = default;

		constexpr bool operator!=(SeedValue const& other) const = default;

	private:
		detail::StateType m_value;
	};

	class Engine: private detail::RngImpl
	{
		using Impl = detail::RngImpl;

	public:
		Engine(): detail::RngImpl{} {}

		explicit Engine(SeedValue seed): Impl{seed.value()} {}

		Engine& seed(SeedValue seed)
		{
			Impl::seed(seed.value());
			return *this;
		}

		using Impl::operator();
		using Impl::max;
		using Impl::min;
	};

	Engine& engine();

	inline void seed(SeedValue val) { engine().seed(val); }

	inline SeedValue genSeed()
	{
		auto& rng    = engine();
		auto const a = detail::StateType{rng()};
		auto const b = detail::StateType{rng()} << 64;

		return SeedValue{a | b};
	}

	inline void to_json(nlohmann::json& obj, SeedValue val)
	{
		static_assert(std::endian::native == std::endian::little);
		using ArrayType = std::array<std::byte, sizeof(val)>;
		ArrayType data{};
		memcpy(&data, &val, sizeof(val));
		obj = bytesToHex(data);
	}

	inline void from_json(nlohmann::json const& obj, SeedValue& val)
	{
		static_assert(std::endian::native == std::endian::little);
		auto const& str = obj.get<std::string>();
		val = SeedValue{};
		memcpy(&val, std::data(str), std::min(sizeof(SeedValue), std::size(str)));
	}
}

#endif