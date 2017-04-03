#pragma once
#include "EntryPlugin.hpp"
#include "HilbertCurve.hpp"
#include <cstdint>

namespace Rtree
{

/**
 * Entry plugin providing Hilbert value management.
 */
template<class E>
class HilbertEntryPlugin : public EntryPlugin<E>
{
	using HC = HilbertCurve<std::uint64_t, E::dimension>;

	public:
		HilbertEntryPlugin(E& host)
			: EntryPlugin<E>(host), hilbertValue(0)
		{
		};


		HilbertEntryPlugin(E& host, const DataObject&, const Box& bounds)
			: EntryPlugin<E>(host),
				hilbertValue(HC::map(host.getMbr().center(), bounds))
		{
		};


		void include(E& host, const E& entry)
		{
			// Update hilbert value
			hilbertValue = std::max(
					hilbertValue,
					entry.getPlugin().getHilbertValue()
				);

			// Keep sorted
			std::inplace_merge(
					host.begin(),
					host.end() - 1,
					host.end(),
					[](const E& a, const E& b) {
						return a.getPlugin().getHilbertValue()
							< b.getPlugin().getHilbertValue();
					}
				);
		};


		/**
		 * Get the hilbert value associated with this plugins entry.
		 */
		const std::uint64_t& getHilbertValue() const
		{
			return hilbertValue;
		}

	private:
		std::uint64_t hilbertValue;
};

}
