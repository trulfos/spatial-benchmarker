#pragma once
#include "Entry.hpp"
#include <numeric>

namespace Rtree
{
	/**
	 * Simple entry class taking care of capturing the MBR of this entry on
	 * assignment.
	 */
	template<class E>
	class CapturingEntryPlugin : public EntryPlugin<E>
	{
		using M = typename E::M;

		public:

			// Store unsigned in node
			using NodeData = unsigned;


			// Inherit constructors
			using EntryPlugin<E>::EntryPlugin;


			/**
			 * Captures the MBR of the node.
			 *
			 * This should be run after the node has been filled with the first
			 * entries.
			 *
			 * @param host Entry hosting this plugin
			 */
			void init(E& host)
			{
				host.getNode()->data = host.getNode()->size();
			}


			/**
			 * Calculate the original MBR.
			 *
			 * @param host Host entry for this plugin
			 */
			M originalMbr(const E& host) const
			{
				unsigned centerEntries = host.getNode()->data;

				assert(centerEntries > 0);
				assert(centerEntries <= host.getNode()->size());

				return std::accumulate(
						host.begin(), host.begin() + centerEntries,
						host.begin()[0].getMbr(),
						[](const M& s, const E& e) { return s + e.getMbr(); }
					);
			}

	};
}
