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

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <set>
#include <functional>

#include <nogdb/nogdb_expression.h>
#include <nogdb/nogdb_errors.h>
#include "compare.hpp"

using namespace std;
using namespace nogdb;

Expression::Expression(const string &propName)
        : Expression(propName, Comparator::NOT_NULL, Bytes{}) {}

Expression::Expression(const ConditionFunction &func)
        : leaf{func} {}

Expression::Expression(const string &propName,
                       const Comparator &cmp,
                       const Bytes &value)
        : leaf{propName, cmp, Bytes(value)} {}

Expression::Expression(const string &propName,
        const Comparator &cmp,
        const vector<Bytes> &values)
        : leaf{propName, cmp, vector<Bytes>(values)} {}

Expression &Expression::eq(const Bytes &value) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::EQUAL;
    this->leaf.p.value = { value };
    return *this;
}

Expression &Expression::gt(const Bytes &value) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::GREATER;
    this->leaf.p.value = { value };
    return *this;
}

Expression &Expression::lt(const Bytes &value) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::LESS;
    this->leaf.p.value = { value };
    return *this;
}

Expression &Expression::ge(const Bytes &value) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::GREATER_EQUAL;
    this->leaf.p.value = { value };
    return *this;
}

Expression &Expression::le(const Bytes &value) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::LESS_EQUAL;
    this->leaf.p.value = { value };
    return *this;
}

Expression &Expression::contain(const Bytes &value) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::CONTAIN;
    this->leaf.p.value = { value };
    return *this;
}

Expression &Expression::beginWith(const Bytes &value) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::BEGIN_WITH;
    this->leaf.p.value = { value };
    return *this;
}

Expression &Expression::endWith(const Bytes &value) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::END_WITH;
    this->leaf.p.value = { value };
    return *this;
}

Expression &Expression::like(const Bytes &value) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::LIKE;
    this->leaf.p.value = { value };
    return *this;
}

Expression &Expression::regex(const Bytes &value) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::REGEX;
    this->leaf.p.value = { value };
    return *this;
}

Expression &Expression::between(const Bytes &lower, const Bytes &upper, const pair<bool, bool> &includeBound) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    if (!includeBound.first && !includeBound.second) {
        this->leaf.p.cmp = Comparator::BETWEEN_NO_BOUND;
    } else if (!includeBound.first && includeBound.second) {
        this->leaf.p.cmp = Comparator::BETWEEN_NO_LOWER;
    } else if (includeBound.first && !includeBound.second) {
        this->leaf.p.cmp = Comparator::BETWEEN_NO_UPPER;
    } else {
        this->leaf.p.cmp = Comparator::BETWEEN;
    }
    this->leaf.p.value = Bytes::toBytes(pair<Bytes, Bytes>{lower, upper});
    return *this;
}

Expression &Expression::in(const vector<Bytes> &values) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::IS_IN;
    this->leaf.p.value = Bytes::toBytes(values);
    return *this;
}

Expression &Expression::in(const initializer_list<Bytes> &values) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::IS_IN;
    this->leaf.p.value = Bytes::toBytes(vector<Bytes>(values));
    return *this;
}

Expression &Expression::null() {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.cmp = Comparator::NOT_NULL;
    this->negative = true;
    return *this;
}

Expression &Expression::ignoreCase(bool i) {
    this->validate(Operator::NONE, Condition::Type::PROP);
    this->leaf.p.ignoreCase = i;
    return *this;
}

Expression &Expression::operator!() {
    this->negative = !this->negative;
    return *this;
}

Expression Expression::operator&&(const nogdb::Expression &exp) const {
    return Expression(*this) && Expression(exp);
}

Expression Expression::operator||(const nogdb::Expression &exp) const {
    return Expression(*this) || Expression(exp);
}

Expression nogdb::operator&&(Expression &&lhs, Expression &&rhs) {
    return Expression(
            Expression::Operator::AND,
            make_shared<Expression>(move(lhs)),
            make_shared<Expression>(move(rhs))
    );
};

Expression nogdb::operator||(Expression &&lhs, Expression &&rhs) {
    return Expression(
            Expression::Operator::OR,
            make_shared<Expression>(move(lhs)),
            make_shared<Expression>(move(rhs))
    );
};

bool Expression::execute(const Record &record, const PropertyMapType &propMapType) const {
    switch (this->op) {
        case Operator::NONE:
            switch (this->leaf.type) {
                case Condition::Type::PROP: {
                    auto value = record.get(this->leaf.p.name);
                    auto type = propMapType.find(this->leaf.p.name);
                    if (type == propMapType.cend()) {
                        throw NOGDB_CONTEXT_ERROR(NOGDB_CTX_INVALID_PROPTYPE);
                    }
                    return Compare::compare(value, this->leaf.p.value, this->leaf.p.cmp, type->second, this->leaf.p.ignoreCase) ^ this->negative;
                }

                case Condition::Type::FUNC:
                    return this->leaf.func(record);
            }
            return false;
        case Operator::AND:
            return ((this->nodeL->execute(record, propMapType)
                     && this->nodeR->execute(record, propMapType)
                    ) ^ this->negative);
        case Operator::OR:
            return ((this->nodeL->execute(record, propMapType)
                     || this->nodeR->execute(record, propMapType)
                    ) ^ this->negative);
    }
}

set<string> Expression::collectAllPropertyName() const {
    set<string> result;
    function<void(const Expression &exp)> collectProp;
    collectProp = [&](const Expression &exp) {
        if (exp.op != Operator::NONE) {
            collectProp(*exp.nodeL);
            collectProp(*exp.nodeR);
        } else if (Condition::Type::PROP == exp.leaf.type) {
            result.insert(exp.leaf.p.name);
        } else {
            // no-op. (leaf node and condition function)
        }
    };
    collectProp(*this);
    return result;
};

void Expression::validateOperator(const Operator &op) const {
    if (this->op != op) {
        throw NOGDB_CONTEXT_ERROR(NOGDB_CTX_INVALID_EXPRESSION);
    }
}

void Expression::validateCondition(const Condition::Type &t) const {
    if (this->leaf.type != t) {
        throw NOGDB_CONTEXT_ERROR(NOGDB_CTX_INVALID_EXPRESSION);
    }
}
