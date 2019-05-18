# pbsqlite
header only

    #include "pbsqlite.h"
    // insert:
    SQLite::Database db("example.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    Person person;
    ReplaceInto(db, person);
    // select:
    std::vector<Person> rs = Select<Person>(db, "WHERE id>=0");