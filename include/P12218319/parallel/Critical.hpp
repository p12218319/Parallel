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
#ifndef P12218319_PARALLEL_CRITICAL_HPP
#define P12218319_PARALLEL_CRITICAL_HPP

#include "Task.hpp"

namespace P12218319 {

	typedef uint64_t CriticalLabel;

	namespace implementation {
		typedef P12218319::Tasks::implementation::Task Task;
		P12218319_EXPORT_API void P12218319_CALL Critical(P12218319::Tasks::implementation::Task&) throw();
		P12218319_EXPORT_API void P12218319_CALL Critical(const CriticalLabel, Task&) throw();
	}

	template<class FUNCTION_TYPE, class... PARAMS>
	inline void P12218319_CALL Critical(const FUNCTION_TYPE aFunction, PARAMS... aParams) throw() {
		implementation::Critical(implementation::CaptureTaskLocal<FUNCTION_TYPE, PARAMS...>(aFunction, aParams...));
	}

	template<class FUNCTION_TYPE, class... PARAMS>
	inline void P12218319_CALL Critical(const CriticalLabel aLabel, const FUNCTION_TYPE aFunction, PARAMS... aParams) throw() {
		implementation::Critical(aLabel, implementation::CaptureTaskLocal<FUNCTION_TYPE, PARAMS...>(aFunction, aParams...));
	}

}
#endif