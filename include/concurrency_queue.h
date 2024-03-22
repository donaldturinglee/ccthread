#ifndef CONCURRENCY_QUEUE_H
#define CONCURRENCY_QUEUE_H

#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <queue>

namespace ccthread {

template<typename T>
class ConcurrencyQueue {
public:
	ConcurrencyQueue() = default;
	ConcurrencyQueue(ConcurrencyQueue&& source) = default;
	ConcurrencyQueue& operator=(ConcurrencyQueue&& source) = default;
	ConcurrencyQueue(const ConcurrencyQueue& source) = delete;
	ConcurrencyQueue& operator=(const ConcurrencyQueue& source) = delete;
	
	inline void push(const T& element) noexcept {
		const std::lock_guard<std::mutex> lock{mutex_};
		const bool is_empty{queue_.empty()};
		queue_.push(element);
		if(is_empty) {
			cond_var_.notify_all();
		}
	}

	inline const T& front(bool wait_on_front = true) const {
		std::unique_lock<std::mutex> lock{mutex_};
		if(queue_.empty()) {
			if(wait_on_front) {
				while(queue_.empty()) {
					cond_var_.wait(lock);
				}
			}
		}
		return queue_.front();
	}

	inline std::optional<T> pop_front(bool wait_on_front = true) noexcept {
		std::optional<T> dequeued_value;
		std::unique_lock<std::mutex> lock{mutex_};
		if(queue_.empty() && wait_on_front) {
			while(queue_.empty()) {
				cond_var_.wait(lock);
			}
		}
		if(!queue_.empty()) {
			dequeued_value = queue_.front();
			queue_.pop();
		}
		return dequeued_value;
	}

	inline T& front(bool wait_on_front = true) {
		std::unique_lock<std::mutex> lock{mutex_};
		if(queue_.empty()) {
			if(wait_on_front) {
				while(queue_.empty()) {
					cond_var_.wait(lock);
				}
			}
		}
		return queue_.front();
	}

	void pop() noexcept {
		const std::lock_guard<std::mutex> lock{mutex_};
		queue_.pop();
	}
	bool empty() const noexcept {
		const std::lock_guard<std::mutex> lock{mutex_};
		return queue_.empty();
	}
	std::size_t size() const noexcept {
		const std::lock_guard<std::mutex> lock{mutex_};
		return queue_.size();
	}
private:
	std::queue<T, std::deque<T>> queue_;
	mutable std::mutex mutex_;
	mutable std::condition_variable cond_var_;
};

} // namespace ccthread

#endif // CONCURRENCY_QUEUE_H
