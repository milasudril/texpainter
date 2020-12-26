//@	{
//@	"targets":[{"name":"discrete_pdf.test","type":"application", "autorun":1}]
//@	}

#include "./discrete_pdf.hpp"

#include <cassert>
#include <random>

namespace Testcases
{

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
		constexpr auto N = 1024 * 1024 * 1024;
		for(size_t k = 0; k < N; ++k)
		{
			++counts[pdf.eventIndex(U(rng))];
		}

		std::array<float, 4> output_probs{};
		std::array<float, 4> expected_probs{0.25f, 0.25f, 0.25f, 0.25f};
		std::ranges::transform(counts, std::begin(output_probs), [N](auto val) {
			return static_cast<float>(std::round(16384.0 * val / N) / 16384.0);
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
		constexpr auto N = 1024 * 1024 * 1024;
		for(size_t k = 0; k < N; ++k)
		{
			++counts[pdf.eventIndex(U(rng))];
		}

		std::array<float, 4> output_probs{};
		std::ranges::transform(counts, std::begin(output_probs), [N](auto val) {
			return static_cast<float>(16.0 * (std::round(16384.0 * val / N) / 16384.0));
		});
		assert(weights == output_probs);
	}
}

int main(int argc, char*[])
{
	Testcases::texpainterDiscretePdfOutOfBounds();
	if(argc > 2)
	{
		Testcases::texpainterDiscretePdfAllZeros();
		Testcases::texpainterDiscretePdfVerifyDistribution();
	}
	return 0;
}