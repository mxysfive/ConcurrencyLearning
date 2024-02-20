//
// Created by MengXiangyu on 2024/2/19.
//
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>
#include <queue>

int num = 1;
std::condition_variable cvA;
std::condition_variable cvB;
void TestTwoThreadPrint() {
    std::mutex mtxNum;
    std::thread t1([&mtxNum]() {
       for (int i = 0; i < 100; i++) {
           std::unique_lock<std::mutex> lk(mtxNum);
/*           关注条件变量wait 方法的行为， 当谓词为true 时放行， 当谓词判断为false时把传入的锁释放掉再睡眠。
           当唤醒时，先对传入的锁子加锁了才往下运行。*/
           cvA.wait(lk, []() {
               return num == 1;
           });
           std::cout<<std::this_thread::get_id()<<" print: "<<num<<std::endl;
           num = 2;
           cvB.notify_one();
       }
    });
    std::thread t2([&mtxNum]() {
       for (int i = 0; i < 100; i++) {
           std::unique_lock<std::mutex> lk(mtxNum);
           cvB.wait(lk, []() {
               return num == 2;
           });
           std::cout<<std::this_thread::get_id()<<" print: "<<num<<std::endl;
           num = 1;
           cvA.notify_one();
       }
    });
    t1.join();
    t2.join();
}

template <typename T>
class ThreadSafeQueue {
private:
    std::mutex mut;
    std::queue<T> que;
    std::condition_variable dataCond;
public:
    void push(const T& value) {
        std::lock_guard<std::mutex> lk(mut);
        que.push(value);
        dataCond.notify_one();
    }
    void waitPop(T &value) {
        std::unique_lock<std::mutex> lk(mut);
        dataCond.wait(lk, [&]() {
            return !que.empty();
        });
        value = que.front();
        que.pop();
    }
};

void TestSafeQueue() {
    ThreadSafeQueue<int> safeQueue;
    std::mutex printMtx;
    std::thread producer([&]() {
        for (int i = 0; i < 50; i++) {
            safeQueue.push(i);
            {
                std::lock_guard<std::mutex> lg(printMtx);
                std::cout<<"Producer push data: "<<i<<std::endl;
            }
        }
    });
    std::thread consumer1([&]() {
        for(;;) {
            int rcv;
            safeQueue.waitPop(rcv);
            {
                // 这里的打印不是很严谨，因为waitPop和这里的cout不是原子的，可能Pop成功后就切换到了Producer中
                // 但是无碍，功能一定是正确的
                std::lock_guard<std::mutex> printlk(printMtx);
                std::cout << "consumer1 wait and pop data is " << rcv << std::endl;
            }
        }
    });
    std::thread consumer2([&]() {
        for(;;) {
            int rcv;
            safeQueue.waitPop(rcv);
            {
                std::lock_guard<std::mutex> printlk(printMtx);
                std::cout << "consumer2 wait and pop data is " << rcv << std::endl;
            }
        }
    });
    producer.join();
    consumer1.join();
    consumer2.join();
}
#include <iostream>
int main() {
    //TestTwoThreadPrint();
    TestSafeQueue();
}
