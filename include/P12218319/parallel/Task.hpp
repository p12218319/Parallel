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
#ifndef P12218319_PARALLEL_TASK_HPP
#define P12218319_PARALLEL_TASK_HPP

#include "P12218319\core\Core.hpp"

namespace P12218319 { namespace parallel{

	namespace implementation {

		class P12218319_EXPORT_API Task {
		public:
			virtual P12218319_CALL ~Task();
			virtual void P12218319_CALL operator()() const throw() = 0;
		};

		template<class FUNCTION_TYPE, class... PARAMS>
		Task* P12218319_CALL CaptureTask(const FUNCTION_TYPE aFunction, PARAMS... aParams) throw() {
			// Capture varadic params in a lambda
			const auto l = [=]()->void {
				aFunction(aParams...);
			};

			typedef decltype(l) LambdaType;

			// Interface lambda with Task call
			class LambdaTask : public Task {
			private:
				const LambdaType mFunction;
			public:
				P12218319_CALL LambdaTask(const LambdaType aFunction) :
					mFunction(aFunction)
				{}

				// Inherited from Task
				void P12218319_CALL operator()() const  throw() override {
					mFunction();
				}
			};

			return new LambdaTask(l);
		}

		template<class FUNCTION_TYPE, class... PARAMS>
		class CaptureTaskLocal : public Task {
		private:
			//! \todo Use lambda directly
			enum {MAX_TASK_SIZE = 128};
			uint8_t mTask[MAX_TASK_SIZE];
		public:
			P12218319_CALL CaptureTaskLocal(const FUNCTION_TYPE aFunction, PARAMS... aParams) {
				const auto l = [=]()->void {
					aFunction(aParams...);
				};

				typedef decltype(l) LambdaType;

				// Interface lambda with Task call
				class LambdaTask : public Task {
				private:
					const LambdaType mFunction;
				public:
					P12218319_CALL LambdaTask(const LambdaType aFunction) :
						mFunction(aFunction)
					{}

					// Inherited from Task
					void P12218319_CALL operator()() const  throw() override {
						mFunction();
					}
				};

				static_assert(sizeof(LambdaTask) <= MAX_TASK_SIZE, "P12218319::parallel::implementation::CaptureTaskLocal : lambda size is too large");
				new(mTask) LambdaTask(l);
			}

			~CaptureTaskLocal() {
				//reinterpret_cast<Task*>(mTask)->~Task();
			}

			// Inherited from Task
			void P12218319_CALL operator()() const  throw() override {
				reinterpret_cast<const Task*>(mTask)->operator()();
			}
		};

		P12218319_EXPORT_API bool P12218319_CALL TaskSchedule(Task&) throw();
	}

	template<class FUNCTION_TYPE, class... PARAMS>
	inline bool P12218319_CALL TaskSchedule(const FUNCTION_TYPE aFunction, PARAMS... aParams) throw() {
		return implementation::TaskSchedule(*implementation::CaptureTask<>(aFunction, aParams...));
	}

	P12218319_EXPORT_API uint32_t P12218319_CALL TaskWait() throw();


}}
#endif