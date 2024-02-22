#include <iostream>
#include <memory>

template <size_t N>
struct StackStorage {
  StackStorage() {}
  ~StackStorage() {}
  char stack_storage[N];
  int offset = 0;

 private:
  StackStorage(const StackStorage&) {}
  StackStorage& operator=(const StackStorage&) {}
};

template <typename T, size_t N>
struct StackAllocator {
  StackStorage<N>& stack;

  using value_type = T;
  using void_pointer = void*;
  using size_type = size_t;
  using difference_type = int;

  ~StackAllocator() {}
  StackAllocator(StackStorage<N>& storage) : stack(storage) {}
  T* allocate(size_t count);
  void deallocate(T* ptr, size_t count);
  template <typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };
  template <typename U>
  StackAllocator(const StackAllocator<U, N>& other) : stack(other.stack) {}
  template <typename U>
  StackAllocator& operator=(const StackAllocator<U, N>&);

 private:
  StackAllocator() {}
};

template <typename T, size_t N>
T* StackAllocator<T, N>::allocate(size_t count) {
  size_t address =
      reinterpret_cast<uintptr_t>(stack.stack_storage + stack.offset);
  size_t mod = address % alignof(T);
  stack.offset += alignof(T) - mod;
  if (stack.offset + count * sizeof(T) > N) {
    throw std::bad_alloc();
  }
  T* result = reinterpret_cast<T*>(stack.stack_storage + stack.offset);
  stack.offset += count * sizeof(T);
  return result;
}

template <typename T, size_t N>
void StackAllocator<T, N>::deallocate(T*, size_t) {}

template <typename T, size_t N>
template <typename U>
StackAllocator<T, N>& StackAllocator<T, N>::operator=(
    const StackAllocator<U, N>& other) {
  stack = other.stack;
  return *this;
}

template <typename T, size_t N, typename U, size_t M>
bool operator==(const StackAllocator<T, N>& first,
                const StackAllocator<U, M>& second) {
  return first.stack.stack_storage == second.stack.stack_storage;
}

template <typename T, size_t N, typename U, size_t M>
bool operator!=(const StackAllocator<T, N>& first,
                const StackAllocator<U, M>& second) {
  return !(first == second);
}

template <typename T, typename Alloc = std::allocator<T>>
class List {
 private:
  void swap(List& other) {
    std::swap(size_, other.size_);
    std::swap(top_, other.top_);
    std::swap(bottom_, other.bottom_);
  }

  void fill_list(size_t size, T element);

  struct Node {
    ~Node() = default;

    T object;
    Node* prev;
    Node* next;
  };

  Node* top_ = nullptr;
  Node* bottom_ = nullptr;

  size_t size_ = 0;

  using NodeAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  [[no_unique_address]] NodeAlloc allocator_;

 public:
  List() = default;
  ~List();
  List(size_t size);
  List(size_t size, const T& element);
  List(Alloc allocator);
  List(size_t size, Alloc alloc);
  List(size_t size, const T& element, Alloc allocator);
  List(const List& other);
  List<T, Alloc>& operator=(const List<T, Alloc>& other);
  NodeAlloc get_allocator() { return allocator_; }
  size_t size() const;
  void push_back(const T& new_t);
  void push_front(const T& new_t);
  void pop_back();
  void pop_front();
  Node* get_top() const { return top_; }
  Node* get_bottom() const { return bottom_; }

  template <bool IsConst>
  class iterator_common {
   public:
    typename std::conditional<IsConst, const List<T, Alloc>*,
                              List<T, Alloc>*>::type list_iter;
    Node* ptr = nullptr;
    using value_type = T;
    using difference_type = int;
    using reference = typename std::conditional<IsConst, const T&, T&>::type;
    using pointer = typename std::conditional<IsConst, const T*, T*>::type;
    using iterator_category = std::bidirectional_iterator_tag;
    iterator_common(typename std::conditional<IsConst, const List<T, Alloc>*,
                                              List<T, Alloc>*>::type list)
        : list_iter(list), ptr(list->bottom_) {}
    iterator_common(typename std::conditional<IsConst, const List<T, Alloc>*,
                                              List<T, Alloc>*>::type list,
                    Node* ptr_input)
        : list_iter(list), ptr(ptr_input) {}
    reference operator*();
    pointer operator->();
    iterator_common& operator++();
    iterator_common operator++(int);
    iterator_common& operator--();
    iterator_common operator--(int);
    bool operator==(const iterator_common<IsConst>& other) const;
    bool operator!=(const iterator_common<IsConst>& other) const;
    operator iterator_common<true>() { return const_iterator(list_iter, ptr); }
  };

  template <class Iterator>
  class reverse_iterator_common {
   private:
    Iterator iter_;

   public:
    using value_type = T;
    using difference_type = int;
    using reference = typename Iterator::reference;
    using pointer = typename Iterator::pointer;
    using iterator_category = std::bidirectional_iterator_tag;

    reverse_iterator_common(const Iterator& iter_constr) : iter_(iter_constr) {}
    typename Iterator::reference operator*();
    Iterator operator->();
    reverse_iterator_common& operator++();
    reverse_iterator_common& operator--();
    reverse_iterator_common& operator++(int);
    reverse_iterator_common& operator--(int);
    bool operator==(const reverse_iterator_common& other) const;
    bool operator!=(const reverse_iterator_common& other) const;
    Iterator base() const {
      Iterator iter_based(iter_.list_iter, iter_.ptr->next);
      return iter_based;
    }
    operator reverse_iterator_common<iterator_common<true>>() {
      return const_reverse_iterator(iter_);
    }
  };

  using iterator = iterator_common<false>;
  using const_iterator = iterator_common<true>;
  using reverse_iterator = reverse_iterator_common<iterator_common<false>>;
  using const_reverse_iterator = reverse_iterator_common<iterator_common<true>>;
  iterator begin();
  iterator end();
  const_iterator cbegin() const;
  const_iterator cend() const;
  const_iterator begin() const;
  const_iterator end() const;
  reverse_iterator rbegin();
  reverse_iterator rend();
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;
  void insert(const_iterator it, const T& element);
  void erase(const_iterator it);
};

template <typename T, typename Alloc>
List<T, Alloc>::~List() {
  Node* tmp;
  while (size_ > 0) {
    tmp = bottom_;
    bottom_ = bottom_->next;
    std::allocator_traits<NodeAlloc>::destroy(allocator_, tmp);
    std::allocator_traits<NodeAlloc>::deallocate(allocator_, tmp, 1);
    --size_;
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::fill_list(size_t size, T element) {
  for (size_t i = 0; i < size; ++i) {
    Node* obj = allocator_.allocate(1);
    std::allocator_traits<NodeAlloc>::construct(allocator_, &(obj->object),
                                                element);
    if (top_ == nullptr) {
      top_ = obj;
      bottom_ = obj;
      continue;
    }
    top_->next = obj;
    obj->prev = top_;
    top_ = obj;
  }
  size_ = size;
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t size) {
  try {
    fill_list(size, T());
  } catch (...) {
    (*this).~List();
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t size, const T& element) {
  try {
    fill_list(size, element);
  } catch (...) {
    (*this).~List();
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(Alloc alloc)
    : allocator_(std::allocator_traits<
                 NodeAlloc>::select_on_container_copy_construction(alloc)) {}

template <class T, class Alloc>
List<T, Alloc>::List(size_t size, Alloc alloc) : allocator_(alloc) {
  try {
    for (size_t i = 0; i < size; ++i) {
      Node* obj = std::allocator_traits<NodeAlloc>::allocate(allocator_, 1);
      std::allocator_traits<NodeAlloc>::construct(allocator_, &(obj->object));
      if (top_ == nullptr) {
        top_ = obj;
        bottom_ = obj;
        continue;
      }
      top_->next = obj;
      obj->prev = top_;
      top_ = obj;
    }
    size_ = size;
  } catch (...) {
    (*this).~List();
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t size, const T& element, Alloc allocator)
    : allocator_(allocator) {
  try {
    fill_list(size, element);
  } catch (...) {
    (*this).~List();
  }
}

template <class T, class Alloc>
inline List<T, Alloc>::List(const List<T, Alloc>& other)
    : allocator_(std::allocator_traits<NodeAlloc>::
                     select_on_container_copy_construction(other.allocator_)) {
  if (other.size() <= 0) {
    return;
  }
  try {
    Node* head = std::allocator_traits<NodeAlloc>::allocate(allocator_, 1);
    std::allocator_traits<NodeAlloc>::construct(allocator_, &(head->object),
                                                other.bottom_->object);
    ++size_;
    bottom_ = head;
    Node* ptr = head;
    bottom_->prev = nullptr;
    Node* other_ptr = other.bottom_->next;
    while (size_ < other.size()) {
      Node* tmp = std::allocator_traits<NodeAlloc>::allocate(allocator_, 1);
      std::allocator_traits<NodeAlloc>::construct(allocator_, &(tmp->object),
                                                  other_ptr->object);
      tmp->prev = ptr;
      ptr->next = tmp;
      ptr = tmp;
      other_ptr = other_ptr->next;
      ++size_;
    }
    top_ = ptr;
    top_->next = nullptr;
  } catch (...) {
    (*this).~List();
    throw;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(const List<T, Alloc>& other) {
  if constexpr (std::allocator_traits<
                    NodeAlloc>::propagate_on_container_copy_assignment::value) {
    allocator_ = other.allocator_;
  }
  List new_list(other);
  swap(new_list);
  return *this;
}

template <typename T, typename Alloc>
size_t List<T, Alloc>::size() const {
  return size_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back(const T& new_t) {
  insert(--end(), new_t);
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_back() {
  erase(--end());
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front(const T& new_t) {
  insert(begin(), new_t);
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_front() {
  erase(iterator(this, bottom_));
}

template <class T, typename Alloc>
template <bool IsConst>
typename List<T, Alloc>::template iterator_common<IsConst>&
List<T, Alloc>::iterator_common<IsConst>::operator++() {
  if (!ptr) {
    return *this;
  }
  ptr = ptr->next;
  return *this;
}

template <class T, typename Alloc>
template <bool IsConst>
typename List<T, Alloc>::template iterator_common<IsConst>&
List<T, Alloc>::iterator_common<IsConst>::operator--() {
  if (!ptr) {
    ptr = list_iter->get_top();
    return *this;
  }
  ptr = ptr->prev;
  return *this;
}

template <class T, typename Alloc>
template <bool IsConst>
typename List<T, Alloc>::template iterator_common<IsConst>
List<T, Alloc>::iterator_common<IsConst>::operator++(int) {
  iterator_common<IsConst> prev_ptr = *this;
  ++(*this);
  return prev_ptr;
}

template <class T, typename Alloc>
template <bool IsConst>
typename List<T, Alloc>::template iterator_common<IsConst>
List<T, Alloc>::iterator_common<IsConst>::operator--(int) {
  T* prev_ptr = ptr;
  --(*this);
  return prev_ptr;
}

template <class T, typename Alloc>
template <bool IsConst>
bool List<T, Alloc>::iterator_common<IsConst>::operator==(
    const List<T, Alloc>::iterator_common<IsConst>& other) const {
  return ptr == other.ptr;
}

template <class T, typename Alloc>
template <bool IsConst>
bool List<T, Alloc>::iterator_common<IsConst>::operator!=(
    const List<T, Alloc>::iterator_common<IsConst>& other) const {
  return !(*this == other);
}

template <class T, typename Alloc>
template <bool IsConst>
typename std::conditional<IsConst, const T&, T&>::type
List<T, Alloc>::iterator_common<IsConst>::operator*() {
  return ptr->object;
}

template <class T, typename Alloc>
template <bool IsConst>
typename std::conditional<IsConst, const T*, T*>::type
List<T, Alloc>::iterator_common<IsConst>::operator->() {
  return &(ptr->object);
}

template <class T, typename Alloc>
template <class Iterator>
typename List<T, Alloc>::template reverse_iterator_common<Iterator>&
List<T, Alloc>::reverse_iterator_common<Iterator>::operator++() {
  --iter_;
  return *this;
}

template <class T, typename Alloc>
template <class Iterator>
typename List<T, Alloc>::template reverse_iterator_common<Iterator>&
List<T, Alloc>::reverse_iterator_common<Iterator>::operator++(int) {
  Iterator old_iter = iter_;
  --iter_;
  return old_iter;
}

template <class T, typename Alloc>
template <class Iterator>
typename List<T, Alloc>::template reverse_iterator_common<Iterator>&
List<T, Alloc>::reverse_iterator_common<Iterator>::operator--() {
  return ++iter_;
}

template <class T, typename Alloc>
template <class Iterator>
typename List<T, Alloc>::template reverse_iterator_common<Iterator>&
List<T, Alloc>::reverse_iterator_common<Iterator>::operator--(int) {
  return iter_++;
}

template <class T, typename Alloc>
template <class Iterator>
typename Iterator::reference
List<T, Alloc>::reverse_iterator_common<Iterator>::operator*() {
  return *iter_;
}

template <class T, typename Alloc>
template <class Iterator>
Iterator List<T, Alloc>::reverse_iterator_common<Iterator>::operator->() {
  return iter_;
}

template <class T, typename Alloc>
template <class Iterator>
bool List<T, Alloc>::reverse_iterator_common<Iterator>::operator==(
    const reverse_iterator_common& other) const {
  return iter_ == other.iter_;
}

template <class T, typename Alloc>
template <class Iterator>
bool List<T, Alloc>::reverse_iterator_common<Iterator>::operator!=(
    const reverse_iterator_common& other) const {
  return !(*this == other);
}

template <class T, class Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::begin() {
  return iterator(this);
}

template <class T, class Alloc>
typename List<T, Alloc>::iterator List<T, Alloc>::end() {
  iterator iter(this, nullptr);
  if (top_ == nullptr) {
    iter = iterator(this, top_);
  } else {
    iter = iterator(this, top_->next);
  }
  return iter;
}

template <class T, class Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::cbegin() const {
  return const_iterator(this);
}

template <class T, class Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::cend() const {
  const_iterator iter(this, top_->next);
  return iter;
}

template <class T, class Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::begin() const {
  return const_iterator(this);
}

template <class T, class Alloc>
typename List<T, Alloc>::const_iterator List<T, Alloc>::end() const {
  const_iterator iter(this, top_->next);
  return iter;
}

template <class T, class Alloc>
typename List<T, Alloc>::reverse_iterator List<T, Alloc>::rbegin() {
  iterator iter = end();
  --iter;
  return iter;
}

template <class T, class Alloc>
typename List<T, Alloc>::reverse_iterator List<T, Alloc>::rend() {
  iterator iter = begin();
  --iter;
  return iter;
}

template <class T, class Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::crbegin()
    const {
  const_iterator iter = cend();
  --iter;
  return iter;
}

template <class T, class Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::crend() const {
  const_iterator iter = cbegin();
  --iter;
  return iter;
}

template <class T, class Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::rbegin() const {
  const_iterator iter = cend();
  --iter;
  return iter;
}

template <class T, class Alloc>
typename List<T, Alloc>::const_reverse_iterator List<T, Alloc>::rend() const {
  const_iterator iter = cbegin();
  --iter;
  return iter;
}

template <class T, class Alloc>
void List<T, Alloc>::insert(List<T, Alloc>::const_iterator it,
                            const T& element) {
  Node* new_node;
  try {
    new_node = std::allocator_traits<NodeAlloc>::allocate(allocator_, 1);
  } catch (...) {
    throw std::bad_alloc();
  }
  try {
    std::allocator_traits<NodeAlloc>::construct(allocator_, &(new_node->object),
                                                element);
  } catch (...) {
    std::allocator_traits<NodeAlloc>::deallocate(allocator_, new_node, 1);
    throw;
  }
  Node* ptr = it.ptr;
  ++size_;
  if (ptr != nullptr) {
    if (it == --end()) {
      new_node->prev = top_;
      top_->next = new_node;
      top_ = new_node;
      return;
    }
    if (it == begin()) {
      new_node->next = bottom_;
      bottom_->prev = new_node;
      bottom_ = new_node;
      return;
    }
    new_node->next = ptr;
    if (ptr->prev) {
      new_node->prev = ptr->prev;
      ptr->prev->next = new_node;
    } else {
      top_ = new_node;
    }
  } else {
    if (bottom_) {
      bottom_->next = new_node;
      new_node->prev = bottom_;
    } else {
      top_ = new_node;
      bottom_ = new_node;
    }
  }
  if (ptr != nullptr) {
    ptr->prev = new_node;
  }
}

template <class T, class Alloc>
void List<T, Alloc>::erase(List<T, Alloc>::const_iterator it) {
  Node* ptr = it.ptr;
  if (ptr->prev) {
    ptr->prev->next = ptr->next;
    if (ptr->next) {
      ptr->next->prev = ptr->prev;
    } else {
      top_ = top_->prev;
    }
  } else {
    if (ptr->next) {
      bottom_ = ptr->next;
      ptr->next->prev = nullptr;
    } else {
      top_ = nullptr;
      bottom_ = nullptr;
    }
  }
  std::allocator_traits<NodeAlloc>::destroy(allocator_, ptr);
  std::allocator_traits<NodeAlloc>::deallocate(allocator_, ptr, 1);
  --size_;
}
