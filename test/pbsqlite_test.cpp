#include "gtest/gtest.h"

#include "../pbsqlite/pbsqlite.h"
#include "person.pb.h"

using namespace aa;

TEST(bpsqlite, Base) {
    Person person;
    person.set_name("Jonh");
    Person person2;
    person2.set_id(2);
    person2.set_name("David");
    pbsqlite::Database db("example.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    db.CreateTableIfNotExists(person, "id");
    db.ReplaceInto(person);
    db.ReplaceInto(person2);

    std::vector<Person> rs = db.Select<Person>("WHERE id>=0");

    EXPECT_TRUE(rs.size() >= 2);
    EXPECT_EQ(rs[0].id(), 0);
    EXPECT_EQ(rs[1].id(), 2);
    EXPECT_EQ(rs[1].name(), "David");
}

