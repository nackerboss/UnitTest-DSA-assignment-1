#include "VectorStore.h"
// ----------------- ArrayList Implementation -----------------
// nested array and linkedlist for debugging and checking
#define INSTANTIATE_LIST_NESTED(T)                                                                                     \
   template class ArrayList<ArrayList<T>>;                                                                             \
   template class ArrayList<ArrayList<ArrayList<T>>>;                                                                  \
   template class ArrayList<ArrayList<ArrayList<ArrayList<T>>>>;                                                       \
   template class ArrayList<ArrayList<ArrayList<ArrayList<ArrayList<T>>>>>;                                            \
   template class SinglyLinkedList<SinglyLinkedList<T>>;                                                               \
   template class SinglyLinkedList<SinglyLinkedList<SinglyLinkedList<T>>>;                                             \
   template class SinglyLinkedList<SinglyLinkedList<SinglyLinkedList<SinglyLinkedList<T>>>>;                           \
   template class SinglyLinkedList<SinglyLinkedList<SinglyLinkedList<SinglyLinkedList<SinglyLinkedList<T>>>>>;

template <typename T>
ArrayList<T>::ArrayList(int initCapacity)
    : data { (T *)::operator new(initCapacity * sizeof(T)) }
    , capacity { initCapacity }
    , count {}
{
}

template <typename T>
ArrayList<T>::ArrayList(const ArrayList<T> &other) noexcept(std::is_nothrow_copy_constructible_v<T>)
    : data { (T *)::operator new(other.size() * sizeof(T)) }
    , capacity { other.capacity }
    , count { other.size() }
{
   for (int i {}; i < count; i++)
   {
      new (&this->data[i]) T { other.data[i] };
   }
}

template <typename T>
ArrayList<T> &ArrayList<T>::operator=(const ArrayList<T> &other)

    noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>)
{
   if (this != &other)
   {
      for (T &data : *this)
      {
         data.~T();
      }
      ::operator delete(data);

      this->count = other.count;
      this->capacity = other.capacity;
      this->data = (T *)::operator new(capacity * sizeof(T));

      for (int i {}; i < this->count; i++)
      {
         new (&this->data[i]) T { other.data[i] };
      }
   }
   return *this;
}
template <typename T> void ArrayList<T>::clear() noexcept(std::is_nothrow_destructible_v<T>)
{
   if (this->data != nullptr)
   {
      for (T &data : *this)
      {
         data.~T();
      }

      ::operator delete(data);

      this->count = 0;
      this->capacity = 10;
      this->data = (T *)::operator new(capacity * sizeof(T));
   }
}

template <typename T> ArrayList<T>::~ArrayList() noexcept
{
   for (T &data : *this)
   {
      data.~T();
   }
   ::operator delete(this->data);
}

template <typename T> void ArrayList<T>::ensureCapacity(int cap)
{
   if (cap >= this->capacity)
   {
      this->capacity = capacity * 1.5 < cap ? cap : capacity * 1.5;

      T *new_data { (T *)::operator new(capacity * sizeof(T)) };

      for (int i {}; i < this->count; i++)
      {
         new (&new_data[i]) T { std::move(data[i]) };
      }

      for (T &data : *this)
      {
         data.~T(); // too scared to use delete[] with placement new ngl
      }
      ::operator delete(this->data);
      this->data = new_data;
   }
}

template <typename T> void ArrayList<T>::add(T e)
{
   ensureCapacity(this->count + 1);
   return add(this->count, e);
}

template <typename T> void ArrayList<T>::add(int index, T e)
{
   if (index > count || index < 0)
   {
      throw std::out_of_range("Index is invalid!");
   }

   ensureCapacity(count + 1);

   std::move_backward(&data[index], &data[count], &data[count + 1]);
   new (&data[index]) T { std::move(e) };

   this->count++;
}

template <typename T> T ArrayList<T>::removeAt(int index)
{
   if (index >= count || index < 0)
   {
      throw std::out_of_range("Index is invalid!");
   }

   T tmp { std::move(this->data[index]) };

   if (index < count - 1)
   {
      std::move(&data[index + 1], &data[count], &data[index]);
   }

   this->data[count - 1].~T();
   this->count--;

   return tmp;
}

template <typename T> T &ArrayList<T>::get(int index)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }
   return this->data[index];
}

template <typename T> void ArrayList<T>::set(int index, T e)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }
   this->data[index] = std::move(e);
}

template <typename T> int ArrayList<T>::indexOf(T e) const
{
   for (int i {}; i < this->count; i++)
   {
      if (this->data[i] == e)
      {
         return i;
      }
   }
   return -1;
}

template <typename T> std::string ArrayList<T>::toString(std::string (*item2str)(T &)) const
{
   std::ostringstream oss; // support only output,  much much much much less heavier than ss
   oss << "[";

   for (int i {}; i < count; i++)
   {
      if (i)
      {
         oss << ", ";
      }
      item2str ? (oss << item2str(data[i])) : (oss << data[i]);
   }

   oss << "]";
   return oss.str();
}

template <typename T> bool ArrayList<T>::contains(T e) const
{
   return (indexOf(e) != -1);
}

// ----------------- Iterator of ArrayList Implementation -----------------
template <typename T>
ArrayList<T>::Iterator::Iterator(ArrayList<T> *pList, int index)
    : cursor { index }
    , pList { pList }
{
   if (cursor < 0 || cursor > this->pList->count)
   {
      throw std::out_of_range("Iterator cannot advance past end!");
   }
}

template <typename T> typename ArrayList<T>::Iterator &ArrayList<T>::Iterator::operator=(const Iterator &other) noexcept
{
   if (this != &other)
   {
      this->cursor = other.cursor;
      this->pList = other.pList;
   }
   return *this;
}

template <typename T> T &ArrayList<T>::Iterator::operator*()
{
   if (this->cursor >= this->pList->count || this->pList == nullptr)
   {
      throw std::out_of_range("Iterator is out of range");
   }
   return this->pList->data[this->cursor];
}

template <typename T> bool ArrayList<T>::Iterator::operator!=(const Iterator &other) const noexcept
{
   return this->cursor != other.cursor || this->pList != other.pList;
}

template <typename T> typename ArrayList<T>::Iterator &ArrayList<T>::Iterator::operator++()
{
   if (this->cursor == this->pList->count || this->pList == nullptr)
   {
      throw std::out_of_range("Iterator cannot advance past end!");
   }
   this->cursor++;
   return *this;
}

template <typename T> typename ArrayList<T>::Iterator ArrayList<T>::Iterator::operator++(int)
{
   if (this->cursor == this->pList->count || this->pList == nullptr)
   {
      throw std::out_of_range("Iterator cannot advance past end!");
   }
   Iterator temp { *this };
   this->cursor++;
   return temp;
}

template <typename T> typename ArrayList<T>::Iterator &ArrayList<T>::Iterator::operator--()
{
   if (this->cursor == 0 || this->pList == nullptr)
   {
      throw std::out_of_range("Iterator cannot move before begin!");
   }
   this->cursor--;
   return *this;
}

template <typename T> typename ArrayList<T>::Iterator ArrayList<T>::Iterator::operator--(int)
{
   if (cursor == 0 || this->pList == nullptr)
   {
      throw std::out_of_range("Iterator cannot move before begin!");
   }
   Iterator temp { *this };
   this->cursor--;
   return temp;
}

template <typename T> typename ArrayList<T>::Iterator ArrayList<T>::begin() noexcept
{
   return Iterator { this, 0 };
}

template <typename T> typename ArrayList<T>::Iterator ArrayList<T>::end() noexcept
{
   return Iterator { this, count };
}

template <typename T> typename ArrayList<T>::Iterator ArrayList<T>::begin() const noexcept
{
   return Iterator { this, 0 };
}

template <typename T> typename ArrayList<T>::Iterator ArrayList<T>::end() const noexcept
{
   return Iterator { this, count };
}

template <typename T> T &ArrayList<T>::operator[](int index)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index out of range");
   }
   return this->data[index];
}

template <typename T> const T &ArrayList<T>::operator[](int index) const
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index out of range");
   }
   return this->data[index];
}

template <typename T> typename ArrayList<T>::Iterator &ArrayList<T>::Iterator::operator+=(int n)
{
   if (-n > this->cursor)
   {
      throw std::out_of_range("Iterator cannot move before begin!");
   }

   if (this->cursor + n > this->pList->count)
   {
      throw std::out_of_range("Iterator cannot advance past end!");
   }
   this->cursor += n;
   return *this;
}

template <typename T> typename ArrayList<T>::Iterator ArrayList<T>::Iterator::operator+(int n) const
{
   Iterator temp = { *this };
   temp += n;
   return temp;
}

template <typename T> typename ArrayList<T>::Iterator &ArrayList<T>::Iterator::operator-=(int n)
{
   return *this += (-n);
}

template <typename T> typename ArrayList<T>::Iterator ArrayList<T>::Iterator::operator-(int n) const
{
   Iterator temp = { *this };
   temp -= n;
   return temp;
}

template <typename T> bool ArrayList<T>::Iterator::operator==(const Iterator &other) const noexcept
{
   return this->pList == other.pList && this->cursor == other.cursor;
}

template <typename T> int ArrayList<T>::Iterator::operator-(const Iterator &other) const
{
   if (this->pList != other.pList)
   {
      throw std::invalid_argument("Iterators from different ArrayLists cannot be subtracted!");
   }
   return this->cursor - other.cursor;
}

template <typename T> bool ArrayList<T>::Iterator::operator<(const Iterator &other) const noexcept
{
   return this->cursor < other.cursor;
}

template <typename T> bool ArrayList<T>::Iterator::operator<=(const Iterator &other) const noexcept
{
   return this->cursor <= other.cursor;
}

template <typename T> bool ArrayList<T>::Iterator::operator>(const Iterator &other) const noexcept
{
   return this->cursor > other.cursor;
}

template <typename T> bool ArrayList<T>::Iterator::operator>=(const Iterator &other) const noexcept
{
   return this->cursor >= other.cursor;
}

template <typename T> T &ArrayList<T>::Iterator::operator[](int n) const
{
   int index { this->cursor + n };
   if (index < 0 || index >= this->pList->count)
   {
      throw std::out_of_range("Iterator index out of range!");
   }
   return (*this->pList)[index];
}

// TODO: implement other methods of ArrayList::Iterator

// ----------------- SinglyLinkedList Implementation -----------------

template <typename T>
SinglyLinkedList<T>::SinglyLinkedList() noexcept
    : head { nullptr }
    , tail { nullptr }
    , count {}
{
}

template <typename T> SinglyLinkedList<T>::~SinglyLinkedList() noexcept
{
   clear();
}

template <typename T> void SinglyLinkedList<T>::add(int index, T e)
{
   if (index < 0 || index > count)
   {
      throw std::out_of_range("Index is invalid!");
   }

   if (index == this->count)
   {
      return add(e);
   }

   Node **prev { &this->head };

   for (int i {}; i < index; i++)
   {
      prev = &(*prev)->next;
   }

   *prev = new Node { std::move(e), *prev };

   this->count++;
}

template <typename T> void SinglyLinkedList<T>::add(T e)
{
   Node *tmp { new Node { e } };
   this->count++;

   if (tail)
   {
      this->tail->next = tmp;
      this->tail = tmp;
      return;
   }
   this->head = this->tail = tmp;
}

template <typename T> T SinglyLinkedList<T>::removeAt(int index)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }

   Node **prev { &head };

   for (int i {}; i < index; i++)
   {
      prev = &(*prev)->next;
   }

   Node *Deleted { *prev };
   *prev = Deleted->next;

   if (Deleted == this->tail)
   {
      this->tail = *prev;
   }

   T data { std::move(Deleted->data) };
   delete Deleted;

   this->count--;

   return data;
}

template <typename T> bool SinglyLinkedList<T>::removeItem(T e)
{
   Node *current { this->head };

   for (int i {}; i < this->count; i++)
   {
      if (current->data == e)
      {
         removeAt(i);
         return true;
      }
      current = current->next;
   }

   return false;
}

template <typename T> bool SinglyLinkedList<T>::contains(T e) const
{
   Node *current { this->head };

   for (int i {}; i < this->count; i++)
   {
      if (current->data == e)
      {
         return true;
      }
      current = current->next;
   }
   return false;
}

template <typename T> T &SinglyLinkedList<T>::get(int index)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }

   Node *current { this->head };

   for (int i {}; i < (index); i++)
   {
      current = current->next;
   }

   return current->data;
}

template <typename T> void SinglyLinkedList<T>::clear() noexcept(std::is_nothrow_destructible_v<T>)
{
   Node *current { this->head };
   while (current != nullptr)
   {
      Node *next = current->next;
      delete current;
      current = next;
   }
   this->head = nullptr;
   this->tail = nullptr;
}

template <typename T> string SinglyLinkedList<T>::toString(string (*item2str)(T &)) const
{
   if (this->head == nullptr)
   {
      return "[]";
   }

   std::ostringstream oss;
   Node *current { this->head };

   while (current != nullptr)
   {
      oss << "[";
      if (item2str)
      {
         oss << item2str(current->data);
      }
      else
      {
         oss << current->data;
      }
      oss << "]";
      if (current->next != nullptr)
      {
         oss << "->";
      }
      current = current->next;
   }

   return oss.str();
}

template <typename T>
SinglyLinkedList<T>::Iterator::Iterator(Node *node) noexcept
    : current { node }
{
}

template <typename T>
typename SinglyLinkedList<T>::Iterator &SinglyLinkedList<T>::Iterator::operator=(const Iterator &other) noexcept
{
   if (this != &other)
   {
      this->current = other.current;
   }
   return *this;
}

template <typename T> T &SinglyLinkedList<T>::Iterator::operator*()
{
   if (current == nullptr)
   {
      throw std::out_of_range("Iterator is out of range!");
   }
   return current->data;
}

template <typename T> bool SinglyLinkedList<T>::Iterator::operator!=(const Iterator &other) const noexcept
{
   return current != other.current;
}

template <typename T> bool SinglyLinkedList<T>::Iterator::operator==(const Iterator &other) const noexcept
{
   return current == other.current;
}
template <typename T> typename SinglyLinkedList<T>::Iterator &SinglyLinkedList<T>::Iterator::operator++()
{
   if (current == nullptr)
   {
      throw std::out_of_range("Iterator cannot advance past end!");
   }
   current = current->next;
   return *this;
}

template <typename T> typename SinglyLinkedList<T>::Iterator SinglyLinkedList<T>::Iterator::operator++(int)
{
   Iterator temp { *this };
   ++(*this);
   return temp;
}

template <typename T> typename SinglyLinkedList<T>::Iterator SinglyLinkedList<T>::begin() const noexcept
{
   return Iterator { head };
}

template <typename T> typename SinglyLinkedList<T>::Iterator SinglyLinkedList<T>::end() const noexcept
{
   return Iterator { nullptr };
}

// ----------------- VectorStore Implementation -----------------

VectorStore::VectorRecord::~VectorRecord() noexcept
{
   delete vector;
   vector = nullptr;
}

VectorStore::VectorRecord::VectorRecord(int id, const string &rawText, SinglyLinkedList<float> *vector) noexcept
    : id { id }
    , rawText { rawText }
    , rawLength { static_cast<int>(rawText.size()) }
    , vector { vector }
{
}

VectorStore::VectorStore(int dimension, EmbedFn embeddingFunction) noexcept
    : records {}
    , dimension { dimension }
    , count {}
    , embeddingFunction { embeddingFunction }
{
}

VectorStore::~VectorStore() noexcept
{
   for (VectorRecord *record : this->records)
   {
      delete record;
   }
}

void VectorStore::clear() noexcept
{
   for (VectorRecord *record : this->records)
   {
      delete record;
   }
   this->records.clear();
   this->count = 0;
}

SinglyLinkedList<float> *VectorStore::preprocessing(string rawText) const noexcept
{
   if (embeddingFunction == nullptr)
   {
      return nullptr;
   }

   SinglyLinkedList<float> *vec { embeddingFunction(rawText) };

   while (vec->size() > dimension)
   {
      vec->removeAt(vec->size() - 1);
   }

   while (vec->size() < dimension)
   {
      vec->add(0.0f);
   }

   return vec;
}

void VectorStore::addText(string rawText)
{
   SinglyLinkedList<float> *vec { preprocessing(rawText) };

   int id { this->empty() ? 1 : this->records[count - 1]->id + 1 };

   this->records.add(new VectorRecord { id, rawText, vec });

   this->count++;
}

SinglyLinkedList<float> &VectorStore::getVector(int index) const
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }
   return *(this->records[index]->vector);
}

std::string VectorStore::getRawText(int index) const
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }
   return this->records[index]->rawText;
}

int VectorStore::getId(int index) const
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }
   return this->records[index]->id;
}

void VectorStore::setEmbeddingFunction(VectorStore::EmbedFn newEmbeddingFunction) noexcept
{
   this->embeddingFunction = newEmbeddingFunction;
}

bool VectorStore::removeAt(int index)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }
   delete records[index];

   this->records.removeAt(index);
   this->count--;

   return true;
}

bool VectorStore::updateText(int index, string newRawText)
{
   if (index < 0 || index >= count)
   {
      throw std::out_of_range("Index is invalid!");
   }

   if (embeddingFunction == nullptr)
   {
      return false;
   }

   delete this->records[index];

   this->records[index]->rawText = std::move(newRawText);
   this->records[index]->rawLength = static_cast<int>(newRawText.size());
   this->records[index]->vector = preprocessing(newRawText);

   return true;
}

void VectorStore::forEach(void (*action)(SinglyLinkedList<float> &, int, string &)) noexcept
{
   if (action == nullptr)
   {
      return;
   }

   for (VectorRecord *record : this->records)
   {
      action(*record->vector, record->id, record->rawText);
   }
}

double VectorStore::cosineSimilarity(const SinglyLinkedList<float> &v1, const SinglyLinkedList<float> &v2) const
{
   if (v1.size() != v2.size() || v1.size() == 0 || v2.size() == 0)
   {
      return 0.0;
   }
   double dot { algorithms::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0) };

   if (dot == 0.0)
   {
      return 0.0;
   }

   double norm1 { algorithms::norm(v1.begin(), v1.end(), 0.0) };

   double norm2 { algorithms::norm(v2.begin(), v2.end(), 0.0) };

   return dot / std::sqrt(norm1 * norm2);
}

double VectorStore::l1Distance(const SinglyLinkedList<float> &v1, const SinglyLinkedList<float> &v2) const
{
   return algorithms::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0,
       [](float a, float b)
       {
          return std::fabs(static_cast<double>(a) - b);
       });
}

double VectorStore::l2Distance(const SinglyLinkedList<float> &v1, const SinglyLinkedList<float> &v2) const
{
   return std::sqrt(algorithms::inner_product(v1.begin(), v1.end(), v2.begin(), 0.0, std::plus<double> {},
       [](float a, float b)
       {
          return (static_cast<double>(a) - b) * (static_cast<double>(a) - b);
       }));
}

int VectorStore::findNearest(const SinglyLinkedList<float> &query, const string &metric) const
{
   if (metric != "cosine" && metric != "manhattan" && metric != "euclidean")
   {
      throw invalid_metric();
   }

   int metricId = algorithms::accumulate(metric.begin(), metric.end(), 0,
       [](int acc, char c)
       {
          return acc + static_cast<unsigned char>(c);
       });

   auto scoreFn = [&](const SinglyLinkedList<float> &vec) -> double
   {
      switch (metricId)
      {
      case 641:
         return l2Distance(query, vec);
      case 938:
         return l2Distance(query, vec);
      }
      return l1Distance(query, vec);
   };

   std::pair<int, double> best { 0, scoreFn(*records[0]->vector) };

   best = algorithms::accumulate(this->records, best,
       [&, index = 0](const std::pair<int, double> &acc, const VectorRecord *rec) mutable -> std::pair<int, double>
       {
          double score { scoreFn(*rec->vector) };
          int currentIndex { index++ };

          if ((metricId == 641 && score > acc.second) || (metricId != 641 && score < acc.second))
          {
             return { currentIndex++, score };
          }
          return acc;
       });

   return best.first;
}

int *VectorStore::topKNearest(const SinglyLinkedList<float> &query, int k, const string &metric) const
{
   if (metric != "cosine" && metric != "manhattan" && metric != "euclidean")
   {
      throw invalid_metric();
   }
   if (k <= 0 || k > count)
   {
      throw invalid_k_value();
   }

   int metricId = algorithms::accumulate(metric.begin(), metric.end(), 0,
       [](int acc, char c)
       {
          return acc + static_cast<unsigned char>(c);
       });

   auto scoreFn = [&](const SinglyLinkedList<float> &vec) -> double
   {
      switch (metricId)
      {
      case 641:
         return l2Distance(query, vec);
      case 938:
         return l2Distance(query, vec);
      }
      return l1Distance(query, vec);
   };

   ArrayList<std::pair<int, double>> scores(count, { 0, 0.0 });

   algorithms::transform(this->records, scores.begin(),
       [&, index = 0](const VectorRecord *a) mutable -> std::pair<int, double>
       {
          return { index++, scoreFn(*a->vector) };
       });

   algorithms::stable_sort(scores.begin(), scores.end(),
       [&](const std::pair<int, double> &a, const std::pair<int, double> &b)
       {
          if (metricId == 641)
          {
             return a.second > b.second;
          }
          return a.second < b.second;
       });

   int *result { new int[k] };

   algorithms::transform_n(scores.begin(), k, result,
       [](auto &p)
       {
          return p.first;
       });

   return result;
}
// TODO: implement other methods of VectorStore

// Explicit template instantiation for char, string, int, double, float, and
// Point

template class ArrayList<char>;
template class ArrayList<string>;
template class ArrayList<int>;
template class ArrayList<double>;
template class ArrayList<float>;
template class ArrayList<Point>;

template class SinglyLinkedList<char>;
template class SinglyLinkedList<string>;
template class SinglyLinkedList<int>;
template class SinglyLinkedList<double>;
template class SinglyLinkedList<float>;
template class SinglyLinkedList<Point>;

template std::string ArrayList<VectorStore::VectorRecord *>::toString(
    std::string (*)(VectorStore::VectorRecord *&)) const;

// INSTANTIATE_LIST_NESTED(char)
// INSTANTIATE_LIST_NESTED(string)
// INSTANTIATE_LIST_NESTED(int)
// INSTANTIATE_LIST_NESTED(double)
// INSTANTIATE_LIST_NESTED(float)
