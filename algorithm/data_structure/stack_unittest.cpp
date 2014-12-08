/*
 * stack_unittest.cpp
 * Created on: Aug 20, 2014
 * Author: chenguangyu
 */

#include "stack.h"
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace longan;

TEST(StackTest, SmallCase) {
    Stack<std::string> stack;     
    ASSERT_TRUE(stack.Empty());
    ASSERT_EQ(0, stack.Size());
    stack.Push("aa");
    stack.Push("bb");
    stack.Push("cc");
    ASSERT_EQ("cc", stack.Top());
    stack.Pop();
    ASSERT_EQ("bb", stack.Pop());
    stack.Pop();
    ASSERT_THROW(stack.Pop(), LonganRuntimeError);
    stack.Push("dd");
    ASSERT_EQ("dd", stack.Top());
}

TEST(StackTest, LargeCase) {
    Stack<int> stack(10);
    for (int i = 0; i < 1024; ++i) {
        stack.Push(i);
        ASSERT_EQ(i, stack.Top());
    }
    for (int i = 1023; i >= 512; --i) {
        ASSERT_EQ(i, stack.Pop());
    }
    for (int i = 0; i < 512; ++i) {
        stack.Push(-i);
    }
    EXPECT_EQ(1024,stack.Size());
}

class Obj {
public:
    Obj(int ii) { ++sCount; i = ii;}
    Obj(const Obj& o) { ++sCount; i = o.i; }
    Obj(Obj&& o) { ++sCount; i = o.i; o,i = 0; }
    ~Obj() { --sCount; }
public:
    static int sCount;
    int i;
};
int Obj::sCount = 0;

TEST(StackTest, NoLeak) {
    Stack<Obj>* pStack = new Stack<Obj>();
    for (int i = 0; i < 100; ++i) {
        pStack->Push(Obj(i));
        pStack->Push(Obj(-i));
        pStack->Pop();
    }
    ASSERT_EQ(100, Obj::sCount);
    delete pStack;
    ASSERT_EQ(0, Obj::sCount);
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
