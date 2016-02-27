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
#ifndef P12218319_PARALLEL_FOR_HPP
#define P12218319_PARALLEL_FOR_HPP

#include "Sections.hpp"

namespace P12218319 { namespace parallel{

	template<const uint32_t THREAD_COUNT = P12218319_DEFAULT_THREAD_COUNT, class INDEX_TYPE = uint32_t, class FUNCTION_TYPE = void>
	void For(const INDEX_TYPE aBegin, const INDEX_TYPE aEnd, const FUNCTION_TYPE aBody) {
		const uint32_t depth = implementation::IncrementParallelDepth();
		if(THREAD_COUNT > 1 && (depth == 1 || IsNestedParallelismEnabled())) {
			const uint32_t range = aEnd - aBegin;
			const uint32_t iterationsPerthread = range / THREAD_COUNT;

			const auto Loop = [](const FUNCTION_TYPE aBody, const INDEX_TYPE aBegin, const INDEX_TYPE aEnd) {
				for(INDEX_TYPE i = aBegin; i != aEnd; ++i) aBody(i);
			};

			enum {NEW_THREAD_COUNT = THREAD_COUNT - 1};

			std::thread threads[NEW_THREAD_COUNT];
			INDEX_TYPE begin = iterationsPerthread;
			INDEX_TYPE end = begin + iterationsPerthread;
			for(uint32_t i = 0; i < NEW_THREAD_COUNT; ++i) {
				threads[i] = std::thread(Loop, aBody, begin, i + 1 == NEW_THREAD_COUNT ? aEnd : end);
				begin = end;
				end += iterationsPerthread;
			}

			for(INDEX_TYPE i = aBegin; i != aBegin + iterationsPerthread; ++i) aBody(i);

			for(std::thread& i : threads) i.join();
		}else {
			for(INDEX_TYPE i = aBegin; i != aEnd; ++i) aBody(i);
		}
		implementation::DecrementParallelDepth();
	}

}}
#endif