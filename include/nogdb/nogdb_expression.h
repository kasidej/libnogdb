/*
 *  Copyright (C) 2018, Throughwave (Thailand) Co., Ltd.
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

#ifndef NOGDB_NOGDB_EXPRESSION_H
#define NOGDB_NOGDB_EXPRESSION_H

#include <memory>
#include <functional>
#include <vector>
#include <set>
#include <string>

#include "nogdb_types.h"

namespace nogdb {

    using ConditionFunction = std::function<bool(const Record &)>;

    class Expression {
    public:
        friend struct Compare;

        enum class Comparator {
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
            IS_IN,
            BETWEEN,
            BETWEEN_NO_UPPER,
            BETWEEN_NO_LOWER,
            BETWEEN_NO_BOUND
        };

        explicit Expression(const std::string &propName);
        explicit Expression(const ConditionFunction &func);
        Expression(const std::string &propName, const Comparator &cmp, const Bytes &value);
        Expression(const std::string &propName, const Comparator &cmp, const std::vector<Bytes> &values);
        ~Expression() noexcept = default;

        /* Use implicit copy/move constructor because we can't edit this class.
        Expression(const Expression &rhs);
        Expression(Expression &&rhs);
         */

        Expression &eq(const Bytes &value);
        Expression &gt(const Bytes &value);
        Expression &lt(const Bytes &value);
        Expression &ge(const Bytes &value);
        Expression &le(const Bytes &value);
        Expression &contain(const Bytes &value);
        Expression &beginWith(const Bytes &value);
        Expression &endWith(const Bytes &value);
        Expression &like(const Bytes &value);
        Expression &regex(const Bytes &value);
        Expression &between(const Bytes &lower, const Bytes &upper, const std::pair<bool, bool> &includeBound = {true, true});
        Expression &in(const std::vector<Bytes> &values);
        Expression &in(const std::initializer_list<Bytes> &values);
        Expression &null();
        Expression &ignoreCase(bool i = true);

        Expression &operator!();
        Expression operator&&(const Expression &exp) const;
        Expression operator||(const Expression &exp) const;
        friend Expression operator&&(Expression &&lhs, Expression &&rhs);
        friend Expression operator||(Expression &&lhs, Expression &&rhs);

        bool execute(const Record &record, const PropertyMapType &propMapType) const;
        std::set<std::string> collectAllPropertyName() const;

    private:
        struct Condition {
            enum Type {
                PROP,
                FUNC,
            };

            Condition() = default;
            Condition(const std::string &prop, const Comparator &cmp, Bytes &&value)
                    : type{Type::PROP}, p{prop, cmp, std::move(value), false} {};
            Condition(const std::string &prop, const Comparator &cmp, std::vector<Bytes> &&values)
                    : type{Type::PROP}, p{prop, cmp, Bytes(std::move(values)), false } {};
            explicit Condition(ConditionFunction func_)
                    : type{Type::FUNC}, func(std::move(func_)) {}

            Type type;
            struct {
                std::string name;
                Comparator cmp;
                Bytes value;
                bool ignoreCase;
            } p;
            ConditionFunction func;
        };

        enum class Operator {
            NONE, AND, OR
        };

        Expression(const Operator &op_, std::shared_ptr<Expression> nodeL_, std::shared_ptr<Expression> nodeR_)
                : op{op_}, nodeL{std::move(nodeL_)}, nodeR{std::move(nodeR_)} {}

        void validateOperator(const Operator &op) const;
        void validateCondition(const Condition::Type &t) const;

        inline void validate(const Operator &op, const Condition::Type &t) const {
            this->validateOperator(op);
            this->validateCondition(t);
        }

        Operator op{Operator::NONE};
        std::shared_ptr<Expression> nodeL{nullptr};
        std::shared_ptr<Expression> nodeR{nullptr};
        Condition leaf;
        bool negative{false};
    };
}

#endif //NOGDB_NOGDB_EXPRESSION_H
