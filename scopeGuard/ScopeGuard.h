#ifndef SCOPEGUARD_H
#define SCOPEGUARD_H

#include <type_traits>

/**
 * @brief RAII 作用域守卫类，用于自动执行清理函数
 * 
 * 使用方法：
 *   auto guard = ScopeGuard([&] { / * 清理代码 * / });
 */
template<typename F>
class ScopeGuard {
public:
  /**
    * @brief 构造函数，接收一个可调用对象作为清理函数
    * @param f 清理函数，将在析构时自动执行
    */
  explicit ScopeGuard(F&& f) : cleanup_(std::forward<F>(f)), active_(true) {}

  /** @brief 析构函数，自动执行清理函数 */
  ~ScopeGuard() { 
    if (active_) {
      cleanup_();
    }
  }

  /** @brief 禁止拷贝构造 */
  ScopeGuard(const ScopeGuard&) = delete;
  /** @brief 禁止赋值操作 */
  ScopeGuard& operator=(const ScopeGuard&) = delete;

  /** @brief 允许移动构造 */
  ScopeGuard(ScopeGuard&& other) noexcept 
    : cleanup_(std::move(other.cleanup_)), active_(other.active_) {
    other.active_ = false;  // 防止被移动的守卫再次执行清理
  }

  /**
    * @brief 取消清理操作
    * 调用此函数后，析构时不会执行清理函数
    */
  void dismiss() { active_ = false; }

private:
  std::decay_t<F> cleanup_;
  bool active_;
};

/**
 * @brief 辅助函数，用于创建 ScopeGuard 对象
 * @param f 清理函数
 * @return ScopeGuard 对象
 */
template<typename F>
auto makeScopeGuard(F&& f) {
    return ScopeGuard<std::decay_t<F>>(std::forward<F>(f));
}

#endif // SCOPEGUARD_H