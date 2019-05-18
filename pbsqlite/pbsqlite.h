#pragma once

#include <fmt/format.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <google/protobuf/message.h>

namespace pbsqlite {
inline std::vector<std::string> GetColumnNames(const google::protobuf::Message& m) {
    std::vector<std::string> ret;
    using namespace google::protobuf;
    const Descriptor *desc = m.GetDescriptor();
    const Reflection *refl = m.GetReflection();
    int fieldCount = desc->field_count();

    for (int i = 0; i < fieldCount; i++) {
        const FieldDescriptor *field = desc->field(i);
        ret.push_back(field->name());
    }
    return ret;
}

inline std::vector<std::string> GetColumnTypeNames(const google::protobuf::Message& m) {
    std::vector<std::string> ret;
    using namespace google::protobuf;
    const Descriptor *desc = m.GetDescriptor();
    const Reflection *refl = m.GetReflection();
    int fieldCount = desc->field_count();

    for (int i = 0; i < fieldCount; i++) {
        const FieldDescriptor *field = desc->field(i);
        ret.push_back(field->type_name());
    }
    return ret;
}

inline std::string GetSQLiteTypeName(const std::string& messageTypeName) {
    if (messageTypeName == "string")
        return "TEXT";
    if (messageTypeName == "int32")
        return "INTEGER";
    if (messageTypeName == "int64")
        return "INTEGER";
    if (messageTypeName == "uint32")
        return "INTEGER";
    if (messageTypeName == "uint64")
        return "INTEGER";
    if (messageTypeName == "float")
        return "REAL";
    if (messageTypeName == "double")
        return "REAL";
    assert(false);
    return "TEXT";
}

inline std::string ToCreateTableQuery(const google::protobuf::Message& m, const std::string& primaryKey) {
    using namespace google::protobuf;
    const Descriptor *desc = m.GetDescriptor();
    const Reflection *refl = m.GetReflection();
    std::string tableName = desc->name();
    std::string query = fmt::format("CREATE TABLE if not exists {0} (", tableName);
    int fieldCount = desc->field_count();

    for (int i = 0; i < fieldCount; i++) {
        const FieldDescriptor *field = desc->field(i);
        query += fmt::format("{0} {1},", field->name(), GetSQLiteTypeName(field->type_name()));
    }
    query += "PRIMARY KEY (" + primaryKey + "));";
    return query;
}

inline std::string ToInsertString(const google::protobuf::Message& m, const std::string& insertType = "INSERT INTO") {
    using namespace google::protobuf;
    const Descriptor *desc = m.GetDescriptor();
    const Reflection *refl = m.GetReflection();
    std::string tableName = desc->name();
    std::string query = fmt::format("{0} {1} VALUES(", insertType, tableName);
    int fieldCount = desc->field_count();

    for (int i = 0; i < fieldCount; i++) {
        const FieldDescriptor *field = desc->field(i);
        switch (field->cpp_type()) {
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            query += fmt::format("'{0}',", refl->GetString(m, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            query += fmt::format("{0},", refl->GetInt32(m, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            query += fmt::format("{0},", refl->GetInt64(m, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            query += fmt::format("{0},", refl->GetFloat(m, field));
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            query += fmt::format("{0},", refl->GetDouble(m, field));
            break;
        default:
            break;
        }
    }
    query.pop_back();
    query += ");";
    return query;
}

inline void CreateTableIfNotExists(SQLite::Database& db, const google::protobuf::Message& m, const std::string& primaryKey) {
    db.exec(ToCreateTableQuery(m, primaryKey));
}
inline void InsertInto(SQLite::Database& db, const google::protobuf::Message& m) {
    db.exec(ToInsertString(m));
}
inline void ReplaceInto(SQLite::Database& db, const google::protobuf::Message& m) {
    db.exec(ToInsertString(m, "REPLACE INTO"));
}
template<typename T>
std::vector<T> Select(SQLite::Database& db, const std::string& condition = "") {
    std::vector<T> ret;
    T m;
    using namespace google::protobuf;
    const Descriptor *desc = m.GetDescriptor();
    const Reflection *refl = m.GetReflection();
    int fieldCount = desc->field_count();
    std::string tableName = desc->name();
    std::string query = fmt::format("SELECT * FROM {0} {1}", tableName, condition);
    SQLite::Statement stmt(db, query);
    while (stmt.executeStep()) {
        for (int i = 0; i < fieldCount; i++) {
            const FieldDescriptor *field = desc->field(i);
            switch (field->cpp_type()) {
            case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                refl->SetString(&m, field, stmt.getColumn(i).getString());
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
                refl->SetInt32(&m, field, stmt.getColumn(i).getInt());
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
                refl->SetInt64(&m, field, stmt.getColumn(i).getInt64());
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
                refl->SetFloat(&m, field, stmt.getColumn(i).getDouble());
                break;
            case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
                refl->SetDouble(&m, field, stmt.getColumn(i).getDouble());
                break;
            default:
                break;
            }
        }
        ret.push_back(m);
    }
    return ret;
}
}