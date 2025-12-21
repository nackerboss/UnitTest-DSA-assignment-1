#pragma once
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <vector>

/*** Basic diff utility.
     Valid range for difference is 32 bit.
     only shows line differences.
***/

namespace bsdiff
{
    // ANSI color for outputing.
    constexpr const char *RED = "\033[31m";
    constexpr const char *GREEN = "\033[32m";
    constexpr const char *RESET = "\033[0m";

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

        size_t n_full = A.size();
        size_t m_full = B.size();

        size_t prefix_idx = 0;
        while (prefix_idx < n_full && prefix_idx < m_full && A[prefix_idx] == B[prefix_idx])
        {
            diff_buffer.emplace_back(Operation::KEEP, A[prefix_idx]);
            prefix_idx++;
        }

        if (prefix_idx == n_full && prefix_idx == m_full)
        {
            return;
        }

        int n = static_cast<int>(n_full - prefix_idx);
        int m = static_cast<int>(m_full - prefix_idx);

        size_t max_d = n + m;
        std::vector<int> v(2 * max_d + 1);
        std::vector<std::vector<int>> trace;

        // we shift from k in [-d,d] to [0,2d] to avoid negative indexing.
        v[1 + max_d] = 0;

        for (int d = 0; d <= max_d; d++)
        {
            trace.push_back(v);
            for (int k = -d; k <= d; k += 2)
            {
                int x = 0;

                if (k == -d || (k != d && v[(k - 1) + max_d] < v[(k + 1) + max_d]))
                {
                    // we go down from k+1 so x doesnt change.
                    // correspond to a vertical move.
                    x = v[k + 1 + max_d];
                }
                // otherwise if we go from k-1 we have to increase v[k-1+max_d] by 1.
                // correspond to a horizontal move.
                else
                {
                    x = v[k - 1 + max_d] + 1;
                }

                int y = x - k;
                while (x < n && y < m && A[prefix_idx + x] == B[prefix_idx + y])
                {
                    x++;
                    y++;
                }
                v[k + max_d] = x;
                if (x >= n && y >= m)
                {
                    goto backtrack;
                }
            }
        }

    backtrack:
        std::vector<Diff> mid_diff;
        mid_diff.reserve(n + m);
        int x = n;
        int y = m;

        for (int d = static_cast<int>(trace.size() - 1); d > 0; d--)
        {
            int k = x - y;
            int pre_k = 0;

            const std::vector<int> &pre_v = trace[d];

            if (k == -d || (k != d && pre_v[(k - 1) + max_d] < pre_v[(k + 1) + max_d]))
            {
                pre_k = k + 1;
            }
            else
            {
                pre_k = k - 1;
            }
            int pre_x = pre_v[pre_k + max_d];
            int pre_y = pre_x - pre_k;
            // if both (x,y) > (pre_x,pre_y) then we are in a diagonal since diagonal increase both of them.
            // and diagonal = KEEP.
            while (x > pre_x && y > pre_y)
            {
                mid_diff.emplace_back(Operation::KEEP, A[prefix_idx + x - 1]);
                x--;
                y--;
            }
            // an increase in y correspond to an insert.
            if (y > pre_y)
            {
                mid_diff.emplace_back(Operation::INSERT, B[prefix_idx + y - 1]);
            }
            else
            {
                mid_diff.emplace_back(Operation::DELETE, A[prefix_idx + x - 1]);
            }

            x = pre_x;
            y = pre_y;
        }
        // final diagonal trace.
        while (x > 0 && y > 0)
        {
            mid_diff.emplace_back(Operation::KEEP, A[prefix_idx + x - 1]);
            x--;
            y--;
        }

        diff_buffer.insert(diff_buffer.end(), mid_diff.rbegin(), mid_diff.rend());
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

    for (const Diff &d : mdiff)
    {
        if (d.op == Operation::KEEP)
        {
            std::cout << "  " << old_ln++ << " " << new_ln++ << "  " << d.text << "\n";
        }
        else if (d.op == Operation::DELETE)
        {
            std::cout << bsdiff::RED << "- " << old_ln++ << "    " << d.text << bsdiff::RESET << "\n";
        }
        else // INSERT
        {
            std::cout << bsdiff::GREEN << "+    " << new_ln++ << " " << d.text << bsdiff::RESET << "\n";
        }
    }
}
