#include <memory>
#include <string>
#include <mutex>

enum ActionItem : int {
    kStartServer,
    kStopServer,
    kUpdateConfiguration,
    kClearCache,
};

template<class T>
class queue {
private:
    struct node {
        node(const T& value_, std::unique_ptr<node> next_)
            : value{value_}
            , next{std::move(next_)}
        {}
        T value;
        std::unique_ptr<node> next;
    };

    std::unique_ptr<node> head;
    node* tail;
    std::unique_ptr<std::mutex> mutex_ptr;

public:
    queue()
        : head{std::make_unique<node>(T{}, nullptr)}
        , tail{head.get()}
        , mutex_ptr{std::make_unique<std::mutex>()}
    {}

    queue(const queue&) = default;
    queue(queue&&) = default;
    queue& operator=(const queue&) = default;
    queue& operator=(queue&&) = default;

    void push(const T& value) {
        std::lock_guard<std::mutex> _{*mutex_ptr};
        tail->value = value;
        tail->next = std::make_unique<node>(T{}, nullptr);
        tail = tail->next.get();
    }

    T front() {
        std::lock_guard<std::mutex> _{*mutex_ptr};
        return head->value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> _{*mutex_ptr};
        return head.get() == tail;
    }

    void pop() {
        std::lock_guard<std::mutex> {*mutex_ptr};
        head = std::move(head->next);
    }
};

using ActionQueue = queue<ActionItem>;

std::string to_string(int action) {
    switch (action) {
        case kStartServer:
            return "Start server";
        case kStopServer:
            return "Stop server";
        case kUpdateConfiguration:
            return "Update configuration";
        default:
            return nullptr;
    }
}
