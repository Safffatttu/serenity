#include <serenity.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <LibThreading/Thread.h>

int perf_register_string(char const* string, size_t string_length);

int main()
{
	auto buffer = "13";

	constexpr auto thread_count = 1;
	NonnullRefPtrVector<Threading::Thread, thread_count> threads;


	for (int i = 0; i < thread_count; i++)
	{
		threads.append(Threading::Thread::construct([&] {
			printf("Started running");
			for(int j = 0; j < 100000; j++)
			{
				printf("%d\n", j);
				auto b = (char*)malloc(strlen(buffer));
				strcpy(b, buffer);

				auto result = perf_register_string(b, strlen(b));
				if (result != 0) {
					printf("YO ERROR");
				}

				free(b);
			}
			return 0;
		}));

		threads.last().start();
	}


	// printf("got here");

	// while (true)
	// {
	// 	printf("got here");
	// 	auto b = (char*)malloc(strlen(buffer) + 1);
	// 	strcpy(b, buffer);

	// 	auto result = perf_register_string(b, strlen(b));
	// 	if (result != 0) {
	// 		printf("YO ERROR");
	// 	}

	// 	free(b);
	// }


	for(auto& thread : threads) {
		[[maybe_unused]] auto res = thread.join();
	}

	// for(int i = 0; i < 100; i++){
	// 	auto result = perf_register_string(buffer, strlen(buffer));
	// 	printf("Result %d\n", result);
	// }

	return 0;
}