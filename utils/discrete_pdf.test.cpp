//@	{
//@	"targets":[{"name":"discrete_pdf.test","type":"application", "autorun":1}]
//@	}

#include "./discrete_pdf.hpp"

#include <cassert>
#include <random>

namespace Testcases
{
	constexpr auto N        = 1024 * 1024 * 4;
	constexpr double factor = 512.0;

	void texpainterDiscretePdfOutOfBounds()
	{
		std::array<float, 7> weights{0.0f, 1.0f, 2.0f, 4.0f, 9.0f, 0.0f, 0.0f};

		Texpainter::DiscretePdf pdf{weights};
		assert(pdf.eventIndex(1.0f) == 4);
		assert(pdf.eventIndex(std::nextafter(0.0f, -1.0f)) == 1);
	}

	void texpainterDiscretePdfAllZeros()
	{
		std::array<float, 4> weights{0.0f, 0.0f, 0.0f, 0.0f};

		Texpainter::DiscretePdf pdf{weights};
		std::random_device rng;
		std::uniform_real_distribution U{0.0f, 1.0f};

		std::array<size_t, 4> counts{};
		for(size_t k = 0; k < N; ++k)
		{
			++counts[pdf.eventIndex(U(rng))];
		}

		std::array<float, 4> output_probs{};
		std::array<float, 4> expected_probs{0.25f, 0.25f, 0.25f, 0.25f};
		std::ranges::transform(counts, std::begin(output_probs), [](auto val) {
			return static_cast<float>(
			    std::round(factor * static_cast<double>(val) / static_cast<double>(N)) / factor);
		});

		assert(output_probs == expected_probs);
	}

	void texpainterDiscretePdfUniformDistribution()
	{
		std::array<float, 4> weights{1.0f, 1.0f, 1.0f, 1.0f};

		Texpainter::DiscretePdf pdf{weights};
		std::random_device rng;
		std::uniform_real_distribution U{0.0f, 1.0f};

		std::array<size_t, 4> counts{};
		for(size_t k = 0; k < N; ++k)
		{
			++counts[pdf.eventIndex(U(rng))];
		}

		std::array<float, 4> output_probs{};
		std::array<float, 4> expected_probs{0.25f, 0.25f, 0.25f, 0.25f};
		std::ranges::transform(counts, std::begin(output_probs), [](auto val) {
			return static_cast<float>(
			    std::round(factor * static_cast<double>(val) / static_cast<double>(N)) / factor);
		});

		assert(output_probs == expected_probs);
	}

	void texpainterDiscretePdfVerifyDistribution()
	{
		std::array<float, 4> weights{1.0f, 2.0f, 4.0f, 9.0f};

		Texpainter::DiscretePdf pdf{weights};
		std::random_device rng;
		std::uniform_real_distribution U{0.0f, 1.0f};

		std::array<size_t, 4> counts{};
		for(size_t k = 0; k < N; ++k)
		{
			++counts[pdf.eventIndex(U(rng))];
		}

		std::array<float, 4> output_probs{};
		std::ranges::transform(counts, std::begin(output_probs), [](auto val) {
			return static_cast<float>(
			    16.0
			    * (std::round(factor * static_cast<double>(val) / static_cast<double>(N))
			       / factor));
		});
		assert(weights == output_probs);
	}
}

int main(int, char*[])
{
	Testcases::texpainterDiscretePdfOutOfBounds();
	Testcases::texpainterDiscretePdfAllZeros();
	Testcases::texpainterDiscretePdfUniformDistribution();
	Testcases::texpainterDiscretePdfVerifyDistribution();
	return 0;
}
