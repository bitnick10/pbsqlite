# pbsqlite
header only

    #include "pbsqlite.h"
    // insert:
    pbsqlite::Database db("example.db", SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
    Person person;
    db.ReplaceInto(person);
    // select:
    std::vector<Person> rs = db.Select<Person>("WHERE id>=0");