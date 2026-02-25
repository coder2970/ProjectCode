#pragma once
// 格式化字串:从日志消息中取指定的元素,追加到一块内存空间中
// 设计思想:
// 1. 抽象格式化子项基类
// 2. 基于基类,派生出不同的格式化子项子类 - 主体消息,日志等级...
// 在父类中定义父类指针的数组,指向不同的格式化子项子类对象

#include <memory>
#include <ctime>
#include <sstream>
#include <cassert>
#include <vector>
#include "util.hpp"
#include "level.hpp"
#include "message.hpp"

namespace ns_log
{
    // 抽象格式化子项基类
    class FormatItem
    {
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual void format(std::ostream &out, const LogMessage &message) = 0;
    };

    // 派生格式化子项子类
    // 消息主体
    class MessageFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMessage &message) override
        {
            out << message._payload;
        }
    };
    // 日志等级
    class LevelFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMessage &message) override
        {
            out << LogLevel::ToString(message._level);
        }
    };

    // 时间
    class TimeFormatItem : public FormatItem
    {
    public:
        TimeFormatItem(const std::string &format = "%H:%M:%S") : _time_format(format)
        {
        }
        void format(std::ostream &out, const LogMessage &message) override
        {
            struct tm t;
            localtime_r(&message._ctime, &t);
            char tmp[32] = {0};
            strftime(tmp, 31, _time_format.c_str(), &t);

            out << tmp;
        }

    private:
        std::string _time_format; // 格式:%H:%M:%S
    };
    // 文件名
    class FileFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMessage &message) override
        {
            out << message._file;
        }
    };

    // 行号
    class LineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMessage &message) override
        {
            out << message._line;
        }
    };

    // 线程ID
    class ThreadFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMessage &message) override
        {
            out << message._tid;
        }
    };

    // 日志器名称
    class LoggerFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMessage &message) override
        {
            out << message._logger;
        }
    };

    // 制表符
    class TableFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMessage &message) override
        {
            out << "\t";
        }
    };

    // 换行
    class NewlineFormatItem : public FormatItem
    {
    public:
        void format(std::ostream &out, const LogMessage &message) override
        {
            out << "\n";
        }
    };

    // 其他格式化子项
    class OtherFormatItem : public FormatItem
    {
    public:
        OtherFormatItem(const std::string &str) : _str(str)
        {
        }
        void format(std::ostream &out, const LogMessage &message) override
        {
            out << _str;
        }

    private:
        std::string _str;
    };

    // 格式化器
    // %d 日期 包含子格式{%H:%M:%S}
    // %t 线程id
    // %c 日志器名称
    // %f 源码文件名
    // %l 源码行号
    // %p 级别
    // %T 缩进
    // %m 主题消息
    // %n 换行
    class Formatter
    {
    private:
        // 创建格式化子项
        FormatItem::ptr Create(const std::string &key, /*格式化字符*/ const std::string &val /*对应的值*/)
        {
            if (key == "d")
            {
                return std::make_shared<TimeFormatItem>(val);
            }
            if (key == "t")
            {
                return std::make_shared<ThreadFormatItem>();
            }
            if (key == "c")
            {
                return std::make_shared<LoggerFormatItem>();
            }
            if (key == "f")
            {
                return std::make_shared<FileFormatItem>();
            }
            if (key == "l")
            {
                return std::make_shared<LineFormatItem>();
            }
            if (key == "p")
            {
                return std::make_shared<LevelFormatItem>();
            }
            if (key == "T")
            {
                return std::make_shared<TableFormatItem>();
            }
            if (key == "m")
            {
                return std::make_shared<MessageFormatItem>();
            }
            if (key == "n")
            {
                return std::make_shared<NewlineFormatItem>();
            }
            if(key.empty())
                return std::make_shared<OtherFormatItem>(val);
            std::cout << "没有对应的格式化字符串: %" << key << std::endl;
            abort();
            return FormatItem::ptr();
        }
        // 解析
        bool ParsePattern()
        {
            // 1. 对格式化规则字符串进行解析 - 找%
            std::vector<std::pair<std::string, std::string>> format_order;
            size_t pos = 0;
            std::string key, value;

            while (pos < _pattern.size())
            {
                // 处理原始字符串
                if (_pattern[pos] != '%')
                {
                    value.push_back(_pattern[pos++]);
                    continue;
                }
                // %%
                if (pos + 1 < _pattern.size() && _pattern[pos + 1] == '%')
                {
                    value.push_back('%');
                    pos += 2;
                    continue;
                }
                // 添加原始字符串
                if (value.empty() == false)
                {
                    format_order.push_back(std::make_pair("", value));
                    value.clear();
                }
                // 格式化字符的处
                pos += 1;
                if (pos == _pattern.size())
                {
                    std::cout << "%后无内容" << std::endl;
                    return false;
                }
                key = _pattern[pos];
                pos += 1;
                if (pos < _pattern.size() && _pattern[pos] == '{')
                {
                    pos += 1; // 此时pos指向子规则的起始位置
                    while (pos < _pattern.size() && _pattern[pos] != '}')
                    {
                        value.push_back(_pattern[pos++]);
                    }
                    // 没找到}
                    if (pos == _pattern.size())
                    {
                        std::cerr << "子规则{}匹配出错" << std::endl;
                        return false;
                    }
                    pos += 1; // 指向}的下一个位置
                }
                format_order.push_back({key, value});
                key.clear();
                value.clear();
            }
            // 2. 根据解析得到的数据初始化格式化子项数组成员
            for (auto &it : format_order)
            {
                _items.push_back(Create(it.first, it.second));
            }
            return true;
        }

    public:
        using ptr = std::shared_ptr<Formatter>;
        Formatter(const std::string &pattern = "[%d{%H:%M:%S}][%t][%c][%f:%l][%p]%T%m%n")
            : _pattern(pattern)
        {
            assert(ParsePattern());
        }
        
        // 把格式化的消息放入IO流
        void format(std::ostream &out, const LogMessage &message)
        {
            for (auto &item : _items)
            {
                item->format(out, message);
            }
        }
        // 对消息进行格式化,返回格式化后的字串
        std::string format(const LogMessage &message)
        {
            std::stringstream ss;
            format(ss, message);
            return ss.str();
        }

    private:
        std::string _pattern;                // 格式化规则字符串
        std::vector<FormatItem::ptr> _items; // 格式化字串数组
    };
}