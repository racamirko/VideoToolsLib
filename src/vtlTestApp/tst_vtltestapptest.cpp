#include <QString>
#include <QtTest>

class VtlTestAppTest : public QObject
{
    Q_OBJECT
    
public:
    VtlTestAppTest();
    
private Q_SLOTS:
    void testCase1();
};

VtlTestAppTest::VtlTestAppTest()
{
}

void VtlTestAppTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(VtlTestAppTest)

#include "tst_vtltestapptest.moc"
