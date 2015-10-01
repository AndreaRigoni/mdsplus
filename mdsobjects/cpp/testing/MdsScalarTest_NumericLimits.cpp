#include <string>
#include <cstring>

#include <mdsobjects.h>

#include "testing.h"
#include "testutils/testutils.h"
#include "testutils/Singleton.h"

#include "MdsDataTest.h"

using namespace MDSplus;
using namespace testing;


////////////////////////////////////////////////////////////////////////////////
//  NumericLimits Test  ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace testing {

///
/// \brief The TestLimits class
///
/// This tests numeric conversion of Data object with numerical limits ..

///
class TestLimits {
public:
    template < typename _MdsT >
    static void print_type_conversion_test(const std::string &type_name) {
        typedef typename detail::mds2cpp_typemap<_MdsT>::type c_type;
        c_type min = numeric_limits<c_type>::lowest();
        c_type eps = numeric_limits<c_type>::epsilon();
        c_type max = numeric_limits<c_type>::highest();

        Data * mds_min = new _MdsT(min);
        Data * mds_eps = new _MdsT(eps);
        Data * mds_max = new _MdsT(max);

        std::cout << "testing::TestLimits::test_conversion_limits<" << type_name << ">("
                  << "\"" << mdsdata_to_string(mds_min) << "\","
                  << "\"" << mdsdata_to_string(mds_eps) << "\","
                  << "\"" << mdsdata_to_string(mds_max) << "\");\n";

        deleteData(mds_min);
        deleteData(mds_eps);
        deleteData(mds_max);
    }

    template < typename _MdsT >
    static void test_conversion_limits(const std::string &tmin = "",
                                       const std::string &teps = "",
                                       const std::string &tmax = "")
    {
        typedef typename detail::mds2cpp_typemap<_MdsT>::type c_type;
        c_type min = numeric_limits<c_type>::lowest();
        c_type eps = numeric_limits<c_type>::epsilon();
        c_type max = numeric_limits<c_type>::highest();

        { // MIN //
            c_type & value = min;
            const std::string &str = tmin;

            Data * data = new _MdsT(value);
            print_type_encoded(data);
            MdsDataTest::test_data_numerics(data,value);
            if(!str.empty())
                MdsDataTest::test_data_string(data,str.c_str());
            deleteData(data);
        }
        { // 0 + EPSILON //
            c_type & value = eps;
            const std::string &str = teps;

            Data * data = new _MdsT(value);
            print_type_encoded(data);
            MdsDataTest::test_data_numerics(data,value);
            if(!str.empty())
                MdsDataTest::test_data_string(data,str.c_str());
            deleteData(data);
        }
        { // MAX //
            c_type & value = max;
            const std::string &str = tmax;

            Data * data = new _MdsT(value);
            print_type_encoded(data);
            MdsDataTest::test_data_numerics(data,value);
            if(!str.empty())
                MdsDataTest::test_data_string(data,str.c_str());
            deleteData(data);
        }
    }



    // TODO: check limits for arrays //
    template < typename _MdsT >
    static void test_type_conversion_array(const std::string &tmin = "",
                                           const std::string &teps = "",
                                           const std::string &tmax = "")
    {}

};



// SPECIAL TRAIT FOR FLOAT  ..  //
//template <>
//void TestLimits::test_conversion_limits<Float32>(const std::string &tmin,
//                                            const std::string &teps,
//                                            const std::string &tmax)
//{
//    typedef typename detail::mds2cpp_typemap<Float32>::type c_type;
//    c_type max = std::numeric_limits<c_type>::max();
//    std::cout << " TESTING TRAIT FOR FLOAT: \n"
//              << " max = " << max << "\n"
//              << " static_cast<int>(max) = " << static_cast<int>(max) << "\n"
//              << " numeric_cast<int>(max) = " << Scalar::numeric_cast<int>(max) << "\n";
//}

} // testing







////////////////////////////////////////////////////////////////////////////////
//  MAIN  //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// This is a simple print of the test line to automatically compute type
/// limits if the print argument is passed to the test execution.
/// 
#define MDS_TEST_PRINT_TESTLINE(type) \
    testing::TestLimits::print_type_conversion_test<type>(#type);



int main(int argc, char *argv[])
{
    BEGIN_TESTING(Data);

    if(argc > 1 && !strcmp(argv[1],"print") ) {
        std::cout << "--- CUT FROM HERE ------------------------------------\n";

        MDS_TEST_PRINT_TESTLINE(Int8);
        MDS_TEST_PRINT_TESTLINE(Int16);
        MDS_TEST_PRINT_TESTLINE(Int32);
        MDS_TEST_PRINT_TESTLINE(Int64);

        MDS_TEST_PRINT_TESTLINE(Uint8);
        MDS_TEST_PRINT_TESTLINE(Uint16);
        MDS_TEST_PRINT_TESTLINE(Uint32);
        MDS_TEST_PRINT_TESTLINE(Uint64);

        MDS_TEST_PRINT_TESTLINE(Float32);
        MDS_TEST_PRINT_TESTLINE(Float64);
//        MDS_TEST_PRINT_TESTLINE(Complex32);
//        MDS_TEST_PRINT_TESTLINE(Complex64);

        std::cout << "--- END CUT ------------------------------------------\n";
        exit(0);
    }


    testing::TestLimits::test_conversion_limits<Int8>("-128B","0B","127B");
    testing::TestLimits::test_conversion_limits<Int16>("-32768W","0W","32767W");
    testing::TestLimits::test_conversion_limits<Int32>("-2147483648","0","2147483647");
    testing::TestLimits::test_conversion_limits<Int64>("0X8000000000000000Q","0X0Q","0X7fffffffffffffffQ");
    testing::TestLimits::test_conversion_limits<Uint8>("0BU","0BU","255BU");
    testing::TestLimits::test_conversion_limits<Uint16>("0WU","0WU","65535WU");
    testing::TestLimits::test_conversion_limits<Uint32>("0LU","0LU","4294967295LU");
    testing::TestLimits::test_conversion_limits<Uint64>("0X0QU","0X0QU","0XffffffffffffffffQU");
    testing::TestLimits::test_conversion_limits<Float32>("-340.282E36","119.209E-9","340.282E36");
    testing::TestLimits::test_conversion_limits<Float64>("-179.7693134862316D306","222.0446049250313D-18","179.7693134862316D306");



    END_TESTING;
}




