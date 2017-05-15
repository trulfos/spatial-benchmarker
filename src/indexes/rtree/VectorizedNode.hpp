#pragma once
#include "BaseNode.hpp"
#include "spatial/Coordinate.hpp"
#include "immintrin.h"
#include <bitset>

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
	class VectorizedNode : public BaseNode<VectorizedNode<D, C, P>, C, P>
	{
		using Base = BaseNode<VectorizedNode<D, C, P>, C, P>;
		using Coordinate = Spatial::Coordinate;


		static_assert(
				std::is_same<Coordinate, double>::value,
				"Vector node assumes doubles"
			);

		static constexpr unsigned BLOCK_SIZE = 4;
		static constexpr unsigned N_BLOCKS = (C + BLOCK_SIZE - 1) / BLOCK_SIZE;


		public:
			using Mbr = ::Rtree::Mbr<D>;
			using Link = ::Rtree::Link<VectorizedNode>;
			using Plugin = P;

			// Inherit constructor and operator=
			using Base::Base;
			using Base::operator=;

			// Depends on template parameters
			using Base::getSize;


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
						if (index < node->getSize()) {
							bitset = scanBlock();
							findNext();
						}
					};

					ScanIterator& operator++()
					{
						assert(index < node->getSize());

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
						assert(index < node->getSize());
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
						while (!(bitset & 1) && index < node->getSize()) {
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
			 * Scan node and return set of matching entries.
			 */
			std::pair<ScanIterator, ScanIterator> scan(const Mbr& mbr) const
			{
				return std::make_pair(
						ScanIterator(this, mbr, 0),
						ScanIterator(this, mbr, getSize())
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
			 * Override delete operator to use correct delete method.
			 */
			void operator delete (void * pointer)
			{
				free(pointer);
			}
			


			/**
			 * Get the plugin of an entry in this node.
			 *
			 * @param index Index of entry
			 * @return Plugin of entry at the given index
			 */
			Plugin getPlugin(unsigned index) const
			{
				return plugins[index];
			}


			/**
			 * Get the link of an entry in this node.
			 *
			 * @param index Index of entry
			 * @return Link of entry at the given index
			 */
			Link getLink(unsigned index) const
			{
				return links[index];
			}


			/**
			 * Get the MBR of an entry in this node.
			 *
			 * @param index Index of entry
			 * @return MBR of entry at the given index
			 */
			Mbr getMbr(unsigned index) const
			{
				std::array<Coordinate, D> bottom, top;

				auto base = reinterpret_cast<const double *>(
						coordinates + 2 * D * (index / BLOCK_SIZE)
					) + index % BLOCK_SIZE;

				for (unsigned d = 0; d < D; ++d) {
					bottom[d] = base[0];
					top[d] = base[BLOCK_SIZE];
					base += 2 * BLOCK_SIZE;
				}

				return Mbr(top, bottom);
			}


			/**
			 * Set the MBR of an entry in this node.
			 *
			 * @param index Index of entry for which to set MBR
			 */
			void setMbr(unsigned index, const Mbr& m)
			{
				auto top = m.getTop();
				auto bottom = m.getBottom();

				auto base = reinterpret_cast<double *>(
						coordinates + 2 * D * (index / BLOCK_SIZE)
					) + index % BLOCK_SIZE;

				for (unsigned d = 0; d < D; ++d) {
					base[0] = bottom[d];
					base[BLOCK_SIZE] = top[d];
					base += 2 * BLOCK_SIZE;
				}
			}


			/**
			 * Set the link of an entry in this node.
			 *
			 * @param index Index of entry for which to set link
			 */
			void setLink(unsigned index, const Link& l)
			{
				links[index] = l;
			}


			/**
			 * Set the plugin of an entry in this node.
			 *
			 * @param index Index of entry for which to set plugin
			 */
			void setPlugin(unsigned index, const Plugin& p)
			{
				plugins[index] = p;
			}


		private:
			__m256d coordinates[N_BLOCKS * 2 * D];
			Link links[C];
			Plugin plugins[C];
	};

}
