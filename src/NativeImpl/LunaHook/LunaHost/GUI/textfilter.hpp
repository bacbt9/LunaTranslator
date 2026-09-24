#pragma once
// Repetition filters for hooked text (pure std::wstring code, no Windows deps so it can be unit tested).
//
// Typical garbage produced by visual novel hooks:
//   repeated characters   ここここんんんんににちちはは   -> こんにちは   (every char drawn N times)
//   repeated phrases      今日は晴れ。今日は晴れ。       -> 今日は晴れ。 (line sent twice)
//   typewriter text       ここんこんにこんにちは         -> こんにちは   (each partial redraw captured)
#include <string>
#include <vector>
#include <cwctype>

namespace textfilter
{
    struct Config
    {
        bool repeatedChars = true;   // collapse "ここんんにに" -> "こんに"
        int charRepeatCount = 0;     // 0 = auto-detect, otherwise only this count
        int sensitivity = 80;        // % of the (non-space) text that must look repeated
        bool repeatedPhrases = true; // collapse adjacent duplicated blocks
        int minPhraseLength = 6;     // shorter blocks are left alone (わかったわかった stays)
        bool typewriter = false;     // collapse growing prefixes "ここんこんに" -> "こんに"
        bool duplicateLines = false; // drop a line identical to the previous line of the same hook
    };

    inline bool isSpace(wchar_t c)
    {
        return c == L'\r' || c == L'\n' || c == L'\t' || c == L' ' || c == L'\x3000';
    }

    // Repeated characters. Returns true if the text was changed.
    inline bool collapseRepeatedChars(std::wstring &s, int fixedCount, int sensitivity)
    {
        struct Run
        {
            wchar_t c;
            size_t len;
        };
        std::vector<Run> runs;
        size_t total = 0;
        for (size_t i = 0; i < s.size();)
        {
            size_t j = i;
            while (j < s.size() && s[j] == s[i])
                ++j;
            runs.push_back({s[i], j - i});
            if (!isSpace(s[i]))
                total += j - i;
            i = j;
        }
        if (total < 4)
            return false;

        int best = 0;
        int lo = fixedCount > 1 ? fixedCount : 2, hi = fixedCount > 1 ? fixedCount : 10;
        for (int n = lo; n <= hi; ++n)
        {
            size_t covered = 0;
            wchar_t first = 0;
            bool twoDifferent = false; // "……　……" is one char repeated, not doubled text
            for (auto &r : runs)
            {
                if (isSpace(r.c))
                    continue;
                if (r.len % n == 0)
                {
                    covered += r.len;
                    if (!first)
                        first = r.c;
                    else if (r.c != first)
                        twoDifferent = true;
                }
            }
            // largest count that still explains the text wins: all-4 runs are x4, not x2
            if (twoDifferent && covered * 100 >= total * (size_t)sensitivity)
                best = n;
        }
        if (!best)
            return false;

        std::wstring out;
        for (auto &r : runs)
        {
            size_t len = (!isSpace(r.c) && r.len % best == 0) ? r.len / best : r.len;
            out.append(len, r.c);
        }
        if (out == s)
            return false;
        s = std::move(out);
        return true;
    }

    // Adjacent duplicated blocks: "ABCABC" -> "ABC", "xxABCABCABCyy" -> "xxABCyy".
    // O(n^2): for each block length, one linear scan counting how many consecutive positions
    // satisfy s[k] == s[k + len]; a run of `len` such positions is a duplicated block.
    inline bool collapseRepeatedPhrases(std::wstring &s, int minLength)
    {
        if (minLength < 2)
            minLength = 2;
        if (s.size() > 2000) // real dialogue lines are far shorter; keeps the worst case bounded
            return false;
        bool any = false;
        // changes[q]: number of positions <= q whose char differs from the previous non-space char.
        // nextsolid[q]: first non-space position >= q. A block [i, i+len) has at least two different
        // non-space chars iff changes[end] - changes[first non-space] > 0 -> O(1) per check.
        std::vector<size_t> changes, nextsolid;
        auto recompute = [&]()
        {
            size_t n = s.size();
            changes.assign(n, 0);
            nextsolid.assign(n + 1, n);
            wchar_t last = 0;
            bool have = false;
            for (size_t q = 0; q < n; ++q)
            {
                size_t c = q ? changes[q - 1] : 0;
                if (!isSpace(s[q]))
                {
                    if (have && s[q] != last)
                        c += 1;
                    last = s[q];
                    have = true;
                }
                changes[q] = c;
            }
            for (size_t q = n; q-- > 0;)
                nextsolid[q] = isSpace(s[q]) ? nextsolid[q + 1] : q;
        };
        recompute();
        for (size_t len = s.size() / 2; len >= (size_t)minLength; --len)
        {
            size_t run = 0;
            for (size_t k = 0; k + len < s.size(); ++k)
            {
                run = (s[k] == s[k + len]) ? run + 1 : 0;
                if (run < len)
                    continue;
                size_t i = k + 1 - len; // s[i, i+len) == s[i+len, i+2len)
                // skip "…………" and friends: a block made of a single repeated char isn't a phrase
                size_t f = nextsolid[i];
                bool single = f >= i + len || changes[i + len - 1] == changes[f];
                if (single)
                {
                    run = len - 1; // keep sliding; the next position gets checked the same way
                    continue;
                }
                s.erase(i + len, len);
                recompute();
                any = true;
                // rescan this length from just before the removed copy
                k = (i > 0 ? i - 1 : 0);
                run = 0;
                if (i == 0)
                    k = (size_t)-1; // loop increment brings it back to 0
            }
            if (len > s.size() / 2)
                len = s.size() / 2 + 1; // string shrank; continue with the lengths that still fit
        }
        // "ABC\nABC" / "ABC ABC": identical blocks separated by whitespace
        for (bool changed = true; changed;)
        {
            changed = false;
            for (size_t i = 0; i < s.size() && !changed; ++i)
            {
                if (!isSpace(s[i]))
                    continue;
                size_t sepEnd = i;
                while (sepEnd < s.size() && isSpace(s[sepEnd]))
                    ++sepEnd;
                size_t len = i; // block before separator starts at 0 .. only whole-line case
                if (len >= (size_t)minLength && s.size() - sepEnd == len && s.compare(0, len, s, sepEnd, len) == 0)
                {
                    s.erase(i);
                    changed = any = true;
                }
            }
        }
        return any;
    }

    // Typewriter effect captured as growing prefixes: "こ" "こん" "こんに" -> "こんに".
    // Needs at least 2 partial copies with strictly increasing length before the full text.
    inline bool collapseTypewriter(std::wstring &s)
    {
        size_t n = s.size();
        if (n < 4 || n > 1000)
            return false;
        // the full sentence is the longest one, so try the latest possible start first
        for (size_t j = n - 1; j >= 1; --j)
        {
            if (s[j] != s[0])
                continue;
            size_t flen = n - j;
            size_t k = 0, prev = 0, segments = 0;
            bool ok = true;
            while (k < j)
            {
                size_t maxm = 0;
                while (k + maxm < j && maxm < flen && s[k + maxm] == s[j + maxm])
                    ++maxm;
                size_t pick = 0;
                for (size_t m = prev + 1; m <= maxm; ++m)
                    if (k + m == j || s[k + m] == s[0])
                    {
                        pick = m;
                        break;
                    }
                if (!pick || pick >= flen)
                {
                    ok = false;
                    break;
                }
                prev = pick;
                k += pick;
                ++segments;
            }
            if (ok && segments >= 2)
            {
                s = s.substr(j);
                return true;
            }
        }
        return false;
    }

    inline bool filterText(std::wstring &s, const Config &cfg)
    {
        bool changed = false;
        if (cfg.typewriter)
            changed |= collapseTypewriter(s);
        if (cfg.repeatedChars)
            changed |= collapseRepeatedChars(s, cfg.charRepeatCount, cfg.sensitivity);
        if (cfg.repeatedPhrases)
            changed |= collapseRepeatedPhrases(s, cfg.minPhraseLength);
        return changed;
    }
}
