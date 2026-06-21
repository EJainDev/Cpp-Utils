import annotest;

import std;

using namespace annotest;

struct AssertionTestSuite {
  template <typename A = int, typename B = int>
  [[
    = Test{},
    = dualParameterize(
        Pair<int, int>::InitM(
            tuple(3, 3), tuple(0, 0), tuple(-1, -1), tuple(1, 1),
            tuple(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()),
            tuple(std::numeric_limits<int>::min(), std::numeric_limits<int>::min())),
        Pair<double, double>::InitM(
            tuple(3.14431764703641736497123649871264, 3.14431764703641736497123649871264),
            tuple(0.0, 0.0), tuple(-1.0, -1.0), tuple(1.0, 1.0),
            tuple(std::numeric_limits<double>::max(), std::numeric_limits<double>::max()),
            tuple(std::numeric_limits<double>::min(), std::numeric_limits<double>::min())),
        Pair<int, unsigned int>::InitM(
            tuple(0, 0u), tuple(1, 1u), tuple(53, 53u),
            tuple(std::numeric_limits<int>::max(),
                  static_cast<unsigned int>(std::numeric_limits<int>::max()))))
  ]] void AssertEqual(A a, B b) {
    assertEqual(a, b);
  }

  template <typename A = int, typename B = int>
  [[
    = Test{},
    = dualParameterize(
        Pair<int, int>::InitM(
            tuple(3, 4), tuple(0, 1), tuple(-1, 0), tuple(1, 2),
            tuple(std::numeric_limits<int>::max(), std::numeric_limits<int>::min()),
            tuple(std::numeric_limits<int>::min(), std::numeric_limits<int>::max())),
        Pair<double, double>::InitM(
            tuple(3.14431764703641736497123649871264, 3.14431764703641736497123665),
            tuple(0.0, 1.0), tuple(-1.0, -0.9999999999999999), tuple(1.0, 1.0000000000000002),
            tuple(std::numeric_limits<double>::max(), std::numeric_limits<double>::min()),
            tuple(std::numeric_limits<double>::min(), std::numeric_limits<double>::max())),
        Pair<int, unsigned int>::InitM(
            tuple(0, 1u), tuple(1, 2u), tuple(53, 54u),
            tuple(std::numeric_limits<int>::max(),
                  static_cast<unsigned int>(std::numeric_limits<int>::min()))))
  ]] void AssertNotEqual(A a, B b) {
    assertNotEqual(a, b);
  }
};

int main(int argc, char** argv) { return test<AssertionTestSuite>(argc, argv); }