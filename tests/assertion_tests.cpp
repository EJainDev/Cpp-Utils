import annotest;

import std;

using namespace annotest;

struct AssertionTestSuite {
  template <typename A = int, typename B = int>
  [[
    = Test{},
    = ParameterizeMatrix{tuple(ParameterizePair<long long, double>::WithValues{tuple(4, 4.0)},
                               ParameterizePair<int, int>::WithValues{tuple(1, 1)},
                               ParameterizePair<int, double>::WithValues{tuple(2, 2.0)})}
  ]] void AssertEqual(A a, B b) {
    assertEqual(a, b);
  }
  [[
    = Test{},
    = Parameterize{tuple(3, 3), tuple(0, 0), tuple(-1, -1), tuple(1, 1),
                   tuple(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()),
                   tuple(std::numeric_limits<int>::min(), std::numeric_limits<int>::min())}
  ]] void AssertEqualInt(int a, int b) {
    assertEqual(a, b);
  }

  template <typename T = float>
  [[ = Test{}, = ParameterizeTemplate<int, double>{} ]] void AssertEqualTemplate() {
    assertEqual(42, 42);
  }
};

int main(int argc, char** argv) { return test<AssertionTestSuite>(argc, argv); }