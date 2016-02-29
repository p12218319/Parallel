/*
Copyright 2016 Adam Smith - P12218319

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

email : p12218319@myemail.dmu.ac.uk
*/

#include <iostream>
#include <vector>
#include <thread>
#include "P12218319\parallel\Sections.hpp"
#include "P12218319\parallel\Nesting.hpp"

namespace P12218319 { namespace Sections {


	struct Sections {
		std::vector<implementation::Task*> Tasks;
		std::vector<std::thread::id> ThreadIDs;
		uint32_t ThreadCount;

		Sections(const uint32_t aThreadCount) :
			Tasks(),
			ThreadCount(aThreadCount)
		{}
	};

	thread_local std::vector<Sections> SECTIONS;

	namespace implementation {
		bool P12218319_CALL Section(Task& aTask) throw() {
			if(SECTIONS.empty()) return false;
			SECTIONS.back().Tasks.push_back(&aTask);
			return true;
		}
	}

	void P12218319_CALL Begin(const uint32_t aThreadCount) throw() {
		SECTIONS.push_back(Sections(aThreadCount));
	}
	
	bool P12218319_CALL End() throw() {
		if(SECTIONS.empty()) return false;
		Sections& sections = SECTIONS.back();

		// Increment the parallel depth
		const uint32_t depth = P12218319::implementation::IncrementParallelDepth();

		// Execute the sections in parallel
		if(sections.ThreadCount > 1 && (depth == 1 || IsNestedParallelismEnabled())) {

			// Distribute tasks
			const uint32_t taskCount = sections.Tasks.size();
			const uint32_t maxDistributions = (taskCount / sections.ThreadCount) + 1;
			implementation::Task** const distributions = new implementation::Task*[sections.ThreadCount * maxDistributions];
			uint16_t* const distributionCounts = new uint16_t[sections.ThreadCount];
			for(uint32_t i = 0; i < sections.ThreadCount; ++i) distributionCounts[i] = 0;
			{
				uint8_t currentThread = 0;
				for(uint32_t i = 0; i < taskCount; ++i) {
					distributions[(currentThread * maxDistributions) + distributionCounts[currentThread]++] = sections.Tasks[i];
					++currentThread;
					if(currentThread == sections.ThreadCount) currentThread = 0;
				}
			}

			// Create and launch threads
			const uint32_t newThreadCount = sections.ThreadCount - 1;
			std::thread* const threads = new std::thread[newThreadCount]; 
			for(uint32_t i = 0; i < newThreadCount; ++i) {
				threads[i] = std::thread([=]()->void {
					const uint32_t i2 = i + 1;
					const uint32_t end = distributionCounts[i2];
					for(uint32_t j = 0; j < end; ++j) distributions[i2 * maxDistributions + j]->operator()();
				});
			}

			// Execute the first section block on this thread
			for(uint32_t i = 0; i < distributionCounts[0]; ++i) distributions[i]->operator()();

			// Wait for threads
			for(uint32_t i = 0; i < newThreadCount; ++i) threads[i].join();
			

			// Delete sections
			delete[] threads;
			delete[] distributions;
			delete[] distributionCounts;
			for(implementation::Task* i : sections.Tasks) delete i;
		// Execute the sections in sequence
		}else {
			for(implementation::Task* i : sections.Tasks) {
				i->operator()();
				delete i;
			}
		}

		P12218319::implementation::DecrementParallelDepth();
		SECTIONS.pop_back();
		return true;
	}

	uint32_t P12218319_CALL GetThreadCount() throw() {
		if(SECTIONS.empty()) return 0;
		const Sections& sections = SECTIONS.back();
		return sections.ThreadCount > 1 && (P12218319::implementation::CheckParallelDepth() == 1 || IsNestedParallelismEnabled()) ? sections.ThreadCount : 2;
	}

	uint32_t P12218319_CALL GetThread() throw() {
		if(SECTIONS.empty()) return 0;
		const Sections& sections = SECTIONS.back();
		const std::thread::id id = std::this_thread::get_id();
		const uint32_t threads = sections.ThreadIDs.size();
		for(uint32_t i = 0; i < threads; ++i) if(id == sections.ThreadIDs[i]) return i + 1;
		return 0;
	}
}}