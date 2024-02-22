#include <cassert>

#include "NFA.h"

void regular_expression_tests() {
  bool catch_ = false;
  try {
    RegularExpression("ab");
  } catch (...) {
    catch_ = true;
  }
  assert(catch_);
  catch_ = false;
  RegularExpression a("ab.");
  assert(*(++a.begin()) == 'b');
  assert((++a.begin()) == (--(--a.end())));
  assert(*a.begin() == 'a');
  assert(*(--(--a.end())) == 'b');
  try {
    RegularExpression("ab.c+");
  } catch (...) {
    assert(false);
  }
  catch_ = false;
  std::string b = "ab.+";
  try {
    RegularExpression(std::move(b));
  } catch (...) {
    catch_ = true;
  }
  assert(catch_);
  try {
    RegularExpression('a');
  } catch (...) {
    assert(false);
  }
  try {
    RegularExpression(a);
  } catch (...) {
    assert(false);
  }
  catch_ = false;
  RegularExpression c("ab.c+");
  RegularExpression d("ab.ab.c++");
  const RegularExpression const_a("ab.");
  assert(a + c == d);
  assert(a[0] == 'a');
  assert(const_a[0] == 'a');
  const std::string str = "*";
  const std::string str_not_in_alph = "d";
  try {
    RegularExpression e(str);
  } catch (...) {
    catch_ = true;
  }
  assert(catch_);
  catch_ = false;
  try {
    RegularExpression f(str_not_in_alph);
  } catch (...) {
    catch_ = true;
  }
  assert(catch_);
  catch_ = false;
  try {
    RegularExpression g("a*");
  } catch (...) {
    assert(false);
  }
}

void NFA_constructors_tests() {
  RegularExpression a("ab.c+*");
  try {
    NFA(a, 'a');
  } catch (...) {
    assert(false);
  }
  try {
    NFA('a', 'a');
  } catch (...) {
    assert(false);
  }
}

void NFA_answer_test() {
  assert(NFA(RegularExpression("a*"), 'a').check_suf(1));
  assert(NFA(RegularExpression("a*"), 'a').check_suf(100));
  assert(NFA(RegularExpression("a*"), '1').check_suf(50));
  assert(!NFA(RegularExpression("ab+c.aba.*.bac.+.+*"), 'a').check_suf(2));
  assert(NFA(RegularExpression("acb..bab.c.*.ab.ba.+.+*a."), 'c').check_suf(0));
}

int main() {
  regular_expression_tests();
  NFA_constructors_tests();
  NFA_answer_test();
  return 0;
}