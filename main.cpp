#ifndef TESTING
#define TESTING
#include "VectorStore.h"
#include "cstdlib"
#include "string"
#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>

#pragma region println
namespace format
{
   // horrible performance , but do the tricks
   inline void format_impl(std::ostringstream &oss, const std::string &fmt, size_t pos)
   {
      oss << fmt.substr(pos);
   }

   template <typename T, typename... Args>
   void format_impl(std::ostringstream &oss, const std::string &fmt, size_t pos, T &&value, Args &&...args)
   {
      size_t brace { fmt.find("{}", pos) };
      if (brace == std::string::npos)
      {
         oss << fmt.substr(pos);
         return;
      }

      oss << fmt.substr(pos, brace - pos) << std::forward<T>(value);
      format_impl(oss, fmt, brace + 2, std::forward<Args>(args)...);
   }

   template <typename... Args> std::string format(const std::string &fmt, Args &&...args)
   {
      std::ostringstream oss;
      format_impl(oss, fmt, 0, std::forward<Args>(args)...);
      return oss.str();
   }

}; // namespace format
//

template <typename... Args> void print(const std::string &fmt, Args &&...args)
{
   std::cout << format::format(fmt, args...);
}

template <typename T> void print(const T &value)
{
   std::cout << value;
}

inline void println()
{
   std::cout << "\n";
}
template <typename T> void println(const T &value)
{
   std::cout << value << "\n";
}

template <typename... Args> void println(const std::string &fmt, Args &&...args)
{
   std::cout << format::format(fmt, args...) << "\n";
}
#pragma endregion
using namespace std;
#pragma region compare string
template <typename GenericIt1, typename GerenicIt2, typename T, typename BinaryOp1, typename BinaryOp2>
T inner_product(GenericIt1 first1, GenericIt1 last1, GerenicIt2 first2, T init, BinaryOp1 op1, BinaryOp2 op2)
{
   while (first1 != last1)
   {
      init = std::move(op1(std::move(init), std::move(op2(*first1, *first2))));
      ++first1;
      ++first2;
   }
   return init;
}

template <typename InputIt, typename OutputIt, typename UnaryOp>
OutputIt transform(InputIt first1, InputIt last1, OutputIt d_first, UnaryOp unary_op)
{
   while (first1 != last1)
   {
      *d_first = unary_op(*first1); // no std::move , because we dont wants a bunch of stupid side effcts
      ++first1;
      ++d_first;
   }
   return d_first;
}

template <typename InputIt, typename OutputIt, typename UnaryOp>
OutputIt transform_n(InputIt first1, size_t n, OutputIt d_first, UnaryOp unary_op)
{
   for (size_t i {}; i < n; i++)
   {
      *d_first = std::move(unary_op(*first1));
      ++first1;
      ++d_first;
   }
   return d_first;
}

template <typename Container, typename OutputIt, typename UnaryOp>
OutputIt transform(const Container &a, OutputIt d_first, UnaryOp unary_op)
{
   return transform(begin(a), end(a), d_first, unary_op);
}
#pragma endregion

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
   void VectorStoreTest();
   void FileIOtest(string nums);
   void TestArrayList() {}
};

TestHelper::TestHelper(string folder)
{
   this->path = folder;
   this->inputFile = "";
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
   // median of the two
   return (cs + ls) * 100 / 2.0; // or: return std::min(std::max(cs, ls), std::max(...)) for true median
}
TestHelper::~TestHelper()
{
   // đối chiếu với ans
   this->ansfile = freopen(string(this->path + this->answerFile).c_str(), "r", stdin);
   std::stringstream ansBuffer;
   ansBuffer << cin.rdbuf();
   fclose(this->ansfile);

   this->outfile = freopen(string(this->path + this->outputFile).c_str(), "r", stdin);
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
   // int n1 = a.size(), n2 = b.size();

   // auto result = inner_product(a.data(), a.data() + min(a.size(), b.size()), // first string
   //     b.data(),                                                             // second string
   //     0,                                                                    // init value
   //     std::plus<>(),                                                        // combine accumulator
   //     [](char x, char y)
   //     {
   //        return x != y ? 1 : 0;
   //     } // per-char comparison
   // );
   //
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
void TestHelper::VectorStoreTest()
{
   this->outfile = freopen(string(this->path + this->outputFile).c_str(), "w", stdout);
   VectorStore *vs = new VectorStore(10, dummyEmbedding);

   vector<string> Holder {
      "ZQWIqRX",
      "Ki1P2Z",
      "XqR0PgY",
      "UOjRVPQ",
      "1Vdzx0R",
      "09FaACj",
      "AhrUJlm",
      "ljrG26S",
      "6GIOrG7",
      "Cy9SZ9o",
      "gUBCXiv",
      "eEK8DP1",
      "BBrUS0j",
      "lpiUcmu",
      "VcXqgLC",
      "VEF3XY1",
      "VI7pSwe",
      "XsPY7up",
      "lh8epKT",
      "jvesDcO",
      "1vqDFXP",
      "Z2mrkML",
      "mf5KjCh",
      "LjsKGek",
      "1e6zis9",
      "y5DSMjY",
      "bJrN4gw",
      "4msMUZo",
      "VKo5WSe",
      "qfUMtjZ",
      "njjqcYv",
      "BdDBEX5",
      "QIZjSQq",
      "VDRPmjb",
      "g8UTCl5",
      "DRLLV47",
      "M53A5Fc",
      "U9dDG4y",
      "Gm6TqsX",
      "hVUhkyI",
      "vJEREDy",
      "wlWuXSC",
      "DvtDEDg",
      "QT1W1Rm",
      "lfXWLsk",
      "YvKx1yv",
      "9iJ6Rvb",
      "M5sYNHf",
      "rPr6Ddd",
      "B5Ri8xi",
      "MOhcyYg",
      "gFQDPOW",
      "cbUYq2H",
      "fGp0u3G",
      "g1P4g3U",
      "WeHiwue",
      "JoE24S8",
      "GIXQGxk",
      "O7rhYbf",
      "mqrQVhw",
      "uaNneKQ",
      "oa7og91",
      "uXlFPEV",
      "2wfOn0S",
      "d3CSPm5",
      "u3ImpDT",
      "ZUQj7wA",
      "hdJGTH1",
      "wPUefTx",
      "4JrxR2t",
      "FTx4HQx",
      "VHHvahh",
      "NiJq128",
      "RfoIKXG",
      "af8L0my",
      "NUi5sBR",
      "wyAlLzD",
      "qHmQMh0",
      "Ga9WICT",
      "JF1Uk1c",
      "qmXMLvV",
      "x6vpY0S",
      "7d7Lz0I",
      "SWfDx4V",
      "ot9cO83",
      "R1TfKAC",
      "1DsOsSF",
      "c5NYsPs",
      "eaUmrse",
      "pI0aVtw",
      "GkZfGAY",
      "NcvX0Cg",
      "NpXDbvB",
      "am0HVcd",
      "LCMTrSZ",
      "Fa3HDVa",
      "s0cvHva",
      "EKGllw5",
      "RFkXCdp",
      "uYEnQMK",
   };

   int sample_size { 100 };
   println(sample_size);

   int par_size { sample_size };

   for (int i {}; i < par_size; i++)
   {
      vs->addText(Holder[i]);
   }

   cout << "vs->records.toString(func) : \n" << vs->records.toString(VectorRec2String) << "\n";

   println("");
   auto D = vs->preprocessing("lemonade");
   println("Test Vector: ");
   println(D->toString());
   println("");

   int *k = vs->topKNearest(*D, par_size, "euclidean");

   int nearest = (vs->findNearest(*D, "euclidean"));
   
   println("sorted indices: ");
   for (int i {}; i < par_size; i++)
   {
      print("{}, ", (k[i]));
   }
   double *n { new double[par_size] };
   
   for (int i {}; i < par_size; i++)
   {
      n[i] = vs->l2Distance(vs->getVector(i), *D);
   }
   // println();
   // for (int i = 0; i < par_size; i++)
   // {
   //    println(n[k[i]]);
   // }
   
   println();
   println();
   println("Unsorted Distance measure array: ");
   for (int i {}; i < par_size; i++)
   {
      print("{}, ", (n[i]));
   }
   
   println();
   println();
   println("nearest: {}", n[nearest]);
   delete[] k;
   delete D;
   delete vs;
   delete[] n;

   fclose(this->outfile);
}
void TestHelper::FileIOtest(string nums)
{
   string dir = this->path + "/FileIOTest/" + nums + "/" + this->inputFile; 
   FILE *inFIle = freopen(dir.c_str(),"r",stdin);
   int n,k,dim;
   cin>>n>>dim;
   vector<string> dictionary(n);
   for (int i = 0; i < n; i++)
   {
      cin>>dictionary[i];
   }
   string querry,metric;
   cin>>k>>metric;
   cin>>querry;
   VectorStore *vs = new VectorStore(dim,dummyEmbedding);
   for (int i = 0; i < n; i++)
   {
      vs->addText(dictionary[i]);
   }
   auto trans = vs->preprocessing(querry);
   fclose(inFIle);//fropen về console
   //int* res = vs->topKNearest(*trans,k,metric);
   //xử lí rồi output từ dưới
}
#pragma endregion
#endif

// bool compareint(int a, int b)
// {
//     return a>b;
// }

// void test_005() {
//     SinglyLinkedList<int> sll;
//     sll.add(10);
//     sll.add(20);
//     sll.add(30);
//     sll.removeAt(1);
//     cout << "test_005: " << sll.toString() << endl;
// }

int main()
{
   // Student can use this main function to do some basic testing
   TestHelper *test = new TestHelper("test");
   test->VectorStoreTest();
   delete test;
   // cout<<double(Hashing(31))/7.0;
   return 0;
}
