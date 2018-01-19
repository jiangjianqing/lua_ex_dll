#include <QString>
#include <QtTest>

class Unit_test_1Test : public QObject
{
    Q_OBJECT

public:
    Unit_test_1Test();

private Q_SLOTS:
    void testCase1();
};

Unit_test_1Test::Unit_test_1Test()
{
}

void Unit_test_1Test::testCase1()
{
    char *token = NULL;

    char delim[] = " ,!";
    int count = 0;

    char* cmd = "abc def ttt";
    char* buf = strdup(cmd);//复制一份cmd作为buf，后面用strsep会修改该值
    for(token = strsep(&buf, delim); token != NULL; token = strsep(&buf, delim)) {
        //One difference between strsep and strtok_r is that if the input string contains more
        //than one character from delimiter in a row strsep returns an empty string for each
        //pair of characters from delimiter. This means that a program normally should test
        //for strsep returning an empty string before processing it.
        if(token != NULL && strlen(token) == 0){
            continue;
        }
        printf(token);
        printf("+");
    }
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(Unit_test_1Test)

#include "tst_unit_test_1test.moc"
