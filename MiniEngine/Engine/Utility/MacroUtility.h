#pragma once
#include <sstream>

#define SAFE_DELETE_POINTER(ptr) \
    delete (ptr); \
    (ptr) = nullptr;

#define DELETE_CONSTRUCTOR_FIVE(name) \
name(const name& other) = delete; name(name&& other) = delete; \
name& operator=(const name& other) = delete; name& operator=(name&& other) = delete;

#define DEFAULT_CONSTRUCTOR_FIVE(name) \
name(const name& other) = default; name(name&& other) = default; \
name& operator=(const name& other) = default; name& operator=(name&& other) = default;

#define CREATE_COMPARE_FUNCTOR_INCREASE(name, compareMember, T)\
class name{\
public:\
bool operator()(const T lhs, const T rhs) const\
{\
return lhs->compareMember < rhs->compareMember;\
}\
}

#define CREATE_COMPARE_FUNCTOR_DECREASE(name, compareMember, T)\
class name{\
public:\
bool operator()(const T lhs, const T rhs) const\
{\
return lhs->compareMember < rhs->compareMember;\
}\
}

#define FOCUS_DEBUG

#if (defined(DEBUG)) && !defined(FOCUS_DEBUG)
#define DEBUG_FOCUS_PRINT(...) (void)0;

#define DEBUG_PRINT(...) printf(__VA_ARGS__ ); fflush(stdout);

#define DEBUG_PRINT_NULLPTR(name)\
printf("<%s> is nullptr!", name); fflush(stdout);

#define DEBUG_PRINT_START(GameObjectName,ComponentName) \
printf("<%s> <%s> Component Start()\n", GameObjectName, ComponentName);\
fflush(stdout);

#define DEBUG_PRINT_UPDATE(GameObjectName,ComponentName) \
printf("<%s> <%s> Component Update()\n", GameObjectName, ComponentName);\
fflush(stdout);

#define DEBUG_PRINT_FIXED_UPDATE(GameObjectName,ComponentName)\
printf("<%s> <%s> Component FixedUpdate()\n", GameObjectName, ComponentName);\
fflush(stdout);

#elif ((defined(DEBUG)) && defined(FOCUS_DEBUG))
#define DEBUG_FOCUS_PRINT(...) printf(__VA_ARGS__ ); fflush(stdout);

#define DEBUG_PRINT(...) (void)0
#define DEBUG_PRINT_NULLPTR(name, ...) (void)0
#define DEBUG_PRINT_START(GameObjectName,ComponentName,...) (void)0
#define DEBUG_PRINT_UPDATE(GameObjectName,ComponentName,...) (void)0
#define DEBUG_PRINT_FIXED_UPDATE(GameObjectName,ComponentName,...) (void)0

#else
#define DEBUG_FOCUS_PRINT(...) (void)0;
#define DEBUG_PRINT(...) (void)0
#define DEBUG_PRINT_NULLPTR(name, ...) (void)0
#define DEBUG_PRINT_START(GameObjectName,ComponentName,...) (void)0
#define DEBUG_PRINT_UPDATE(GameObjectName,ComponentName,...) (void)0
#define DEBUG_PRINT_FIXED_UPDATE(GameObjectName,ComponentName,...) (void)0
#endif