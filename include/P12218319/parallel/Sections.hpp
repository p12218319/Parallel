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


#include <vector>
#include <thread>
#include "P12218319\core\Core.hpp"
#include "Task.hpp"

namespace P12218319 {

	class P12218319_EXPORT_API Sections {
	private:
		std::vector<P12218319::Tasks::implementation::Task*> mTasks;
		std::thread* const mWorkerThreads;
		const std::thread::id mMasterThread;
		const uint32_t mThreadCount;
	private:
		P12218319_CALL Sections(Sections&&) throw() = delete;
		P12218319_CALL Sections(const Sections&) throw() = delete;
		Sections& P12218319_CALL operator=(Sections&&) throw() = delete;
		Sections& P12218319_CALL operator=(const Sections&) throw() = delete;

		bool P12218319_CALL Section_(P12218319::Tasks::implementation::Task&) throw();
	public:
		P12218319_CALL Sections(const uint32_t) throw();
		P12218319_CALL ~Sections() throw();

		uint32_t P12218319_CALL GetThreadCount() const throw();
		uint32_t P12218319_CALL GetThread() const throw();

		template<class FUNCTION_TYPE, class... PARAMS>
		inline bool P12218319_CALL Section(const FUNCTION_TYPE aFunction, PARAMS... aParams) throw() {
			return Section_(*P12218319::Tasks::implementation::CaptureTask<>(aFunction, aParams...));
		}
	};
}
#endif