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

			/**
			 * Default constructor.
			 */
			CapturingEntryPlugin() = default;

			/**
			 * Called whenever the host is initialized with a list of entries.
			 *
			 * Called prior to adding the entries, such that the new entries can
			 * be found in the host entry's node.
			 *
			 * @param host Entry hosting this plugin (which new children)
			 */
			CapturingEntryPlugin(E& host)
			{
				host.getNode()->data = host.getNode()->size();
			}

			// Inherit most constructors
			using EntryPlugin<E>::EntryPlugin;


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
