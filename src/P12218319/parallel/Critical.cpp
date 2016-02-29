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

#include <mutex>
#include <map>
#include "P12218319\parallel\Critical.hpp"

namespace P12218319 {

	enum {
		CRITICAL_COUNT = 1024
	};

	std::recursive_mutex CRITICAL_MAP[CRITICAL_COUNT];

	namespace implementation {
		void P12218319_CALL Critical(Task& aTask) throw() {
			std::lock_guard<std::recursive_mutex> lock(CRITICAL_MAP[0]);
			aTask();
		}

		void P12218319_CALL Critical(const CriticalLabel aLabel, Task& aTask) throw() {
			std::lock_guard<std::recursive_mutex> lock(CRITICAL_MAP[aLabel % CRITICAL_COUNT]);
			aTask();
		}
	}

}