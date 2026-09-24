// Unit tests for textfilter.hpp (not part of the build). Run anywhere with a C++17 compiler:
//   g++ -std=c++17 textfilter_test.cpp -o t && ./t
#include "textfilter.hpp"
#include <cstdio>
#include <locale>
#include <iostream>
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
  printf("%d failures\n",fails); return fails;
}
