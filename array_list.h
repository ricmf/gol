/* Copyright 2017 Ricardo Meneghin Filho
    2
    3 Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software
    4 and associated documentation files (the "Software"), to deal in the
   Software without
    5 restriction, including without limitation the rights to use, copy, modify,
   merge, publish,
    6 distribute, sublicense, and/or sell copies of the Software, and to permit
   persons to whom the
    7 Software is furnished to do so, subject to the following conditions:
    8
    9 The above copyright notice and this permission notice shall be included in
   all copies or
   10 substantial portions of the Software.
   11
   12 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING
   13 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
   PARTICULAR PURPOSE AND
   14 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
   LIABLE FOR ANY CLAIM,
   15 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
   OTHERWISE, ARISING FROM,
   16 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
   17 */

#ifndef STRUCTURES_ARRAY_LIST_H
#define STRUCTURES_ARRAY_LIST_H

#include <cstdint>
#include <stdexcept>  // C++ exceptions

namespace structures {

/** An implementation of a vector List.
     */
template <typename T>
class ArrayList {
 public:
  /** Initializes with default size
      */
  ArrayList() {
    max_size_ = 10;
    size_ = 0;
    contents = new T[10];
  }
  /** Initializes an empty list with the supplied maximum size.
     */
  explicit ArrayList(std::size_t max_size) {
    max_size_ = max_size;
    size_ = 0;
    contents = new T[max_size_];
  }

  ~ArrayList() { delete[] contents; }

  /** removes all elements from the list.
   */
  inline void clear() { size_ = 0; }

  /** inserts a element at the last position of the list
     */
  void push_back(const T& data) {
    //if (full()) {
    //  throw std::out_of_range("list is full");
    //}
    contents[size_] = data;
    size_++;
  }

  /** inserts an element at the first position of the list
       */
  void push_front(const T& data) { insert(data, 0); }

  /** inserts an element at the given index.
       */
  void insert(const T& data, std::size_t index) {
    if (full()) {
      throw std::out_of_range("list is full");
    }
    if (index > size_) {
      throw std::out_of_range("invalid index");
    }
    for (int i = size_; i > index; --i) {
      contents[i] = contents[i - 1];
    }
    contents[index] = data;
    size_++;
  }

  /** inserts an element in before the first element on the list that is greater
   * than it.
   * This method assumes the list is sorted.
          */
  void insert_sorted(const T& data) {
    for (int i = 0; i < size_; ++i) {
      if (contents[i] > data) {
        insert(data, i);
        return;
      }
    }
    push_back(data);
  }

  /** removes and returns the element at the given index
       */
  T pop(std::size_t index) {
    if (index >= size_) {
      throw std::out_of_range("invalid index");
    }
    auto data = contents[index];
    for (int i = index; i < size_-1; ++i) {
      contents[i] = contents[i + 1];
    }
    size_--;
    return data;
  }

  /** removes the last element from the list
       */
  T pop_back() {
    if (empty()) {
      throw std::out_of_range("invalid index");
    }
    size_--;
    return contents[size_];
  }

  /** removes the first element
       */
  T pop_front() { return pop(0); }

  /** removes the first appearance of the given element
       */
  void remove(const T& data) {
    for (int i = 0; i < size_; ++i) {
      if (contents[i] == data) {
        pop(i);
      }
    }
  }

  /** self-explanatory
       */
  bool full() const { return size_ == max_size_; }

  /** self-explanatory
       */
  bool empty() const { return size_ == 0; }

  /** returns true if the given element appears on the list
       */
  bool contains(const T& data) const {
    for (int i = 0; i < size_; ++i) {
      if (contents[i] == data) {
        return true;
      }
    }
    return false;
  }

  /** returns the index of the first appearance of the element in the list
       */
  std::size_t find(const T& data) const {
    for (int i = 0; i < size_; ++i) {
      if (contents[i] == data) {
        return i;
      }
    }
    return size_;
  }
  /** returns the current size of the list
       */
  std::size_t size() const { return size_; }
  /** returns the maximum size of the list
       */
  std::size_t max_size() const { return max_size_; }
  /** returns the element at the given index
       */
  inline T& at(std::size_t index) {
    //if (index >= size_) {
    //  throw std::out_of_range("invalid index");
    //}
    return contents[index];
  }
  /** returns the element at the given index
       */
  T& operator[](std::size_t index) {
    if (index >= size_) {
      throw std::out_of_range("invalid index");
    }
    return contents[index];
  }
  /** returns the element at the given index
       */
  inline const T& at(std::size_t index) const {
    //if (index >= size_) {
  //    throw std::out_of_range("invalid index");
  //  }
    return contents[index];
  }
  /** returns the element at the given index
       */
  const T& operator[](std::size_t index) const {
    if (index >= size_) {
      throw std::out_of_range("invalid index");
    }
    return contents[index];
  }

 private:
  T* contents;
  std::size_t size_;
  std::size_t max_size_;

  static const auto DEFAULT_MAX = 10u;
};
}  // namespace structures

#endif
