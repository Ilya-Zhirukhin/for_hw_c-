// ДЗ - исследование процесса записи данных в файл в синхронном и многопоточном режимах
// 1. Реализовать функцию записи данных в файл в синхронном режиме
// 2. Реализовать функцию записи данных в файл в многопоточном режиме
// 3. Измерить время выполнения шага 1 и шага 2

//Реализация функции записи данных в файл в синхронном режиме:
#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>
#include <cstdlib>

using namespace std;

void writeSync() {
    const char* filename = "test.txt";
    const size_t fileSize = 5ull * 1024ull * 1024ull * 1024ull; // 5GB
    const size_t blockSize = 0x1000; // 4KB
    char* data = new char[blockSize];
    memset(data, 'A', blockSize);

    ofstream file(filename, ios::out | ios::binary);
    if (!file) {
        cerr << "Failed to open file for writing\n";
        exit(EXIT_FAILURE);
    }

    auto start = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < fileSize / blockSize; i++) {
        file.write(data, blockSize);
    }
    auto end = chrono::high_resolution_clock::now();

    file.close();
    delete[] data;

    cout << "Sync write time: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms\n";
}


//запись данных в файл в многопоточном режиме:
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

std::mutex mtx;
std::condition_variable cv;
std::vector<std::thread> threads;

void write_data(std::ofstream& out, const char* data, const size_t size, const size_t offset) {
    out.seekp(offset);
    out.write(data, size);
}

void threaded_write(const char* data, const size_t block_size, const size_t num_blocks, const std::string& filename) {
    std::ofstream out(filename, std::ios::binary | std::ios::out);
    if (!out) {
        std::cerr << "Failed to open file " << filename << std::endl;
        return;
    }

    const size_t num_threads = std::thread::hardware_concurrency();
    const size_t blocks_per_thread = num_blocks / num_threads;
    size_t remaining_blocks = num_blocks % num_threads;
    size_t offset = 0;

    for (size_t i = 0; i < num_threads; ++i) {
        size_t num_blocks_this_thread = blocks_per_thread;
        if (remaining_blocks > 0) {
            --remaining_blocks;
            ++num_blocks_this_thread;
        }

        threads.emplace_back([&out, data, block_size, num_blocks_this_thread, offset]() {
            for (size_t j = 0; j < num_blocks_this_thread; ++j) {
                write_data(out, data, block_size, offset + (j * block_size));
            }

            std::unique_lock<std::mutex> lock(mtx);
            cv.notify_one();
        });

        offset += num_blocks_this_thread * block_size;
    }

    for (auto& thread : threads) {
        thread.join();
    }

    out.close();
}

int main() {
    const size_t block_size = 0x1000;
    const size_t num_blocks = (5ull << 30) / block_size;  // 5GB
    const std::string filename = "test.txt";

    char* data = new char[block_size];
    std::fill(data, data + block_size, 'A');

    const auto start_time = std::chrono::steady_clock::now();

    threaded_write(data, block_size, num_blocks, filename);

    const auto end_time = std::chrono::steady_clock::now();
    const auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "Elapsed time: " << elapsed_time << " ms" << std::endl;

    delete[] data;

    return 0;
}



// Примечание:
// 1. Использовать все доступные потоки (кол-во доступных получать через API)
// 2. Размер файла 5ГБ
// 3. Данные записываются блочно, размер блока данных = 0x1000 (4096)


// код ниже соответствует примечанию, так как:
//Для использования всех доступных потоков используется функция std::thread::hardware_concurrency(), которая возвращает количество потоков, которые могут быть запущены параллельно в текущей системе.
//Размер файла задается в переменной fileSize, которая установлена на 5 ГБ.
//Данные записываются блочно, размер блока данных установлен в переменной block_size, которая установлена на 0x1000 (или 4096 байт).





// Вопросы:
// 1. Какая из функций выполнилась быстрее? Почему?
// 2. Как проводилось измерение? Какой API был использован и почему?
// 3. Как изменится время выполнения, если увеличить размер файла (например, 10ГБ)?
// 4. Как изменится время выполнения, если использовать в 2 раза меньше потоков? В 2 раза больше?

//1) функция записи данных в файл в синхронном режиме (writeSync)
// будет выполняться быстрее, поскольку она не имеет накладных расходов
// на создание и управление потоками, что может замедлить работу второй функции
// (threaded_write). Однако, это может зависеть от конкретной системы и условий выполнения.
//
//2) Измерение времени выполнения было проведено с помощью стандартной
// библиотеки C++ и функции chrono::high_resolution_clock::now().
// Она была выбрана из-за своей точности и высокой разрешающей способности,
// что позволяет измерять время выполнения с высокой точностью.
//
//3) Время выполнения увеличится пропорционально увеличению размера файла.
// В многопоточном режиме это может привести к увеличению времени выполнения,
// так как большой объем данных может вызвать более сильную конкуренцию за ресурсы между потоками.
//
//4) Изменение количества потоков может повлиять
// на время выполнения в многопоточном режиме. Если использовать в
// два раза меньше потоков, время выполнения может увеличиться, поскольку
// задача записи данных в файл может занимать больше времени на выполнение.
// Если использовать в два раза больше потоков, это может привести к ускорению
// выполнения задачи, но только до тех пор, пока система имеет достаточно ресурсов
// для поддержки дополнительных потоков. Далее увеличение количества потоков
// может вызвать конкуренцию за ресурсы, что может замедлить выполнение.