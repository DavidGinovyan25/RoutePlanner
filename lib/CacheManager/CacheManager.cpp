#include <iostream>
#include <iterator>
#include <filesystem>
#include <fstream>

#include "CacheManager.hpp"

int CacheManager::hit_count = 0;
int CacheManager::miss_count = 0;

int CacheManager::GetHitCount() {
    return hit_count;
};

int CacheManager::GetMissCount() {
    return miss_count;
};

bool CacheManager::HasCachedResponse(const std::string& entered_url) {
    return storage.contains(entered_url);
}

void CacheManager::CacheResponse(std::string& url, json& json_answer) {
    if (HasCachedResponse(url))
        return;
    order.push_back(url);
    auto new_added_url = std::prev(order.end());
    storage[url] = {
        {
            .json_answer = json_answer,
            .fixed_time = Clock::now()
        },
        new_added_url
    };
}

std::optional<json> CacheManager::GetResponce(std::string url) {
    if (!HasCachedResponse(url)) {
        std::cerr << "ERROR: cache doesn`t have this URL" << std::endl;
        ++miss_count;
        return std::nullopt;
    }
    ++hit_count;
    auto it = std::find(order.begin(), order.end(), url);
    order.erase(it);
    order.push_back(url);
    storage[url].first.fixed_time = Clock::now();
    storage[url].second = std::prev(order.end());
    return storage[url].first.json_answer;
}

bool CacheManager::IsFull() {
    return max_size == storage.size();
}

bool CacheManager::IsEmpty() {
    return storage.empty();
}

void CacheManager::Clear() {
    storage.clear();
    order.clear();
}

void CacheManager::EvictFromFull() {
    if (!IsFull())
        return;
    while (order.size() != max_size / 2) {
        auto key = order.front();
        order.pop_front();
        storage.erase(key);
    }
}

void CacheManager::ExpireOldest() {
    if (IsEmpty())
        return;
    auto current_time = Clock::now();
    for (auto [url, s] : storage) {
        auto key = order.front();
        if (storage[key].first.fixed_time + max_age <= current_time) {
            order.pop_front();
            storage.erase(key);
        }
    }
}

void CacheManager::LoadFromFileSystem() {
    if (!std::filesystem::exists(upload_file)) {
        std::cerr << "ERROR: file doesn`t exist" << std::endl;
        return;
    }

    if (std::filesystem::file_size(upload_file) == 0) {
        std::cerr << "FROM API" << std::endl;
        return;
    }

    std::ifstream file(upload_file);
    if (!file.is_open()) {
        std::cerr << "ERROR: cannot open cache file" << std::endl;
        return;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (content.empty()) {
        std::cerr << "FROM API" << std::endl;
        return;
    }
    json json_answers_array = json::parse(content, nullptr, false);
    if (json_answers_array.is_discarded() || !json_answers_array.is_array()) {
        std::cerr << "ERROR: invalid cache file format" << std::endl;
        return;
    }
    std::cout << "LOADING FROM FILECACHE" << std::endl;
    Clear();

    for (auto& item : json_answers_array) {
        if (!item.is_object())
            continue;

        if (!item.contains("url") || !item["url"].is_string())
            continue;
        if (!item.contains("json_answer"))
            continue;
        if (!item.contains("fixed_time") || !item["fixed_time"].is_number_integer())
            continue;

        std::string url = item["url"].get<std::string>();
        order.push_back(url);
        auto it = std::prev(order.end());
        storage[url] = {
            {
                .json_answer = item,
                .fixed_time = Clock::from_time_t(item["fixed_time"].get<std::time_t>())
            },
            it
        };
    }
}



void CacheManager::SaveInFileSystem() {
    if (!std::filesystem::exists(upload_file)) {
        std::cerr << "ERROR: file doesn`t exist" << std::endl;
        return;
    }
    if (IsEmpty()) {
        std::cerr << "ERROR: empty RAM cache" << std::endl;
        return;
    }
    json json_answers_array = json::array();

    if (std::filesystem::file_size(upload_file) != 0) {
        std::ifstream in(upload_file);
        if (in.is_open()) {
            std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            if (!content.empty()) {
                json old = json::parse(content, nullptr, false);
                if (!old.is_discarded() && old.is_array())
                    json_answers_array = old;
            }
        }
    }
    for (auto& [url, record] : storage) {
        json obj = record.first.json_answer;
        obj["url"] = url;
        obj["fixed_time"] = Clock::to_time_t(record.first.fixed_time);
        bool replaced = false;
        for (auto& item : json_answers_array) {
            if (item.is_object() && item.contains("url") && item["url"].is_string()
                && item["url"].get<std::string>() == url) {
                item = obj;
                replaced = true;
                break;
            }
        }
        if (!replaced)
            json_answers_array.push_back(obj);
    }
    std::ofstream out(upload_file, std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "ERROR: cannot open cache file for writing" << std::endl;
        return;
    }
    out << json_answers_array.dump(4);
}



