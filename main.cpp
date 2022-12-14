#include <iostream>
#include <unistd.h>
#include <random>
#include <string>

pthread_mutex_t mutex;
bool E = true;
int countC;
std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(1.0, 10.0);

class Hive {
public:
    int honey;
    int bees;
};

Hive hive;

void *beeMove(void *args) {
    while (E) {
        if (hive.bees > 1) {
            int k = dist(mt) < 5 ? 1 : 2;
            if (k == 1) {
                pthread_mutex_lock(&mutex);
                --hive.bees;
                std::cout << "Bee fly    bees in the hive " << hive.bees << "\n";
                pthread_mutex_unlock(&mutex);
                sleep(10);
                pthread_mutex_lock(&mutex);
                if (hive.honey < 30) {
                    ++hive.honey;
                    std::cout << "    Add honey   honey in the hive " << hive.honey << "\n";
                }
                ++hive.bees;
                pthread_mutex_unlock(&mutex);
            } else {
                sleep(10);
            }
        } else {
            pthread_mutex_unlock(&mutex);
            sleep(10);
        }
    }
    return nullptr;
}

void *bearMove(void *args) {
    while (E) {
        sleep(25);
        if (hive.honey > 15) {
            if (hive.bees < 3) {
                std::cout << "\n" << "Bear: Empty hive\n";
                hive.honey = 0;
            } else {
                std::cout << "\n" << "Bear: The bear was running\n";
            }
        } else {
            std::cout << "\n" << "Bear: little honey\n";
        }
        pthread_mutex_lock(&mutex);
        if (countC == 0) {
            E = false;
            continue;
        }
        pthread_mutex_unlock(&mutex);
        --countC;
    }
    return nullptr;
}

int main() {
    std::string check;
    bool checkConsole = true;
    int num;
    while (checkConsole) {
        try {
            std::cout << "enter the number of bees\n";
            std::cin>>check;
            num = stoi(check);
            if (num <= 0) {
                std::cout << "Incorrect data\n";
            } else {
                checkConsole = false;
            }
        }
        catch (std::invalid_argument &e) {
            std::cout << "Incorrect data\n";
        }
    }
    checkConsole = true;
    pthread_t thread[num + 1];
    hive.bees = num;
    std::cout << "how many trips will the bear make?\n";
    while (checkConsole) {
        try {
            std::cin>>check;
            countC = stoi(check);
            if (countC <= 0) {
                std::cout << "Incorrect data\n";
            } else {
                checkConsole = false;
            }
        }
        catch (std::invalid_argument &e) {
            std::cout << "Incorrect data\n";
        }
    }
    --countC;
    pthread_mutex_init(&mutex, nullptr);
    for (int i = 0; i < num; ++i) {
        pthread_create(&thread[i], nullptr, beeMove, nullptr);
    }
    pthread_create(&thread[num], nullptr, bearMove, nullptr);

    pthread_join(thread[num], nullptr);
    pthread_mutex_destroy(&mutex);
}
