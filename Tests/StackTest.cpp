#include "gtest/gtest.h"
#include "stack.hpp"

class stackTest : public ::testing::Test 
{
protected:
	stackTest()
	 :s_stack{}
	{
		s_stack.push('A');
		s_stack.push('B');
		s_stack.push('C');
	}
	void pushDandE()
	{
		s_stack.push('D');
		s_stack.push('E');
		EXPECT_EQ(s_stack.size(), 5);
	}

	virtual ~stackTest(){ }

	stack<char, 5> s_stack;
};

TEST_F(stackTest, stackSize)
{
	EXPECT_TRUE(s_stack.size() == 3);
}

TEST_F(stackTest, stackPush)
{
	s_stack.push('D');
	EXPECT_EQ(s_stack.size(), 4);
	
	s_stack.push('E');
	EXPECT_EQ(s_stack.size(), 5);

	//maximun size of the stack, should not increment anymore
	s_stack.push('1');
	s_stack.push('1');
	s_stack.push('1');
	s_stack.push('1');
	EXPECT_EQ(s_stack.size(), 5);
}

TEST_F(stackTest, stackPop)
{
	pushDandE();

	char Evalue = s_stack.pop();
	EXPECT_EQ(Evalue, 'E');
	EXPECT_EQ(s_stack.size(), 4);
	
	char Dvalue = s_stack.pop();
	EXPECT_EQ(Dvalue, 'D');
	EXPECT_EQ(s_stack.size(), 3);
	
	char Cvalue = s_stack.pop();
	EXPECT_EQ(s_stack.size(), 2);
	EXPECT_EQ(Cvalue, 'C');

	char Bvalue = s_stack.pop();
	EXPECT_EQ(s_stack.size(), 1);
	EXPECT_EQ(Bvalue, 'B');

	char Avalue = s_stack.pop();
	EXPECT_EQ(s_stack.size(), 0);
	EXPECT_EQ(Avalue, 'A');

	//should return an empty value
	char emptyValue = s_stack.pop();
	EXPECT_EQ(emptyValue,'\0');
	EXPECT_EQ(s_stack.size(), 0);
}


TEST_F(stackTest,copyAssignable )
{
	stack<char, 5> new_stack;

	new_stack = s_stack;
	EXPECT_EQ(new_stack.size(), 3);
	EXPECT_EQ(s_stack.size(), 3);
	EXPECT_TRUE(s_stack.size() == new_stack.size());
	EXPECT_TRUE(s_stack.pop() == new_stack.pop());

}

TEST_F(stackTest, stackCopyAsignamentConstructed)
{
	stack<char, 5> new_stack = s_stack;
	EXPECT_EQ(new_stack.size(), 3);
	EXPECT_EQ(s_stack.size(), 3);
	EXPECT_TRUE(s_stack.size() == new_stack.size());
	EXPECT_TRUE(s_stack.pop() == new_stack.pop());
}


TEST_F(stackTest, stackCopyConstructed)
{	
	stack<char, 5> new_stack( s_stack );
	EXPECT_EQ(new_stack.size(), 3);
	EXPECT_EQ(s_stack.size(), 3);
	EXPECT_TRUE(s_stack.size() == new_stack.size());
	EXPECT_TRUE(s_stack.pop() == new_stack.pop());
}


TEST_F(stackTest, stackCopyConstructedWithBrackets)
{
	stack<char, 5> new_stack{ s_stack };
	EXPECT_EQ(new_stack.size(), 3);
	EXPECT_EQ(s_stack.size(), 3);
	EXPECT_TRUE(s_stack.size() == new_stack.size());
	EXPECT_TRUE(s_stack.pop() == new_stack.pop());
}