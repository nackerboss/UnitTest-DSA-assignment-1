#pragma once
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <vector>

// Basic line diff utility.

// ANSI color for outputing.
constexpr const char *RED = "\033[31m";
constexpr const char *GREEN = "\033[32m";
constexpr const char *RESET = "\033[0m";

namespace bsdiff
{
   enum class Operation : uint8_t
   {
      KEEP,
      INSERT,
      DELETE
   };

   struct Diff
   {
      Operation op;
      std::string_view text;

      Diff(Operation op, std::string_view text)
          : op(op)
          , text(text)
      {
      }
   };

   // Takes a std::string_view buffer which preferably you read from a file.
   // split them line by line into a vector of string_view.
   inline std::vector<std::string_view> split_lines(std::string_view buffer)
   {
      std::vector<std::string_view> lines;
      // heuristic guess since UNIX line usually 80 characters long.
      lines.reserve(buffer.size() / 80 + 2);

      size_t start {};
      while (start <= buffer.size())
      {
         size_t end { buffer.find('\n', start) };
         if (end == std::string_view::npos)
         {
            end = buffer.size();
         }

         size_t len { end - start };
         if (len && buffer[start + len - 1] == '\r')
         {
            --len;
         }

         lines.emplace_back(buffer.data() + start, len);
         start = end + 1;

         if (end == buffer.size())
         {
            break;
         }
      }
      return lines;
   }
   // Take 2 vectors containing lines and a std::vector<Diff> note the order is reversed.
   // since vector has rbegin and rend skipping a std::reverse is what im choosing to do.
   inline void diff(
       const std::vector<std::string_view> &A, const std::vector<std::string_view> &B, std::vector<Diff> &diff_buffer)
   {
      if (diff_buffer.empty() == false)
      {
         diff_buffer.clear();
      }
      size_t n { A.size() };
      size_t m { B.size() };

      uint16_t *dp = new uint16_t[(n + 1) * (m + 1)];

      auto at = [&](size_t i, size_t j) -> uint16_t &
      {
         return dp[i * (m + 1) + j];
      };

      // Dp for Longest common subsequence.
      for (size_t i { 1 }; i <= n; i++)
      {
         std::string_view Ai = A[i - 1];
         for (size_t j { 1 }; j <= m; j++)
         {
            if (Ai == B[j - 1])
            {
               at(i, j) = at(i - 1, j - 1) + 1;
            }
            else
            {
               at(i, j) = std::max(at(i - 1, j), at(i, j - 1));
            }
         }
      }
      // Backtrack

      diff_buffer.reserve(n + m);

      int i { (int)n }; // just to be safe.
      int j { (int)m };

      while (i > 0 || j > 0)
      {
         if (i > 0 && j > 0 && A[i - 1] == B[j - 1])
         {
            diff_buffer.emplace_back(Operation::KEEP, A[i - 1]);
            --i;
            --j;
         }
         else if (j > 0 && (i == 0 || at(i, j - 1) >= at(i - 1, j)))
         {
            diff_buffer.emplace_back(Operation::INSERT, B[j - 1]);
            --j;
         }
         else
         {
            diff_buffer.emplace_back(Operation::DELETE, A[i - 1]);
            --i;
         }
      }
      delete[] dp;
   }
} // namespace bsdiff

// binding class LDiff
class LDiff
{
   using Diff = bsdiff::Diff;
   using Operation = bsdiff::Operation;

 private:
   std::vector<Diff> mdiff;
   std::vector<std::string_view> morg;
   std::vector<std::string_view> mnew;

 public:
   LDiff(std::string_view A, std::string_view B) noexcept;

 public:
   void print() const;
};

inline LDiff::LDiff(std::string_view A, std::string_view B) noexcept
    : morg { bsdiff::split_lines(A) }
    , mnew { bsdiff::split_lines(B) }
{
   bsdiff::diff(morg, mnew, mdiff);
}

inline void LDiff::print() const
{
   size_t old_ln = 1;
   size_t new_ln = 1;

   for (auto it = mdiff.rbegin(); it != mdiff.rend(); ++it)
   {
      const Diff &d = *it;

      if (d.op == Operation::KEEP)
      {
         std::cout << "  " << old_ln++ << " " << new_ln++ << "  " << d.text << "\n";
      }
      else if (d.op == Operation::DELETE)
      {
         std::cout << RED << "- " << old_ln++ << "    " << d.text << RESET << "\n";
      }
      else // INSERT
      {
         std::cout << GREEN << "+    " << new_ln++ << " " << d.text << RESET << "\n";
      }
   }
}
