#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <cmath>

// 音を再生する関数
void playSound(int value, int maxHeight) {
    float frequency = 440.0f + (value / static_cast<float>(maxHeight)) * 880.0f;
    sf::SoundBuffer buffer;
    const int sampleRate = 44100;
    const int duration = 100; // ms
    const int sampleCount = sampleRate * duration / 1000;

    std::vector<sf::Int16> samples(sampleCount);
    for (int i = 0; i < sampleCount; ++i) {
        samples[i] = static_cast<sf::Int16>(32767 * std::sin(2 * 3.14159 * frequency * i / sampleRate));
    }

    buffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate);
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
    sf::sleep(sf::milliseconds(duration));
}

// 棒グラフを描画する関数
void drawBars(const std::vector<int>& data, sf::RenderWindow& window, const std::vector<sf::Color>& colors) {
    window.clear(sf::Color::White);

    float barWidth = window.getSize().x / static_cast<float>(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        sf::RectangleShape bar(sf::Vector2f(barWidth, data[i]));
        bar.setPosition(i * barWidth, window.getSize().y - data[i]);
        bar.setFillColor(colors[i]);
        window.draw(bar);
    }

    window.display();
}

// クイックソート
void quickSort(std::vector<int>& data, sf::RenderWindow& window, int low, int high, int maxHeight) {
    if (low < high) {
        int pivot = data[high];
        int i = low - 1;

        for (int j = low; j < high; ++j) {
            if (data[j] < pivot) {
                ++i;
                std::swap(data[i], data[j]);

                // 描画と音声を同時に処理
                std::vector<sf::Color> colors(data.size(), sf::Color::Blue);
                colors[i] = sf::Color::Red;
                colors[j] = sf::Color::Red;

                drawBars(data, window, colors);
                std::thread(playSound, data[i], maxHeight).detach(); // 音声再生を別スレッドで
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }

        std::swap(data[i + 1], data[high]);

        std::vector<sf::Color> colors(data.size(), sf::Color::Blue);
        colors[i + 1] = sf::Color::Green;
        drawBars(data, window, colors);
        std::thread(playSound, data[i + 1], maxHeight).detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        quickSort(data, window, low, i, maxHeight);
        quickSort(data, window, i + 2, high, maxHeight);
    }
}

int main() {
    const int numBars = 1000; // 棒の本数
    const int windowWidth = numBars + 100;
    const int windowHeight = numBars;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Quick Sort Visualization");

    // ランダムな重複なしデータを生成
    std::vector<int> data(numBars);
    for (int i = 0; i < numBars; ++i) {
        data[i] = (i + 1) * (windowHeight / numBars);
    }

    // データをシャッフル
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(data.begin(), data.end(), g);

    quickSort(data, window, 0, data.size() - 1, windowHeight);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}
