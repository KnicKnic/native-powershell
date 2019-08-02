
#define DENY_COPY(T) T(T&)=delete ; T& operator=(T&) = delete;
#define DEFAULT_MOVE(T) T(T&&)=default; T& operator=(T&&) = default;
