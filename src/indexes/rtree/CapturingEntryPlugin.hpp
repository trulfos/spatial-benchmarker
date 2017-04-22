#pragma once
#include "Entry.hpp"
#include <numeric>

namespace Rtree
{
	/**
	 * Simple entry class taking care of capturing the MBR of this entry on
	 * assignment.
	 */
	class CapturingEntryPlugin : public EntryPlugin
	{
		public:

			// Store unsigned in node
			using NodeData = unsigned;

			// Inherit constructors
			using EntryPlugin::EntryPlugin;

			/**
			 * Captures the MBR of the node.
			 *
			 * This should be run after the node has been filled with the first
			 * entries.
			 *
			 * @param host Entry hosting this plugin
			 */
			template<class E>
			void init(E& host)
			{
				host.getNode().data = host.getNode().getSize();
			}


			/**
			 * Calculate the original MBR.
			 *
			 * @param host Host entry for this plugin
			 */
			template<class E>
			typename E::Mbr originalMbr(const E& host) const
			{
				unsigned centerEntries = host.getNode().data;

				assert(centerEntries > 0);
				assert(centerEntries <= host.getNode().getSize());

				auto& node = host.getNode();

				return std::accumulate(
						node.begin(), node.begin() + centerEntries,
						node[0].getMbr(),
						[](const typename E::Mbr& s, const E& e) {
							return s + e.getMbr();
						}
					);
			}

	};
}
