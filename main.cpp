#include <iostream>
#include <unistd.h>
#include <random>
#include <string>

// мьтекс для корректного доступа к данным
pthread_mutex_t mutex;
// используется для завершения потока.
bool E = true;
// количество потенциальных приходов медведя).
int countC;
// создаю более корректную генерация случайных чисел.
std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(1.0, 10.0);
// класс улья
class Hive {
public:
    int honey;
    int bees;
};

Hive hive;

// функция потока пчел.
void *beeMove(void *args) {
    // зацикливаем действия в потоке.
    while (E) {
        // проверяем, может ли улететь.
        if (hive.bees > 1) {
            // рандомизируем возмодность вылета, чтобы все пчелы кроме одной не вылетали за раз.
            int k = dist(mt) < 5 ? 1 : 2;
            // пчела вылетает.
            if (k == 1) {
                pthread_mutex_lock(&mutex);
                // количество пчел убывает.
                --hive.bees;
                std::cout << "Bee fly    bees in the hive " << hive.bees << "\n";
                pthread_mutex_unlock(&mutex);
                // полет занимает время.
                sleep(10);
                pthread_mutex_lock(&mutex);
                // проверям, может ли пчела добавить мед.
                if (hive.honey < 30) {
                    ++hive.honey;
                    std::cout << "    Add honey   honey in the hive " << hive.honey << "\n";
                }
                // прилетает пчела
                ++hive.bees;
                pthread_mutex_unlock(&mutex);
            } else {
                // пчела отдыхает в улье.
                sleep(10);
            }
        } else {
            pthread_mutex_unlock(&mutex);
            // пчела решила отдохнуть еще больше.
            sleep(10);
        }
    }
    return nullptr;
}
// функция потока медведя.
void *bearMove(void *args) {
    // зацикливаем действия в потоке.
    while (E) {
        // ждет или лечится))
        sleep(25);
        // проверяет, достаточно ли для него меда.
        if (hive.honey > 15) {
            // покусают ли его?
            if (hive.bees < 3) {
                std::cout << "\n" << "Bear: Empty hive\n";
                // опустошает улье.
                hive.honey = 0;
                
            } else {
                // медведя покусали
                std::cout << "\n" << "Bear: The bear was running\n";
            }
        } else {
            std::cout << "\n" << "Bear: little honey\n";
        }
        pthread_mutex_lock(&mutex);
        // осчитывает кол-во итераций.
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
    // ввод кол-ва пчел.
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
    // ввод кол-ва итераций
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
    // инициализируем mutex.
    pthread_mutex_init(&mutex, nullptr);
    // создаем n потоков.
    for (int i = 0; i < num; ++i) {
        pthread_create(&thread[i], nullptr, beeMove, nullptr);
    }
    // создаем потоков медведя
    pthread_create(&thread[num], nullptr, bearMove, nullptr);
    // отслеживаем поток медведя
    pthread_join(thread[num], nullptr);
    // уничтожаем мьтекс 
    pthread_mutex_destroy(&mutex);
}
