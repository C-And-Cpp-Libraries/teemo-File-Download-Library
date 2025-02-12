#include "gtest/gtest.h"
#include "teemo/teemo.h"
#include "test_data.h"
#include <future>
using namespace teemo;

void DoBreakpointTest(const std::vector<TestData>& test_datas, int thread_num) {
  for (const auto &test_data : test_datas) {
    std::future<void> test_task = std::async(std::launch::async, [test_data, thread_num]() {
      Teemo efd;

      efd.setThreadNum(thread_num / 2);
      if (test_data.md5.length() > 0)
        efd.setHashVerifyPolicy(ALWAYS, MD5, test_data.md5);

      std::shared_future<Result> r = efd.start(
          test_data.url, test_data.target_file_path,
          [test_data](Result result) {
            printf("\nResult: %s\n", GetResultString(result));
            EXPECT_TRUE(result == SUCCESSED || result == CANCELED);
          },
          [](int64_t total, int64_t downloaded) {
            if (total > 0)
              printf("%3d%%\b\b\b\b", (int)((double)downloaded * 100.f / (double)total));
          },
          nullptr);

      std::this_thread::sleep_for(std::chrono::milliseconds(1000));

      efd.stop();

      r.wait();

      efd.setThreadNum(thread_num);

      Result ret =
          efd.start(
                 test_data.url, test_data.target_file_path,
                 [test_data](Result result) {
                   printf("\nResult: %s\n", GetResultString(result));
                   EXPECT_TRUE(result == SUCCESSED);
                 },
                 [](int64_t total, int64_t downloaded) {
                   if (total > 0)
                     printf("%3d%%\b\b\b\b", (int)((double)downloaded * 100.f / (double)total));
                 },
                 nullptr)
              .get();
    });
    test_task.wait();
  }
}

TEST(BreakPointHttpTest, Http_ThreadNum_1_Breakpoint) {
  DoBreakpointTest(http_test_datas, 1);

  // set test case interval
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

TEST(BreakPointHttpTest, Http_ThreadNum_3_Breakpoint) {
  DoBreakpointTest(http_test_datas, 3);

  // set test case interval
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

TEST(BreakPointHttpTest, Http_ThreadNum_10_Breakpoint) {
  DoBreakpointTest(http_test_datas, 10);

  // set test case interval
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}
