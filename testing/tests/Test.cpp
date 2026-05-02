import cpputils.testing;

import std;

using namespace cpputils::testing;

struct TestSuite {
  [[= BeforeAll{}]] void setupSuite() { std::cout << "Setting up test suite...\n"; }

  [[= BeforeEach{}]] void setupTest() { std::cout << "Setting up test...\n"; }

  [[= Test{}]][[= Parameterize<4, int>{tuple(1), tuple(5), tuple(15), tuple(50)}]] void test1(
      int param = 3) {
    assertEqual(param, param);
  }

  [[= Test{}]] void test2() { assertEqual(2, 2); }

  [[= Test{}]][[= Disabled{}]] void disabledTest() { assertEqual(2, 2); }

  [[= AfterEach{}]] void teardownTest() { std::cout << "Tearing down test...\n"; }

  [[= AfterAll{}]] void teardownSuite() { std::cout << "Tearing down test suite...\n"; }
};

int main() { test(TestSuite{}); }