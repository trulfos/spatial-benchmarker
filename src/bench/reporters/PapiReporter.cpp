#include "PapiReporter.hpp"
#include "ProgressLogger.hpp"
#include <random>
#include <algorithm>
#include <papi.h>

namespace Bench
{

	/**
	 * Check the PAPI status code given and throw an appropriate exception if
	 * the code is an error.
	 *
	 * @param code Code to check
	 */
	void check(int code)
	{
		if (code != PAPI_OK) {
			throw std::runtime_error(
					std::string("PAPI error! ") + PAPI_strerror(code)
				);
		}
	}


	void PapiReporter::run(
			const SpatialIndex& index,
			std::ostream& logStream
		)
	{
		ProgressLogger progress(logStream, runs * REORDER_RUNS);
		std::default_random_engine engine (11);

		// Set up performance counters (move to constructor?)
		std::array<int, 4> events = {
				PAPI_TOT_INS,
				PAPI_L3_TCM,
				PAPI_RES_STL,
				PAPI_TLB_DM
			};

		int nHardwareCounters = PAPI_num_counters();
		if (nHardwareCounters < int(events.size())) {
			throw std::runtime_error("Too few hardware counters");
		}

		// Load queries into memory
		auto querySet = getQuerySet();

		std::vector<RangeQuery> originalQueries (querySet.getSize());
		std::copy(
				querySet.begin(), querySet.end(),
				originalQueries.begin()
			);

		// Make space for results
		for (unsigned j = 0; j < runs; j++) {

			// Make a copy that can be reshuffeled
			std::vector<RangeQuery> queries = originalQueries;

			std::array<int long long, events.size()> totals = {};
			int long long runtime = 0;
			int long long virtRuntime = 0;

			// Test multiple times
			for (unsigned i = 0; i < REORDER_RUNS; ++i) {

				// Clear cache
				clearCache();

				Results r;
				r.reserve(MIN_RESULT_SIZE);

				std::array<int long long, events.size()> results = {};

				// Start measurements
				check(PAPI_start_counters(events.data(), events.size()));
				int long long startTime = PAPI_get_real_nsec();
				int long long virtStartTime = PAPI_get_virt_nsec();

				// Run the code to 
				for (const RangeQuery& query : queries) {
					r.clear();
					index.search(r, query);
				}

				// Stop measurements
				int long long endTime = PAPI_get_real_nsec();
				int long long virtEndTime = PAPI_get_virt_nsec();
				check(PAPI_stop_counters(results.data(), results.size()));

				for (unsigned k = 0; k < results.size(); k++) {
					totals[k] += results[k];
				}


				runtime += endTime - startTime;
				virtRuntime += virtEndTime - virtStartTime;

				// Rearrange queries
				std::shuffle(
						queries.begin(), queries.end(),
						engine
					);

				// Update progress bar
				progress.increment();
			}

			// Store metrics
			for (unsigned i = 0; i < events.size(); ++i) {
				char name[PAPI_MAX_STR_LEN];
				if (PAPI_event_code_to_name(events[i], name) != PAPI_OK) {
					throw std::runtime_error("Could not translate event name");
				}

				addEntry(std::string(name), totals[i] / REORDER_RUNS);
			}

			addEntry("PAPI_REAL_NSEC", runtime / REORDER_RUNS);
			addEntry("PAPI_VIRT_NSEC", virtRuntime / REORDER_RUNS);

		}

	}

}
