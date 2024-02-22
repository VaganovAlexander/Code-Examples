#include <cassert>
#include <sstream>

#include "BigInteger.h"

void basic_test_bi() {
  BigInteger a(5);
  a += BigInteger(10);
  assert(a == 15);
  a += 10_bi;
  assert(a == 25);
  BigInteger b("100000");
  assert(b / a == 4000);
  assert(b + a == 100025);
  assert(b * a == 2500000_bi);
  assert(b - a == 99975);
  std::istringstream iss("123 285");
  BigInteger c, d;
  iss >> c >> d;
  assert(c + d == 408);
}

void basic_test_rational() {
  Rational a(15);
  a /= 20;
  assert(a.asDecimal(2) == "0.75");
  Rational b(7);
  a /= b;
  assert(a.get_denominator() == 28);
  assert(a.get_numerator() == 3);
  assert(a.toString() == "3/28");
}

int main() {
  basic_test_bi();
  basic_test_rational();
}