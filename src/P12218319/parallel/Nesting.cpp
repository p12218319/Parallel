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

#include <atomic>
#include "P12218319\parallel\Nesting.hpp"

namespace P12218319 { namespace parallel {

	std::atomic_uint32_t PARALLEL_DEPTH = 0;
	std::atomic_bool NESTED_PARALLELISM = false;

	namespace implementation {
		uint32_t P12218319_CALL IncrementParallelDepth() throw() {
			return ++PARALLEL_DEPTH;
		}

		uint32_t P12218319_CALL DecrementParallelDepth() throw() {
			return --PARALLEL_DEPTH;
		}

		uint32_t P12218319_CALL CheckParallelDepth() throw() {
			return PARALLEL_DEPTH;
		}

	}

	void P12218319_CALL EnabledNestedParallelism() throw() {
		NESTED_PARALLELISM = true;
	}

	void P12218319_CALL DisableNestedParallelism() throw() {
		NESTED_PARALLELISM = false;
	}

	bool P12218319_CALL IsNestedParallelismEnabled() throw() {
		return NESTED_PARALLELISM;
	}

}}