#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <list>
#include <memory>

struct test {
    // test()= default;
    std::string teststr = "as;ldfasl";
    int a = 10;
};


std::mutex mulock;
std::condition_variable cond;

std::mutex fullmulock;
std::condition_variable fullcond;

std::list<std::shared_ptr<int>> test_put_list;
std::list<std::shared_ptr<int>> test_get_list1;
std::list<std::shared_ptr<int>> test_get_list2;
std::list<std::shared_ptr<int>> test_get_list3;


int i = 0;

int put() {
    while (true) {
        {

            std::unique_lock<std::mutex> ulock(mulock);
            test_put_list.push_back(std::make_shared<int>(i++));

//            std::this_thread::sleep_for(std::chrono::milliseconds(1));


            int size = test_put_list.size();
            if (size > 50000) {
                std::cout << size << std::endl;
//                std::unique_lock<std::mutex> fulock(fullmulock);
                fullcond.wait(ulock);
            }
        }
        cond.notify_all();
    }

    return 0;
}

int get(int i) {
    while (true) {
        {
            std::unique_lock<std::mutex> ulock(mulock);
//            cond.wait(ulock);

            if (i == 1) {
                std::swap(test_get_list1, test_put_list);
                if(test_get_list1.empty()) cond.wait(ulock);
            } else if (i == 2) {
                std::swap(test_get_list2, test_put_list);
                if(test_get_list2.empty()) cond.wait(ulock);
            } else if (i == 3) {
                std::swap(test_get_list3, test_put_list);
                if(test_get_list3.empty()) cond.wait(ulock);
            }
        }
        fullcond.notify_one();

        if (i == 1) {
            while (!test_get_list1.empty()) {
                auto a = test_get_list1.front();
                test_get_list1.pop_front();

                std::weak_ptr<int> t = a;
                std::cout << "a1:" << *a << ",wtnum:" << t.use_count() << ",time:" << time(0) << std::endl;
            }

        } else if (i == 2) {
            while (!test_get_list2.empty()) {
                auto a = test_get_list2.front();
                test_get_list2.pop_front();

                std::weak_ptr<int> t = a;
//                std::cout << "a2:" << *a << ",wtnum:" << t.use_count() << ",time:" << time(0) << std::endl;
            }
            std::unique_lock<std::mutex> fulock(fullmulock);
            fullcond.notify_one();
        } else if (i == 3) {
            while (!test_get_list3.empty()) {
                auto a = test_get_list3.front();
                test_get_list3.pop_front();

                std::weak_ptr<int> t = a;
//                std::cout << "a3:" << *a << ",wtnum:" << t.use_count() << ",time:" << time(0) << std::endl;
            }
        }
    }

    return 0;
}


int main() {

    test_put_list.clear();
    test_get_list1.clear();
    test_get_list2.clear();
    test_get_list3.clear();

    std::thread t1(put);
    std::thread t2(get, 1);
    std::thread t3(get, 2);
    std::thread t4(get, 3);

    std::this_thread::sleep_for(std::chrono::seconds(10000));


    return 0;
}