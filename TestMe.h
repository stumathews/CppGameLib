#pragma once
#ifndef TESTME_H
#define TESTME_H

namespace gamelib
{
// This allows you to set your class as a friend of a google test class so that you can keep your
// private members private but google test can call them directly to test them
#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test
}

#endif
