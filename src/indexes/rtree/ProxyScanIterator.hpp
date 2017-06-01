#pragma once
#include "ProxyEntry.hpp"

namespace Rtree
{

	/**
	 * Basic class for scan iterator using an internal proxy entry for state.
	 *
	 * @tparam N Node type
	 */
	template<class N>
	class ProxyScanIterator
	{
		public:
			using iterator_category = std::input_iterator_tag;
			using value_type = Entry<const N>;
			using difference_type = int;
			using pointer = const ProxyEntry<const N> *;
			using reference = const ProxyEntry<const N>&;

			/**
			 * Default constructible - because the standard requires this.
			 */
			ProxyScanIterator() = default;


			ProxyScanIterator(const ProxyScanIterator& other)
			{
				operator=(other);
			}


			ProxyScanIterator(const N * node, unsigned index)
				: entry(node, index)
			{
			}


			/**
			 * Assign another proxy scan iterator to this.
			 *
			 * This is necessary to avoid actually assigning the data pointed to
			 * by the proxy entry of the other iterator to the entry in this
			 * iterator (which would otherwise happen).
			 */
			ProxyScanIterator& operator=(const ProxyScanIterator& other)
			{
				entry.reset(other.entry);
				return *this;
			}

			reference operator*()
			{
				assert(entry.index < entry.node->getSize());
				return entry;
			}

			pointer operator->()
			{
				return &operator*();
			}

			bool operator==(const ProxyScanIterator& other) const
			{
				assert(other.entry.node == entry.node);
				return other.entry.index == entry.index;
			}

			bool operator!=(const ProxyScanIterator& other) const
			{
				return !(*this == other);
			}


		protected:
			ProxyEntry<const N> entry;
	};
}
