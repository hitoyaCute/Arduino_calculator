#ifndef INCLUDE_LIBS_STR_ACC_HPP_
#define INCLUDE_LIBS_STR_ACC_HPP_

// string accumulator
template <unsigned char CAP>
class str_acc_t {
    char str[CAP] = {};
    unsigned char len = 0;

public:
    str_acc_t() = default;

    void append(char c) {
        if ((this->len - 1) < CAP) {
            this->str[this->len++] = c;
            this->str[this->len + 1] = 0;
        }
    }

    void pop() {
        if (this->len > 0) {
            this->len--;
            this->str[this->len] = 0;
        }
    }
    
    const char* view() const {
        return this->str;
    }

    void clear() { this->str[0] = 0; this->str[1] = 0;this->len = 0; }
};

#endif  // INCLUDE_LIBS_STR_ACC_HPP_
