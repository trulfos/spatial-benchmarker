#pragma once
#include "EntryPlugin.hpp"
#include "HilbertCurve.hpp"
#include <cstdint>

namespace Rtree
{

/**
 * Entry plugin providing Hilbert value management.
 */
class HilbertEntryPlugin : public EntryPlugin
{
	public:

		HilbertEntryPlugin() = default;

		HilbertEntryPlugin(const HilbertEntryPlugin& other)
		{
			hilbertValue = other.hilbertValue;
		}

		HilbertEntryPlugin(HilbertEntryPlugin& other)
		{
			hilbertValue = other.hilbertValue;
		}

		HilbertEntryPlugin& operator=(const HilbertEntryPlugin& other)
		{
			hilbertValue = other.hilbertValue;
			return *this;
		}

		HilbertEntryPlugin& operator=(HilbertEntryPlugin& other)
		{
			hilbertValue = other.hilbertValue;
			return *this;
		}


		template<class E>
		HilbertEntryPlugin(E& host)
			: EntryPlugin(host), hilbertValue(0)
		{
		};


		template<class E>
		HilbertEntryPlugin(E& host, const DataObject&, const Box& bounds)
			: EntryPlugin(host), hilbertValue(
					HilbertCurve<std::uint64_t, E::Mbr::dimension>::map(
							host.getMbr().center(),
							bounds
						)
				)
		{
		};


		template<class E, class BE>
		void include(E& host, BE& entry)
		{
			// Update hilbert value
			hilbertValue = std::max(
					hilbertValue,
					entry.getPlugin().getHilbertValue()
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
