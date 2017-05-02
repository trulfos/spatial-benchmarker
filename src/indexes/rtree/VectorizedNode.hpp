#pragma once
#include "Mbr.hpp"
#include "Entry.hpp"
#include "Link.hpp"
#include "ProxyIterator.hpp"
#include "spatial/Coordinate.hpp"
#include "immintrin.h"
#include <bitset>

using Spatial::Coordinate;

namespace Rtree
{

	/**
	 * Represents a vectorized node where an arrays of subfields approach has
	 * been combined with vectorized scans (and full node scans).
	 *
	 * @tparam D Dimension
	 * @tparam C Max capacity
	 * @tparam P Plugin
	 */
	template<unsigned D, unsigned C, class P = EntryPlugin>
	class VectorizedNode
	{
		public:
			// Type definitions
			using Mbr = ::Rtree::Mbr<D>;
			using Link = ::Rtree::Link<VectorizedNode>;
			using Plugin = P;
			using Entry = ::Rtree::Entry<VectorizedNode>;


			// Constants
			static constexpr unsigned capacity = C;

			/**
			 * Proxy entry for the default node class.
			 *
			 * @see Rtree::BaseEntry
			 */
			class ProxyEntry : public BaseEntry<VectorizedNode>
			{
				using Base = BaseEntry<VectorizedNode>;

				public:
					// Types
					using Mbr = typename Base::Mbr;
					using Link = typename Base::Link;
					using Plugin = typename Base::Plugin;

					// Allows default constructing the iterator
					ProxyEntry() = default;


					/**
					 * Create a proxy entry based on the node and the index of
					 * the entry within the node.
					 *
					 * @param node Node in which the entry resides
					 * @param index Index of entry in node
					 */
					ProxyEntry(VectorizedNode * node, unsigned index)
						: node(node), index(index)
					{
					}

					ProxyEntry& operator=(const ProxyEntry& other)
					{
						//TODO: Why doesn't the below template handle this?
						setMbr(other.getMbr());
						setLink(other.getLink());
						setPlugin(other.getPlugin());
						return *this;
					}

					template<class E>
					ProxyEntry& operator=(const E& other)
					{
						setMbr(other.getMbr());
						setLink(other.getLink());
						setPlugin(other.getPlugin());

						return *this;
					}

					const Mbr getMbr() const override
					{
						return node->getMbr(index);
					}


					const Link getLink() const override
					{
						return node->links[index];
					}


					const Plugin getPlugin() const override
					{
						return node->plugins[index];
					}

					/**
					 * Make sure proxy nodes are swappable
					 *
					 * This swaps what the proxies refers to and not the fields
					 * of the proxies themselves.
					 */
					friend void swap(ProxyEntry& a, ProxyEntry& b)
					{
						Entry tmp;
						tmp = a;
						a = b;
						b = tmp;
					}

				protected:
					void setMbr(const Mbr& m) override
					{
						auto top = m.getTop();
						auto bottom = m.getBottom();

						auto base = reinterpret_cast<double *>(
								node->coordinates + 2 * D * (index / BLOCK_SIZE)
							) + index % BLOCK_SIZE;

						for (unsigned d = 0; d < D; ++d) {
							base[0] = bottom[d];
							base[BLOCK_SIZE] = top[d];
							base += 2 * BLOCK_SIZE;
						}
					}

					void setPlugin(const Plugin& p) override
					{
						node->plugins[index] = p;
					}

					void setLink(const Link& l) override
					{
						node->links[index] = l;
					}

				private:
					VectorizedNode * node;
					unsigned index;

					friend ProxyIterator<VectorizedNode>;
					friend ConstProxyIterator<VectorizedNode>;
			};

			using iterator = ProxyIterator<VectorizedNode>;
			using const_iterator = ConstProxyIterator<VectorizedNode>;
			using reference = ProxyEntry;


			class ScanIterator
			{
				public:
					using iterator_category = std::input_iterator_tag;
					using value_type = Link;
					using difference_type = int;
					using pointer = const Link *;
					using reference = const Link&;

					ScanIterator() = default;

					ScanIterator(
							const VectorizedNode * node,
							const Mbr& mbr,
							unsigned index
						) : node(node), mbr(&mbr), index(index)
					{
						if (index < node->size) {
							bitset = scanBlock();
							findNext();
						}
					};

					ScanIterator& operator++()
					{
						assert(index < node->size);

						next();
						findNext();

						return *this;
					}

					ScanIterator operator++(int)
					{
						ScanIterator it = *this;
						operator++();
						return it;
					}

					reference operator*()
					{
						assert(index < node->size);
						return node->links[index];
					}

					pointer operator->()
					{
						return &operator*();
					}

					bool operator==(const ScanIterator& other) const
					{
						assert(other.node == node);
						return other.index == index;
					}

					bool operator!=(const ScanIterator& other) const
					{
						return !(*this == other);
					}

				private:
					const VectorizedNode * node;

					const Mbr * mbr;

					// Current index (as seen from the outside)
					unsigned index;

					// Entries intersecting query
					unsigned short bitset;


					/**
					 * Jump to the next position.
					 *
					 * This proceeds without any questions asked, and also keeps
					 * the internal bitset up-to-date.
					 */
					void next()
					{
						bitset >>= 1;
						++index;

						if (index % BLOCK_SIZE == 0) {
							bitset = scanBlock();
						}
					}


					/**
					 * Scans until the next matching entry is found
					 *
					 * Finds the next entry overlappning with the query
					 * rectangle. Also stops when the end of the node is
					 * reached.
					 */
					void findNext()
					{
						while (!(bitset & 1) && index < node->size) {
							next();
						}
					}


					/**
					 * Scan the current block and update bitset.
					 */
					unsigned short scanBlock() const
					{
						auto highs = mbr->getTop();
						auto lows = mbr->getBottom();
						unsigned block = index / BLOCK_SIZE;
						unsigned short bitset = (1 << BLOCK_SIZE) - 1;
						const double * base = reinterpret_cast<const double*>(
								node->coordinates + 2 * D * block
							);

						// Compare across all dimensions
						for (unsigned j = 0; j < D; ++j) {

							// Skip if we know the result
							if (!bitset) {
								break;
							}

							// Load bottom and top for query box
							__m256d bottom = _mm256_broadcast_sd(&lows[j]);
							__m256d top = _mm256_broadcast_sd(&highs[j]);

							// Load bottom and top for subject
							__m256d sbottom = _mm256_load_pd(base);
							__m256d stop = _mm256_load_pd(base + BLOCK_SIZE);

							bitset &= _mm256_movemask_pd(
									_mm256_cmp_pd(top, sbottom, _CMP_GE_OS)
								) & _mm256_movemask_pd(
									_mm256_cmp_pd(stop, bottom, _CMP_GE_OS)
								);

							base += 2 * BLOCK_SIZE;
						}

						return bitset;
					}
			};


			/**
			 * Construct an empty node.
			 */
			VectorizedNode() : size(0)
			{
			}


			/**
			 * Construct a node with a given set of entries.
			 *
			 * @tparam E Entry type
			 * @param entries Set of entries to include
			 */
			template<class E>
			VectorizedNode(const std::initializer_list<E>& entries)
			{
				assign(entries.begin(), entries.end());
			}


			/**
			 * Get a proxy entry to given index.
			 */
			ProxyEntry operator[](unsigned i)
			{
				assert(i < size);
				return ProxyEntry(this, i);
			}


			Entry operator[](unsigned i) const
			{
				assert(i < size);
				return Entry(
						getMbr(i),
						links[i],
						plugins[i]
					);
			}


			/**
			 * Add entry to end.
			 */
			void add(const Entry& entry)
			{
				assert(size < C);
				operator[](size++) = entry;
			}


			/**
			 * Replace the entries in this with given node.
			 *
			 * @param first Iterator to first element of range
			 * @param last Iterator to past-the-end of the range
			 */
			template<class InputIterator>
			void assign(InputIterator first, InputIterator last)
			{
				size = 0;
				add(first, last);
			}


			template<class InputIterator>
			void add(InputIterator first, InputIterator last)
			{
				while (first != last) {
					add(*first);
					++first;
				}
			}


			/**
			 * Assign from initializer list.
			 */
			template<class E>
			VectorizedNode& operator=(const std::initializer_list<E>& entries)
			{
				assign(entries.begin(), entries.end());

				return *this;
			}


			/**
			 * Get the number of entries in this node.
			 *
			 * @return Number of entries
			 */
			unsigned getSize() const
			{
				return size;
			}


			/**
			 * Check whether this node is full.
			 *
			 * @return True if full
			 */
			bool isFull() const
			{
				return size == C;
			}


			/**
			 * Begin iteration through all entries.
			 */
			iterator begin()
			{
				return iterator(this, 0);
			}


			/**
			 * Iterator to past-the-end.
			 */
			iterator end()
			{
				return iterator(this, size);
			}

			/**
			 * Begin iteration through constant node.
			 */
			const_iterator begin() const
			{
				return const_iterator(this, 0);
			}

			/**
			 * Past-the-end const iterator.
			 */
			const_iterator end() const
			{
				return const_iterator(this, size);
			}


			/**
			 * Scan node and return set of matching entries.
			 */
			std::pair<ScanIterator, ScanIterator> scan(const Mbr& mbr) const
			{
				return std::make_pair(
						ScanIterator(this, mbr, 0),
						ScanIterator(this, mbr, size)
					);
			}


			/**
			 * Override new operator to make sure memory is aligned.
			 */
			void * operator new(std::size_t count)
			{
				void * p;
				int error = posix_memalign(&p, sizeof(__m256d), count);

				// Convert C-style error to exception
				if (error) {
					throw std::bad_alloc();
				}

				//TODO: Remove
				if (reinterpret_cast<uintptr_t>(p) % sizeof(__m256d) != 0) {
					throw std::runtime_error("Unaligned memory :-(");
				}

				return p;
			}


			/**
			 * Override delete operator to provide the correct pointer.
			 */
			void operator delete (void * pointer)
			{
				free(pointer);
			}
			


		private:

			//TODO:
			//static_assert(Coordinate == double, "Vector node assumes doubles");

			// Number of lanes in each __m256d
			static constexpr unsigned BLOCK_SIZE = 4;
			static constexpr unsigned N_BLOCKS = (C + BLOCK_SIZE - 1) / BLOCK_SIZE;

			__m256d coordinates[N_BLOCKS * 2 * D];

			Link links[C];
			Plugin plugins[C];
			unsigned size;

			// Memory offset - requried to be able to delete this
			unsigned short offset;

			typename Plugin::NodeData data;
			friend Plugin;


			Mbr getMbr(unsigned index) const
			{
				Point top(D);
				Point bottom(D);

				auto base = reinterpret_cast<const double *>(
						coordinates + 2 * D * (index / BLOCK_SIZE)
					) + index % BLOCK_SIZE;

				for (unsigned d = 0; d < D; ++d) {
					bottom[d] = base[0];
					top[d] = base[BLOCK_SIZE];
					base += 2 * BLOCK_SIZE;
				}

				return Box(top, bottom);
			}
	};

}
