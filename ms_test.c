
#define main test_linear
#include "test\t-nn_linear.c"
#undef main

#define main test_quadratic
#include "test\t-nn_quadratic.c"
#undef main

int main(void)
{
	test_linear();
	test_quadratic();
}
