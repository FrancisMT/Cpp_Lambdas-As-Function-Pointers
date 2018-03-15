#include<iostream>
#include<functional>

// Legacy method with a void pointer as parameter
void Foo(void(*callback_function)(void)) {

	callback_function();
}

void Foo_mod(std::function<void(void)> callback_function) {
	callback_function();
}

void Foo_mod_with_thunk(void(*callback_function)(void*), void* callback_arg) {
	callback_function(callback_arg);
}

int main() {

	// A capturless lambda can be cast to a regular function pointer.
	{
		auto Lambda_Callback = []() {
			std::cout << "Lambda Callback\n";
		};

		Foo(Lambda_Callback);
	}

	// Capturing Lambdas that capture variables from the context cannot be cast to a regular function pointer.
	{
		int CallbackNum = 0;

		auto Lambda_Callback = [&]() {
			std::cout << "Lambda Callback number: " << CallbackNum << "\n";
		};

		// The compiler will generate the following error: "no suitable conversion from "lambda []void->void" to "void(*)()" exists".
		//Foo(Lambda_Callback);
	}

	// [Solution A for using capturing lambdas as function pointers]: modify Foo to support std::function-based callbacks.
	{
		int CallbackNum = 0;

		// Capturing lambda functions create a closure of the used context that needs to be carried around to the point of invocation.
		auto Lambda_Callback = [&]() {
			std::cout << "Lambda Callback number: " << CallbackNum << "\n";
		};

		Foo_mod(Lambda_Callback);
	}

	// [Solution B for using capturing lambdas as function pointers]: when it's not possible to modify Foo to support std::function-based callbacks (we might be dealing with legacy code)
	//we need to pass the lambda funcion as the callback argument and providing a captureless thunk as the callback function pointer.
	{
		int CallbackNum = 0;

		auto Lambda_Callback = [&]() {
			std::cout << "Lambda Callback number: " << CallbackNum << "\n";
		};

		// Thunk is captureless.
		auto Thunk = [](void *arg) {
			(*static_cast<decltype(Lambda_Callback)*>(arg))();
		};
		Foo_mod_with_thunk(Thunk, &Lambda_Callback);
	}

	getchar();
	return 0;
}