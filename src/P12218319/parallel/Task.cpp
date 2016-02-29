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
#include <map>
#include "P12218319\parallel\Nesting.hpp"
#include "P12218319\parallel\Task.hpp"

namespace P12218319 { namespace Tasks {
	enum {
		WORKER_THREAD_COUNT = P12218319_DEFAULT_THREAD_COUNT
	};

	struct TaskData {
		uint64_t ID;
		implementation::Task* Task;
		std::thread::id SourceThread;

		TaskData()  throw() :
			ID(0),
			Task(nullptr),
			SourceThread(std::this_thread::get_id())
		{}

		TaskData(const uint64_t aID, implementation::Task& aTask, const std::thread::id aSourceThread)  throw() :
			ID(aID),
			Task(&aTask),
			SourceThread(aSourceThread)
		{}

		inline operator bool() const throw() {
			return Task ? true : false;
		}
	};

	std::condition_variable TASK_ADDED_CONDITION;
	std::condition_variable TASK_COMPLETED_CONDITION;
	std::vector<TaskData> TASK_LIST;
	thread_local std::vector<std::vector<TaskID>> TASK_GROUPS;
	std::mutex TASK_LIST_LOCK;
	std::thread WORKER_THREADS[WORKER_THREAD_COUNT];
	TaskData WORKER_TASKS[WORKER_THREAD_COUNT];
	std::atomic_bool EXIT_FLAG = false;
	uint64_t ID_BASE = 0;

	void P12218319_CALL TaskWorker(const uint32_t aID) {
		TaskData& taskData = WORKER_TASKS[aID];
		taskData.Task = nullptr;
		while(! EXIT_FLAG) {
			{
				std::unique_lock<std::mutex> lock(TASK_LIST_LOCK);
				TASK_ADDED_CONDITION.wait_for(lock, std::chrono::milliseconds(5));
				if(! TASK_LIST.empty()) {
					taskData = TASK_LIST.back();
					TASK_LIST.pop_back();
				}
			}

			if(taskData) {
				taskData.Task->operator()();
				delete taskData.Task;
				taskData.Task = nullptr;
				TASK_COMPLETED_CONDITION.notify_all();
			}

		}
	}

	void LaunchWorkers() throw() {
		static bool THREADS_LAUNCHED = false;
		if(! THREADS_LAUNCHED) {
			THREADS_LAUNCHED = true;
			P12218319::implementation::IncrementParallelDepth();
			std::atexit([](){
				EXIT_FLAG = true;
				for(uint32_t i = 0; i < WORKER_THREAD_COUNT; ++i) WORKER_THREADS[i].join();
				P12218319::implementation::DecrementParallelDepth();
			});
			for(uint32_t i = 0; i < WORKER_THREAD_COUNT; ++i) {
				WORKER_THREADS[i] = std::thread(TaskWorker, i);
			}
		}
	}

	namespace implementation {
		TaskID P12218319_CALL Schedule(Task& aTask) throw() {
			LaunchWorkers();
			std::vector<TaskID>* group = TASK_GROUPS.empty() ? nullptr : &TASK_GROUPS.back();
			TASK_LIST_LOCK.lock();
			const TaskID id = ++ID_BASE;
			TASK_LIST.push_back(TaskData(id, aTask, std::this_thread::get_id()));
			TASK_LIST_LOCK.unlock();
			TASK_ADDED_CONDITION.notify_one();
			if(group) group->push_back(id);
			return id;
		}

		// Task

		P12218319_CALL Task::~Task() {

		}
	}

	template<class F>
	void P12218319_CALL WaitImplementation(const F aCheckList) throw() {
		bool wait = true;
		{
			std::lock_guard<std::mutex> lock(TASK_LIST_LOCK);
			wait = ! aCheckList();
		}
		while(wait) {
			{
				std::unique_lock<std::mutex> lock(TASK_LIST_LOCK);
				TASK_COMPLETED_CONDITION.wait(lock);
				wait = ! aCheckList();
			}
		}
	}
	
	void P12218319_CALL Wait(const TaskID* const aBegin, const TaskID* const aEnd) throw() {
		WaitImplementation<>([=]()->bool {
			for(const TaskData& i : TASK_LIST) for(const TaskID* j = aBegin; j != aEnd; ++j)	if (i && i.ID == *j) return false;
			for(const TaskData& i : WORKER_TASKS) for(const TaskID* j = aBegin; j != aEnd; ++j)	if (i && i.ID == *j) return false;
			return true;
		});
	}

	void P12218319_CALL Wait(const TaskID aID) throw() {
		WaitImplementation<>([=]()->bool {
			for(const TaskData& i : TASK_LIST)		if(i && i.ID == aID) return false;
			for(const TaskData& i : WORKER_TASKS)	if(i && i.ID == aID) return false;
			return true;
		});
	}

	void P12218319_CALL WaitThread() throw() {
		const std::thread::id id = std::this_thread::get_id();
		WaitImplementation<>([=]()->bool {
			for(const TaskData& i : TASK_LIST)		if(i && i.SourceThread == id) return false;
			for(const TaskData& i : WORKER_TASKS)	if(i && i.SourceThread == id) return false;
			return true;
		});
	}

	void P12218319_CALL WaitGlobal() throw() {
		WaitImplementation<>([]()->bool{
			for(const TaskData& i : TASK_LIST)		if(i) return false;
			for(const TaskData& i : WORKER_TASKS)	if(i) return false;
			return true;
		});
	}

	void P12218319_CALL BeginGroup() throw() {
		TASK_GROUPS.push_back(std::vector<TaskID>());
	}

	bool P12218319_CALL EndGroup() throw() {
		if(TASK_GROUPS.empty()) return false;
		const std::vector<TaskID>& group = TASK_GROUPS.back();
		Wait(&group[0], &group[0] + group.size());
		TASK_GROUPS.pop_back();
		return true;
	}

}}