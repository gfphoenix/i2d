
class noncopyable {
private:
  noncopyable(const noncopyable &c) = delete;
  noncopyable &operator=(const noncopyable &x) = delete;
};
