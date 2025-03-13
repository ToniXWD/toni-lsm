#include "../../include/lsm/two_merge_iterator.h"

TwoMergeIterator::TwoMergeIterator() {}

TwoMergeIterator::TwoMergeIterator(std::shared_ptr<BaseIterator> it_a,
                                   std::shared_ptr<BaseIterator> it_b)
    : it_a(std::move(it_a)), it_b(std::move(it_b)) {
  skip_it_b();              // 跳过与 it_a 重复的 key
  choose_a = choose_it_a(); // 决定使用哪个迭代器
}

bool TwoMergeIterator::choose_it_a() {
  if (it_a->is_end()) {
    return false;
  }
  if (it_b->is_end()) {
    return true;
  }
  return (**it_a).first < (**it_b).first; // 比较 key
}

void TwoMergeIterator::skip_it_b() {
  if (!it_a->is_end() && !it_b->is_end() && (**it_a).first == (**it_b).first) {
    ++(*it_b);
  }
}

BaseIterator &TwoMergeIterator::operator++() {
  if (choose_a) {
    ++(*it_a);
  } else {
    ++(*it_b);
  }
  skip_it_b();              // 跳过重复的 key
  choose_a = choose_it_a(); // 重新决定使用哪个迭代器
  return *this;
}

bool TwoMergeIterator::operator==(const BaseIterator &other) const {
  if (other.get_type() != IteratorType::TwoMergeIterator) {
    return false;
  }
  auto other2 = dynamic_cast<const TwoMergeIterator &>(other);
  if (this->is_end() && other2.is_end()) {
    return true;
  }
  if (this->is_end() && other2.is_end()) {
    return false;
  }
  return it_a == other2.it_a && it_b == other2.it_b &&
         choose_a == other2.choose_a;
}

bool TwoMergeIterator::operator!=(const BaseIterator &other) const {
  return !(*this == other);
}

BaseIterator::value_type TwoMergeIterator::operator*() const {
  if (choose_a) {
    return **it_a;
  } else {
    return **it_b;
  }
}

IteratorType TwoMergeIterator::get_type() const {
  return IteratorType::TwoMergeIterator;
}

bool TwoMergeIterator::is_end() const {
  if (it_a == nullptr && it_b == nullptr) {
    return true;
  }
  if (it_a == nullptr) {
    return it_b->is_end();
  }
  if (it_b == nullptr) {
    return it_a->is_end();
  }
  return it_a->is_end() && it_b->is_end();
}

bool TwoMergeIterator::is_valid() const {
  if (it_a == nullptr && it_b == nullptr) {
    return false;
  }
  if (it_a == nullptr) {
    return it_b->is_valid();
  }
  if (it_b == nullptr) {
    return it_a->is_valid();
  }
  return it_a->is_valid() || it_b->is_valid();
}

TwoMergeIterator::pointer TwoMergeIterator::operator->() const {
  update_current();
  return current.get();
}

void TwoMergeIterator::update_current() const {
  if (choose_a) {
    current = std::make_shared<value_type>(**it_a);
  } else {
    current = std::make_shared<value_type>(**it_b);
  }
}