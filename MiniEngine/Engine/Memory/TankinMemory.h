#pragma once

#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

//Because of Genshin Impact -> OP
//Therefore Tankin Impact -> TP

#ifdef WIN32
#define UNICODE
#else
#endif


typedef std::string TpString;

// Containers
template<class T>
using TpVector = std::vector<T>;

template<class T>
using TpList = std::list<T>;

template<class T, class K>
using TpPair = std::pair<T, K>;

template<class Key, class Value, class Comparer = std::less<Key>>
using TpMap = std::map<Key, Value, Comparer>;

template<class Key, class Value>
using TpUnorderedMap = std::unordered_map<Key, Value>;

template<class Key, class Value, class Comparer = std::less<Key>>
using TpMultiMap = std::multimap<Key, Value, Comparer>;

template<class T, class Comparer = std::less<T>>
using TpSet = std::set<T, Comparer>;

template<class T>
using TpUnorderedSet = std::unordered_set<T>;

template<class T, class Comparer = std::less<T>>
using TpMultiSet = std::multiset<T, Comparer>;

template<class T>
using TpQueue = std::queue<T>;

template<class T>
using TpStack = std::stack<T>;

template<class T>
using TpDeque = std::deque<T>;