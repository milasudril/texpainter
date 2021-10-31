//@	{
//@	  "targets":[{"name":"default_rng.hpp","type":"include"}]
//@	 ,"dependencies_extra":[{"ref":"default_rng.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_UTILS_DEFAULTRNG_HPP
#define TEXPAINTER_UTILS_DEFAULTRNG_HPP

#include "./bytes_to_hex.hpp"

#include "libenum/enum.hpp"

#include "pcg-cpp/include/pcg_random.hpp"

#define JSON_USE_IMPLICIT_CONVERSIONS 0

#include <nlohmann/json.hpp>
#include <bit>
#include <cstring>

namespace Texpainter::DefaultRng
{
	namespace detail
	{
		using RngImpl   = pcg_engines::oneseq_dxsm_128_64;
		using StateType = RngImpl::state_type;
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
		using Impl::result_type;
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

	inline std::string toString(SeedValue val)
	{
		static_assert(std::endian::native == std::endian::little);
		using ArrayType = std::array<std::byte, sizeof(val)>;
		ArrayType data{};
		memcpy(&data, &val, sizeof(val));
		return bytesToHex(data);
	}

	inline SeedValue create(Enum::Empty<SeedValue>, std::string_view src)
	{

		static_assert(std::endian::native == std::endian::little);
		using ArrayType = std::array<std::byte, sizeof(SeedValue)>;
		ArrayType bytes{};
		if(!hexToBytes(src, std::data(bytes))) { throw "Invalid seed format"; }
		detail::StateType seedval{};
		memcpy(&seedval, std::data(bytes), sizeof(SeedValue));
		return SeedValue{seedval};
	}

	inline void to_json(nlohmann::json& obj, SeedValue val) { obj = toString(val); }

	inline void from_json(nlohmann::json const& obj, SeedValue& val)
	{
		val = create(Enum::Empty<SeedValue>{}, obj.get<std::string>().c_str());
	}
}

#endif