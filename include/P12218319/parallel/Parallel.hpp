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
#ifndef P12218319_PARALLEL_PARALLEL_HPP
#define P12218319_PARALLEL_PARALLEL_HPP

#include <thread>
#include "P12218319\core\Core.hpp"
#include "Nesting.hpp"

namespace P12218319 { namespace parallel {

	template<const uint32_t THREAD_COUNT = 4, class FUNCTION_TYPE = void, class... PARAMS>
	static bool P12218319_CALL Parallel(const FUNCTION_TYPE aFunction, PARAMS... aParams) throw() {
		const uint32_t depth = ++implementation::THREAD_DEPTH;
		if(depth == 1 || implementation::ALLOW_NESTED) {
			std::thread threads[THREAD_COUNT];
			for(uint32_t i = 0; i < THREAD_COUNT; ++i) threads[i] = std::thread(aFunction, aParams...);
			for(uint32_t i = 0; i < THREAD_COUNT; ++i) threads[i].join();
		}else {
			for(uint32_t i = 0; i < THREAD_COUNT; ++i) aFunction(aParams...);
		}
		--implementation::THREAD_DEPTH;
		return true;
	}

}}
#endif