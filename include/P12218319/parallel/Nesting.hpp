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
#ifndef P12218319_PARALLEL_NESTING_HPP
#define P12218319_PARALLEL_NESTING_HPP

#include "P12218319\core\Core.hpp"

#ifndef P12218319_DEFAULT_THREAD_COUNT
	#define P12218319_DEFAULT_THREAD_COUNT 4
#endif

namespace P12218319 {
	namespace implementation {
		P12218319_EXPORT_API uint32_t P12218319_CALL IncrementParallelDepth() throw();
		P12218319_EXPORT_API uint32_t P12218319_CALL DecrementParallelDepth() throw();
		P12218319_EXPORT_API uint32_t P12218319_CALL CheckParallelDepth() throw();
	}

	P12218319_EXPORT_API void P12218319_CALL EnabledNestedParallelism() throw();
	P12218319_EXPORT_API void P12218319_CALL DisableNestedParallelism() throw();
	P12218319_EXPORT_API bool P12218319_CALL IsNestedParallelismEnabled() throw();
}
#endif