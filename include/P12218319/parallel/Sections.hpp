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
#ifndef P12218319_PARALLEL_SECTIONS_HPP
#define P12218319_PARALLEL_SECTIONS_HPP

#include <thread>
#include "P12218319\core\Core.hpp"
#include "Nesting.hpp"
#include "Task.hpp"

namespace P12218319 { namespace parallel{

	/*!
		\brief Execute any number of functions on a fixed number of threads.
		\detail 
		Example
		\code
		{
			// Create a 4 thread section group
			Sections<4> group;

			// Add sections to the group
			group.AddSection<>(MyLongFunction, 1, 2, 3);	// This section will execute on thread 0
			group.AddSection<>(MyLongFunction, 4, 5, 6);	// This section will execute on thread 1
			group.AddSection<>(MyOtherFunction);			// This section will execute on thread 2
			group.AddSection<>(MyOtherFunction);			// This section will execute on thread 3
			group.AddSection<>(MyLongFunction, 7, 8, 9);	// This section will execute on thread 0
			group.AddSection<>(MyOtherFunction);			// This section will execute on thread 1

			// Sections will execute here
		}
		\endcode
		\tparam THREAD_COUNT The number of threads to spawn.
		\author Adam Smith
		\version 1.0
		\data 26th Feburary 2016
	*/
	template<const uint32_t THREAD_COUNT = P12218319_DEFAULT_THREAD_COUNT>
	class P12218319_EXPORT_API Sections {
	public:
		enum {
			MAX_SECTIONS = 16	//!< The maximum number of sections that can be added.
		};
	private:
		implementation::Task* mSections[THREAD_COUNT][MAX_SECTIONS];
		uint8_t mSectionCount[THREAD_COUNT];
		uint8_t mCurrentThread;
	public:
		/*!
			\brief Create an empty section group
		*/
		Sections() :
			mCurrentThread(0)
		{
			for (uint32_t i = 0; i < THREAD_COUNT; ++i) mSectionCount[i] = 0;
		}

		/*!
			\brief Sections are executed here.
		*/
		P12218319_CALL ~Sections() {
			// Increment the parallel depth
			const uint32_t depth = implementation::IncrementParallelDepth();

			// Execute the sections in parallel
			if(depth == 1 || IsNestedParallelismEnabled()) {

				// Function that runs on the thread to execute the section(s) for that thread
				const auto threadFn = [](const uint32_t aSectionCount, implementation::Task** aSections)->void {
					for(uint32_t i = 0; i < aSectionCount; ++i) aSections[i]->operator()();
				};

				// Create and launch threads
				std::thread threads[THREAD_COUNT];
				for(uint32_t i = 0; i < THREAD_COUNT; ++i) threads[i] = std::thread(threadFn, mSectionCount[i], mSections[i]);

				// Wait for threads and delete sections
				for(uint32_t i = 0; i < THREAD_COUNT; ++i) {
					threads[i].join();
					for(uint32_t j = 0; j < mSectionCount[i]; ++j) delete mSections[i][j];
				}
			// Execute the sections in sequence
			}else {
				for (uint32_t i = 0; i < THREAD_COUNT; ++i) {
					for (uint32_t j = 0; j < mSectionCount[i]; ++j) {
						mSections[i][j]->operator()();
						delete mSections[i][j];
					}
				}
			}
			implementation::DecrementParallelDepth();
		}

		/*!
			\brief Add a section to be executed in parallel.
			\tparam FUNCTION_TYPE The type of functor to execute.
			\tparam PARAMS The parameter types to pass to the functor.
			\param aFunction The functor that will be executed as a section.
			\param aParams The params that will be passed to the functor.
			\return Returns false if the section could not be added.
		*/
		template<class FUNCTION_TYPE, class... PARAMS>
		bool P12218319_CALL AddSection(const FUNCTION_TYPE aFunction, PARAMS... aParams) throw() {
			// Overflow thread index
			if(mCurrentThread == THREAD_COUNT) mCurrentThread = 0;

			// Check if a section can be added to the current thread
			if(mSectionCount[mCurrentThread] == MAX_SECTIONS) return false;

			// Add the section to the current thread and increment the thread index
			mSections[mCurrentThread][mSectionCount[mCurrentThread]++] = implementation::CaptureTask<>(aFunction, aParams...);
			++mCurrentThread;
			return true;
		}
	};
}}
#endif