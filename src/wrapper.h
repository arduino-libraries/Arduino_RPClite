#include <iostream>
#include <functional>
#include <tuple>

// --- FunctionWrapper definition ---
template<typename R, typename... Args>
class FunctionWrapper {
public:
    using FuncType = R (*)(Args...);

    FunctionWrapper(FuncType f) : func(f) {}

    R operator()(Args... args) {
        Serial.println("Calling function with args...");

        if constexpr (std::is_void<R>::value) {
            func(args...);
            Serial.println("Return: void");
            return;
        } else {
            R result = func(args...);
            Serial.print("Result: ");
            Serial.print(result);
            Serial.println();
            return result;
        }

    }

private:
    FuncType func;
};

// --- Wraps function function pointers only. Lambdas and member functions not currently supported ---
template<typename R, typename... Args>
FunctionWrapper<R, Args...> wrap(R (*f)(Args...)) {
    return FunctionWrapper<R, Args...>(f);
}
