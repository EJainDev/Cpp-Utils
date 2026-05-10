import cpputils.testing;

import std;

using namespace cpputils::testing;

// This is the function we want to test
int add(int a, int b) {
  if (a == -2147483648 && b < 0) {
    throw std::runtime_error("Integer overflow");
  } else if (a == 2147483647 && b > 0) {
    throw std::runtime_error("Integer overflow");
  } else if (b == -2147483648 && a < 0) {
    throw std::runtime_error("Integer overflow");
  } else if (b == 2147483647 && a > 0) {
    throw std::runtime_error("Integer overflow");
  }
  return a + b;
}

struct AddTests {
  int (*func)(int, int);
  [[= BeforeEach{}]] void setup() { func = add; }  // This will execute before each test case

  // The Test{} annotation makes it a test case
  [[= Test{}]] void testAdd() {
    int a = 3, b = 5;
    int expected = a + b;
    int actual = func(a, b);
    assertEqual(expected, actual);
  }

  // The Parameterize annotation runs the test case with each set of parameters provided by the
  // tuple function
  [[
    = Test{}, = Parameterize{tuple(-1, -1), tuple(-1, 0), tuple(-1, 1), tuple(0, -1), tuple(0, 0),
                             tuple(0, 1), tuple(1, -1), tuple(1, 0), tuple(1, 1)}
  ]] void testAddEdgeCases(int a, int b) {
    int expected = a + b;
    int actual = func(a, b);
    assertEqual(expected, actual);  // Ensures the values are equal
  }

  [[
    = Test{}, = Parameterize{tuple(-2147483648, -1), tuple(-2147483648, 0), tuple(-2147483648, 1),
                             tuple(2147483647, -1), tuple(2147483647, 0), tuple(2147483647, 1),
                             tuple(-1, -2147483648), tuple(0, -2147483648), tuple(1, -2147483648),
                             tuple(-1, 2147483647), tuple(0, 2147483647), tuple(1, 2147483647)}
  ]] void testAddOverflow(int a, int b) {
    assertThrows<std::runtime_error>(
        [&]() { func(a, b); });  // Ensures that the function throws an exception for overflow cases
  }
};

int main(int argc, char** argv) { return test<AddTests>(argc, argv); }