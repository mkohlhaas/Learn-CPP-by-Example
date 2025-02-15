#include <functional>
#include <iostream>

using std::cout, std::plus, std::endl;

// Needs C++14 for std::plus<void> so compile with e.g.
// g++ --std=c++14 hello_world.cpp -o ./hello.out
template <typename T, typename U>
auto
simple_plus (T lhs, U rhs) -> decltype (lhs + rhs)
{
  return lhs + rhs;
}

auto
main () -> int
{
  cout << "Hello, world!\n";
  plus<int> adder;
  // plus<> adder;
  cout << adder (1, 1.23) << endl;       // 2
  cout << plus<>{}(1, 1.23) << endl;     // 2.23
  cout << plus{}(1, 1.23) << endl;       // 2.23
  cout << simple_plus (1, 1.23) << endl; // 2.23
}
