/*
 *  Copyright (C) 2018, Throughwave (Thailand) Co., Ltd.
 *  <peerawich at throughwave dot co dot th>
 *
 *  This file is part of libnogdb, the NogDB core library in C++.
 *
 *  libnogdb is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NOGDB_COMPARE_H_INCLUDED_
#define __NOGDB_COMPARE_H_INCLUDED_

#include <iostream> // for debugging
#include <string>
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <memory>

#include "nogdb_errors.h"
#include "nogdb_types.h"

namespace nogdb {

    class MultiCondition;

    class Condition {
    private:
        friend class MultiCondition;

        friend struct Compare;
        friend struct Index;
        enum class Comparator {
            IS_NULL,
            NOT_NULL,
            EQUAL,
            GREATER,
            LESS,
            GREATER_EQUAL,
            LESS_EQUAL,
            CONTAIN,
            BEGIN_WITH,
            END_WITH,
            LIKE,
            REGEX,
            IN,
            BETWEEN,
            BETWEEN_NO_UPPER,
            BETWEEN_NO_LOWER,
            BETWEEN_NO_BOUND
        };
    public:
        Condition(const std::string &propName_);

        ~Condition() noexcept = default;

        template<typename T>
        Condition eq(T value_) const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{value_};
            tmp.comp = Comparator::EQUAL;
            return tmp;
        }

        template<typename T>
        Condition gt(T value_) const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{value_};
            tmp.comp = Comparator::GREATER;
            return tmp;
        }

        template<typename T>
        Condition lt(T value_) const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{value_};
            tmp.comp = Comparator::LESS;
            return tmp;
        }

        template<typename T>
        Condition ge(T value_) const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{value_};
            tmp.comp = Comparator::GREATER_EQUAL;
            return tmp;
        }

        template<typename T>
        Condition le(T value_) const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{value_};
            tmp.comp = Comparator::LESS_EQUAL;
            return tmp;
        }

        template<typename T>
        Condition contain(T value_) const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{value_};
            tmp.comp = Comparator::CONTAIN;
            return tmp;
        }

        template<typename T>
        Condition beginWith(T value_) const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{value_};
            tmp.comp = Comparator::BEGIN_WITH;
            return tmp;
        }

        template<typename T>
        Condition endWith(T value_) const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{value_};
            tmp.comp = Comparator::END_WITH;
            return tmp;
        }

        template<typename T>
        Condition like(T value_) const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{value_};
            tmp.comp = Comparator::LIKE;
            return tmp;
        }

        template<typename T>
        Condition regex(T value_) const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{value_};
            tmp.comp = Comparator::REGEX;
            return tmp;
        }

        Condition ignoreCase() const {
            auto tmp(*this);
            tmp.isIgnoreCase = true;
            return tmp;
        }

        Condition null() const {
            auto tmp(*this);
            tmp.valueBytes = Bytes{};
            tmp.comp = Comparator::IS_NULL;
            return tmp;
        }

        template<typename T>
        Condition between(T lower_, T upper_, const std::pair<bool, bool> isIncludeBound = {true, true}) const {
            auto tmp(*this);
            tmp.valueSet = std::vector<Bytes>{Bytes{lower_}, Bytes{upper_}};
            if (!isIncludeBound.first && !isIncludeBound.second) {
                tmp.comp = Comparator::BETWEEN_NO_BOUND;
            } else if (!isIncludeBound.first && isIncludeBound.second) {
                tmp.comp = Comparator::BETWEEN_NO_LOWER;
            } else if (isIncludeBound.first && !isIncludeBound.second) {
                tmp.comp = Comparator::BETWEEN_NO_UPPER;
            } else {
                tmp.comp = Comparator::BETWEEN;
            }
            return tmp;
        }

        template<typename T>
        Condition in(const std::vector<T> &values) const {
            auto tmp(*this);
            tmp.valueSet = std::vector<Bytes>{};
            for (auto iter = values.cbegin(); iter != values.cend(); ++iter) {
                tmp.valueSet.emplace_back(Bytes{*iter});
            }
            tmp.comp = Comparator::IN;
            return tmp;
        }

        template<typename T>
        Condition in(const std::list<T> &values) const {
            auto tmp(*this);
            tmp.valueSet = std::vector<Bytes>{};
            for (auto iter = values.cbegin(); iter != values.cend(); ++iter) {
                tmp.valueSet.emplace_back(Bytes{*iter});
            }
            tmp.comp = Comparator::IN;
            return tmp;
        }

        template<typename T>
        Condition in(const std::set<T> &values) const {
            auto tmp(*this);
            tmp.valueSet = std::vector<Bytes>{};
            for (auto iter = values.cbegin(); iter != values.cend(); ++iter) {
                tmp.valueSet.emplace_back(Bytes{*iter});
            }
            tmp.comp = Comparator::IN;
            return tmp;
        }

        template<typename T>
        Condition in(const T &value) const {
            auto tmp(*this);
            tmp.valueSet = std::vector<Bytes>{Bytes{value}};
            tmp.comp = Comparator::IN;
            return tmp;
        }

        template<typename T, typename... U>
        Condition in(const T &head, const U &... tail) const {
            auto tmp = in(head);
            for (const auto &value: in(tail...).valueSet) {
                tmp.valueSet.emplace_back(value);
            }
            return tmp;
        }

        MultiCondition operator&&(const Condition &c) const;

        MultiCondition operator&&(const MultiCondition &e) const;

        MultiCondition operator||(const Condition &c) const;

        MultiCondition operator||(const MultiCondition &e) const;

        Condition operator!() const;

    private:
        std::string propName;
        Bytes valueBytes;
        std::vector<Bytes> valueSet;
        Comparator comp;
        bool isIgnoreCase{false};
        bool isNegative{false};
    };

    class MultiCondition {
    private:
        enum Operator {
            AND, OR
        };

        class CompositeNode;

        class ConditionNode;

    public:
        friend class Condition;

        friend struct Compare;
        friend struct Index;

        MultiCondition() = delete;

        MultiCondition &operator&&(const MultiCondition &e);

        MultiCondition &operator&&(const Condition &c);

        MultiCondition &operator||(const MultiCondition &e);

        MultiCondition &operator||(const Condition &c);

        MultiCondition operator!() const;

        bool execute(const Record &r, const PropertyMapType &propType) const;

    private:
        std::shared_ptr<CompositeNode> root;
        std::vector<std::weak_ptr<ConditionNode>> conditions;

        MultiCondition(const Condition &c1, const Condition &c2, Operator opt);

        MultiCondition(const Condition &c, const MultiCondition &e, Operator opt);

        class ExprNode {
        public:
            explicit ExprNode(bool isCondition_);

            virtual ~ExprNode() noexcept = default;

            virtual bool check(const Record &r, const PropertyMapType &propType) const = 0;

            bool checkIfCondition() const;

        private:
            bool isCondition;
        };

        class ConditionNode : public ExprNode {
        public:
            explicit ConditionNode(const Condition &cond_);

            ~ConditionNode() noexcept override = default;

            virtual bool check(const Record &r, const PropertyMapType &propType) const override;

            const Condition &getCondition() const;

        private:
            Condition cond;
        };

        class CompositeNode : public ExprNode {
        public:
            CompositeNode(const std::shared_ptr<ExprNode> &left_,
                          const std::shared_ptr<ExprNode> &right_,
                          Operator opt_,
                          bool isNegative_ = false);

            ~CompositeNode() noexcept override = default;

            virtual bool check(const Record &r, const PropertyMapType &propType) const override;

            const std::shared_ptr<ExprNode> &getLeftNode() const;

            const std::shared_ptr<ExprNode> &getRightNode() const;

            const Operator &getOperator() const;

            bool getIsNegative() const;

        private:
            std::shared_ptr<ExprNode> left;
            std::shared_ptr<ExprNode> right;
            Operator opt;
            bool isNegative;
        };
    };

    class PathFilter {
    public:
        friend struct Algorithm;

        PathFilter() = default;

        PathFilter(bool (*vertexFunc)(const Record &record), bool (*edgeFunc)(const Record &record));

        ~PathFilter() noexcept = default;

        PathFilter &setVertex(bool (*function)(const Record &record));

        PathFilter &setEdge(bool (*function)(const Record &record));

        bool isEnable() const;

        bool isSetVertex() const;

        bool isSetEdge() const;

    private:
        bool (*vertexFilter)(const Record &record) {
            nullptr
        };

        bool (*edgeFilter)(const Record &record) {
            nullptr
        };
    };

    class ClassFilter {
    public:
        ClassFilter() = default;

        ~ClassFilter() noexcept = default;

        ClassFilter(const std::initializer_list<std::string> &initializerList);

        ClassFilter(const std::vector<std::string> &classNames_);

        ClassFilter(const std::list<std::string> &classNames_);

        ClassFilter(const std::set<std::string> &classNames_);

        ClassFilter(const std::vector<std::string>::const_iterator &begin,
                    const std::vector<std::string>::const_iterator &end);

        ClassFilter(const std::list<std::string>::const_iterator &begin,
                    const std::list<std::string>::const_iterator &end);

        ClassFilter(const std::set<std::string>::const_iterator &begin,
                    const std::set<std::string>::const_iterator &end);

        //ClassFilter& exclude();
        void add(const std::string &className);

        void remove(const std::string &className);

        size_t size() const;

        bool empty() const;

        const std::set<std::string> &getClassName() const;
        //bool isExcludeClassName() const;
    private:
        std::set<std::string> classNames{};
        //bool isExclude{false};
    };

}

#endif
