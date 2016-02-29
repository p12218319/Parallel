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

#include "P12218319\parallel\Sections.hpp"
#include "P12218319\parallel\Nesting.hpp"

namespace P12218319 {

	// Sections

	P12218319_CALL Sections::Sections(const uint32_t aThreadCount) throw() :
		mTasks(),
		mWorkerThreads(new std::thread[aThreadCount - 1]),
		mMasterThread(std::this_thread::get_id()),
		mThreadCount(aThreadCount)
	{}

	P12218319_CALL Sections::~Sections() throw() {
		typedef P12218319::Tasks::implementation::Task Task;

		// Increment the parallel depth
		const uint32_t depth = P12218319::implementation::IncrementParallelDepth();
		
		// Execute the sections in parallel
		if(mThreadCount > 1 && (depth == 1 || IsNestedParallelismEnabled())) {
			// Distribute tasks
			const uint32_t taskCount = mTasks.size();
			const uint32_t maxDistributions = (taskCount / mThreadCount) + 1;
			Task** const distributions = new Task*[mThreadCount * maxDistributions];
			uint16_t* const distributionCounts = new uint16_t[mThreadCount];
			for(uint32_t i = 0; i < mThreadCount; ++i) distributionCounts[i] = 0;
			{
				uint8_t currentThread = 0;
				for(uint32_t i = 0; i < taskCount; ++i) {
					distributions[(currentThread * maxDistributions) + distributionCounts[currentThread]++] = mTasks[i];
					++currentThread;
					if(currentThread == mThreadCount) currentThread = 0;
				}
			}
		
			// Create and launch threads
			const uint32_t newThreadCount = mThreadCount - 1;
			for(uint32_t i = 0; i < newThreadCount; ++i) {
				mWorkerThreads[i] = std::thread([=]()->void {
					const uint32_t i2 = i + 1;
					const uint32_t end = distributionCounts[i2];
					for(uint32_t j = 0; j < end; ++j) distributions[i2 * maxDistributions + j]->operator()();
				});
			}
		
			// Execute the first section block on this thread
			for(uint32_t i = 0; i < distributionCounts[0]; ++i) distributions[i]->operator()();
		
			// Wait for threads
			for(uint32_t i = 0; i < newThreadCount; ++i) mWorkerThreads[i].join();
					
		
			// Delete sections
			delete[] distributions;
			delete[] distributionCounts;
			for(Task* i : mTasks) delete i;
		// Execute the sections in sequence
		}else {
			for(Task* i : mTasks) {
				i->operator()();
				delete i;
			}
		}
		
		
		// Remove section
		P12218319::implementation::DecrementParallelDepth();
		delete[] mWorkerThreads;
	}

	uint32_t P12218319_CALL Sections::GetThreadCount() const throw() {
		return mThreadCount;
	}

	uint32_t P12218319_CALL Sections::GetThread() const throw() {
		const std::thread::id id = std::this_thread::get_id();
		if(id == mMasterThread) return 0;
		for(uint32_t i = 0; i < mThreadCount - 1; ++i) if(id == mWorkerThreads[i].get_id()) return i + 1;
		return UINT32_MAX;
	}

	bool P12218319_CALL Sections::Section_(P12218319::Tasks::implementation::Task& aTask) throw() {
		mTasks.push_back(&aTask);
		return true;
	}
}