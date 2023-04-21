#include <queue>


template <typename T>
class TimedHistory {
public:
    struct Element {
        Element(T v, unsigned long t) {
            value = v;
            timestamp = t;
        }

        T value;
        unsigned long timestamp;
    };

private:
    std::queue<Element> _values;
    unsigned long _retention_period;
    unsigned long _granularity;

    void clean() {
        unsigned long current_time = millis();

        while (!_values.empty() && current_time - _values.front().timestamp > _retention_period) {
            _values.pop();
        }
    }

public:
    TimedHistory(unsigned long retention_period_millis, unsigned long granularity_millis) {
        _retention_period = retention_period_millis;
        _granularity = granularity_millis;
    }

    void insert(T value) {
        unsigned long current_time = millis();
        Element new_element(value, current_time);

        if (_values.empty()) {
            _values.push(new_element);
        }
        else {
            unsigned long last_insert_time = _values.back().timestamp;

            if (current_time - last_insert_time > _granularity) {
                _values.push(new_element);
            }

            clean();
        }
    }

    std::optional<Element> oldest_element() {
        clean();

        if(_values.empty()) {
            return {};
        }
        else {
            return _values.front();
        }
    }

    /*
    std::optional<Element> avg_element() {
        clean();

        if(_values.empty()) {
            return {};
        }
        else {
            T sum_value = 0;
            unsigned long sum_timestamp = 0;

            for (auto& element : _values) {
                sum_value += element.value;
                sum_timestamp += element.timestamp;
            }

            T avg_value = sum_value / _values.size();
            unsigned long avg_timestamp = sum_timestamp / _values.size();

            return Element(avg_value, avg_timestamp);
        }
    }
    */
};