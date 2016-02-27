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
#include <vector>
#include <atomic>
#include "P12218319\parallel\Nesting.hpp"
#include "P12218319\parallel\Task.hpp"

namespace P12218319 { namespace parallel {
	enum {
		WORKER_THREAD_COUNT = P12218319_DEFAULT_THREAD_COUNT
	};

	std::condition_variable TASK_ADDED_CONDITION;
	std::condition_variable TASK_COMPLETED_CONDITION;
	std::vector<implementation::Task*> TASK_LIST;
	std::mutex TASK_LIST_LOCK;
	std::thread WORKER_THREADS[WORKER_THREAD_COUNT];
	std::atomic_uint32_t WORKER_FUNCTIONS = 0;
	std::atomic_bool THREADS_LAUNCHED = false;
	std::atomic_bool EXIT_FLAG = false;

	void P12218319_CALL TaskWorker() {
		implementation::Task* task = nullptr;
		while(! EXIT_FLAG) {
			{
				std::unique_lock<std::mutex> lock(TASK_LIST_LOCK);
				TASK_ADDED_CONDITION.wait_for(lock, std::chrono::milliseconds(5));
				if(TASK_LIST.empty()) {
					task = nullptr;
				}else {
					task = TASK_LIST.back();
					TASK_LIST.pop_back();
					++WORKER_FUNCTIONS;
				}
			}

			if(task) {
				task->operator()();
				delete task;
				--WORKER_FUNCTIONS;
				TASK_COMPLETED_CONDITION.notify_all();
			}

		}
	}

	namespace implementation {
		bool P12218319_CALL TaskSchedule(Task& aTask) throw() {
			if(! THREADS_LAUNCHED) {
				THREADS_LAUNCHED = true;
				std::atexit([](){
					EXIT_FLAG = true;
					for (uint32_t i = 0; i < WORKER_THREAD_COUNT; ++i) WORKER_THREADS[i].join();
				});
				for(uint32_t i = 0; i < WORKER_THREAD_COUNT; ++i) {
					WORKER_THREADS[i] = std::thread(TaskWorker);
				}
			}
			TASK_LIST_LOCK.lock();
			TASK_LIST.push_back(&aTask);
			TASK_LIST_LOCK.unlock();
			TASK_COMPLETED_CONDITION.notify_one();
			return true;
		}

		// Task

		P12218319_CALL Task::~Task() {

		}
	}

	uint32_t P12218319_CALL TaskWait() throw() {
		uint32_t count = 0;
	checkList:
		{
			std::lock_guard<std::mutex> lock(TASK_LIST_LOCK);
			if(TASK_LIST.size() + WORKER_FUNCTIONS == 0) return count;
		}
		{
			std::unique_lock<std::mutex> lock(TASK_LIST_LOCK);
			TASK_COMPLETED_CONDITION.wait(lock);
			++count;
		}
		goto checkList;
	}

}}