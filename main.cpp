#ifndef TESTING
#define TESTING
#include "VectorStore.h"
#include "cstdlib"
#include "string"
#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>

#pragma region funct pointer
int seed = 182367;
int prime = 33773;
int Hashing(int a)
{
   int res = a * seed;
   string tmp = to_string(res);
   for (size_t i = 0; i < tmp.size(); i++)
   {
      tmp.erase((i * 2) % tmp.size(), 1);
      tmp.insert(
          (i * 2) % tmp.size(), to_string((tmp[(i * i) % tmp.size()] - '0' + tmp[(i + 1) * (i) % tmp.size()] - '0')));
      i += tmp[i] - '0';
   }
   res = stoi(tmp) % prime;
   int res1 = 0;
   while (res > 10)
   {
      res = res / 10;
      res1 += res % 10;
   }
   return res1 + res;
}
SinglyLinkedList<float> *dummyEmbedding(const string &text)
{
   auto *vec = new SinglyLinkedList<float>();
   for (char c : text)
   {
      int hash = Hashing(c);
      vec->add(static_cast<float>(hash) / 1.8);
      vec->add(static_cast<float>(hash) / 2.0);
      vec->add(static_cast<float>(hash) / 3.6);
      vec->add(static_cast<float>(hash) / 4.9);
      vec->add(static_cast<float>(hash) / 6.3);
      vec->add(static_cast<float>(hash) / 6.6);
   }
   return vec;
}
string VectorRec2String(VectorStore::VectorRecord *&a)
{
   return a->vector->toString() + "\n";
}
#pragma endregion
#pragma region TestHelper
class TestHelper
{
 private:
   string path;
   string inputFile;
   string outputFile;
   string answerFile;
   FILE *outfile;
   FILE *ansfile;

 public:
   TestHelper(string folder);
   ~TestHelper();
   void checkCap();
   void VectorStoreTest(string nums);
   void FileIOtest(string nums);
   void TestArrayList() {}
};

TestHelper::TestHelper(string folder)
{
   this->path = folder;
   this->inputFile = "in.txt";
   this->outputFile = "out.txt";
   // this->outfile = freopen(this->outputFile.c_str(), "w", stdout);
   this->answerFile = "ans.txt";
}

double line_similarity(const std::string &a, const std::string &b)
{
   auto count_lines = [](const std::string &s)
   {
      return std::count(s.begin(), s.end(), '\n') + (!s.empty());
   };

   int n1 = count_lines(a);
   int n2 = count_lines(b);

   if (n1 != n2)
   {
      std::cerr << "Output and answers are missing " << (std::max(n1, n2) - std::min(n1, n2)) << " lines" << "\n";
   }
   if (n1 == 0 && n2 == 0)
      return 1.0; // both empty → 100% similar
   return static_cast<double>(std::min(n1, n2)) / std::max(n1, n2);
}

double hybrid_accuracy(const std::string &a, const std::string &b, int window = 2)
{
   int n = a.size(), m = b.size();
   int i = 0, j = 0;
   int matches = 0;

   while (i < n && j < m)
   {
      if (a[i] == b[j])
      {
         matches++;
         i++;
         j++;
      }
      else
      {
         bool fixed = false;
         for (int k = 1; k <= window && (i + k < n || j + k < m); k++)
         {
            if (i + k < n && a[i + k] == b[j])
            {
               i += k;
               fixed = true;
               break;
            }
            if (j + k < m && a[i] == b[j + k])
            {
               j += k;
               fixed = true;
               break;
            }
         }
         if (!fixed)
         {
            i++;
            j++;
         }
      }
   }
   return std::max(n, m) ? (double)matches / std::max(n, m) : 1.0;
}

double similarity(const std::string &a, const std::string &b)
{
   double cs = hybrid_accuracy(a, b);
   double ls = line_similarity(a, b);
   return (cs + ls) * 100 / 2.0;
}
TestHelper::~TestHelper()
{
   // đối chiếu với ans
   this->ansfile = freopen(string(this->path + this->answerFile).c_str(), "r", stdin);
   std::stringstream ansBuffer;
   ansBuffer << cin.rdbuf();
   fclose(this->ansfile);

   this->outfile = freopen(string(this->path + "/VectorStoreTest/test001/" + this->outputFile).c_str(), "r", stdin);
   std::stringstream outBuffer;
   outBuffer << cin.rdbuf();
   fclose(this->outfile);
   string b = outBuffer.str();
   string a = ansBuffer.str();

   auto normalize = [](std::string &s)
   {
      s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
   };
   normalize(a);
   normalize(b);

   for (size_t i = 0; i < std::min(a.size(), b.size()); i++)
   {
      if (a[i] != b[i])
      {
         std::cerr << "Mismatch at index:" << i << ", a=" << (int)(unsigned char)a[i]
                   << " b=" << (int)(unsigned char)b[i] << " in Ascii" << "\n";

         break;
      }
   }
   string content = to_string(similarity(a, b)) + "%";
   std::cerr << "Output and Test are " + content + " alike\n";
   freopen("content.txt", "w", stdout);
   println(content);
   // clean up repo
   /*string s("rm ");
   s+=this->outputFile;
   fclose(this->outfile);
   system(s.c_str());*/
}

void TestHelper::checkCap()
{
   ArrayList<int> *arr = new ArrayList<int>(1);
   arr->add(0, 0);
   cout << arr->toString() << ' ' << arr->capacity << '\n';
   arr->add(1, 1);
   cout << arr->toString() << ' ' << arr->capacity << '\n';
   arr->add(2, 2);
   cout << arr->toString() << ' ' << arr->capacity << '\n';
   arr->add(3, 3);
   cout << arr->toString() << ' ' << arr->capacity << '\n';
   arr->add(4, 4);
   cout << arr->toString() << ' ' << arr->capacity << '\n';
   arr->add(6);
   cout << arr->toString() << ' ' << arr->capacity << '\n';
   arr->add(5);
   cout << arr->toString() << ' ' << arr->capacity << '\n';
   arr->removeAt(3);
   cout << arr->toString() << ' ' << arr->capacity << '\n';
   arr->clear();
   cout << arr->toString() << ' ' << arr->capacity << '\n';
}

#include <random>
#include <string>

// generate a random string of given length from given charset
std::string random_string(
    size_t length, const std::string &charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789")
{
   static thread_local std::mt19937 rng(std::random_device {}());
   std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);

   std::string result;
   result.reserve(length);

   for (size_t i = 0; i < length; i++)
   {
      result.push_back(charset[dist(rng)]);
   }

   return result;
}
void TestHelper::VectorStoreTest(string nums)
{
   string dir = this->path + "/VectorStoreTest/" + nums + "/" + this->inputFile;
   FILE *inFIle = freopen(dir.c_str(), "r", stdin);
   int n, k, dim;
   cin >> n >> dim;
   vector<string> dictionary(n);
   for (int i = 0; i < n; i++)
   {
      cin >> dictionary[i];
   }
   string querry, metric;
   cin >> k >> metric;
   cin >> querry;
   VectorStore *vs = new VectorStore(dim, dummyEmbedding);
   for (int i = 0; i < n; i++)
   {
      vs->addText(dictionary[i]);
   }
   auto trans = vs->preprocessing(querry);
   fclose(inFIle); // fropen về console
   // int* res = vs->topKNearest(*trans,k,metric);
   // xử lí rồi output từ dưới
   dir = this->path + "/VectorStoreTest/" + nums + "/" + this->outputFile;
   this->outfile = freopen((dir).c_str(), "w", stdout);
   cout << "vs->records.toString(func) : \n" << vs->records.toString(VectorRec2String) << "\n";

   println("");
   auto D = vs->preprocessing(querry);
   println("Test Vector: ");
   println(D->toString());
   println("");

   int *idx = vs->topKNearest(*D, k, metric);

   int nearest = (vs->findNearest(*D, metric));

   println("sorted indices: ");
   for (int i {}; i < k; i++)
   {
      print("{}, ", (idx[i]));
   }
   double *res { new double[n] };

   for (int i {}; i < n; i++)
   {
      res[i] = vs->l2Distance(vs->getVector(i), *D);
   }

   println();
   println();
   println("Unsorted Distance measure array: ");
   for (int i {}; i < n; i++)
   {
      print("{}, ", (res[i]));
   }

   println();
   println();
   println("nearest: {}", res[nearest]);
   delete[] idx;
   delete D;
   delete vs;
   delete[] res;

   fclose(this->outfile);
}
void TestHelper::FileIOtest(string nums)
{
   string dir = this->path + "/FileIOTest/" + nums + "/" + this->inputFile;
   FILE *inFIle = freopen(dir.c_str(), "r", stdin);
   int n, k, dim;
   cin >> n >> dim;
   vector<string> dictionary(n);
   for (int i = 0; i < n; i++)
   {
      cin >> dictionary[i];
   }
   string querry, metric;
   cin >> k >> metric;
   cin >> querry;
   VectorStore *vs = new VectorStore(dim, dummyEmbedding);
   for (int i = 0; i < n; i++)
   {
      vs->addText(dictionary[i]);
   }
   auto trans = vs->preprocessing(querry);
   fclose(inFIle); // fropen về console
   // int* res = vs->topKNearest(*trans,k,metric);
   // xử lí rồi output từ dưới
}
#pragma endregion
#endif

int main()
{
   // Student can use this main function to do some basic testing
   TestHelper *test = new TestHelper("test/");
   test->VectorStoreTest("test001");
   delete test;
   return 0;
}
