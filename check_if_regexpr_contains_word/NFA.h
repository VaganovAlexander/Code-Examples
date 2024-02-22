#include <exception>
#include <iostream>
#include <iterator>
#include <set>
#include <stack>
#include <string>
#include <vector>

template <typename T>
concept RegularExpr = requires(T first, T second, size_t i) {
  first + second;
  first.begin();
  first.end();
  first[i];
  first.check_if_correct();
};

class RegularExpression {
 private:
  std::string reg_expr;
  friend std::istream& operator>>(std::istream& istream,
                                  RegularExpression& reg_expr);

 public:
  RegularExpression() = default;
  ~RegularExpression() = default;
  explicit RegularExpression(char letter);
  explicit RegularExpression(const std::string& expression);
  explicit RegularExpression(std::string&& expression);
  RegularExpression(const RegularExpression& other) = default;
  bool operator==(const RegularExpression& other);
  RegularExpression& operator+=(const RegularExpression& other);
  RegularExpression operator+(const RegularExpression& other);
  [[nodiscard("You are ignoring the returning symbol")]] char& operator[](
      size_t i);
  [[nodiscard("You are ignoring the returning symbol")]] const char& operator[](
      size_t i) const;
  class iterator
      : std::iterator<std::random_access_iterator_tag, char, int, char*, char> {
   public:
    size_t index = 0;
    std::string reg_expr_;
    explicit iterator(size_t index, std::string reg_expr)
        : index(index), reg_expr_(std::move(reg_expr)) {}
    iterator& operator++() {
      ++index;
      return *this;
    }
    iterator& operator--() {
      --index;
      return *this;
    }
    bool operator==(const iterator& other) const {
      return index == other.index;
    }
    char& operator*() { return reg_expr_[index]; }
  };
  [[nodiscard("You are ignoring the iterator value")]] iterator begin() const;
  [[nodiscard("You are ignoring the iterator value")]] iterator end() const;
  void check_if_correct();
};

RegularExpression::RegularExpression(const std::string& expression)
    : reg_expr(expression) {
  check_if_correct();
}

RegularExpression::RegularExpression(std::string&& expression)
    : reg_expr(std::move(expression)) {
  check_if_correct();
}

RegularExpression::RegularExpression(char letter) { reg_expr = letter; }

bool RegularExpression::operator==(const RegularExpression& other) {
  return reg_expr == other.reg_expr;
}

RegularExpression& RegularExpression::operator+=(
    const RegularExpression& other) {
  reg_expr = reg_expr + other.reg_expr + "+";
  return *this;
}

RegularExpression RegularExpression::operator+(const RegularExpression& other) {
  RegularExpression copy(*this);
  return copy += other;
}

char& RegularExpression::operator[](size_t i) { return reg_expr[i]; }

const char& RegularExpression::operator[](size_t i) const {
  return reg_expr[i];
}

std::istream& operator>>(std::istream& istream, RegularExpression& reg_expr) {
  reg_expr.reg_expr.clear();
  char c = static_cast<char>(istream.get());
  do {
    reg_expr.reg_expr.push_back(c);
    c = static_cast<char>(istream.get());
  } while (!std::isspace(c) && !istream.eof());
  reg_expr.check_if_correct();
  return istream;
}

void RegularExpression::check_if_correct() {
  std::stack<RegularExpression> stack;
  std::set<char> operators = {'.', '+', '*'};
  std::set<char> letters = {'a', 'b', 'c', '1'};
  for (char letter : reg_expr) {
    bool is_letter = letters.contains(letter);
    bool is_operator = operators.contains(letter);
    if (!(is_letter || is_operator)) {
      throw std::invalid_argument("Invalid argument for regular expression!");
    }
    if (is_letter) {
      stack.emplace(letter);
      continue;
    }
    if (stack.size() == 1 && letter != '*') {
      throw std::invalid_argument("Invalid argument for regular expression!");
    }
    if (letter == '*') {
      if (stack.empty()) {
        throw std::invalid_argument(
            "Invalid argument for regular expression! Parse stack is empty at "
            "Kleene iteration");
      }
      continue;
    }
    RegularExpression second = stack.top();
    stack.pop();
    RegularExpression first = stack.top();
    stack.pop();
    first += second;
    stack.push(first);
  }
  if (stack.size() != 1) {
    throw std::invalid_argument("Invalid argument for regular expression!");
  }
}

RegularExpression::iterator RegularExpression::begin() const {
  iterator iter(0, reg_expr);
  return iter;
}

RegularExpression::iterator RegularExpression::end() const {
  iterator iter(reg_expr.size(), reg_expr);
  return iter;
}

template <RegularExpr Reg = RegularExpression>
class NFA {
 private:
  struct Edge;
  struct Node {
   private:
    std::vector<Edge*> transitions;
    bool is_terminal = false;
    bool is_start = false;
    long long max_suf_length = 0;
    int depth = 0;

   public:
    Node() = default;
    ~Node() = default;
    void add_to_transitions(Edge* edge) noexcept {
      transitions.push_back(edge);
    }
    [[nodiscard(
        "You are ignoring the vector of transitions")]] std::vector<Edge*>&
    get_transitions() noexcept {
      return transitions;
    }
    void set_terminal(bool status) noexcept { is_terminal = status; }
    void set_start(bool status) noexcept { is_start = status; }
    bool get_terminal() noexcept { return is_terminal; }
    void set_max_suf_length(long long length) noexcept {
      max_suf_length = length;
    }
    size_t get_max_suf_length() noexcept { return max_suf_length; }
    void set_depth(int new_depth) noexcept { depth = new_depth; }
    int get_depth() noexcept { return depth; }
    Node* get_first_transition() { return transitions[0]->get_to(); }
  };

  struct Edge {
   private:
    Node* from = nullptr;
    Node* to = nullptr;
    char symbol = '\0';

   public:
    Edge() = default;
    Edge(Node* from, Node* to, char symbol);
    ~Edge() = default;
    Node* get_from();
    Node* get_to();
    char get_symbol();
  };

  Node* start = nullptr;
  Node* terminal_vertex = nullptr;
  char needed_letter = '\0';

 public:
  NFA() = default;
  ~NFA() = default;
  NFA(const Reg& regular_expression, char letter_find);
  explicit NFA(char letter, char letter_find);
  void concatenate(const NFA& other);
  void Kleene_iteration();
  void combine(const NFA& other);
  bool check_suf(int k) {
    if (needed_letter == '1') {
      return terminal_vertex == start;
    }
    return terminal_vertex->get_max_suf_length() >= k;
  }
  void printing_answer(int k) {
    bool containment = check_suf(k);
    std::cout << (containment ? "YES" : "NO") << "\n";
  }
};

template <RegularExpr Reg>
NFA<Reg>::Edge::Edge(NFA::Node* from, NFA::Node* to, char symbol)
    : from(from), to(to), symbol(symbol) {}

template <RegularExpr Reg>
NFA<Reg>::Node* NFA<Reg>::Edge::get_from() {
  return from;
}

template <RegularExpr Reg>
NFA<Reg>::Node* NFA<Reg>::Edge::get_to() {
  return to;
}

template <RegularExpr Reg>
char NFA<Reg>::Edge::get_symbol() {
  return symbol;
}

template <RegularExpr Reg>
NFA<Reg>::NFA(char letter, char letter_find) {
  needed_letter = letter_find;
  Node* node = new Node();
  start = node;
  node->set_start(true);
  Node* new_node = new Node();
  Edge* edge = new Edge(node, new_node, letter);
  node->add_to_transitions(edge);
  new_node->set_terminal(true);
  terminal_vertex = new_node;
  if (letter == needed_letter) {
    new_node->set_max_suf_length(1);
  }
  new_node->set_depth(1);
}

template <RegularExpr Reg>
NFA<Reg>::NFA(const Reg& regular_expression, char letter_find) {
  needed_letter = letter_find;
  std::stack<NFA<Reg>> stack;
  std::set<char> letters = {'a', 'b', 'c', '1'};
  // std::set<char> operators = {'.', '+', '*'}; Not necessary due to
  // regular_expression checker
  for (auto letter : regular_expression) {
    bool is_letter = letters.contains(letter);
    if (is_letter) {
      (letter == '1') ? stack.emplace('\0', letter_find)
                      : stack.emplace(letter, letter_find);
      continue;
    }
    if (letter == '.') {
      NFA<Reg> second = stack.top();
      stack.pop();
      NFA<Reg> first = stack.top();
      stack.pop();
      first.concatenate(second);
      stack.push(first);
      continue;
    }
    if (letter == '+') {
      NFA<Reg> second = stack.top();
      stack.pop();
      NFA<Reg> first = stack.top();
      stack.pop();
      first.combine(second);
      stack.push(first);
      continue;
    }
    if (letter == '*') {
      NFA<Reg> nfa = stack.top();
      stack.pop();
      nfa.Kleene_iteration();
      stack.push(nfa);
      continue;
    }
    throw std::invalid_argument("Check your correctness-checker function.");
  }
  *this = stack.top();
  stack.pop();
}

template <RegularExpr Reg>
void NFA<Reg>::concatenate(const NFA<Reg>& other) {
  other.start->set_start(false);
  Edge* edge = new Edge(terminal_vertex, other.start, '\0');
  terminal_vertex->add_to_transitions(edge);
  terminal_vertex->set_terminal(false);
  other.start->set_max_suf_length(terminal_vertex->get_max_suf_length());
  if (other.start->get_transitions()[0]->get_symbol() == needed_letter) {
    other.start->get_first_transition()->set_max_suf_length(
        other.start->get_max_suf_length() + 1);
  } else {
    other.start->get_first_transition()->set_max_suf_length(
        other.start->get_max_suf_length());
  }
  other.terminal_vertex->set_max_suf_length(
      other.start->get_first_transition()->get_max_suf_length());
  other.start->set_depth(terminal_vertex->get_depth());
  terminal_vertex = other.terminal_vertex;
  Node* first_transition = other.start->get_first_transition();
  if (other.start->get_transitions()[0]->get_symbol() != '\0') {
    first_transition->set_depth(other.start->get_depth() + 1);
  } else {
    first_transition->set_depth(other.start->get_depth());
  }
  terminal_vertex->set_depth(first_transition->get_depth());
}

template <RegularExpr Reg>
void NFA<Reg>::combine(const NFA<Reg>& other) {
  Node* node = new Node();
  Edge* first_edge = new Edge(node, start, '\0');
  start->set_start(false);
  Edge* second_edge = new Edge(node, other.start, '\0');
  other.start->set_start(false);
  Node* old_start = start;
  start = node;
  node->set_start(true);
  start->add_to_transitions(first_edge);
  start->add_to_transitions(second_edge);
  Edge* edge_to_term = new Edge(other.terminal_vertex, terminal_vertex, '\0');
  other.terminal_vertex->set_terminal(false);
  other.terminal_vertex->add_to_transitions(edge_to_term);
  start->set_depth(old_start->get_depth());
}

template <RegularExpr Reg>
void NFA<Reg>::Kleene_iteration() {
  Edge* edge = new Edge(terminal_vertex, start, '\0');
  terminal_vertex->add_to_transitions(edge);
  terminal_vertex->set_terminal(false);
  if (terminal_vertex->get_depth() == terminal_vertex->get_max_suf_length()) {
    start->set_max_suf_length(INT32_MAX);
  }
  terminal_vertex = start;
  start->set_terminal(true);
}