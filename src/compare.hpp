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

#ifndef __COMPARE_HPP_INCLUDED_
#define __COMPARE_HPP_INCLUDED_

#include <vector>

#include "schema.hpp"
#include "base_txn.hpp"

#include "graph.hpp"
#include "nogdb_types.h"
#include "nogdb_compare.h"
#include "nogdb/nogdb_expression.h"

namespace nogdb {
    struct Compare {
        Compare() = delete;

        ~Compare() noexcept = delete;

        inline static std::string toLower(const std::string &text) {
            auto tmp = std::string{};
            std::transform(text.cbegin(), text.cend(), std::back_inserter(tmp), ::tolower);
            return tmp;
        };

        using CompareFunction = std::function<bool(const Bytes &, const Bytes &, PropertyType, bool)>;

        static bool compareNotNull(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareEqual(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareGreater(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareLess(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareGreaterEqual(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareLessEqual(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareContain(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareBeginWith(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareEndWith(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareLike(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareRegex(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareIn(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareBetween(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareBetweenNoUpper(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareBetweenNoLower(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);
        static bool compareBetweenNoBound(const Bytes &lhs, const Bytes &rhs, PropertyType type, bool ignoreCase);

        static bool compare(const Bytes &lhs, const Bytes &rhs, Expression::Comparator cmp, PropertyType type, bool ignoreCase);

        static bool genericCompareFunc(const Bytes &value,
                                       PropertyType type,
                                       const Bytes &cmpValue1,
                                       const Bytes &cmpValue2,
                                       Condition::Comparator cmp,
                                       bool isIgnoreCase);

        static bool compareBytesValue(const Bytes &value, PropertyType type, const Condition &condition);

        //*****************************************************************
        //*  result set supported functions                               *
        //*****************************************************************

        static ResultSet getRecordCondition(const Txn &txn,
                                            const std::vector<ClassInfo> &classInfos,
                                            const Condition &condition,
                                            PropertyType type);

        static ResultSet getRecordCondition(const Txn &ctx,
                                            const std::vector<ClassInfo> &classInfos,
                                            bool (*condition)(const Record &record));

        static ResultSet getRecordMultiCondition(const Txn &txn,
                                                 const std::vector<ClassInfo> &classInfos,
                                                 const MultiCondition &conditions,
                                                 const PropertyMapType &types);

        static ResultSet getRecordExpression(const Txn &txn,
                                             const std::vector<ClassInfo> &classInfos,
                                             const Expression &expression,
                                             const PropertyMapType &types);

        static ResultSet getEdgeCondition(const Txn &txn,
                                          const RecordDescriptor &recordDescriptor,
                                          const std::vector<ClassId> &edgeClassIds,
                                          std::vector<RecordId>
                                          (Graph::*func)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                          const Condition &condition,
                                          PropertyType type);

        static ResultSet getEdgeCondition(const Txn &txn,
                                          const RecordDescriptor &recordDescriptor,
                                          const std::vector<ClassId> &edgeClassIds,
                                          std::vector<RecordId>
                                          (Graph::*func)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                          bool (*condition)(const Record &record));

        static ResultSet getEdgeMultiCondition(const Txn &txn,
                                               const RecordDescriptor &recordDescriptor,
                                               const std::vector<ClassId> &edgeClassIds,
                                               std::vector<RecordId>
                                               (Graph::*func)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                               const MultiCondition &conditions,
                                               const PropertyMapType &types);

        static ResultSet getEdgeExpression(const Txn &txn,
                                           const RecordDescriptor &recordDescriptor,
                                           const std::vector<ClassId> &edgeClassIds,
                                           std::vector<RecordId>
                                           (Graph::*func)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                           const Expression &exp,
                                           const PropertyMapType &types);

        static ResultSet compareCondition(const Txn &txn,
                                          const std::string &className,
                                          ClassType type,
                                          const Condition &condition,
                                          bool searchIndexOnly = false);

        static ResultSet compareCondition(const Txn &txn,
                                          const std::string &className,
                                          ClassType type,
                                          bool (*condition)(const Record &record));

        static ResultSet compareMultiCondition(const Txn &txn,
                                               const std::string &className,
                                               ClassType type,
                                               const MultiCondition &conditions,
                                               bool searchIndexOnly = false);

        static ResultSet compareExpression(const Txn &txn,
                                           const std::string &className,
                                           ClassType classType,
                                           const Expression &expression,
                                           bool searchIndexOnly = false);

        static ResultSet compareEdgeCondition(const Txn &txn,
                                              const RecordDescriptor &recordDescriptor,
                                              std::vector<RecordId>
                                              (Graph::*func1)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                              std::vector<ClassId>
                                              (Graph::*func2)(const BaseTxn &baseTxn, const RecordId &rid),
                                              bool (*condition)(const Record &),
                                              const ClassFilter &classFilter);

        static ResultSet compareEdgeCondition(const Txn &txn,
                                              const RecordDescriptor &recordDescriptor,
                                              std::vector<RecordId>
                                              (Graph::*func1)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                              std::vector<ClassId>
                                              (Graph::*func2)(const BaseTxn &baseTxn, const RecordId &rid),
                                              const Condition &condition,
                                              const ClassFilter &classFilter);

        static ResultSet compareEdgeMultiCondition(const Txn &txn,
                                                   const RecordDescriptor &recordDescriptor,
                                                   std::vector<RecordId>
                                                   (Graph::*func1)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                                   std::vector<ClassId>
                                                   (Graph::*func2)(const BaseTxn &baseTxn, const RecordId &rid),
                                                   const MultiCondition &conditions,
                                                   const ClassFilter &classFilter);

        static ResultSet compareEdgeExpression(const Txn &txn,
                                               const RecordDescriptor &recordDescriptor,
                                               std::vector<RecordId>
                                               (Graph::*func1)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                               std::vector<ClassId>
                                               (Graph::*func2)(const BaseTxn &baseTxn, const RecordId &rid),
                                               const Expression &exp,
                                               const ClassFilter &classFilter);

        //*****************************************************************
        //*  cursor supported functions                                   *
        //*****************************************************************

        static std::vector<RecordDescriptor>
        getRdescCondition(const Txn &txn, const std::vector<ClassInfo> &classInfos, const Condition &condition,
                          PropertyType type);

        static std::vector<RecordDescriptor>
        getRdescCondition(const Txn &ctx, const std::vector<ClassInfo> &classInfos,
                          bool (*condition)(const Record &record));

        static std::vector<RecordDescriptor>
        getRdescMultiCondition(const Txn &txn, const std::vector<ClassInfo> &classInfos,
                               const MultiCondition &conditions, const PropertyMapType &types);

        static std::vector<RecordDescriptor>
        getRdescExpression(const Txn &txn, const std::vector<ClassInfo> &classInfos,
                           const Expression &exp, const PropertyMapType &types);

        static std::vector<RecordDescriptor>
        getRdescEdgeCondition(const Txn &txn, const RecordDescriptor &recordDescriptor,
                              const std::vector<ClassId> &edgeClassIds,
                              std::vector<RecordId>
                              (Graph::*func)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                              const Condition &condition, PropertyType type);

        static std::vector<RecordDescriptor>
        getRdescEdgeCondition(const Txn &txn, const RecordDescriptor &recordDescriptor,
                              const std::vector<ClassId> &edgeClassIds,
                              std::vector<RecordId> (Graph::*func)(const BaseTxn &baseTxn, const RecordId &rid,
                                                                   const ClassId &classId),
                              bool (*condition)(const Record &record));

        static std::vector<RecordDescriptor>
        getRdescEdgeMultiCondition(const Txn &txn, const RecordDescriptor &recordDescriptor,
                                   const std::vector<ClassId> &edgeClassIds,
                                   std::vector<RecordId>
                                   (Graph::*func)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                   const MultiCondition &conditions, const PropertyMapType &types);

        static std::vector<RecordDescriptor>
        getRdescEdgeExpression(const Txn &txn, const RecordDescriptor &recordDescriptor,
                               const std::vector<ClassId> &edgeClassIds,
                               std::vector<RecordId>
                               (Graph::*func)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                               const Expression &exp, const PropertyMapType &types);

        static std::vector<RecordDescriptor>
        compareConditionRdesc(const Txn &txn, const std::string &className, ClassType type, const Condition &condition,
                              bool searchIndexOnly = false);

        static std::vector<RecordDescriptor>
        compareConditionRdesc(const Txn &txn, const std::string &className, ClassType type,
                              bool (*condition)(const Record &record));

        static std::vector<RecordDescriptor>
        compareMultiConditionRdesc(const Txn &txn, const std::string &className, ClassType type,
                                   const MultiCondition &conditions, bool searchIndexOnly = false);

        static std::vector<RecordDescriptor>
        compareExpressionRdesc(const Txn &txn, const std::string &className, ClassType type,
                               const Expression &exp, bool searchIndexOnly = false);

        static std::vector<RecordDescriptor>
        compareEdgeConditionRdesc(const Txn &txn, const RecordDescriptor &recordDescriptor,
                                  std::vector<RecordId>
                                  (Graph::*func1)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                  std::vector<ClassId> (Graph::*func2)(const BaseTxn &baseTxn, const RecordId &rid),
                                  bool (*condition)(const Record &), const ClassFilter &classFilter);

        static std::vector<RecordDescriptor>
        compareEdgeConditionRdesc(const Txn &txn, const RecordDescriptor &recordDescriptor,
                                  std::vector<RecordId>
                                  (Graph::*func1)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                  std::vector<ClassId> (Graph::*func2)(const BaseTxn &baseTxn, const RecordId &rid),
                                  const Condition &condition, const ClassFilter &classFilter);

        static std::vector<RecordDescriptor>
        compareEdgeMultiConditionRdesc(const Txn &txn, const RecordDescriptor &recordDescriptor,
                                       std::vector<RecordId>
                                       (Graph::*func1)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                       std::vector<ClassId>
                                       (Graph::*func2)(const BaseTxn &baseTxn, const RecordId &rid),
                                       const MultiCondition &conditions, const ClassFilter &classFilter);

        static std::vector<RecordDescriptor>
        compareEdgeExpressionRdesc(const Txn &txn, const RecordDescriptor &recordDescriptor,
                                   std::vector<RecordId>
                                   (Graph::*func1)(const BaseTxn &baseTxn, const RecordId &rid, const ClassId &classId),
                                   std::vector<ClassId>
                                   (Graph::*func2)(const BaseTxn &baseTxn, const RecordId &rid),
                                   const Expression &exp, const ClassFilter &classFilter);
    };
}

#endif
