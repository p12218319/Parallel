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

#include "P12218319\core\Core.hpp"
#include "Nesting.hpp"
#include "Task.hpp"

namespace P12218319 { namespace Sections {
	namespace implementation {
		typedef P12218319::Tasks::implementation::Task Task;
		P12218319_EXPORT_API bool P12218319_CALL Section(Task&) throw();
	}

	P12218319_EXPORT_API void P12218319_CALL Begin(const uint32_t aThreadCount = P12218319_DEFAULT_THREAD_COUNT) throw();
	P12218319_EXPORT_API bool P12218319_CALL End() throw();
	P12218319_EXPORT_API uint32_t P12218319_CALL GetThreadCount() throw();
	P12218319_EXPORT_API uint32_t P12218319_CALL GetThread() throw();

	template<class FUNCTION_TYPE, class... PARAMS>
	inline P12218319_EXPORT_API bool P12218319_CALL Section(const FUNCTION_TYPE aFunction, PARAMS... aParams) throw() {
		return implementation::Section(*P12218319::Tasks::implementation::CaptureTask<>(aFunction, aParams...));
	}
}}
#endif