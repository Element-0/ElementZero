#include <mutex>
#include <functional>
#include <queue>
#include <stdexcept>
#include <string>
#include <atomic>
#include <thread>
#include <condition_variable>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>

#include <Level/Level.h>
#include <Core/AppPlatform.h>

#include <scheduler.h>
#include <hook.h>
#include <base.h>

using namespace Mod::Scheduler;
using namespace boost::multi_index;
using namespace std::chrono;

GameTick::rep current_ticks = 0;

struct SimpleClock {
  using rep        = GameTick::rep;
  using period     = GameTick::period;
  using duration   = GameTick;
  using time_point = time_point<SimpleClock, GameTick>;

  static constexpr bool is_steady = true;
  static time_point now() noexcept { return time_point{GameTick{current_ticks}}; }
};

using TimePoint = SimpleClock::time_point;

struct OneShotEvent {
  Token token;
  Handler handler;
  TimePoint next;
};

template <typename Real = OneShotEvent>
using EventContainer = multi_index_container<
    Real, indexed_by<
              ordered_unique<tag<Token>, member<OneShotEvent, Token, &OneShotEvent::token>>,
              ordered_non_unique<tag<TimePoint>, member<OneShotEvent, TimePoint, &OneShotEvent::next>>>>;

struct IntervalEvent : OneShotEvent {
  GameTick interval;
};

template <typename T> EventContainer<T> container;

void ProcessOneShotEvents() {
  auto now = SimpleClock::now();
  auto &db = container<OneShotEvent>.get<TimePoint>();
  auto it  = db.begin();
  while (it != db.end()) {
    if (it->next > now) return;
    it->handler(it->token);
    it = db.erase(it);
  }
}

void ProcessIntervalEvents() {
  auto now = SimpleClock::now();
  auto &db = container<IntervalEvent>.get<TimePoint>();
  auto it  = db.begin();
  while (it != db.end()) {
    if (it->next > now) return;
    it->handler(it->token);
    auto temp = *it;
    db.erase(it);
    temp.next = now + temp.interval;
    db.emplace(temp);
    it = db.begin();
  }
}

TClasslessInstanceHook(void, "?tick@Level@@UEAAXXZ") {
  current_ticks++;

  ProcessOneShotEvents();
  ProcessIntervalEvents();

  original(this);
}

Token Mod::Scheduler::SetTimeOut(GameTick timeout, Handler fn) {
  static Token max = 0;
  auto current     = max++;
  container<OneShotEvent>.emplace(OneShotEvent{current, fn, (SimpleClock::now() + timeout)});
  return current;
}

Token Mod::Scheduler::SetInterval(GameTick interval, Handler fn) {
  static Token max = 0;
  auto current     = max++;
  container<IntervalEvent>.emplace(IntervalEvent{{current, fn, (SimpleClock::now() + interval)}, interval});
  return current;
}

void Mod::Scheduler::ClearTimeOut(Token token) {
  auto &db = container<OneShotEvent>.get<Token>();
  if (auto it = db.find(token); it != db.end()) db.erase(it);
}

void Mod::Scheduler::ClearInterval(Token token) {
  auto &db = container<IntervalEvent>.get<Token>();
  if (auto it = db.find(token); it != db.end()) db.erase(it);
}

static std::mutex mtx;
static std::condition_variable cv;
static std::atomic_bool exited;
static std::vector<std::function<void()>> tasks;
static std::thread worker{[] {
  while (true) {
    std::unique_lock lk{mtx};
    cv.wait(lk, [] { return exited || tasks.size(); });
    if (exited) return;
    for (auto &task : tasks) task();
    tasks.clear();
  }
}};

void OnExit() {
  exited = true;
  cv.notify_one();
  worker.join();
}

void Mod::Scheduler::RunInWorkerThread(std::function<void()> &&fn) {
  {
    std::lock_guard lg{mtx};
    tasks.emplace_back(std::move(fn));
  }
  cv.notify_one();
}

void Mod::Scheduler::RunInMainThread(std::function<void()> &&fn) {
  LocateService<AppPlatform>()->queueForMainThread(fn);
}