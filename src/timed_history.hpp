#pragma once

#include <deque>

using time_ms = unsigned long;

template <typename T>
class TimedHistory {
   public:
    struct Element {
        Element(T v, time_ms t) {
            value = v;
            timestamp_ms = t;
        }

        T value;
        time_ms timestamp_ms;
    };

   private:
    std::deque<Element> _values;
    time_ms _retention_period;
    time_ms _granularity;

    void clean() {
        time_ms current_time = millis();

        while (!_values.empty() && current_time - _values.front().timestamp_ms > _retention_period) {
            _values.pop_front();
        }
    }

   public:
    TimedHistory(time_ms retention_period_ms, time_ms granularity_ms) {
        _retention_period = retention_period_ms;
        _granularity = granularity_ms;
    }

    void insert(T value) {
        time_ms current_time = millis();
        Element new_element(value, current_time);

        if (_values.empty()) {
            _values.push_back(new_element);
        } else {
            time_ms last_insert_time = _values.back().timestamp_ms;

            if (current_time - last_insert_time > _granularity) {
                _values.push_back(new_element);
            }

            clean();
        }
    }

    std::optional<Element> oldest_element() {
        clean();
        if (_values.empty()) {
            return {};
        } else {
            return _values.front();
        }
    }

    std::optional<Element> newest_element() {
        clean();
        if (_values.empty()) {
            return {};
        } else {
            return _values.back();
        }
    }

    std::optional<Element> avg_element() {
        clean();

        if (_values.empty()) {
            return {};
        } else {
            T sum_value = 0;
            time_ms sum_timestamp = 0;

            for (auto& element : _values) {
                sum_value += element.value;
                sum_timestamp += element.timestamp_ms;
            }

            T avg_value = sum_value / _values.size();
            time_ms avg_timestamp = sum_timestamp / _values.size();

            return Element(avg_value, avg_timestamp);
        }
    }
};