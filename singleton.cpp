//
// Created by MengXiangyu on 2024/2/19.
//
#include <mutex>
#include <iostream>
#include <memory>
#include <thread>

template <typename T>
class Singleton {
private:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance() {
        //C++ 11 保证多线程下静态变量只会有一份，不会有备份
        static std::once_flag onceFlag;
        // 编译器保证在给定flag 上只调用一次callable object
        std::call_once(onceFlag, [](){
            _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }
    void PrintAddress() {
        std::cout<<_instance.get()<<std::endl;
    }
    ~Singleton() {
        // new T 由shared_ptr管理
        std::cout<<"This is singleton deconstructor"<<std::endl;
    }
};
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
class BigObject {
public:
    BigObject() = default;
    void PrintAddress() {
        std::cout<<"Addr: "<< this<<std::endl;
    }
};

void TestSingleton() {
    std::thread t1([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Singleton<BigObject>::GetInstance()->PrintAddress();
    });
    std::thread t2([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Singleton<BigObject>::GetInstance()->PrintAddress();
    });
    std::thread t3([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Singleton<BigObject>::GetInstance()->PrintAddress();
    });
    std::thread t4([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Singleton<BigObject>::GetInstance()->PrintAddress();
    });
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}
int main() {
    TestSingleton();
}



