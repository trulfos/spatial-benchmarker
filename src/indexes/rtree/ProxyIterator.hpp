#pragma once
#include "Entry.hpp"

namespace Rtree
{

	/**
	 * Iterates through the proxies of a node.
	 */
	template<class Node>
	class ProxyIterator
	{
		using ProxyEntry = typename Node::reference;

		public:
			using iterator_category = std::random_access_iterator_tag;
			using reference = ProxyEntry&;
			using pointer = ProxyEntry *;
			using value_type = Entry<Node>;
			using difference_type = int;

			// Because standards...
			ProxyIterator() = default;

			ProxyIterator(Node * node, unsigned index)
				: proxy(node, index)
			{
				assert(index <= node->getSize());
			}

			ProxyIterator& operator++()
			{
				++proxy.index;
				return *this;
			}

			ProxyIterator operator++(int)
			{
				ProxyIterator it = *this;
				operator++();
				return it;
			}

			ProxyIterator& operator--()
			{
				--proxy.index;
				return *this;
			}

			ProxyIterator operator--(int)
			{
				ProxyIterator it = *this;
				operator--();
				return it;
			}

			reference operator*()
			{
				return proxy;
			}

			const ProxyEntry& operator*() const
			{
				return proxy;
			}

			pointer operator->()
			{
				return &proxy;
			}

			ProxyIterator& operator+=(difference_type distance)
			{
				assert(distance + proxy.index >= 0);
				proxy.index += distance;
				return *this;
			}


			ProxyIterator& operator-=(difference_type distance)
			{
				return operator+(-distance);
			}


			ProxyIterator operator+(difference_type distance) const
			{
				ProxyIterator it = *this;
				it += distance;
				return it;
			}


			ProxyIterator operator-(difference_type distance) const
			{
				return operator+(-distance);
			}

			difference_type operator-(const ProxyIterator& other) const
			{
				assert(proxy.node == other.proxy.node);
				return difference_type(proxy.index)
					- difference_type(other.proxy.index);
			}


			ProxyIterator operator[](difference_type index)
			{
				return *(*this + index);
			}

			bool operator>(const ProxyIterator& other) const
			{
				assert(proxy.node == other.proxy.node);
				return proxy.index > other.proxy.index;
			}


			bool operator<(const ProxyIterator& other) const
			{
				assert(proxy.node == other.proxy.node);
				return proxy.index < other.proxy.index;
			}


			bool operator>=(const ProxyIterator& other) const
			{
				assert(proxy.node == other.proxy.node);
				return proxy.index >= other.proxy.index;
			}


			bool operator<=(const ProxyIterator& other) const
			{
				assert(proxy.node == other.proxy.node);
				return proxy.index <= other.proxy.index;
			}


			bool operator==(const ProxyIterator& other) const
			{
				return proxy.node == other.proxy.node &&
					proxy.index == other.proxy.index;
			}

			bool operator!=(const ProxyIterator& other) const
			{
				return !(*this == other);
			}

			ProxyIterator& operator=(const ProxyIterator& other)
			{
				proxy.node = other.proxy.node;
				proxy.index = other.proxy.index;
				return *this;
			}

			/**
			 * Get the node from which this iterator comes.
			 *
			 * The weired name is to avoid confusion with the
			 * `getNode()` method of entries as `i.getNode()` can easily
			 * be confused with `i->getNode()`.
			 *
			 * @return Node associated with this iterator
			 */
			Node& getContainingNode() const
			{
				return *proxy.node;
			}

		private:
			ProxyEntry proxy;
	};


	/**
	 * Const edition of proxy iterator.
	 *
	 * This one does however only satisfy InputIterator as nothing else was
	 * needed.
	 */
	template<class Node>
	class ConstProxyIterator
	{
		public:
			using iterator_category = std::input_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using reference = const Entry<Node>&;
			using pointer = const Entry<Node> *;
			using value_type = Entry<Node>;

			ConstProxyIterator(const Node * node, unsigned index)
				: node(node), index(index)
			{
				assert(index <= node->getSize());
				updateCachedEntry();
			}

			ConstProxyIterator& operator++()
			{
				++index;
				updateCachedEntry();
				return *this;
			}

			ConstProxyIterator operator++(int)
			{
				ConstProxyIterator it = *this;
				operator++();
				return it;
			}

			reference operator*() const
			{
				return entry;
			}

			pointer operator->() const
			{
				return &operator*();
			}

			bool operator==(const ConstProxyIterator& other) const
			{
				return node == other.node &&
					index == other.index;
			}

		private:
			const Node * node;
			unsigned index;
			value_type entry;


			void updateCachedEntry()
			{
				if (index < node->getSize()) {
					entry = (*node)[index];
				}

			}
	};

}
