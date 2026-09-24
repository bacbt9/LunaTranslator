// Unit tests for textfilter.hpp (not part of the build). Run anywhere with a C++17 compiler:
//   g++ -std=c++17 textfilter_test.cpp -o t && ./t
#include "textfilter.hpp"
#include <cstdio>
#include <locale>
#include <iostream>
#include <chrono>
using namespace textfilter;
int fails=0;
void chk(const wchar_t* in, const wchar_t* want, Config c={}){
  std::wstring s=in; filterText(s,c);
  bool ok = s==want; if(!ok) fails++;
  std::wcout<<(ok?L"ok   ":L"FAIL ")<<in<<L" -> "<<s<<(ok?L"":L"   want: "+std::wstring(want))<<L"\n";
}
int main(){
  std::locale::global(std::locale("C.UTF-8")); std::wcout.imbue(std::locale());
  Config tw; tw.typewriter=true;
  // repeated chars
  chk(L"ここここんんんんににににちちちちはははは",L"こんにちは");
  chk(L"ここんんににちちはは",L"こんにちは");
  chk(L"ああああ、、そそううだだねね",L"ああ、そうだね");   // legit ああ kept
  chk(L"「「ははいい」」\nききみみはは？？",L"「はい」\nきみは？");
  chk(L"aaaaaaaaaaaabbbbbbcccdddaabbbcccddd",L"aaaabbcdaabcd"); // mixed: mostly not explained
  // normal text untouched
  chk(L"ああ、そうだね。",L"ああ、そうだね。");
  chk(L"……",L"……");
  chk(L"「…………」",L"「…………」");
  chk(L"ははは、それはいい！",L"ははは、それはいい！");
  chk(L"うんうん",L"うんうん");
  chk(L"わかったわかった",L"わかったわかった");
  chk(L"ねえ、ねえ、聞いてよ",L"ねえ、ねえ、聞いてよ");
  chk(L"This is a normal sentence. はい",L"This is a normal sentence. はい");
  chk(L"",L""); chk(L" ",L" ");
  // phrases
  chk(L"今日はいい天気ですね。今日はいい天気ですね。",L"今日はいい天気ですね。");
  chk(L"今日はいい天気ですね。今日はいい天気ですね。今日はいい天気ですね。",L"今日はいい天気ですね。");
  chk(L"【春香】今日はいい天気ですね。今日はいい天気ですね。",L"【春香】今日はいい天気ですね。");
  chk(L"今日はいい天気ですね。\n今日はいい天気ですね。",L"今日はいい天気ですね。");
  // typewriter
  chk(L"ここんこんにこんにちこんにちは",L"こんにちは",tw);
  chk(L"ねえ、ねえ、聞いてよ",L"ねえ、ねえ、聞いてよ",tw);
  chk(L"ああ、そうだね。",L"ああ、そうだね。",tw);
  // chars + phrase combined
  chk(L"ああいいううええおおかかききああいいううええおおかかきき",L"あいうえおかき");
  chk(L"「「そそれれでではは、、行行ここうう」」「「そそれれでではは、、行行ここうう」」",L"「それでは、行こう」");
  // timing: worst cases must stay fast (they used to take seconds)
  {
    Config all; all.typewriter=true; all.duplicateLines=true;
    std::vector<std::wstring> nasty={std::wstring(2000,L'…'), std::wstring(5000,L'…')};
    std::wstring alt; for(int i=0;i<1000;i++) alt+=L"あい"; nasty.push_back(alt);
    std::wstring mixed; for(int i=0;i<300;i++) mixed+=L"…… …"; nasty.push_back(mixed);
    std::wstring rnd; unsigned x=1; for(int i=0;i<2000;i++){x=x*1103515245+12345; rnd+=wchar_t(0x3042+(x>>16)%80);} nasty.push_back(rnd);
    for(auto &t:nasty){
      auto a=std::chrono::steady_clock::now(); auto c=t; filterText(c,all);
      auto ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-a).count();
      bool ok=ms<200; if(!ok) fails++;
      std::wcout<<(ok?L"ok   ":L"FAIL ")<<L"timing len="<<t.size()<<L" -> "<<ms<<L" ms\n";
    }
  }
  chk(L"今日はいい天気ですね。今日はいい天気ですね。……………………",L"今日はいい天気ですね。……………………");
  chk(L"……　……　……　……",L"……　……　……　……");
  std::wcout<<fails<<L" failures\n"; return fails;
}
