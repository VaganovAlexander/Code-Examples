#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

class BigInteger {
 public:
  // class
  enum class Sign { POSITIVE = 1, NEUTRAL = 0, NEGATIVE = -1 };

  // constructors
  BigInteger(int number);
  BigInteger(const std::string& number);
  BigInteger(const BigInteger& big_int) = default;
  BigInteger() = default;

  // operators
  BigInteger operator-() const;
  BigInteger& operator=(const BigInteger& other) = default;
  explicit operator bool() const;
  bool operator!() const;
  BigInteger& operator+=(const BigInteger& other);
  BigInteger& operator-=(BigInteger other);
  BigInteger& operator*=(const BigInteger& other);
  BigInteger& operator/=(const BigInteger& other);
  BigInteger& operator%=(const BigInteger& other);
  BigInteger& operator++();
  BigInteger operator++(int);
  BigInteger& operator--();
  BigInteger operator--(int);

  // methods
  [[nodiscard]] std::string toString() const;
  void sign_reverse();
  BigInteger reverse_sign_bi();
  BigInteger absolute_value();

  // getters
  [[nodiscard]] Sign get_sign() const;
  [[nodiscard]] size_t get_digits_size() const;
  [[nodiscard]] std::vector<long long> get_digits() const;

 private:
  std::vector<long long> digits_;
  Sign sign_ = Sign::NEUTRAL;
  static const int base_ = 10'000'000;
  static const int base_step_ = 7;

  // helpers
  [[nodiscard]] bool is_null() const;
  void normalize();
  void digit_sum_different_sign(size_t i, int digit);
  void get_max_not_null_index(size_t i);
  void delete_first_nulls();
  void become_null();
  [[nodiscard]] int divide(const BigInteger& other) const;
  void insert(int digit);
};

//---------------------------------BigInteger---------------------------------//

//---------------------constructors----------------------//

BigInteger::BigInteger(int number) {
  if (number == 0) {
    become_null();
    return;
  }
  if (number > 0) {
    sign_ = Sign::POSITIVE;
  } else {
    number *= -1;
    sign_ = Sign::NEGATIVE;
  }
  while (number > 0) {
    digits_.push_back(number % base_);
    number /= base_;
  }
}

BigInteger::BigInteger(const std::string& numb) {
  if (numb[0] == '0') {
    become_null();
    return;
  }
  int number = 0;
  int counter = 0;
  for (size_t i = numb.size() - 1; i >= 1; --i) {
    number += (numb[i] - '0') * static_cast<int>(std::pow(10, counter));
    ++counter;
    if (counter == base_step_) {
      digits_.push_back(number);
      number = 0;
      counter = 0;
    }
  }
  bool start;
  if (numb[0] == '-') {
    sign_ = Sign::NEGATIVE;
    start = true;
  } else {
    sign_ = Sign::POSITIVE;
    start = false;
  }
  if (!start) {
    number += (numb[0] - '0') * static_cast<int>(std::pow(10, counter));
    ++counter;
  }
  if (counter != 0) {
    digits_.push_back(number);
  }
}

//-----------------------operators-----------------------//

bool operator==(const BigInteger& bi_left, const BigInteger& bi_right) {
  if (bi_left.get_sign() != bi_right.get_sign()) {
    return false;
  }
  if (bi_left.get_digits_size() != bi_right.get_digits_size()) {
    return false;
  }
  for (int i = 0; i < bi_left.get_digits_size(); ++i) {
    if (bi_left.get_digits()[i] != bi_right.get_digits()[i]) {
      return false;
    }
  }
  return true;
}

bool operator!=(const BigInteger& bi_left, const BigInteger& bi_right) {
  return !(bi_left == bi_right);
}

BigInteger::Sign compare_abs(const BigInteger& first,
                             const BigInteger& second) {
  if (first.get_digits_size() != second.get_digits_size()) {
    return (first.get_digits_size() > second.get_digits_size())
               ? BigInteger::Sign::POSITIVE
               : BigInteger::Sign::NEGATIVE;
  }
  for (int i = static_cast<int>(first.get_digits_size() - 1); i >= 0; --i) {
    if (first.get_digits()[i] < second.get_digits()[i]) {
      return BigInteger::Sign::NEGATIVE;
    }
    if (first.get_digits()[i] > second.get_digits()[i]) {
      return BigInteger::Sign::POSITIVE;
    }
  }
  return BigInteger::Sign::NEUTRAL;
}

bool operator<(const BigInteger& first, const BigInteger& second) {
  if (first.get_sign() != second.get_sign()) {
    return (first.get_sign() < second.get_sign());
  }
  if (first.get_sign() == BigInteger::Sign::NEGATIVE) {
    return compare_abs(first, second) != BigInteger::Sign::POSITIVE;
  } else if (first.get_sign() == BigInteger::Sign::POSITIVE) {
    return (compare_abs(first, second) == BigInteger::Sign::NEGATIVE);
  }
  return false;
}

bool operator<=(const BigInteger& first, const BigInteger& second) {
  return (first < second || first == second);
}

bool operator>(const BigInteger& first, const BigInteger& second) {
  return (!(first <= second));
}

bool operator>=(const BigInteger& first, const BigInteger& second) {
  return (first > second || first == second);
}

BigInteger operator+(const BigInteger& bi_left, const BigInteger& bi_right) {
  BigInteger copy = bi_left;
  copy += bi_right;
  return copy;
}

BigInteger operator-(const BigInteger& bi_left, const BigInteger& bi_right) {
  BigInteger copy = bi_left;
  copy -= bi_right;
  return copy;
}

BigInteger operator*(const BigInteger& first, const BigInteger& second) {
  BigInteger copy = first;
  copy *= second;
  return copy;
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
  if (is_null()) {
    *this = other;
    return *this;
  }
  if (other.is_null()) {
    return *this;
  }
  if (*this == -other) {
    become_null();
    return *this;
  }
  if (sign_ == other.get_sign()) {
    if (other.get_digits_size() > get_digits_size()) {
      for (int i = get_digits_size(); i < other.get_digits_size(); ++i) {
        digits_.push_back(0);
      }
    }
    for (size_t i = 0, carry = 0; i < other.digits_.size() || carry; ++i) {
      if (i == digits_.size()) {
        digits_.push_back(0);
      }
      digits_[i] +=
          carry + (i < other.digits_.size() ? other.get_digits()[i] : 0);
      carry = (digits_[i] >= base_);
      if (carry) {
        digits_[i] -= base_;
      }
    }
    delete_first_nulls();
    return *this;
  }
  if (compare_abs(*this, other) == BigInteger::Sign::POSITIVE) {
    for (int i = 0; i < other.get_digits_size(); ++i) {
      digit_sum_different_sign(i, other.get_digits()[i]);
    }
    delete_first_nulls();
    return *this;
  } else if (compare_abs(*this, other) == BigInteger::Sign::NEGATIVE) {
    BigInteger diff = other;
    diff.sign_ = other.get_sign();
    for (int i = 0; i < get_digits_size(); ++i) {
      diff.digit_sum_different_sign(i, digits_[i]);
    }
    diff.delete_first_nulls();
    *this = diff;
    return *this;
  } else {
    BigInteger diff = 0;
    diff.sign_ = Sign::NEUTRAL;
    diff.delete_first_nulls();
    *this = diff;
    return *this;
  }
}

BigInteger& BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger copy = *this;
  ++*this;
  return copy;
}

BigInteger& BigInteger::operator--() {
  *this += (-1);
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger copy = *this;
  --*this;
  return copy;
}

BigInteger& BigInteger::operator-=(BigInteger other) {
  return *this += other.reverse_sign_bi();
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  if (*this == 0 || other == 0) {
    *this = 0;
    return *this;
  }
  if (*this == 1) {
    *this = other;
    return *this;
  }
  if (other == 1) {
    return *this;
  }
  BigInteger res;
  for (size_t i = 0; i < digits_.size() + other.get_digits_size(); ++i) {
    res.digits_.push_back(0);
  }
  res.sign_ = (static_cast<int>(sign_) * static_cast<int>(other.sign_) == 1)
                  ? BigInteger::Sign::POSITIVE
                  : BigInteger::Sign::NEGATIVE;
  for (size_t i = 0; i < digits_.size(); ++i) {
    for (size_t j = 0; j < other.digits_.size(); ++j) {
      res.digits_[i + j] += (digits_[i] * other.digits_[j]);
    }
  }
  res.normalize();
  *this = res;
  return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
  if (get_digits_size() < other.get_digits_size() || *this == 0) {
    *this = 0;
    return *this;
  }
  if (*this == other) {
    *this = 1;
    return *this;
  }
  BigInteger res, tmp, copy = other;
  tmp.sign_ = Sign::POSITIVE;
  size_t locator = get_digits_size() - other.get_digits_size();
  for (size_t i = get_digits_size(); i > locator;) {
    tmp.insert(digits_[--i]);
  }
  copy.sign_ = Sign::POSITIVE;
  while (true) {
    int divisor = tmp.divide(copy);
    res.insert(divisor);
    tmp -= (BigInteger(divisor) * copy);
    if (locator == 0) {
      break;
    }
    tmp.insert(digits_[--locator]);
  }
  res.normalize();
  if (res == 0) {
    res.sign_ = Sign::NEUTRAL;
  } else {
    res.sign_ = (static_cast<int>(sign_) * static_cast<int>(other.sign_) == 1)
                    ? BigInteger::Sign::POSITIVE
                    : BigInteger::Sign::NEGATIVE;
  }
  *this = res;
  return *this;
}

BigInteger operator/(const BigInteger& bi_left, const BigInteger& bi_right) {
  BigInteger copy = bi_left;
  copy /= bi_right;
  return copy;
}

BigInteger operator%(const BigInteger& bi_left, const BigInteger& bi_right) {
  BigInteger copy = bi_left;
  copy %= bi_right;
  return copy;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
  if (*this == 0 || other == 0) {
    *this = 0;
    return *this;
  }
  *this -= (other * (*this / other));
  return *this;
}

BigInteger BigInteger::operator-() const {
  BigInteger res = *this;
  res.sign_ = (res.sign_ == BigInteger::Sign::POSITIVE)
                  ? BigInteger::Sign::NEGATIVE
                  : BigInteger::Sign::POSITIVE;
  return res;
}

BigInteger::operator bool() const { return (!is_null()); }

bool BigInteger::operator!() const { return (!bool(*this)); }

BigInteger operator""_bi(unsigned long long number) {
  BigInteger res(static_cast<int>(number));
  return res;
}

BigInteger operator""_bi(const char* number) {
  BigInteger res(number);
  return res;
}

std::ostream& operator<<(std::ostream& ostream, const BigInteger& big_int) {
  ostream << big_int.toString();
  return ostream;
}

std::istream& operator>>(std::istream& istream, BigInteger& big_int) {
  std::string res;
  istream >> res;
  big_int = BigInteger(res);
  return istream;
}

//------------------------getters------------------------//

BigInteger::Sign BigInteger::get_sign() const { return sign_; }

size_t BigInteger::get_digits_size() const { return digits_.size(); }

std::vector<long long> BigInteger::get_digits() const { return digits_; }

//------------------------methods------------------------//

std::string BigInteger::toString() const {
  std::string line;
  std::string line_part;
  for (int i = static_cast<int>(digits_.size()) - 1; i >= 0; --i) {
    line_part = std::to_string(digits_[i]);
    if (i != static_cast<int>(digits_.size() - 1)) {
      for (int j = line_part.size(); j < base_step_; ++j) {
        line_part = '0' + line_part;
      }
    }
    line += line_part;
  }
  if (sign_ == BigInteger::Sign::NEGATIVE) {
    line = '-' + line;
  }
  return line;
}

void BigInteger::sign_reverse() {
  if (sign_ == BigInteger::Sign::NEUTRAL) {
    return;
  }
  sign_ = (sign_ == BigInteger::Sign::POSITIVE) ? BigInteger::Sign::NEGATIVE
                                                : BigInteger::Sign::POSITIVE;
}

BigInteger BigInteger::absolute_value() {
  BigInteger copy = *this;
  if (copy.get_sign() == BigInteger::Sign::NEGATIVE) {
    copy.sign_ = BigInteger::Sign::POSITIVE;
  }
  return copy;
}

BigInteger BigInteger::reverse_sign_bi() {
  sign_reverse();
  return *this;
}

//------------------------helpers------------------------//

bool BigInteger::is_null() const {
  return (sign_ == BigInteger::Sign::NEUTRAL);
}

void BigInteger::digit_sum_different_sign(size_t i, int digit) {
  if (digits_[i] - digit < 0) {
    get_max_not_null_index(i + 1);
    delete_first_nulls();
    digits_[i] += base_;
  }
  digits_[i] -= digit;
}

void BigInteger::become_null() {
  digits_.clear();
  digits_.push_back(0);
  sign_ = Sign::NEUTRAL;
}

void BigInteger::get_max_not_null_index(size_t i) {
  while (digits_[i] == 0 && i < digits_.size()) {
    digits_[i] = base_ - 1;
    ++i;
  }
  --digits_[i];
}

void BigInteger::delete_first_nulls() {
  int i = static_cast<int>(digits_.size()) - 1;
  while (i != 0 && digits_[i] == 0) {
    digits_.pop_back();
    --i;
  }
  if (digits_.empty()) sign_ = Sign::NEUTRAL;
}

void BigInteger::normalize() {
  long long carry;
  for (size_t i = 0; i < digits_.size(); ++i) {
    if (digits_[i] >= base_) {
      carry = digits_[i] / base_;
      digits_[i] = digits_[i] % base_;
      digits_[i + 1] += carry;
    }
  }
  delete_first_nulls();
}

int BigInteger::divide(const BigInteger& other) const {
  int l = 0, r = base_;
  while (r - l > 1) {
    int mid = (l + r) / 2;
    BigInteger copy(other);
    copy.sign_ = Sign::POSITIVE;
    copy *= mid;
    if (copy <= *this) {
      l = mid;
    } else {
      r = mid;
    }
  }
  return l;
}

void BigInteger::insert(int digit) {
  *this *= base_;
  digits_[0] = digit;
}

//----------------------------------Rational----------------------------------//

class Rational {
 private:
  BigInteger numerator;
  BigInteger denominator;
  static const int kDefaultPrecision = 20;

 public:
  // constructors
  Rational(const BigInteger& numb);
  Rational(int numb);
  Rational() = default;

  // operators
  Rational& operator+=(const Rational& numb);
  Rational& operator-=(Rational numb);
  Rational& operator*=(const Rational& numb);
  Rational& operator/=(const Rational& numb);
  Rational operator-() const;
  explicit operator double() const;

  // methods
  std::string toString();
  [[nodiscard]] std::string asDecimal(size_t precision) const;
  void normalize();
  static BigInteger gcd(BigInteger first, BigInteger second);

  // getters
  [[nodiscard]] BigInteger get_numerator() const;
  [[nodiscard]] BigInteger get_denominator() const;
};

//---------------------constructors----------------------//

Rational::Rational(const BigInteger& numb) {
  numerator = numb;
  denominator = 1;
}

Rational::Rational(int numb) {
  numerator = numb;
  denominator = 1;
}

//-----------------------operators-----------------------//

Rational Rational::operator-() const {
  Rational res = *this;
  res.numerator.sign_reverse();
  return res;
}

bool operator==(const Rational& r_left, const Rational& r_right) {
  if (r_left.get_numerator().get_sign() != r_right.get_numerator().get_sign()) {
    return false;
  }
  Rational copy1 = r_left;
  Rational copy2 = r_right;
  copy1 *= copy2.get_denominator();
  copy2 *= copy1.get_denominator();
  return copy1.get_numerator() == copy2.get_numerator();
}

bool operator!=(const Rational& r_left, const Rational& r_right) {
  return !(r_left == r_right);
}

Rational& Rational::operator+=(const Rational& numb) {
  if (denominator == numb.denominator) {
    numerator += numb.numerator;
    return *this;
  }
  numerator *= numb.get_denominator();
  numerator += numb.get_numerator() * get_denominator();
  denominator *= numb.get_denominator();
  normalize();
  return *this;
}

Rational& Rational::operator-=(Rational numb) {
  return *this += numb.numerator.reverse_sign_bi();
}

Rational& Rational::operator*=(const Rational& numb) {
  numerator *= numb.numerator;
  denominator *= numb.denominator;
  normalize();
  return *this;
}

Rational& Rational::operator/=(const Rational& numb) {
  numerator *= numb.denominator;
  denominator *= numb.numerator;
  normalize();
  return *this;
}

Rational operator+(const Rational& r_left, const Rational& r_right) {
  Rational copy = r_left;
  copy += r_right;
  return copy;
}

Rational operator-(const Rational& r_left, const Rational& r_right) {
  Rational copy = r_left;
  copy -= r_right;
  return copy;
}

Rational operator*(const Rational& r_left, const Rational& r_right) {
  if (r_left == 0 || r_right == 0) {
    return 0;
  }
  Rational copy = r_left;
  copy *= r_right;
  return copy;
}

Rational operator/(const Rational& r_left, const Rational& r_right) {
  if (r_left == 0) {
    return 0;
  }
  Rational copy = r_left;
  copy /= r_right;
  return copy;
}

bool operator<(const Rational& r_left, const Rational& r_right) {
  if (r_left.get_numerator().get_sign() != r_right.get_numerator().get_sign()) {
    return (r_left.get_numerator().get_sign() <
            r_right.get_numerator().get_sign());
  }
  Rational copy1 = r_left;
  Rational copy2 = r_right;
  copy1 *= copy2.get_denominator();
  copy2 *= copy1.get_denominator();
  return (r_right.get_numerator() - r_left.get_numerator()).get_sign() ==
         BigInteger::Sign::POSITIVE;
}

bool operator<=(const Rational& r_left, const Rational& r_right) {
  return ((r_left < r_right) || (r_left == r_right));
}

bool operator>(const Rational& r_left, const Rational& r_right) {
  return (!(r_left <= r_right));
}

bool operator>=(const Rational& r_left, const Rational& r_right) {
  return ((r_left > r_right) || (r_left == r_right));
}

Rational::operator double() const {
  return std::stod(asDecimal(kDefaultPrecision));
}

//------------------------methods------------------------//

void Rational::normalize() {
  if (denominator.get_sign() == BigInteger::Sign::NEGATIVE) {
    numerator.sign_reverse();
    denominator.sign_reverse();
  }
  if (denominator == 1 || numerator == 1) {
    return;
  }
  BigInteger max_fractal =
      gcd(numerator.absolute_value(), denominator.absolute_value());
  if (max_fractal == 1) {
    return;
  }
  numerator /= max_fractal;
  denominator /= max_fractal;
  normalize();
}

std::string Rational::toString() {
  std::string line;
  Rational copy = *this;
  copy.normalize();
  if (denominator == 1) {
    line = copy.numerator.toString();
  } else {
    line = copy.numerator.toString() + "/" + copy.denominator.toString();
  }
  return line;
}

std::string Rational::asDecimal(size_t precision) const {
  std::string line = (numerator / denominator).toString();
  if (numerator.get_sign() == BigInteger::Sign::NEGATIVE) {
    line = '-' + line;
  }
  if (precision != 0) {
    line += '.';
    Rational tmp = *this;
    if (tmp.numerator.get_sign() == BigInteger::Sign::NEGATIVE) {
      tmp.numerator.sign_reverse();
    }
    tmp.numerator %= denominator;
    for (size_t i = 0; i < precision; ++i) {
      tmp.numerator *= 10;
      line += (tmp.numerator / tmp.denominator).toString();
      tmp.numerator %= denominator;
    }
  }
  return line;
}

BigInteger Rational::gcd(BigInteger first, BigInteger second) {
  if (first == 0 || second == 0) {
    return 0;
  }
  if (first == second) {
    return first;
  }
  if (first < second) {
    std::swap(first, second);
  }
  while (first != 0 && second != 0) {
    first %= second;
    std::swap(first, second);
  }
  return ((first == 0) ? second : first);
}

//------------------------getters------------------------//

BigInteger Rational::get_numerator() const { return numerator; }

BigInteger Rational::get_denominator() const { return denominator; }