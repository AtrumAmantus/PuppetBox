#pragma once

#include <string>
#include <vector>

class UserInput
{
public:
    void append(char c)
    {
        input_.insert(input_.begin() + cursorPosition_++, c);
    };

    std::string read() const
    {
        std::vector<char> copy = input_;
        copy.push_back('\0');

        std::string input = std::string(&copy[0]);
        return std::move(input);
    };

    bool isEmpty()
    {
        return input_.empty();
    };

    void clear()
    {
        input_.clear();
        cursorPosition_ = 0;
    };

    void startReading()
    {
        readingInput_ = true;
    };

    void stopReading()
    {
        readingInput_ = false;
    };

    bool isReading() const
    {
        return readingInput_;
    };

    void targetComponent(const std::shared_ptr<PB::UIComponent>& component)
    {
        targetComponent_ = component;
    }

    std::shared_ptr<PB::UIComponent>& component()
    {
        return targetComponent_;
    }
private:
    std::vector<char> input_;
    std::uint32_t cursorPosition_ = 0;
    bool readingInput_ = false;
    std::shared_ptr<PB::UIComponent> targetComponent_;
};